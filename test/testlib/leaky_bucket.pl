#!/usr/bin/perl  -w

sub init_vars()
{
    $sample_rate=0;
    $sample_size=0;
    $num_channels=0;
    $minMhz=1000.0;
    $maxMhz=0.0;
    $buffer_interval=50000;
    $border_condition=0.5;
    $nframes_skip=0;
    $nframes=0;
    $step=0.03;
    $hw_platform=0;
}

sub calculate_mhz()
{
	if (0) {
		$microseconds[$frame] = (1000000 * $bytes[$frame]) / ($sample_size*$num_channels*$sample_rate);
	} else {
		my $ms = (1000000 * $bytes[$frame]) / ($sample_size*$num_channels*$sample_rate);
		if (0 && ($microseconds[$frame] != $ms)) {
			print("warning: ms read from raw-prof-file ".$microseconds[$frame]." != ".$ms." calculated. Frame $frame\n");
		}
	}
	
	$mhz[$frame] = ($cycles[$frame] + $stored_cycles) / $microseconds[$frame];
       	if($minMhz > $mhz[$frame]) {
		$minMhz = $mhz[$frame];
	}
	if($maxMhz < $mhz[$frame]) {
		$maxMhz = $mhz[$frame];
	}
	$stored_cycles=0;
}

sub read_file()
{
        open(CONFIG, "<$config")   || die "Can not open file ".$config." for reading. Filename should be the first argument";
	$frame=0;
	$read_frame=0;
	$stored_cycles=0;
	$max_sample_rate=0;
        $max_num_channels=0;
	$max_sample_size=0;        
	while (<CONFIG>) {
		s/\n//g;
		s/\r//g;
	  if(/Samplerate/) {
		  @word = split(/:/);
		  $sample_rate=$word[1];
		  if ($sample_rate > $max_sample_rate) 
		  {
			$max_sample_rate = $sample_rate;
		  }
#		  printf("Samplerate:         %u\n", $sample_rate);
	  }
	  if(/Number of channels/) {
		  @word = split(/:/);
		  $num_channels=$word[1];
		  if ($num_channels > $max_num_channels) 
		  {
			$max_num_channels = $num_channels;
		  }
#		  printf("Number of channels: %u\n", $num_channels);
	  }
	  if(/Sample size/) {
		  @word = split(/:/);
		  $sample_size=$word[1];
		  if ($sample_size > $max_sample_size) 
		  {
			$max_sample_size = $sample_size;
		  }
#		  printf("Sample size:        %u\n", $sample_size);
	  }
	  if(/D\$ cycles/) {
		  $hw_platform=1;
	  }
		
          if ($num_channels>0 && /^[0-9]/) {
		$read_frame++;
		if ( $read_frame <= $nframes_skip ) {
			next;
		}
		if ( $read_frame > ($nframes + $nframes_skip)&& $nframes > 0 ) {
			last;
		}
		 s/#.*//g;
		@word = split(/,/);
		if( $word[1] == 0 )
		{
			$stored_cycles+=$word[2];
			next;
		}
		$bytes[$frame]=$word[1];
		$cycles[$frame]=$word[2];
		if($hw_platform == 1){
			$i_cycles[$frame]=$word[7];
			$d_cycles[$frame]=$word[8];
			$microseconds[$frame]=$word[9];
		} else {
			$microseconds[$frame]=$word[7];
		}
		calculate_mhz();
#		printf ("%u: mks=%u mhz=%.3f \n", $frame, $microseconds[$frame], $mhz[$frame]);
		$frame++;
	  }
	}
	if ( $stored_cycles > 0 && $frame > 0)
	{
		$frame--;
		calculate_mhz();
		$frame++;
	}
        close(CONFIG);
	$frames = $frame;
        printf("Samplerate:         %u\n", $max_sample_rate);
        printf("Number of channels: %u\n", $max_num_channels);
	printf("Sample size:        %u\n", $max_sample_size);

        printf("File %s loaded: minMhz=%.3f, maxMhz=%.3f\n", $config, $minMhz, $maxMhz);
}

sub tryIt()
{
    #printf(" Try MHz=%.3f\n", $tryMhz);
    $buffer_size = $initial_buffer_size;
    for($frame=0;$frame<$frames;$frame++)
    {
        $leaked = $microseconds[$frame] * (1 - $mhz[$frame]/$tryMhz);
        $buffer_size += $leaked;
        #printf ("%u: mks=%u mhz=%.3f leaked=%.3f buffer=%.3f \n", $frame, $microseconds[$frame], $mhz[$frame], $leaked, $buffer_size);
        if($buffer_size < 0) {
            #printf("exit #2\n");
            return 1;
        }
        if($buffer_size > $buffer_interval) {
            $buffer_size = $buffer_interval;
        }
    }
    #printf("exit #1\n");
    return 0;
}

sub main()
{
    read_file();
    $initial_buffer_size = $buffer_interval * $border_condition;
    $tryMhz = $minMhz;
    while(1) {
        tryIt();
        if($buffer_size >= $initial_buffer_size) {
            printf("===========================================================\n");
            printf("Frequency:%.3f MHz\n", $tryMhz);
            return 0;
        }
        if($tryMhz > $maxMhz) {
            printf("===========================================================\n");
            printf("It is strange, but leaky bucket algorithm cannot found required frequency\n");
            printf("Up to %.3f MHz was looked\n", $tryMhz);
            return 1;
        }
        $tryMhz += $step;
    }
}

sub calc_hw()
{
    $bytes_per_window=$buffer_interval*$sample_rate*$sample_size*$num_channels/1000000;
    $bytes_per_second=$sample_rate*$sample_size*$num_channels; 
    $bytes_summary = 0;
    $d_cycles_summary = 0;
    $i_cycles_summary = 0;
    $first_window_frame=0;
    $max_d_bw = 0;
    $max_i_bw = 0;
    for($frame=0;$frame<$frames;$frame++)
    {
	    $bytes_summary += $bytes[$frame];
	    $playedTime = 1000000*$bytes_summary / $bytes_per_second;

	    $d_cycles_summary += 4*$d_cycles[$frame];
	    $i_cycles_summary += 4*$i_cycles[$frame];
	    if($bytes_summary >= $bytes_per_window)
	    {
		    $avg_d_bw = $d_cycles_summary / $playedTime;
		    $avg_i_bw = $i_cycles_summary / $playedTime; 

		    if($avg_d_bw > $max_d_bw)    {	    
			    $max_d_bw = $avg_d_bw;		    
		    }
		    if($avg_i_bw > $max_i_bw)    {          
			    $max_i_bw = $avg_i_bw;                  
		    }
		    $bytes_summary -= $bytes[$first_window_frame];
		    $d_cycles_summary -= 4*$d_cycles[$first_window_frame];
		    $i_cycles_summary -= 4*$i_cycles[$first_window_frame];
		    $first_window_frame++;
	    }
    }

    printf("I\$ Bus BandWidth:%.2f MB/S\n", $max_i_bw);
    printf("D\$ Bus BandWidth:%.2f MB/S\n", $max_d_bw);
    printf("Bytes per window:    %u\n", $bytes_per_window);
    printf("Bytes per second:   %u\n", $bytes_per_second);

}

if(@ARGV == 0) {
    printf("Usage: [-nframes 44] [-nframes_skip 3]  [-prof_interval 50000] [-border 0.5] ../testvectors/wbps16.rpd\n");
    exit 0;
}

init_vars();
$thing = shift;
while ( $thing ) {
	if ($thing =~ /-nframes_skip/) {
		$nframes_skip=shift;
		printf("nframes_skip=%s\n", $nframes_skip);
	}
	elsif ($thing =~ /-nframes$/) {
		$nframes=shift;
		printf("nframes=%s\n", $nframes);
	}
	elsif ($thing =~ /-border/) {
		$border_condition=shift;
		printf("border_condition=%s\n", $border_condition);
	}
	elsif ($thing =~ /-prof_interval/) {
		$buffer_interval=shift;
		printf("buffer_interval=%s\n", $buffer_interval);
	}
	else {
		$config = $thing;
		printf("File:		%s\n", $config);
	}
	$thing = shift;
}

main();
if($hw_platform == 1){
	calc_hw();
}

