#!/usr/bin/perl -w
eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
if 0; #$running_under_some_shell

use strict;
use File::Find;

my $packages="";
open(FILE, "<./packages");
while(<FILE>){$packages.=$_;}
close(FILE);

my ($dirname,$dir,$cfg,$filename,$tagname,$name,$caps,$pre,$post,$sources,@dupecheck);
$cfg = shift || die;
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
	$pre = "menu \"$tagname\"\n";
	$post = "endmenu\n";
}

open(CFG, ">$cfg") || die "Unable to open $cfg for writing\n";
select(CFG);
print $pre;
@dupecheck=();
File::Find::find({wanted => \&wanted}, $dirname);
print $post;
select(STDOUT);
close(CFG);

exit;

open(FILE,">$dir/config.in");
select(FILE);
print "menu \"$name\"\n";
print "\n";
my @subdirs=();
my @dirs;
foreach(grep(/^$dir/, @dirs)){
	chomp;
	/^$dir\/$name.pro$/ && next;
	my $localdir=$_;
	if($dir=~m,^$localdir$,){
		next;
	}
#($locadir=$_)~s,/[^/]+$,,g;
	if($localdir=~/^\.$/){next;}
	if(grep(/^$localdir$/, @subdirs)){next;}
	my $nslashes = $localdir =~ tr!/!!;
	my $dirnslashes = $dir =~ tr!/!!;
	$dirnslashes++;
	if($dirnslashes != $nslashes ){next;}
	print STDERR "$localdir/config.in\n";
	print "  source $localdir/config.in\n";
	push(@subdirs, $localdir);
	print "endmenu\n";
	select(STDOUT);
	close(FILE);
}

use vars qw/*name *dir *prune/;
*name   = *File::Find::name;
*dir    = *File::Find::dir;
*prune  = *File::Find::prune;
	
sub wanted {
	if( /config.in/s ) {
		if(grep(/^$File::Find::dir\/config.in$/, @dupecheck)){
			return;
		}
		my $nslashes = $File::Find::dir =~ tr!/!!;
		my $dirnslashes = $dirname =~ tr!/!!;
		$dirnslashes++;
#		print STDERR "dirnslashes is $dirnslashes\n";
#		print STDERR "nslashes is $nslashes\n";
		if($dirnslashes != $nslashes){return;}
		print "  source " . $File::Find::dir . "/config.in\n";
		push(@dupecheck, $File::Find::dir . "/config.in");
	}
}
