 #!/bin/bash
 g++ -std=c++11 -pthread 0416324_hw3-1.cpp
 ./a.out
./MAE.out ANS/Blur1.bmp Blur1.bmp
./MAE.out ANS/Blur2.bmp Blur2.bmp
./MAE.out ANS/Blur3.bmp Blur3.bmp
./MAE.out ANS/Blur4.bmp Blur4.bmp
./MAE.out ANS/Blur5.bmp Blur5.bmp

./Speed.sh
