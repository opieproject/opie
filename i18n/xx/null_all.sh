#!/bin/sh
echo "this script adds 'FOO' everywhere"
perl -pi -e "s/<translation.*/<translation>FOO<\/translation>/" *.ts
echo "all strings replaced with 'FOO'"
