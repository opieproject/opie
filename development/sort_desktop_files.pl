#!/usr/bin/perl -w
# Copyright (c) 2002,2003 by 
# Bruno Rodrigues <bruno.rodrigues@litux.org>
# under the GNU GPL license 2.0 or newer
# sort_desktop_files.pl,v 1.1.2.1 2003/07/01 02:22:56 davipt Exp
# -----------------------------------------------------
# This script reads a .desktop file and sorts its values
# in the following format:
# [Desktop Entry]
# field1=val1  \
# field2=val2   > Sorted by field name
# ...          /
# Name=name
# Comment=comment
# Name[lang1]=...    \
# Comment[lang1]=...  > Sorted by language name
# Name[lang2]=...    /

use strict;
use vars qw{$line %data %tr};

%data = (); %tr = (); 

# Read first line and make sure it's "[Desktop Entry]"
$line = <>; chop($line);
die "E: File does not start with [Desktop Entry] ($line)\n" unless $line =~ /^\[Desktop Entry\]$/;

my $end=0;
while($line = <>) {
	# Ignore fields without values
	next if $line =~ /^\s*([^\s]+)\s*=\s*$/; 

	# Ignore empty line in the end
	if($line =~ /^\s*$/) { 
		if($end==0) { $end=1; next;
		} else { die "E: Empty line in middle of file\n";
		}
	}

	# Die if line is not "field = value"
	die "E: Error in line - not field=value ($line)\n" unless $line =~ /^\s*([^\s]+)\s*=\s*(.+?)\s*$/;

	my ($key, $data) = ($1, $2);

	# Grab Name[lang] and Comment[lang]
	if($key =~ /^(Name|Comment)\[(.+)\]$/i) {
		$tr{$2}{$1} = $data;
	} 
	# Grab Name and Comment
	elsif($key =~ /^(Name|Comment)$/i) {
		$tr{"0"}{$1} = $data;
	} 
	# Die if there is other field[x]
	elsif($key =~ /\[|\]/) {
		die "E: Error in line - unknown field with [] ($line)\n";
	} 
	# Grab regular fields
	else {
		$data{$1} = $2;
	}
}


print "[Desktop Entry]\n";
foreach my $key (sort keys %data) {
	print "$key=".$data{$key}."\n";
}
foreach my $key (sort keys %tr) {
	my %tr2 = %{$tr{$key}};
	foreach my $k2 (reverse sort keys %tr2) {
		print $k2;
		print "[$key]" unless $key eq "0";
		print "=". $tr2{$k2}. "\n";
	}
}

