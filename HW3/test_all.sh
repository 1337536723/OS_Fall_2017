 #!/bin/bash
g++ -std=c++11 -pthread 0416324_hw3-1.cpp
#g++ -std=c++11 -pthread example_hw3-1.cpp

g++ -std=c++11 -pthread 0416324_hw3-2.cpp

./a.out
#gprof ./a.out gmon.out -p
./MAE.out ANS/Sobel1.bmp Sobel1.bmp
./MAE.out ANS/Sobel2.bmp Sobel2.bmp
./MAE.out ANS/Sobel3.bmp Sobel3.bmp
./MAE.out ANS/Sobel4.bmp Sobel4.bmp
./MAE.out ANS/Sobel5.bmp Sobel5.bmp

./Speed.sh

./a.out
#gprof ./a.out gmon.out -p
./MAE.out ANS/Blur1.bmp Blur1.bmp
./MAE.out ANS/Blur2.bmp Blur2.bmp
./MAE.out ANS/Blur3.bmp Blur3.bmp
./MAE.out ANS/Blur4.bmp Blur4.bmp
./MAE.out ANS/Blur5.bmp Blur5.bmp

./Speed.sh
