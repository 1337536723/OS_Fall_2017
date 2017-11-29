#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
#define THREAD_CNT 6
#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0
int imgWidth, imgHeight, onethread_height, FILTER_BORDER;
int FILTER_SIZE;
int FILTER_SCALE;
int *filter_G;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

const char *inputfile_name[5] =
{
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputBlur_name[5] =
{
	"Blur1.bmp",
	"Blur2.bmp",
	"Blur3.bmp",
	"Blur4.bmp",
	"Blur5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final, *pic_padding;
inline unsigned char RGB2grey(int w, int h) __attribute__((optimize("-O3")));
inline unsigned char RGB2grey(int w, int h)
{
	int tmp =	(pic_padding[3*(h*(imgWidth+2*(FILTER_BORDER/2))+w)+MYRED]+
				pic_padding[3*(h*(imgWidth+2*(FILTER_BORDER/2))+w)+MYGREEN]+
				pic_padding[3*(h*(imgWidth+2*(FILTER_BORDER/2))+w)+MYBLUE])/3;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}
inline unsigned char GaussianFilter(int w, int h) __attribute__((optimize("-O3")));
inline unsigned char GaussianFilter(int w, int h)
{
	//w col, h row
	int tmp = 0;
	int a, b;
	int ws = (int)sqrt((float)FILTER_SIZE);
	for (int j = 0; j<ws; j++)
		for (int i = 0; i<ws; i++)
		{
			a = w + i - (ws / 2);
			b = h + j - (ws / 2);
			tmp += filter_G[j*ws + i] * pic_grey[b*imgWidth + a];
		}
	tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}
//multithread image processing
inline void* onethread_process_grey(void* args) __attribute__((optimize("-O3")));
inline void* onethread_process_grey(void* args)
{
	long cur_thread=(long)args;

	int upper_bound=cur_thread+1;
	//last one, uneven divide
	if(cur_thread==THREAD_CNT-1)
	{
		for(int i=cur_thread*onethread_height;i<(imgHeight+2*(FILTER_BORDER/2));i++)
		{
			for(int j=0;j<(imgWidth+2*(FILTER_BORDER/2));j++)
			{
				pic_grey[i*(imgWidth+2*(FILTER_BORDER/2))+j] = RGB2grey(j, i);
			}
		}
	}
	else
	{
		for(int i=cur_thread*onethread_height;i<onethread_height*upper_bound;i++)
		{
			for(int j=0;j<imgWidth+2*(FILTER_BORDER/2);j++)
			{
				pic_grey[i*(imgWidth+2*(FILTER_BORDER/2))+j] = RGB2grey(j, i);
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}
inline void multithread_grey() __attribute__((optimize("-O3")));
inline void multithread_grey()
{
	onethread_height=(imgHeight+2*(FILTER_BORDER/2))/THREAD_CNT; //split by row
	pthread_t thread_id[THREAD_CNT];
	for(long cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		pthread_create(&thread_id[cur_thread],NULL,onethread_process_grey,(void *) cur_thread);
	}
	for(long  cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		pthread_join(thread_id[cur_thread],NULL);
	}

}
inline void* onethread_process_gaussian(void* args) __attribute__((optimize("-O3")));
inline void* onethread_process_gaussian(void* args)
{
	long cur_thread=(long)args;

	int upper_bound=cur_thread+1;
	//last one, uneven divide
	if(cur_thread==THREAD_CNT-1)
	{
		for(int i=cur_thread*onethread_height;i<(imgHeight+2*(FILTER_BORDER/2));i++)
		{
			for(int j=0;j<imgWidth+2*(FILTER_BORDER/2);j++)
			{
				pic_blur[i*(imgWidth+2*(FILTER_BORDER/2))+j]=GaussianFilter(j/*col*/,i/*row*/);
			}
		}
	}
	else
	{
		for(int i=cur_thread*onethread_height;i<onethread_height*upper_bound;i++)
		{
			for(int j=0;j<imgWidth+2*(FILTER_BORDER/2);j++)
			{
				pic_blur[i*(imgWidth+2*(FILTER_BORDER/2))+j]=GaussianFilter(j/*col*/,i/*row*/);
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}
inline void multithread_gaussian() __attribute__((optimize("-O3")));
inline void multithread_gaussian()
{
	onethread_height=(imgHeight+2*(FILTER_BORDER/2))/THREAD_CNT; //split by row
	pthread_t thread_id[THREAD_CNT];
	for(long  cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		pthread_create(&thread_id[cur_thread],NULL,onethread_process_gaussian,(void *) cur_thread);
	}
	for(long  cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		pthread_join(thread_id[cur_thread],NULL);
	}

}
int main() __attribute__((optimize("-O3")));
int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Gaussian.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	fscanf(mask, "%d", &FILTER_SCALE);

	filter_G = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_G[i]);
	fclose(mask);

	FILTER_BORDER = (int)sqrt(FILTER_SIZE);

	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++)
	{
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc((imgWidth+2*(FILTER_BORDER/2))*(imgHeight+2*(FILTER_BORDER/2))*sizeof(unsigned char));
		pic_blur = (unsigned char*)malloc((imgWidth+2*(FILTER_BORDER/2))*(imgHeight+2*(FILTER_BORDER/2))*sizeof(unsigned char));
		pic_padding = (unsigned char*)malloc(3*(imgWidth+2*(FILTER_BORDER/2))*(imgHeight+2*(FILTER_BORDER/2))*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3*(imgWidth)*(imgHeight)*sizeof(unsigned char));
		//printf("Padded imgWidth %d imgHeight %d \n",imgWidth+2*(FILTER_BORDER/2),imgHeight+2*(FILTER_BORDER/2));
		//padding the pic for fater convolution algorithm
		for(int i=0;i<imgHeight+2*(FILTER_BORDER/2);i+=3)
		{
			for(int j=0;j<imgWidth+2*(FILTER_BORDER/2);j+=3)
			{
				if((i>=FILTER_BORDER/2)&&(i<(FILTER_BORDER/2)+imgHeight)&&(j>=FILTER_BORDER/2)&&(j<(FILTER_BORDER/2)+imgWidth))
				{
					pic_padding[i*(imgWidth+2*(FILTER_BORDER/2))+j+MYRED]=pic_in[(i-FILTER_BORDER/2)*(imgWidth)+(j-FILTER_BORDER/2)+MYRED];
					pic_padding[i*(imgWidth+2*(FILTER_BORDER/2))+j+MYGREEN]=pic_in[(i-FILTER_BORDER/2)*(imgWidth)+(j-FILTER_BORDER/2)+MYGREEN];
					pic_padding[i*(imgWidth+2*(FILTER_BORDER/2))+j+MYBLUE]=pic_in[(i-FILTER_BORDER/2)*(imgWidth)+(j-FILTER_BORDER/2)+MYBLUE];
				}
				else
				{
					pic_padding[i*(imgWidth+2*(FILTER_BORDER/2))+j+MYRED]=0;
					pic_padding[i*(imgWidth+2*(FILTER_BORDER/2))+j+MYGREEN]=0;
					pic_padding[i*(imgWidth+2*(FILTER_BORDER/2))+j+MYBLUE]=0;
				}
				//printf("i%d j%d\n",i,j);
			}
		}
		/*}
		for(int i=0;i<imgHeight+2*(FILTER_BORDER/2);i++)
		{
			for(int j=0;j<imgWidth+2*(FILTER_BORDER/2);j++)
			{
				cout<<(unsigned int)pic_padding[i*(imgWidth+2*(FILTER_BORDER/2))+j]<<" ";
			}
			cout<<endl<<endl;
			//getchar();
		}*/
		pthread_mutex_lock(&mutex1);
		multithread_grey();
		//printf("hello");
		multithread_gaussian();
		//printf("hello");
		pthread_mutex_unlock(&mutex1);

		//getchar();
		//modified the writing back algorithm, crop the "REAL" image part without the padding part
		for(int i=0;i<imgHeight+2*(FILTER_BORDER/2);i++)
		{
			for(int j=0;j<imgWidth+2*(FILTER_BORDER/2);j++)
			{
				if((i>=FILTER_BORDER/2)&&(i<(FILTER_BORDER/2)+imgHeight)&&(j>=FILTER_BORDER/2)&&(j<(FILTER_BORDER/2)+imgWidth))
				{
					pic_final[3*((i-FILTER_BORDER/2)*(imgWidth)+(j-FILTER_BORDER/2))+MYRED]=pic_blur[i*(imgWidth+2*(FILTER_BORDER/2))+j];
					pic_final[3*((i-FILTER_BORDER/2)*(imgWidth)+(j-FILTER_BORDER/2))+MYGREEN]=pic_blur[i*(imgWidth+2*(FILTER_BORDER/2))+j];
					pic_final[3*((i-FILTER_BORDER/2)*(imgWidth)+(j-FILTER_BORDER/2))+MYBLUE]=pic_blur[i*(imgWidth+2*(FILTER_BORDER/2))+j];
				}
			}
		}

		// write output BMP file
		bmpReader->WriteBMP(outputBlur_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_blur);
		free(pic_final);
		free(pic_padding);
	}

	return 0;
}
