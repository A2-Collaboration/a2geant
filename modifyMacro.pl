#!/usr/bin/perl
use warnings;
use strict;

die "Supply regex replacement as argument." unless @ARGV == 1;
my $regex = $ARGV[0];
die "Regex replacement should start with s." unless $regex =~ m/^s/;

my $n_lines_changed = 0;
while(my $line = <STDIN>) {
  $n_lines_changed++ if eval '$line =~ '.$regex;
  die $@ if $@;
  print $line;
}

die "$n_lines_changed > 1 Lines were changed." if $n_lines_changed > 1;
