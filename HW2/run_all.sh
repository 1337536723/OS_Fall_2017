#!/bin/bash
gpp="g++"
gpp_flags="-std=c++11"
$gpp $gpp_flags 0416324_hw2-1.cpp -o p1
$gpp $gpp_flags 0416324_hw2-2.cpp -o p2
$gpp $gpp_flags 0416324_hw2-3.cpp -o p3
$gpp $gpp_flags 0416324_hw2-4.cpp -o p4

./p1 && ./p2 && ./p3 && ./p4
