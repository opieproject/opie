#!/bin/sh
echo "this script adds nullstrings everywhere"
perl -pi -e "s/<translation.*/<translation>FOO<\/translation>/" *.ts
