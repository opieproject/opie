#!/usr/bin/perl -w

use strict;

my %depends;
my %tokenpath;

sub getdepends ($$)
{
	my ($token, $path, $intoken);
	$token = shift || return;
	$token =~ s/CONFIG_//;
	$path = shift || return;
#print "opening $path/config.in\n";
#	print "token is $token\n";
	open( FILE, "< $path/config.in" ) || return;
	$intoken = 0;
	while( <FILE> ) {
		if( $intoken == 1 ) {
			/depends\S*(.*)/ && return $1;
			if( /\S*(config|menu)/ ) {
				$intoken = 0;
				return;
			}
		} else {
			/$token/ || next;
			$intoken = 1;
		}
		/$token/ || next;
	}
	close( FILE );
	return;
}

sub makedepends ($$)
{
	my ($depends, $tokenpath, $token, $depword, $mustbesep, $state);
	$depends = shift || return;
	$tokenpath = shift || return;
	$mustbesep = 0;

	for $token (keys %$depends){
		print ${$tokenpath}{$token} . " : ";
		for (split(/\s+/, ${$depends}{$token})){
			/^\s+$/ && next;
			/\&\&/ && next;
			/\|\|/ && next;
			/^$/ && next;
			if(defined(${$tokenpath}{"CONFIG_" . $_})){
				print '$(if $(CONFIG_' . $_ . '),' . ${$tokenpath}{"CONFIG_" . $_} . ') ';
			} else {
				print STDERR "ERROR in dependency generation, unable to locate path for token CONFIG_$_\n";
			}
		}
		print "\n";
	}
}

while( <> ) {
	my $dep;
	my ($token, $path, $pro);
	chomp;
	s/^\s*//g;
	s/\s*$//g;
	($token, $path, $pro) = split(/\s+/,$_);
	$tokenpath{$token} = $path;
	$dep = getdepends($token, $path);
	if( $dep ) {
		$depends{$token} = $dep;
	}
}

makedepends(\%depends, \%tokenpath);
