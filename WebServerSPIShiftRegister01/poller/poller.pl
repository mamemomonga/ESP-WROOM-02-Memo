#!/usr/bin/env perl
# http://esp8266.local/polling を1秒ごとにGETするスクリプト
use utf8;
use strict;
use warnings;
use feature 'say';

use Furl;
use Time::HiRes;
use JSON::XS;
use Data::Dumper;

my $furl=Furl->new( timeout=>10 );


while(1) {

	my $res=$furl->get('http://esp8266.local/polling');

	next unless ($res->is_success);
	my $content=$res->content;

	say Dumper(JSON::XS->new()->utf8->decode($content));

	Time::HiRes::sleep(1);
}

