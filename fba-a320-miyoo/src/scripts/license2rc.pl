#!/usr/bin/perl -w

use strict;

my $Outfile;
my $Infile;

# Process command line arguments
for ( my $i = 0; $i < scalar @ARGV; $i++ ) {{

	# Output file
	if ( $ARGV[$i] =~ /^-o/i ) {
		if ( $ARGV[$i] =~ /^-o$/i ) {
			$i++;
			if ( $i < scalar @ARGV ) {
				$Outfile = $ARGV[$i];
			}
		} else {
			$ARGV[$i] =~ /(?<=-o)(.*)/i;
			$Outfile = $1;
		}
		next;
	}

	$Infile  = $ARGV[$i];
	next;
}}

unless ( $Outfile and $Infile ) {
	die "Usage: $0 -o <output file> <input file>\n\n";
}

open( INFILE, $Infile ) or die "\nError: Couldn't open INPUT file $Infile $!";
open( OUTFILE, ">$Outfile" ) or die "\nError: Couldn't open OUTPUT file $Outfile $!";

print "Generating $Outfile...\n";

print OUTFILE << "ENDRC";
#ifdef APSTUDIO_INVOKED
 #error This file cannot be edited using the Visual C resource editor
#endif

STRINGTABLE DISCARDABLE
BEGIN
ENDRC

printf OUTFILE "\tID_LICENSE " . '"';

while ( my $line = <INFILE> ) {

	# kill line endings (Cygin perl has problems with chomp)
	$line =~ s/[\r\n]//gm;

	# Escape quotes and backslashes
	$line =~ s/([\"\\])/$1$1/g;

	printf OUTFILE "$line" . '\\n';
}

print OUTFILE '"' . "\nEND\n";

close( OUTFILE );
close( INFILE );
