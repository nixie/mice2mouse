#!/usr/bin/env perl

use strict;
use warnings;

sub deltcor {
    my $last_correct = 0;
    foreach my $line (<STDIN>){
        next if (substr($line,0,1) eq '#');

        my @vals = split / /, $line;
        if ($vals[1] == 1){
            print $vals[0] - $last_correct;
            $last_correct = $vals[0];
        }
    }
}

deltcor(@ARGV);
