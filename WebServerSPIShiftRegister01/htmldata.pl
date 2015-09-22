#!/usr/bin/env perl
#
# HTMLファイルからhtmldata.hを生成します。
# handle_send_XXXX という関数を呼び出すとレスポンスを返します。
#  
use feature qw( say );
use utf8;
use strict;
use warnings;
binmode(STDIN, ':utf8');
binmode(STDOUT,':utf8');
binmode(STDERR,':utf8');
 
use FindBin;
use lib "$FindBin::Bin/local/lib/perl5";
 
use IO::All;
use File::Basename;

my $data="#define CONFIG_HTML_DATA ";

sub html2header {
	my ($contenttype,$filename)=@_;
	my $buf=io($filename)->utf8->slurp;
	$buf=~s/\\/\\\\/g;
	$buf=~s/\r?\n/\\n/g;
	$buf=~s/"/\\"/g;

	my $varname=basename($filename);
	$varname=~s/\./_/g;
	$varname=~s/-/_/g;

	my $code=qq{void handle_send_$varname\(void)\{server.send(200,"$contenttype","$buf");\};};

	$data.=$code;

	say "handle_send_$varname();";
}

html2header('text/html','index.html');

io('htmldata.h')->utf8->print($data."\n");

