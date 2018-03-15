#include <stdio.h>
#include <string.h>
#include <sys/time.h>  

#include <unistd.h>  

#include <pthread.h>
#include "tgapi.h"
#include "TGComApi.h"
#include "function.h"

extern pthread_t pthread_recv_tid;
extern pthread_mutex_t mutex_thread;


extern int fd_hid;
extern int recv_flag ;
extern int get_fw_flag ;
extern int get_sn_flag ;
extern int get_camera_flag;
extern int get_feature_flag;

extern char sn_str[100];
extern char fw_str[100];
extern unsigned char camera_data[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT];
extern unsigned char feature[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT];
extern TAG_HULL tag;


int TGGetStatus()
{
	return fd_hid;
}

int TGOpenDev(int *fd)
{
	int ret = -1;
	int time = 30;
	
	recv_flag = 1;
	usleep(1000*200);

	if(pthread_create(&pthread_recv_tid,NULL,RecvThread,NULL)){
		perror("fail to RecvThread");
		return -1;
	}
	pthread_detach(pthread_recv_tid);
	
	do{
		
		if(TGGetStatus()<0) 
			ret = -1;
		else {
			ret = 0;
			break;
		}
		usleep(1000*30);
		//printf("%d %d %d %d\n",time,ret,fd_hid,recv_flag);
	}
	while(time--);	


/*	if(-1 == ret){
		if(pthread_cancel(pthread_recv_tid))																				
				perror("RecvThread pthread_cancel  fail "); 									
	}*/

	
	fd = &fd_hid;
	return ret;
}

int TGCloseDev( )
{
	recv_flag=1;
	if(fd_hid<0) return -1;

	while(recv_flag!=2) usleep(1000*20);
	
	return 0;
}
/*
return : -1 :cannot get image
		1: get image
*/

int quit=0;
int TGDevGetImage(unsigned char* pImageData,int timeout)
{
	int Img_Size;
	int Img_Width,Img_Height;
	unsigned char temp_buf[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT];
	unsigned char label[] = {0x85,0x88,0x92,0x91,0x7D,0x7C,0x92,0x83};
	time_t timetmp;
	struct tm * timeinfo;
	char buf[20];
	memset(buf,0,sizeof(buf));
	
	
	int start_time,end_time;
	struct timeval tv;
	int ret = -1;
	get_camera_flag = 0;
	while(quit!=0)
	{
		quit=0;
		usleep(10000);
	}
	quit=1;
	//TG_SendPackage(fd_hid,DEV_TYPE,0,418,0,NULL,NULL);
	gettimeofday(&tv,NULL);
	start_time = tv.tv_sec;
	while(1)
	{
		if(get_camera_flag)
		{	
			memset(temp_buf,0,CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT*sizeof(unsigned char));
			memcpy(&tag,camera_data,get_camera_flag);
			
			//printf("*************************tag.Length:%d\n",tag.length);
			XorEncryptDecrypt( tag.data_value, tag.data_length, 1 );
			jpeg2rgb(tag.data_value, tag.data_length, temp_buf, &Img_Size, &Img_Width, &Img_Height);
			//printf("*************************Img_Size:%d\n",Img_Size);
			//printf("*************************Img_Width:%d\n",Img_Width);
			//printf("*************************Img_Height:%d\n",Img_Height);
			//TGSaveImage("Camera.bmp",temp_buf, CAMERA_ROI_WIDTH ,CAMERA_ROI_HEIGHT) ;
			
			memcpy(&temp_buf[0],&Img_Width,2);
			memcpy(&temp_buf[2],&Img_Height,2);
			memcpy(&temp_buf[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT-26],tag.heard,2);
			memcpy(&temp_buf[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT-24],tag.sn_value,16);
			memcpy(&temp_buf[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT-8],label,8);
			
			memcpy(tag.data_value,temp_buf,CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT);
			tag.data_length = Img_Size;
			tag.length = 208+tag.data_length;
			timetmp = time(&timetmp);
			timeinfo = localtime(&timetmp);
			strftime(buf,sizeof(buf),"%Y%m%d%H%M%S",timeinfo);
			//printf("*************Current time:%s\n",buf);
			memcpy(tag.time_value,buf,14);
			
			//printf_tag();
			//write_data_hex(&tag,tag.length,"LabelImage");
			XorEncryptDecrypt( tag.data_value, tag.data_length, 1 ); 
			//memcpy(pImageData,tag.data_value,CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT);
			memcpy(pImageData,&tag,tag.length);
			//printf("*************************tag.length=%d\n",tag.length);
			get_camera_flag = 0;
			
			return 0;
		}		
		usleep(10*1000);	
		gettimeofday(&tv,NULL);
		end_time = tv.tv_sec;
		if(end_time-start_time >= timeout||quit==0)
			break;	
	}
	quit=0;
	get_camera_flag = 0;
	return ret;
}
int quit1=0;

int TGDevGetFeature(unsigned char* FeatureData,int timeout)
{
	int start_time,end_time;
	struct timeval tv;
	int ret = -1;
	get_camera_flag = 0;
	while(quit1!=0){
		quit1=0;
		usleep(10000);
	}
	quit1=1;
	
	gettimeofday(&tv,NULL);
	start_time = tv.tv_sec;
	while(1)
	{
		if(get_feature_flag)
		{
			memcpy(FeatureData,feature,1936);
			get_feature_flag = 0;
		//	printf("333333333333333\n");
		//	XorEncryptDecrypt( camera_data, CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT, 1 );
		//	TGSaveImage("out.bmp",FeatureData, CAMERA_ROI_WIDTH ,CAMERA_ROI_HEIGHT) ;
		//write_data_hex(feature,5776,"feature.dat");
			return 0;
		}		
		usleep(10*1000);	
		gettimeofday(&tv,NULL);
		end_time = tv.tv_sec;
		if(end_time-start_time >= timeout||quit1==0)
			break;	
	}
	quit1=0;
	get_feature_flag = 0;
	return ret;
}

int TGDevPlayVoice(int VoiceValue)
{
	int ret;

	ret=TG_SendPackage(fd_hid,DEV_TYPE,VoiceValue,410,0,NULL,NULL);
	
	return ret;
}

int TGDevGetFW(char *fw)
{
	int start_time,end_time;
	int timeout = 5;
	struct timeval tv;
	get_fw_flag = 0;
	
	TG_SendPackage(fd_hid,DEV_TYPE,6,411,0,NULL,NULL);
	usleep(1000*200);
	gettimeofday(&tv,NULL);
	start_time = tv.tv_sec;
	while(1)
	{
		if(get_fw_flag)
		{
			//strcpy(fw,fw_str);
			memcpy(fw,fw_str,16);
			get_fw_flag = 0;
			return 0;
		}		
		usleep(10*1000);	
		gettimeofday(&tv,NULL);
		end_time = tv.tv_sec;
		if(end_time-start_time >= timeout)
			break;	
	}
	get_fw_flag = 0;
	return -1;

}

int TGDevGetSN(char *sn)
{
	int start_time,end_time;
	int timeout = 5;
	struct timeval tv;
	get_sn_flag = 0;
	TG_SendPackage(fd_hid,DEV_TYPE,7,412,0,NULL,NULL);
	usleep(1000*200);
	gettimeofday(&tv,NULL);
	start_time = tv.tv_sec;
	while(1)
	{
		if(get_sn_flag)
		{
			//strcpy(sn,sn_str);
			memcpy(sn,sn_str,16);
			get_sn_flag = 0;
			return 0;
		}		
		usleep(10*1000);	
		gettimeofday(&tv,NULL);
		end_time = tv.tv_sec;
		if(end_time-start_time >= timeout)
			break;	
	}
	get_sn_flag = 0;
	return -1;
}


int TGSetMode(int mode)
{
	int ret;
	if(1 == mode)
		ret = TG_SendPackage(fd_hid,DEV_TYPE,0,413,0,NULL,NULL);

	if(2 == mode)
		ret = TG_SendPackage(fd_hid,DEV_TYPE,0,414,0,NULL,NULL);
	return ret;
}



