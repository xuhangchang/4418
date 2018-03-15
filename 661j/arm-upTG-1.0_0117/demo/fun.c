#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "fun.h"


void TGSaveImage(char *file,unsigned char* bmp,int w ,int h)  
{  
	int i = 0;	
	char color = 0;  
	char end[2] = {0,0};  
	char patte[1024] = {0};  
	int pos = 0;  
  
	unsigned char heard[54] = {0x42,0x4d,0x30,0x0C,0x01,0,0,0,0,0,0x36,4,0,0,0x28,0,0,0,0xF5,0,0,0,0x46,0,0,0,0x01,0,8,0,0,0,0,0,0xF8,0x0b,0x01,0,0x12,0x0b,0,0,0x12,0x0b,0,0,0,0,0,0,0,0,0,0};  
	int size = w*h;  
	int allsize = size +1080;  
  
	heard[2] = allsize&0xFF;  
	heard[3] = (allsize>>8)&0xFF;  
	heard[4] = (allsize>>16)&0xFF;	
	heard[5] = (allsize>>24)&0xFF;	
  
	heard[18] = w&0xFF;  
	heard[19] = (w>>8)&0xFF;  
	heard[20] = (w>>16)&0xFF;  
	heard[21] = (w>>24)&0xFF;  
  
	heard[22] = h&0xFF;  
	heard[23] = (h>>8)&0xFF;  
	heard[24] = (h>>16)&0xFF;  
	heard[25] = (h>>24)&0xFF;  
  
	allsize -=1078;  
	heard[34] = allsize&0xFF;  
	heard[35] = (allsize>>8)&0xFF;	
	heard[36] = (allsize>>16)&0xFF;  
	heard[37] = (allsize>>24)&0xFF;  
  
	for(i=0;i<1024;i+=4)  
	{  
		patte[pos++] = color;  
		patte[pos++] = color;  
		patte[pos++] = color;  
		patte[pos++] = 0;  
		color++;  
	}  
  
	FILE* fd = NULL;  
  
	fd = fopen(file,"wb+");  
	if(fd == NULL)	
	{  
		return;  
	}  
	else  
	{  
		fwrite(heard,54,1,fd);	
		fwrite(patte,1024,1,fd);  
		fwrite(bmp,size,1,fd);	
		fwrite(end,2,1,fd);  
	}  
	fclose(fd);  
}  


/*****************************************************************  
*
*						.dat operation
*
******************************************************************/ 


/*****************************************************************  
* function:		dat_get_usr_num
* description:  get the file num in a dir
* param1:     	char* addr	: path of dir (input)
* return:    	-1		: open dir failed 
*				else	: file num            -   
* others:
* date:       	2017/03/03			
* author:     	Hangchang Xu
******************************************************************/ 
int tg_get_usr_num(char* addr)
{
	int num = 0;
	DIR *dirptr = NULL;
	struct dirent *entry;
	if((dirptr = opendir(addr)) == NULL)  
	{  
		printf("open dir failed!\n");  
		return -1;  
	} 				
	while (entry = readdir(dirptr))  
	{ 
		if(entry->d_type == 8)
		{
//			printf("%s\n", entry->d_name);/* print all name in this dir  */ 
			num ++;
		}
	}
	closedir(dirptr);
	return num;
}

/*****************************************************************
* function:		dat_get_usr_dat
* description:  get the all file dat in a dir
* param1:     	char* addr	: path of dir	(input)
* param2:     	stu_dat_usr *usr_addr	: all dat path	(output)
* param3:     	unsigned char *dat	: pointer of all dat	(output)
*
* return:    	1	: succeed 
* 				-1	: open dir failed            -   
* others:		one file  = 5136*3 Bytes
* date:       	2017/03/03			
* author:     	Hangchang Xu
******************************************************************/ 
int tg_get_usr_dat(char* addr,stu_usr *usr_addr,unsigned char *dat)
{
	int i = 0;
	int sizeFeature3 = FEATURE_SIZE_X3;
	char str_addr[50] = {0};
	DIR *dirptr = NULL;
	struct dirent *entry;
	unsigned char * tempData;
	tempData = (unsigned char *)malloc(sizeFeature3*sizeof(unsigned char));
	
	if((dirptr = opendir(addr)) == NULL)  
	{  
		printf("open dir failed!\n");  
		return -1;  
	} 				
	
	while (entry = readdir(dirptr))  
	{ 
		if(entry->d_type == 8)
		{
//			printf("%s\n", entry->d_name);/* print all name in this dir  */ 
			memset(str_addr,0,50*sizeof(char));
			sprintf(str_addr,"%s/%s",addr,entry->d_name);

//			printf("%s\n", str_addr);/* print all address*/ 
//			memset(tempData,0,sizeFeature3*sizeof(char));
			read_data_hex(tempData,sizeFeature3*sizeof(char),str_addr);
			memcpy(dat + i*sizeFeature3,tempData,sizeFeature3);

			memset((usr_addr+i)->addr,0,50*sizeof(char));
			strcpy((usr_addr+i)->addr,str_addr);
			(usr_addr+i)->num = i;
			i++;
//			printf("i = %d\n",i);
		}
	}
	closedir(dirptr);	
	free(tempData);
	tempData = NULL;
	return 1;  
}


/*****************************************************************
* function:		write_id_table
* description:  save all id in a file for send to pc
* param1:     	char *file	: path of file	(input)
* param2:     	int num	: user num	(input)
* param3:     	int *id_table	: int pointer of all id (input)
*
* return:    	1	: succeed 
* 				-1	: open file failed            -   
* others:		
* date:       	2017/03/03			
* author:     	Hangchang Xu
******************************************************************/
int write_id_table(char *file,int num,int *id_table)
{	
	int i;	
	FILE *fp;	
	if(fp = fopen(file,"wb"))	
		{		
			for(i = 0;i < num;i++)		
				{			
					fprintf(fp,"id = %-4d |\n",*(id_table+i));		
				}		
			fprintf(fp,"\ntotal id num = %d\n",num);	
		}	
	else		
		return -1;	
	fclose(fp);	
	fp = NULL;	
	return 1;
}


unsigned char **test_Make2DArray_unsigned(int row,int col)
{
	unsigned char **a;
	int i;

	a=(unsigned char **)calloc(row,sizeof(unsigned char *));

	for(i=0;i<row;i++)
	{
		a[i]=(unsigned char *)calloc(col,sizeof(unsigned char));
	}

	return a;
}

void test_Free2DArray_unsigned(unsigned char **a,int row)
{
	int i;
	for(i=0;i<row;i++)
	{
		free(a[i]);
		a[i] = NULL;
	}
	free(a);
	a = NULL;
}

int read_data_hex(unsigned char *buf,int length,char *string)
{
	FILE *fp;
	fp = fopen(string,"rb");
	if(NULL == fp)
	{
		printf("file open Fail!\n");
		return -1;
	}
	fread(buf,sizeof(char),length,fp);
	fclose(fp);
	return 0;
}

int write_data_hex(unsigned char * array,int length,char *string)
{
	int i = 0;
	FILE *fp;
	fp = fopen(string,"wb+");
	if(NULL == fp)
	{
		printf("file open Fail!\n");
		return -1;
	}
	while(i < length)
	{
		fwrite(&array[i],sizeof(char ),1,fp);
		i++;
	}
	fclose(fp);
	return 0;
}

