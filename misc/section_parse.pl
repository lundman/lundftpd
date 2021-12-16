#!/usr/bin/perl -n

# Sample log line
# Fri May 28 10:32:48 2004 RETR mrpink DiVX (none) 1511 0.49 divx/The.Butterfly.Effect.DVDRip.XviD-DMT/imdb.nfo

/^\w+\s+\w+\s+\d+\s+[:\d]+\s+\d+\s+([A-Z]+)\s+(\S+)\s+(.+)\s+([\(\)\w]+)\s+(\d+)\s+([\.\d]+)\s+.+$/
    and do {
  #print "CMD=$1\n";
  #print "User=$2\n";
  #print "Section=$3\n";
  #print "Group=$4\n";
  #print "Size=$5\n";
  #print "Speed=$6\n";
  #print "\n";
	$S{$3}{$1} += $5;
	$U{$2}{$1} += $5;
    };

END {

    for ( keys %S ) {
	$S{$_}{RATIO} = $S{$_}{STOR} > 0 ? $S{$_}{RETR}/$S{$_}{STOR} : 0;
    }

    
    print "Sorted by download\n";
    printf ("%12.12s  %12s  %12s  %12s \n",
	    "Section",
	    "Download",
	    "Upload",
	    "Ratio");

    for ( sort { $S{$b}{RETR} <=> $S{$a}{RETR} } keys %S ) {
	printf ("%12.12s  %12s  %12s  %12.2f \n",
		$_,
		$S{$_}{RETR},
		$S{$_}{STOR},
		$S{$_}{RATIO});
    }


    print "\n\nSorted by upload\n";
    printf ("%12.12s  %12s  %12s  %12s \n",
	    "Section",
	    "Download",
	    "Upload",
	    "Ratio");

    for ( sort { $S{$b}{STOR} <=> $S{$a}{STOR} } keys %S ) {
	printf ("%12.12s  %12s  %12s  %12.2f \n",
		$_,
		$S{$_}{RETR},
		$S{$_}{STOR},
		$S{$_}{RATIO});
    }



    print "\n\nSorted by ratio\n";
    printf ("%12.12s  %12s  %12s  %12s \n",
	    "Section",
	    "Download",
	    "Upload",
	    "Ratio");

    for ( sort { $S{$b}{RATIO} <=> $S{$a}{RATIO} } keys %S ) {
	printf ("%12.12s  %12s  %12s  %12.2f \n",
		$_,
		$S{$_}{RETR},
		$S{$_}{STOR},
		$S{$_}{RATIO});
    }


#    print "Users\n";
#    for ( keys %U ) {
#	print "$_\t$U{$_}{RETR}/$U{$_}{STOR}\n";
#    }
}
