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
my $rf_hash     = "";
my $drv_commit  = `git log -1`;
my $fw_commit   = `git log -1 ../firmware`;
my $rf_commit   = `git log -1 ../firmware/t9026_wifi_cali`;
my $fw_size   = `stat ../firmware/bin_rom/wifi_fw_w1.bin | grep -w "Size" | awk '{print \$2}'`;
$fw_size=~s/[\n\r]+$//;
my $fw_date   = `stat ../firmware/bin_rom/wifi_fw_w1.bin | grep -w "Modify" | awk '{print \$2 " " \$3}' | cut -d . -f 1`;
$fw_date=~s/[\n\r]+$//;
my $rf_date = `git log --date=iso -1 ../firmware/t9026_wifi_cali | sed -n '3p' | awk '{print \$2 " " \$3}'`;
$rf_date=~s/[\n\r]+$//;

if ($drv_commit =~ m/commit (.*)/){
  $drv_hash = $1;
}

if ($fw_commit =~ m/commit (.*)/){
  $fw_hash = $1;
}

if ($rf_commit =~ m/commit (.*)/){
  $rf_hash = $1;
}

open OUTPUT, ">", "$output" or die "open $output fail";

print OUTPUT "#include \"wifi_hal_com.h\"\n\n";
print OUTPUT "void print_driver_version(void) {\n";
print OUTPUT "    printk(\"driver compile date: $date,driver hash: $drv_hash\\n\");\n";
print OUTPUT "    printk(\"fw compile date: $fw_date,fw hash: $fw_hash,fw size: $fw_size\\n\");\n";
print OUTPUT "    printk(\"rf cali: last commit: $rf_date hash:$rf_hash\\n\");\n";
print OUTPUT "}\n";

close OUTPUT;
