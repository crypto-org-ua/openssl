#! /usr/bin/perl

use strict;
use warnings;
use OpenSSL::Test qw/:DEFAULT top_dir/;

setup("test_dstu");

$ENV{OPENSSL_ENGINES} =
    $ENV{BIN_D} ? top_dir($ENV{BIN_D}) : top_dir("engines", "uadstu");

plan tests => 1;
ok(run(test(["dstutest"])), 'running dstutest');
