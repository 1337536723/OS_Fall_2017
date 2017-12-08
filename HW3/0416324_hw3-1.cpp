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
int imgWidth, imgHeight, onethread_height, filter_border;
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

unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;
inline unsigned char RGB2grey(int w, int h)
{
	int tmp2=3 * (h*imgWidth + w) ;
	int tmp =(pic_in[tmp2+MYRED]+pic_in[tmp2+MYGREEN]+pic_in[tmp2+MYBLUE])/3;
	return tmp&0xFF;
}
inline unsigned char GaussianFilter(int w, int h)
{
	register int tmp=0,tmp2,tmp3;
	register int a, b;
	if(w>=filter_border&&w<imgWidth-filter_border&&h>=filter_border&&h<imgHeight-filter_border)
	{
		for (register int j = 0; j<filter_border; ++j)
		{
			tmp2=j*filter_border;
			b = h + j - (filter_border >>1);
			tmp3=b*imgWidth;
			for (register int i = 0; i<filter_border; ++i)
			{
				a = w + i - (filter_border >>1);
				// detect for borders of the image
				tmp += filter_G[tmp2 + i] * pic_grey[tmp3 + a];
			}
		}
	}
	else
	{
		for (register int j = 0; j<filter_border; ++j)
		{
			tmp2=j*filter_border;
			b = h + j - (filter_border >>1);
			tmp3=b*imgWidth;
			for (register int i = 0; i<filter_border; ++i)
			{
				a = w + i - (filter_border >>1);
				// detect for borders of the image
				if(a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;
				//inborder, do filter
				tmp += filter_G[tmp2 + i] * pic_grey[tmp3 + a];
			}
		}
	}
	tmp /= FILTER_SCALE;
	return tmp&0xFF;
}
//multithread image processing
inline void* onethread_process_grey(void* args)
{
	long cur_thread=(long)args;
	int upper_bound=cur_thread+1;
	register int tmp;
	pthread_mutex_lock(&mutex1);
	if(!(cur_thread^THREAD_CNT-1))
	{
		for(register int i=cur_thread*onethread_height;i<imgHeight;++i)
		{
			tmp=i*imgWidth;
			for(register int j=0;j<imgWidth;++j)
			{
				pic_grey[tmp + j] = RGB2grey(j, i);
			}
		}
	}
	else
	{
		for(register int i=cur_thread*onethread_height;i<onethread_height*upper_bound;++i)
		{
			tmp=i*imgWidth;
			for(register int j=0;j<imgWidth;++j)
			{
				pic_grey[tmp + j] = RGB2grey(j, i);
			}
		}
	}
	pthread_mutex_unlock(&mutex1);
	pthread_exit(EXIT_SUCCESS);
}
inline void multithread_grey()
{
	int onethread_height = imgHeight / THREAD_CNT; //split by row
	pthread_t thread_id[THREAD_CNT];
	for(int cur_thread=0;cur_thread<THREAD_CNT;++cur_thread)
	{
		pthread_create(&thread_id[cur_thread],NULL,onethread_process_grey,(void *) cur_thread);
	}
	for(int cur_thread=0;cur_thread<THREAD_CNT;++cur_thread)
	{
		pthread_join(thread_id[cur_thread],NULL);
	}
}
inline void* onethread_process_gaussian(void* args)
{
	long cur_thread=(long)args;
	int upper_bound=cur_thread+1;
	register int tmp;
	pthread_mutex_lock(&mutex1);
	if(!(cur_thread^THREAD_CNT-1))
	{
		for(register int i=cur_thread*onethread_height;i<imgHeight;++i)
		{
			tmp=i*imgWidth;
			for(register int j=0;j<imgWidth;++j)
			{
				pic_blur[tmp + j]=GaussianFilter(j/*col*/,i/*row*/);
			}
		}
	}
	else
	{
		for(register int i=cur_thread*onethread_height;i<onethread_height*upper_bound;++i)
		{
			tmp=i*imgWidth;
			for(register int j=0;j<imgWidth;++j)
			{
				pic_blur[tmp + j]=GaussianFilter(j/*col*/,i/*row*/);
			}
		}
	}
	pthread_mutex_unlock(&mutex1);
	pthread_exit(EXIT_SUCCESS);
}
inline void multithread_gaussian()
{
	int /*onethread_width = imgWidth / THREAD_CNT, */onethread_height = imgHeight / THREAD_CNT; //split by row
	pthread_t thread_id[THREAD_CNT];
	for(int cur_thread=0;cur_thread<THREAD_CNT;++cur_thread)
	{
		pthread_create(&thread_id[cur_thread],NULL,onethread_process_gaussian,(void *) cur_thread);
	}
	for(int cur_thread=0;cur_thread<THREAD_CNT;++cur_thread)
	{

		pthread_join(thread_id[cur_thread],NULL);

	}
}
int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Gaussian.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	fscanf(mask, "%d", &FILTER_SCALE);

	filter_G = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; ++i)
		fscanf(mask, "%d", &filter_G[i]);
	fclose(mask);

 	register int tmp3,tmp4,tmp5;
	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++)
	{
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		tmp3=imgWidth*imgHeight;
		pic_grey = (unsigned char*)calloc(tmp3,sizeof(unsigned char));
		pic_blur = (unsigned char*)calloc(tmp3,sizeof(unsigned char));
		pic_final = (unsigned char*)calloc((tmp3<<1)+tmp3,sizeof(unsigned char));
		filter_border = (int)sqrt((float)FILTER_SIZE);
		multithread_grey();
		multithread_gaussian();
		for (register int j = 0; j<imgHeight; ++j)
		{
			tmp3=j*imgWidth;
			for (register int i = 0; i<imgWidth; ++i)
			{
				tmp5=tmp3+i;
				tmp4=3 * (tmp5);
				pic_final[tmp4 + MYRED] = pic_blur[tmp5];
				pic_final[tmp4 + MYGREEN] = pic_blur[tmp5];
				pic_final[tmp4 + MYBLUE] = pic_blur[tmp5];
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
/* pthread_create usage
int pthread_create(pthread_t *thread,
			  const pthread_attr_t *restrict_attr,
			  void*(*start_rtn)(void*),
			  void *restrict arg);
*/
