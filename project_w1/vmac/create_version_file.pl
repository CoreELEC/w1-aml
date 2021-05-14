#! /usr/bin/perl

use strict;
use File::Basename;

my $dirname    = dirname(__FILE__);
my $output     = "print_version.c";

my ($sec,$min,$hour,$day,$mon,$year,$weekday,$yeardate,$savinglightday) = (localtime(time));
$sec   =  ($sec  <   10)? "0$sec":$sec;
$min   =  ($min  <   10)? "0$min":$min;
$hour  =  ($hour <   10)? "0$hour":$hour;
$day   =  ($day  <   10)? "0$day":$day;
$mon   =  ($mon  <   9) ? "0".($mon+1):($mon+1);
$year  += 1900;
my $date   =  "$year-$mon-$day $hour:$min:$sec";

chdir($dirname);

my $FW_FILE_INFO = `ls -al ../common/fucode_em4.h`;

my $drv_hash    = "";
my $fw_hash     = "";
my $drv_commit  = `git log -1`;
my $fw_commit   = `git log -1 ../firmware`;
my @FW_FILE_TIME = split / /, $FW_FILE_INFO;

if ($drv_commit =~ m/commit (.*)/){
  $drv_hash = $1;
}

if ($fw_commit =~ m/commit (.*)/){
  $fw_hash = $1;
}

open OUTPUT, ">", "$output" or die "open $output fail";

print OUTPUT "#include \"wifi_hal_com.h\"\n\n";
print OUTPUT "void print_driver_version(void) {\n";
print OUTPUT "    printk(\"driver compile date:$date, driver hash: drv_hash:$drv_hash\\n\");\n";
print OUTPUT "    printk(\"fw compile date: $FW_FILE_TIME[4] $FW_FILE_TIME[5], $FW_FILE_TIME[6], fw hash: fw_hash:$fw_hash\\n\");\n";
print OUTPUT "}\n";

close OUTPUT;
