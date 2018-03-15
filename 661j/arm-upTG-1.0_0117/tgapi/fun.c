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
#include <dirent.h>
#include <errno.h>
#include "tgapi.h"

int write_id_table(char *file,int num,int *id_table)
{
	int i;
	FILE *fp;
	if((fp = fopen(file,"wb")))
	{
		for(i = 0;i < num;i++)
		{
			fprintf(fp,"id = %-4d|\n",*(id_table+i));
		}
		fprintf(fp,"\ntotal id num = %d\n",num);
	}
	else
		return -1;
	fclose(fp);
	fp = NULL;
	return 1;
}


unsigned char **test_Make2DArray_uint8(int row,int col){
	unsigned char **a;
	int i;
	
	a=(unsigned char **)calloc(row,sizeof(unsigned char *));
	
	for(i=0;i<row;i++)
	{
		a[i]=(unsigned char *)calloc(col,sizeof(unsigned char));
	}
	
	return a;
}


void test_Free2DArray_uint8(unsigned char **a,int row){
	
	int i;
	for(i=0;i<row;i++)
	{
		free(a[i]);
		a[i] = NULL;
	}
	free(a);
	a = NULL;
	//printf("part");
}


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
	while ((entry = readdir(dirptr)))  
	{ 
		if(entry->d_type == 8)
		{
//			printf("%s\n", entry->d_name);/* ???????????? */ 
			num ++;
		}
	}
	closedir(dirptr);
	return num;
}

int tg_get_usr_dat(char* addr,stu_usr *usr_addr,unsigned char *dat)
{
	int i = 0;
    int sizeFeature3 = (5136+640) *3;
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
	
	while ((entry = readdir(dirptr)))  
	{ 
		if(entry->d_type == 8)
		{
//			printf("%s\n", entry->d_name);/* ???????????? */ 
			memset(str_addr,0,50*sizeof(char));
			sprintf(str_addr,"%s%s",addr,entry->d_name);

			//printf("%s\n", str_addr);/* ????addr */ 
			memset(tempData,0,sizeFeature3*sizeof(char));
            read_data_hex(tempData,sizeFeature3,str_addr);
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


unsigned char **Make2DArray_uint8(int row,int col)
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


// 2.???????row?????
void Free2DArray_uint8(unsigned char **a,int row){

    int i;
    for(i=0;i<row;i++)
    {
        free(a[i]);
        a[i] = NULL;
    }
    free(a);
    a = NULL;
    //printf("part");
}

void trans_encrypt(unsigned char * data,int length)
{
	int i,j,temp1,temp2;
	unsigned char key[32] = {0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f};

	temp1 = length / 32;
	temp2 = length % 32;
	for(i = 0;i < temp1;i++)
	{
		for(j = 0;j < 32;j++)
		{
			data[i*32+j] ^= key[j];		
		}
	}
	for(j = 0;j < temp2;j++)
	{
		data[i*32+j] ^=  key[j];		
	}
}


void XorEncryptDecrypt(unsigned char * Veindata, int length, int flag)
{
	int i, j, temp1, temp2;
	//unsigned char key[32] = { 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f };
	unsigned char key[128] = {  0x3F, 0xE2, 0x58, 0x26, 0xD9, 0x74, 0x40, 0x5B, 0xFE, 0x9F, 0x43, 0xE3, 0xF9, 0xA5, 0xDF, 0x07,
								0xEF, 0x93, 0x2D, 0xD0, 0x05, 0xE3, 0x89, 0x37, 0x7B, 0xD4, 0x2B, 0x54, 0x2D, 0xD4, 0xCD, 0xEA,
								0x9A, 0x3F, 0x0D, 0x43, 0x95, 0x0C, 0x26, 0x60, 0xCF, 0xC0, 0x3B, 0xD6, 0x08, 0x64, 0x1C, 0xE3,
								0x8E, 0x6F, 0xFD, 0x59, 0x20, 0x43, 0x59, 0xCE, 0x42, 0x18, 0x21, 0x3A, 0x5A, 0x64, 0x19, 0xB5,
								0x9D, 0xA9, 0x1B, 0xF8, 0x8E, 0xFF, 0xAA, 0xB0, 0xEF, 0x5E, 0xCC, 0x8F, 0x05, 0xB8, 0x4F, 0x6D,
								0x9A, 0x8E, 0xCC, 0x1F, 0x5B, 0x84, 0x13, 0x8B, 0xC2, 0xD6, 0xB2, 0xA3, 0xD4, 0x6A, 0x60, 0x0B,
								0x4E, 0x8D, 0xE5, 0x09, 0x99, 0xC8, 0x11, 0x96, 0x20, 0xB7, 0x9C, 0xB9, 0x73, 0x40, 0xCE, 0xEC,
								0x41, 0x2C, 0xC0, 0x1C, 0x3A, 0x2E, 0xCE, 0x6E, 0x8E, 0x11, 0xA9, 0xFD, 0x4A, 0x00, 0xC6, 0x65 };
	if (1 == flag)
	{
		temp1 = length / 128;
		temp2 = length % 128;
		for (i = 0;i < temp1;i++)
		{
			for (j = 0;j < 128;j++)
			{
				Veindata[i * 128 + j] ^= key[j];
			}
		}
		for (j = 0;j < temp2;j++)
		{
			Veindata[i * 128 + j] ^= key[j];
		}
	}
	else if (2 == flag)
	{
		unsigned char * tempData = (unsigned char *)malloc((length - 16) * sizeof(unsigned char));//特征值减去头尾8个字节 
		memcpy(tempData, Veindata + 8, length - 16);
		temp1 = (length - 16) / 128;
		temp2 = (length - 16) % 128;
		for (i = 0;i < temp1;i++)
		{
			for (j = 0;j < 128;j++)
			{
				tempData[i * 128 + j] ^= key[j];
			}
		}
		for (j = 0;j < temp2;j++)
		{
			tempData[i * 128 + j] ^= key[j];
		}
		memcpy(Veindata + 8, tempData, length - 16);
		free(tempData);
	}
}
