#!/bin/sh
echo "this script adds nullstrings everywhere"
perl -pi -e "s/<translation type=\"unfinished\"><\/translation>/<translation> <\/translation>/" *.ts
