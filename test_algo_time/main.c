
/******************************************************************************
*以下为工程部分头文件
******************************************************************************/

#include "fun.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <semaphore.h>  
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "TGFingerVeinApi.h"

#define DAT_ADDR "./FVDB"
#define REG_FINGER_PATH "reg_finger.dat"

#define PTHREAD_NUM 4
#define USER_NUM 3000

#define DEV_TYPE "VM661J"
#define MAX_FV_NUM 100
#define TG_OPEN_OK 0
#define TG_OPEN_FAIL -1
#define TG_NO_FINGER 2

unsigned int IdentifyFinger(void);

void *ComparePthread(void *arg);

int test_tg_get_usr_dat(char* addr,int start_num,int max_num,unsigned char *dat);


pthread_t pth_id[PTHREAD_NUM];

typedef struct 
{
	unsigned char *register_data;
    int user_num;
	int pthread_id;
}CompareData;

unsigned char **allData;


void main(int argc,char **argv)
{
	IdentifyFinger();
	return ;
}


/******************************************************************************
*以下为验证子函数
******************************************************************************/
/*
unsigned int IdentifyFinger(void)
{
	long l1,l2;
	struct timeval tv;
	int ret;
	int usr_num = 0;
	int  sizeFeature3 =FEATURE_SIZE_X3, loc;

	unsigned char * feature;
	unsigned char * tempUpdateData;
	unsigned char * registerData;
	stu_usr *usr_addr = NULL;

	feature = (unsigned char *)malloc((IMAGE_ROI_WIDTH*IMAGE_ROI_HEIGHT) * sizeof(unsigned char));
	tempUpdateData = (unsigned char *)malloc(sizeFeature3 * sizeof(unsigned char));

	usr_num = tg_get_usr_num(DAT_ADDR);
	printf("user num = %d\n",usr_num);
	registerData = (unsigned char *)malloc(sizeFeature3*usr_num * sizeof(unsigned char));
	usr_addr = (stu_usr *)malloc(usr_num * sizeof(stu_usr));
	gettimeofday(&tv,NULL);
	l1 = tv.tv_sec*1000*1000 + tv.tv_usec;
	tg_get_usr_dat(DAT_ADDR, usr_addr, registerData);
	gettimeofday(&tv,NULL);
	l2 = tv.tv_sec*1000*1000 + tv.tv_usec;
	printf("read time = %ld ms\n",(l2 - l1)/1000);

	//printf("请自然轻放手指\n");
	read_data_hex(feature, FEATURE_SIZE, REG_FINGER_PATH);

	//write_data_hex(registerData, sizeFeature3, "./regFea.dat");
	gettimeofday(&tv,NULL);
	l1 = tv.tv_sec*1000*1000 + tv.tv_usec;
	ret = TGMatchFeature1N(feature, registerData, usr_num, &loc, tempUpdateData);
	gettimeofday(&tv,NULL);
	l2 = tv.tv_sec*1000*1000 + tv.tv_usec;
	printf("compare time = %ld ms\n",(l2 - l1)/1000);

	if (0 == ret)
		printf("compare success, path = %s\n", (usr_addr + loc - 1)->addr);
	else
		printf("compare fail\n");

	free(feature);
	feature = NULL;
	free(tempUpdateData);
	tempUpdateData = NULL;
	free(registerData);
	registerData = NULL;
	free(usr_addr);
	usr_addr = NULL;
	return 0;
}
*/

unsigned int IdentifyFinger(void)
{
	int i,i_ret;
	long l1,l2;
	struct timeval tv;
	int ret;
	int usr_num = 0;
	int  sizeFeature3 =FEATURE_SIZE_X3;
	
	int pthread_count = 0; 
	int usernum_per_pthread = USER_NUM/PTHREAD_NUM;
	int usernum_last_pthread = USER_NUM%PTHREAD_NUM;

	if(usernum_last_pthread)
		pthread_count++;
	else 
		pthread_count = 0;
	pthread_count += PTHREAD_NUM;
	CompareData temp_compare_data[pthread_count];

	printf("pthread_count = %d,usernum_per_pthread = %d\n",pthread_count,usernum_per_pthread);
	allData = test_Make2DArray_unsigned(pthread_count,sizeFeature3*usernum_per_pthread);
	for(i = 0;i<pthread_count;i++)
	{
		if(i == (pthread_count - 1))
		{
			if(0 == usernum_last_pthread)
	   			temp_compare_data[i].user_num = usernum_per_pthread;
	   		else
				temp_compare_data[i].user_num = usernum_last_pthread;
		}
		else
			temp_compare_data[i].user_num = usernum_per_pthread;
		temp_compare_data[i].pthread_id = i + 1;

		test_tg_get_usr_dat(DAT_ADDR,i*usernum_per_pthread+ 1,temp_compare_data[i].user_num,*(allData+i));
		temp_compare_data[i].register_data = *(allData + i);
//		write_data_hex(*(allData+i),temp_compare_data[i].user_num * FEATURE_SIZE_X3,"/temp.dat");
	}


	gettimeofday(&tv,NULL);
	l1 = tv.tv_sec*1000*1000 + tv.tv_usec;
	for(i = 0;i<pthread_count;i++)
	{
		i_ret= pthread_create(pth_id+i, NULL, ComparePthread, temp_compare_data+i);		 
		if(i_ret != 0)		// ��0�򴴽�ʧ��	   		 
			printf("create thread failed.\n"); 		
		else 	   
			printf("create thread %d success.\n",i+1); 					   
	}
	for(i=0;i<pthread_count;i++)
	   pthread_join(*(pth_id+i), NULL);
	gettimeofday(&tv,NULL);
	l2 = tv.tv_sec*1000*1000 + tv.tv_usec;
	printf("all pthread compare time = %ld ms\n",(l2 - l1)/1000);
	test_Free2DArray_unsigned(allData,pthread_count);
	return 0;
}



void *ComparePthread(void *arg)
{
	long l1,l2;
	struct timeval tv;
	int loc;
	int ret;
	
	unsigned char feature[FEATURE_SIZE_X3];
	unsigned char tempUpdateData[FEATURE_SIZE_X3];
	CompareData *compare_data = (CompareData *)arg;
	read_data_hex(feature, FEATURE_SIZE, REG_FINGER_PATH);
	printf("pthread id %d ; pthread user_num = %d\n",compare_data->pthread_id,compare_data->user_num);
	gettimeofday(&tv,NULL);
	l1 = tv.tv_sec*1000*1000 + tv.tv_usec;
	ret = TGMatchFeature1N(feature, compare_data->register_data, compare_data->user_num, &loc, tempUpdateData);
	gettimeofday(&tv,NULL);
	l2 = tv.tv_sec*1000*1000 + tv.tv_usec;

	if (0 == ret)
		printf("pthread %d ; success loc = %d; compare time = %ld ms\n",compare_data->pthread_id,loc,(l2 - l1)/1000);
	else
		printf("pthread %d ; fail            ; compare time = %ld ms\n",compare_data->pthread_id,(l2 - l1)/1000);
}


/*****************************************************************
* function:		test_tg_get_usr_dat
* description:  get the file dat in a dir
* param1:     	char* addr	: path of dir	(input)
* param2:     	int start_num	: start dat num	(input)
* param3:     	int max_num	: the num of user	(input)
* param3:     	unsigned char *dat	: pointer of usr dat	(output)
*
* return:    	1	: succeed 
* 				-1	: open dir failed            -   
* others:		one file  
* date:       	2018/01/11			
* author:     	Hangchang Xu
******************************************************************/ 
int test_tg_get_usr_dat(char* addr,int start_num,int max_num,unsigned char *dat)
{
	int i = 0;
	int sizeFeature3 = FEATURE_SIZE_X3;
	char str_addr[50] = {0};
	unsigned char * tempData;
	tempData = (unsigned char *)malloc(sizeFeature3*sizeof(unsigned char));
				
	
	for(i = 0;i<max_num;i++)
	{
			memset(str_addr,0,50*sizeof(char));
			sprintf(str_addr,"%s/%d.dat",addr,start_num+i);
//			printf("%s\n", str_addr);/* print all address*/ 
//			memset(tempData,0,sizeFeature3*sizeof(char));
			read_data_hex(tempData,sizeFeature3*sizeof(char),str_addr);
			memcpy(dat + i*sizeFeature3,tempData,sizeFeature3);
	}
	free(tempData);
	tempData = NULL;
	return 1;  
}



