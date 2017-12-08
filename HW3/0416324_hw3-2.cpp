#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
#define THREAD_CNT 4
#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0

int imgWidth, imgHeight, onethread_height;
int FILTER_SIZE;
int FILTER_SCALE;
int *filter_GX;
int *filter_GY;
sem_t binary_semaphore;
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
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;
inline unsigned char RGB2grey(int w, int h)
{
	int tmp2=3*(h*imgWidth+w) ;
	int tmp =(pic_in[tmp2+MYRED]+pic_in[tmp2+MYGREEN]+pic_in[tmp2+MYBLUE])/3;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

inline unsigned char sobel_filter(int w, int h)
{
	register int tmp=0,tmp2=0,tmp3=0;
	register int a=0, b=0;
	int img_x=0,img_y=0;
	int filter_border = (int)sqrt((float)FILTER_SIZE);
	for (register int j = 0; j<filter_border; j++)
	{
		tmp2=j*filter_border;
		b = h + j - (filter_border / 2);
		tmp3=b*imgWidth;
		for (register int i = 0; i<filter_border; i++)
		{
			a = w + i - (filter_border / 2);
			// detect for borders of the image
			if(a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;
			//inborder, do filter
			img_x += filter_GX[tmp2 + i] * pic_grey[tmp3 + a];
			img_y += filter_GY[tmp2 + i] * pic_grey[tmp3 + a];
		}
	}

	if (img_x<0) img_x=0;
	if (img_y<0) img_y=0;
	return ((int)(sqrt(img_x*img_x+img_y*img_y)))>255?255:(int)(sqrt(img_x*img_x+img_y*img_y));
}
//multithread image processing

inline void* onethread_process_grey(void* args)
{
	long cur_thread=(long)args;
	int upper_bound=cur_thread+1;
	register int tmp;
	if(cur_thread==THREAD_CNT-1)
	{
		for(int i=cur_thread*onethread_height;i<imgHeight;i++)
		{
			tmp=i*imgWidth;
			for(int j=0;j<imgWidth;j++)
			{
				pic_grey[tmp + j] = RGB2grey(j, i);
			}
		}
	}
	else
	{
		for(int i=cur_thread*onethread_height;i<onethread_height*upper_bound;i++)
		{
			tmp=i*imgWidth;
			for(int j=0;j<imgWidth;j++)
			{
				pic_grey[tmp=i*imgWidth + j] = RGB2grey(j, i);
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}
inline void multithread_grey()
{
	int onethread_height = imgHeight / THREAD_CNT; //split by row
	pthread_t thread_id[THREAD_CNT];
	for(long cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		pthread_create(&thread_id[cur_thread],NULL,onethread_process_grey,(void *) cur_thread);
	}
	for(long  cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		//sem_trywait(&binary_semaphore); //try lock if not locked
		pthread_join(thread_id[cur_thread],NULL);
		//sem_post(&binary_semaphore);
	}
}
inline void* onethread_process_sobel(void* args)
{

	long cur_thread=(long)args;
	int upper_bound=cur_thread+1;
	register int tmp;
	if(cur_thread==THREAD_CNT-1)
	{
		for(int i=cur_thread*onethread_height;i<imgHeight;i++)
		{
			tmp=i*imgWidth;
			for(int j=0;j<imgWidth;j++)
			{
				pic_blur[tmp + j]=sobel_filter(j/*col*/,i/*row*/);
			}
		}
	}
	else
	{
		for(int i=cur_thread*onethread_height;i<onethread_height*upper_bound;i++)
		{
			tmp=i*imgWidth;
			for(int j=0;j<imgWidth;j++)
			{
				pic_blur[tmp + j]=sobel_filter(j/*col*/,i/*row*/);
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}
inline void multithread_sobel()
{
	int onethread_height = imgHeight / THREAD_CNT; //split by row
	pthread_t thread_id[THREAD_CNT];

	for(long  cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		pthread_create(&thread_id[cur_thread],NULL,onethread_process_sobel,(void *) cur_thread);

	}
	for(long  cur_thread=0;cur_thread<THREAD_CNT;cur_thread++)
	{
		//sem_trywait(&binary_semaphore); //try lock if not locked
		pthread_join(thread_id[cur_thread],NULL);
		cout<<999<<endl;
		//sem_post(&binary_semaphore);
	}

}
int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	//fscanf(mask, "%d", &FILTER_SCALE);

	filter_GX = new int[FILTER_SIZE];
	filter_GY = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_GX[i]);

	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_GY[i]);


	fclose(mask);
	register int tmp3,tmp4;
	BmpReader* bmpReader = new BmpReader();
	for (register int k = 0; k<5; k++)
	{
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		tmp3=imgWidth*imgHeight;
		pic_grey = (unsigned char*)calloc(tmp3,sizeof(unsigned char));
		pic_blur = (unsigned char*)calloc(tmp3,sizeof(unsigned char));
		pic_final = (unsigned char*)calloc((tmp3<<1)+tmp3,sizeof(unsigned char));
		//sem_init(&binary_semaphore,0/*only one process in this hw*/,6);
		//open lock binary semaphore to let one process access the critical section at atime
		multithread_grey();
		multithread_sobel();

		for (register int j = 0; j<imgHeight; j++)
		{
			tmp3=j*imgWidth;
			for (register int i = 0; i<imgWidth; i++)
			{
				tmp4=3 * (tmp3 + i);
				pic_final[tmp4 + MYRED] = pic_blur[tmp3 + i];
				pic_final[tmp4 + MYGREEN] = pic_blur[tmp3 + i];
				pic_final[tmp4 + MYBLUE] = pic_blur[tmp3 + i];
			}
		}

		// write output BMP file
		bmpReader->WriteBMP(outputBlur_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_blur);
		free(pic_final);
	}

	return 0;
}
