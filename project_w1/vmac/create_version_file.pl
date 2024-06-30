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

my $drv_hash    = "";
my $fw_hash     = "";
my $drv_commit  = `git log -1`;
my $fw_commit   = `git log -1 ../firmware`;
my $fw_size   = `stat ../common/fucode_em4.h | grep -w "Size" | awk '{print \$2}'`;
$fw_size=~s/[\n\r]+$//;
my $fw_date   = `stat ../common/fucode_em4.h | grep -w "Modify" | awk '{print \$2 " " \$3}' | cut -d . -f 1`;
$fw_date=~s/[\n\r]+$//;

if ($drv_commit =~ m/commit (.*)/){
  $drv_hash = $1;
}

if ($fw_commit =~ m/commit (.*)/){
  $fw_hash = $1;
}

open OUTPUT, ">", "$output" or die "open $output fail";

print OUTPUT "#include \"wifi_hal_com.h\"\n\n";
print OUTPUT "void print_driver_version(void) {\n";
print OUTPUT "    pr_info(\"driver compile date: $date,driver hash: $drv_hash\\n\");\n";
print OUTPUT "    pr_info(\"fw compile date: $fw_date,fw hash: $fw_hash,fw size: $fw_size\\n\");\n";
print OUTPUT "}\n";

close OUTPUT;
