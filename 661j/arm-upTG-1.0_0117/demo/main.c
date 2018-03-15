
/******************************************************************************
*以下为工程部分头文件
******************************************************************************/
// #include "testFunctions.h"
//#include "BaseFunction.h"
#include "fun.h"
// #include "bmp.h"


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
/******************************************************************************
*以下为DLL相关文件
*DLL需要同时存在于下列相关目录中,如果目录不存在请先生成解决方案

******************************************************************************/
#include "TGComApi.h"
#include "TGFingerVeinApi.h"


/******************************************************************************
*以下为宏定义
******************************************************************************/
#define IMAGE_ROI_WIDTH 500
#define IMAGE_ROI_HEIGHT 200

#define DAT_ADDR "./FVDB"
#define DEV_TYPE "VM661J"
#define MAX_FV_NUM 100

#define TG_OPEN_OK 0
#define TG_OPEN_FAIL -1


#define TG_NO_FINGER 2



//#define FRONT_ISP


/******************************************************************************
*以下为全局变量
******************************************************************************/
int	fd;
unsigned char Img[640*480];

/******************************************************************************
*以下为函数声明
******************************************************************************/
unsigned int RegFinger(void);
unsigned int IdentifyFinger(void);


int get_dev_info()
{
	char fw[128],sn[128];

	memset(fw,0,128);
	memset(sn,0,128);
	
	TGDevGetFW(fw);
	TGDevGetSN(sn);
	printf("fw:%s\nsn:%s\n",fw,sn);
	
	TGDevPlayVoice(BI);
	usleep(1000*100);
}

int input = 1,exitb=1;
int *Detect_Mode()
{
	while(exitb)
	{
		usleep(100*1000);
		scanf("%d",&input);
		if(TGGetStatus()>0)
		{
			switch(input)
			{
			 case 0:
//			   printf("进入注册模式\n");
			   printf("enter register mode\n");
			   RegFinger();
			   input = 1;
			   break;
//			 case 1:
//			   break;
			 default:
			  // printf("输入的是非0\n");
			   break;
			}
		}
	}
	return 0;		
}
/******************************************************************************
*以下为主函数
******************************************************************************/
void main(int argc,char **argv)
{
	int ret;
	int Handle;
	pthread_t pt_t;
	if(argc!=2)
	{
		input = 1;
	}
	ret = TGOpenDev(&fd);
	if (ret != TG_OPEN_OK)
	{
//		printf("设备未连接\n");
		printf("dev not connected\n");
		return;
	}
//	printf("设备已连接\n");
	printf("dev connected\n");
	Handle = pthread_create(&pt_t,NULL,Detect_Mode,NULL);
	if (Handle != 0)
	{
		perror("fail to RecvThread");
		TGCloseDev();
		exit(-1);
	}
//	printf("【帮助：输入0注册模式】\n");
	printf("[help : enter 0 register mode]\n");
	get_dev_info();
	TGDevPlayVoice(BI);
	while(1)
	{
		if(TGGetStatus()>0)
		{
			if(input)
			{
				IdentifyFinger();	
			}			
		}
		else
		{
			ret = TGOpenDev(&fd);
			if (ret != TG_OPEN_OK)
			{
//				printf("设备未连接\n");
				printf("dev not connected\n");
				usleep(1000*1000);
				
				continue;
			}
			get_dev_info();
		}	
		usleep(20*1000);
	}
		
	exitb = 0;
	TGDevPlayVoice(BIBI);
	TGCloseDev();
//	printf("设备已关闭\n");
	printf("dev closed\n");
	usleep(1000*1000);
	return ;
}

/******************************************************************************
*以下为登记子函数
******************************************************************************/
unsigned int RegFinger(void)
{
	int sizeFeature3 =FEATURE_SIZE_X3, userName;
	int loc;
	int usr_num = 0;
	int i,j,ret;
	int m = 0;
	char featrue_addr[256] = { 0 };

	unsigned char ** pic_data;
	unsigned char ** feature;
	unsigned char * tempRegisterData;
	unsigned char * tempUpdateData;
	unsigned char * registerData = NULL;
	stu_usr * usr_addr = NULL;

	pic_data = test_Make2DArray_unsigned(3, IMAGE_ROI_WIDTH*IMAGE_ROI_HEIGHT);
	feature = test_Make2DArray_unsigned(3,FEATURE_SIZE);
	tempRegisterData = (unsigned char *)malloc(sizeFeature3 * sizeof(unsigned char));
	tempUpdateData = (unsigned char *)malloc(sizeFeature3 * sizeof(unsigned char));
	usr_num = tg_get_usr_num(DAT_ADDR);

	registerData = (unsigned char *)malloc(sizeFeature3*usr_num * sizeof(unsigned char));
	usr_addr = (stu_usr *)malloc(usr_num * sizeof(stu_usr));

	tg_get_usr_dat(DAT_ADDR, usr_addr, registerData);

	userName = usr_num;
	
	if (usr_num == MAX_FV_NUM)
	{
//		printf("登记已满\n");
		printf("register full\n");
		TGDevPlayVoice(VEIN_FULL);
		return 1;
	}

	for (i = 0, j = 0; i < 6; i++)
	{
		if (0 == i)
		{
//			printf("请自然轻放手指\n");
			printf("please press finger\n");
			TGDevPlayVoice(PLS_PUT_SOFTLY);
		}
#ifdef FRONT_ISP
		if (0 == TGDevGetFeature(*(feature + j),10))
		{
			ret=0;
#else
		if (0 == TGDevGetImage(Img,10))
		{
			
			memcpy(*(pic_data + j), Img, IMAGE_ROI_WIDTH * IMAGE_ROI_HEIGHT * sizeof(char));

			ret = TGImgExtractFeature(*(pic_data + j),  IMAGE_ROI_WIDTH, IMAGE_ROI_HEIGHT, *(feature + j), "license.dat");
#endif

			if (1 == ret)
			{
//				printf("许可证过期,登记失败\n");
				printf("license overtime,reg fail\n");
				TGDevPlayVoice(REG_FAIL);
				userName--;
				break;
			}
			else if (2 == ret)
			{
//				printf("许可证无效,登记失败\n");
				printf("license wrong,reg fail\n");
				TGDevPlayVoice(REG_FAIL);
				userName--;
				break;
			}
			else
			{
				if ((0 != ret) && (i < 5))
				{
					m++;
					if (m > 3)
					{
//						printf("登记失败\n");
						printf("reg fail\n");
						TGDevPlayVoice(REG_FAIL);
						userName--;
						break;
					}
					else
					{
//						printf("请正确放入手指\n");
						printf("please press finger  rightly\n");
						TGDevPlayVoice(PLS_PUT_CRUCLY);
					}
				}
				else
				{
					if ((j < 2) && (i < 5))
					{
						j++;
//						printf("请再次放入\n");
						printf("repress finger \n");
						TGDevPlayVoice(PLS_REPUT);
					}
					else
					{
						if ((2 != j) || (0 != TGFusionFeature(*(feature), *(feature + 1), *(feature + 2), tempRegisterData)))
						{
//							printf("登记失败\n");
							printf("reg,fail\n");
							TGDevPlayVoice(REG_FAIL);
							userName--;
						}
						else
						{
							if (0 != TGMatchFeature1N(tempRegisterData, registerData, usr_num, &loc, tempUpdateData))
							{
								sprintf(featrue_addr, "%s/%d.dat", DAT_ADDR, userName);
								printf("addr = %s\n",featrue_addr);
								write_data_hex(tempRegisterData, sizeFeature3, featrue_addr);

//								printf("登记成功,特征为.%s.%d.dat\n", DAT_ADDR, userName);
								printf("reg success,feature = .%s.%d.dat\n", DAT_ADDR, userName);
								TGDevPlayVoice(REG_SUCCESS);
							}
							else
							{
//								printf("重复登记\n");
								printf("repeat reg\n");
								TGDevPlayVoice(REG_FAIL);
								userName--;
							}
						}
						break;
					}
				}
			}
		}
		else
		{
			//printf("设备已断开\n");
			userName--;
			break;
		}
		sleep(1);	
	}

	test_Free2DArray_unsigned(pic_data, 3);
	test_Free2DArray_unsigned(feature, 3);
	free(tempRegisterData);
	tempRegisterData = NULL;
	free(tempUpdateData);
	tempUpdateData = NULL;
	free(registerData);
	registerData = NULL;
	free(usr_addr);
	usr_addr = NULL;

	return 0;
}

/******************************************************************************
*以下为验证子函数
******************************************************************************/
unsigned int IdentifyFinger(void)
{
	int ret;
	int usr_num = 0;
	int  sizeFeature3 =FEATURE_SIZE_X3, loc;

	unsigned char * pic_data;
	unsigned char * feature;
	unsigned char * tempUpdateData;
	unsigned char * registerData;
	stu_usr *usr_addr = NULL;

	pic_data = (unsigned char *)malloc((IMAGE_ROI_WIDTH*IMAGE_ROI_HEIGHT) * sizeof(unsigned char));
	feature = (unsigned char *)malloc((IMAGE_ROI_WIDTH*IMAGE_ROI_HEIGHT) * sizeof(unsigned char));
	tempUpdateData = (unsigned char *)malloc(sizeFeature3 * sizeof(unsigned char));

	usr_num = tg_get_usr_num(DAT_ADDR);
	registerData = (unsigned char *)malloc(sizeFeature3*usr_num * sizeof(unsigned char));
	usr_addr = (stu_usr *)malloc(usr_num * sizeof(stu_usr));
	tg_get_usr_dat(DAT_ADDR, usr_addr, registerData);

	//printf("请自然轻放手指\n");
	//TGDevPlayVoice(PLS_PUT_SOFTLY);

#ifdef FRONT_ISP
	if (0 == TGDevGetFeature(feature,10))
	{
		ret=0;
		
#else
	if (0 == TGDevGetImage(Img,10))
	{
		memcpy(pic_data, Img, IMAGE_ROI_WIDTH * IMAGE_ROI_HEIGHT);


		ret = TGImgExtractFeature(pic_data, IMAGE_ROI_WIDTH, IMAGE_ROI_HEIGHT, feature,"license.dat");
#endif

		
		if (1 == ret)
		{
//			printf("许可证过期,验证失败\n");
			printf("license overtime,reg fail\n");
			TGDevPlayVoice(IDENT_FAIL);
		}
		else if (2 == ret)
		{
//			printf("许可证无效,验证失败\n");
			printf("license wrong,reg fail\n");
			TGDevPlayVoice(IDENT_FAIL);
		}
		else
		{
			//write_data_hex(feature, 1936, "./fea.dat");
			//write_data_hex(registerData, sizeFeature3, "./regFea.dat");
			ret = TGMatchFeature1N(feature, registerData, usr_num, &loc, tempUpdateData);
		//	printf("ret:%d\n");

			if (0 == ret)
			{
//				printf("验证成功, 特征为%s\n", (usr_addr + loc - 1)->addr);
				printf("compare success, feature = %s\n", (usr_addr + loc - 1)->addr);
				TGDevPlayVoice(IDENT_SUCCESS);
			}
			else
			{
//				printf("验证失败\n");
				printf("compare fail\n");
				TGDevPlayVoice(IDENT_FAIL);
			}
		}
	}
	else
	{
		;//printf("timeout wait\n");
	}

	free(pic_data);
	pic_data = NULL;
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


