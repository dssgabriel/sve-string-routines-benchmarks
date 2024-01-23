#!/bin/bash

BIN_NAME=build/bench-sve-string-routines

$BIN_NAME --memcmp > results/raw/memcmp.dat
$BIN_NAME --strchr > results/raw/strchr.dat
$BIN_NAME --strcmp > results/raw/strcmp.dat
$BIN_NAME --strcpy > results/raw/strcpy.dat
$BIN_NAME --strlen > results/raw/strlen.dat
$BIN_NAME --strncmp > results/raw/strncmp.dat
$BIN_NAME --strnlen > results/raw/strnlen.dat
$BIN_NAME --strrchr > results/raw/strrchr.dat
