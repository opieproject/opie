#!/usr/bin/perl -w
eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
if 0; #$running_under_some_shell

use strict;
use File::Find;

my $packages="";
open(FILE, "<./packages");
while(<FILE>){$packages.=$_;}
close(FILE);

my ($dirname,$dir,$file,@files,$filename,$tagname,$name,$caps,$pre,$post,$sources,@dupecheck,$basedir);
my @founddirs;
$file = shift || die;
$basedir = shift;

my $cfg=$file;
($dirname=$cfg)=~s,(.*)/(.*),$1,;
($filename=$cfg)=~s,(.*/)(.*),$2,;
($tagname=$dirname)=~s,.*/,,;
($caps=$tagname)=~tr/[a-z]/[A-Z]/;
#print "cfg is $cfg\n";
#print "dir is $dirname\n";
#print "filename is $filename\n";
sub wanted;

if(-e "$dirname/config.in.in"){
	my $contents;
	open(FILE, "<$dirname/config.in.in");
	while(<FILE>){ $contents.=$_; }
	close(FILE);
	if($contents=~/\@sources\@/){
		($post = $contents) =~ s/^.*\@sources\@//s;
		($pre = $contents) =~ s/\@sources\@.*$//s;
	} else {
		$pre = $contents;
	}
} else {
	my ($firstletter, $rest);
	$firstletter = substr($tagname, 0, 1);
	$rest = substr($tagname, 1);
	$firstletter =~ tr/a-z/A-Z/;
	$tagname = $firstletter . $rest;
	$pre = "menu \"$tagname\"\n";
	$post = "endmenu\n";
}

open(CFG, ">$cfg") || die "Unable to open $cfg for writing\n";
select(CFG);
print $pre;
@dupecheck=();
File::Find::find({wanted => \&wanted}, $dirname);
@::founddirs=sort(@::founddirs);
foreach (@::founddirs) {
	if(grep(/^$_\/config.in$/, @dupecheck)){
		next;
	}
	my $nslashes = $_ =~ tr!/!!;
	my $dirnslashes = $dirname =~ tr!/!!;
	$dirnslashes++;
	if($dirnslashes != $nslashes){next;}
	my $reldir;
	if (defined($basedir)) {
		($reldir=$_)=~s,^$basedir/,,;
	}
	print "  source " . $reldir . "/config.in\n";
	push(@dupecheck, $_ . "/config.in");
}
print $post;
select(STDOUT);
close(CFG);
exit;

use vars qw/*name *dir *prune/;
*name   = *File::Find::name;
*dir    = *File::Find::dir;
*prune  = *File::Find::prune;
	
sub wanted {
	if( /config.in$/s ) {
		push(@::founddirs,$File::Find::dir);
	}
}
