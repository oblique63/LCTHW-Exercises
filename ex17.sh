#!/bin/sh

# Testing script for exercise 17

set -e

echo "Compiling code"
rm -f ex17
make ex17

echo "\nCreating Database"
./ex17 db.dat c 100 100

echo "\nSetting Records"
./ex17 db.dat s 1 foo foo@bar.com
./ex17 db.dat s 2 cow cow@moo.com
./ex17 db.dat s 3 test test@testing.com
./ex17 db.dat s 4 cat cat@meow.com

echo "\nGetting record 3"
./ex17 db.dat g 3

echo "\nDeleting record 3"
./ex17 db.dat d 3

echo "\nListing database contents"
./ex17 db.dat l

echo "\nFind record with 'cat'"
./ex17 db.dat f cat

echo "\nFind record with 'foo@bar.com'"
./ex17 db.dat f foo@bar.com
