/*������bmpλͼ�ļ������Է�Ϊ�ļ���Ϣͷ��λͼ��Ϣͷ��RGB��ɫ�����������֡�
�ļ���Ϣͷ��Ҫ�������Ƿ���BMP�ļ������ļ��Ĵ�С����Ϣ����λͼ��Ϣͷ����Ҫ
����bmp�ļ���λͼ��ȣ��߶ȣ�λƽ�棬ͨ��������Ϣ����RGB��ɫ���У������
����������������Ҫ��bmpλͼ���������ݡ���Ҫ���ѵ��ǣ�bmpλͼ����ɫ���в�
�֣��������ݵĴ洢�������½�Ϊԭ�㡣Ҳ����˵�������һ��bmpͼƬ����ʾ��
������Ļ�ϵ�ʱ��ʵ���ڴ洢��ʱ�����ͼƬ�������½ǵ����������ȱ��洢��
bmp�ļ��еġ�֮�󣬰��մ����ң����µ��ϵ�˳��,���ν����������ݵĴ洢��
�������洢����3ͨ����λͼ���ݣ�Ҳ��������ͨ��˵�Ĳ�ͼ������ô���ǰ���
B0G0R0B1G1R1B2G2R2...��˳����д洢�ģ�ͬʱ����Ҫ���ǵ�4�ֽڶ�������⡣
*/
  
#include <stdio.h>   
#include <stdlib.h>  
#include "Bmp.h" 
#include "tgapi.h"

void TG_LoadImage(Image* bmpImg,char* path)  
{  
  //Image* bmpImg;  
    FILE* pFile;  
    uint16_t fileType;  
    BitMapFileHeader bmpFileHeader;  
    BitMapInfoHeader bmpInfoHeader;  
    int channels = 1;  
    int width = 0;  
    int height = 0;  
    int step = 0;  
    int offset = 0;  
    unsigned char pixVal;  
    RgbQuad* quad;  
    int i, j;  
	//int k;
  
    pFile = fopen(path, "rb"); //?????????????????????????????????;???????????????????????????????????
	                           // ??????????????????NULL?????????????????errno ?? 
    if (!pFile)  
    {  
        free(bmpImg);  
//		printf("??????????????�????????\n");
 		bmpImg = NULL;
		exit(0);
    }  
  
    fread(&fileType, sizeof(uint16_t), 1, pFile);//1.??????????????????
	                                                   //2.??????�?????????????????????????
	                                                   //3.???count????????�????????size?????
	                                                   //4.??????
                                                       //????????????????????
	if (fileType == 0x4D42)  
    {  
        //printf("bmp file! \n");   
  
        fread(&bmpFileHeader, sizeof(BitMapFileHeader), 1, pFile);  

        fread(&bmpInfoHeader, sizeof(BitMapInfoHeader), 1, pFile);  
  
        if (bmpInfoHeader.biBitCount == 8)  
        {  
            channels = 1;  
            width = bmpInfoHeader.biWidth;  
            height = bmpInfoHeader.biHeight;  
            offset = (channels*width)%4; //??? 
            if (offset != 0)  
            {  
                offset = 4 - offset;  
            }  
            bmpImg->width = width;  
            bmpImg->height = height;  
            bmpImg->channels = 1;  

            step = channels*width; //???? 
  
            quad = (RgbQuad*)malloc(sizeof(RgbQuad)*256);  
            fread(quad, sizeof(RgbQuad), 256, pFile);  
            free(quad);  
			quad = NULL;
            //?????????????????imageData
            for (i=0; i<height; i++)  
            {  
                for (j=0; j<width; j++)  
                {  
                    fread(&pixVal, sizeof(unsigned char), 1, pFile);  
                    //bmpImg->imageData[(height-1-i)*step+j] = pixVal; 
					*(*(bmpImg->imageData+(height-1-i))+j) = pixVal;
                }  
                if (offset != 0)  
                {  
                    for (j=0; j<offset; j++)  
                    {  
                        fread(&pixVal, sizeof(unsigned char), 1, pFile);  
                    }  
                }  
            }             
        }  
     
    }  
	
	fclose(pFile);
}  
  
 int TG_SaveImage(char* path, Image* bmpImg)  
 {  
     FILE *pFile;  
     uint16_t fileType;  
     BitMapFileHeader bmpFileHeader;  
     BitMapInfoHeader bmpInfoHeader;  
     int step;  
     int offset;  
     unsigned char pixVal = '\0';  
     int i, j;  
     RgbQuad* quad;  
   
     pFile = fopen(path, "wb");  
     if (!pFile)  
     {  
         return 0;  
     }  
   
     fileType = 0x4D42; // "BM"???????????????????0x4d42
     fwrite(&fileType, sizeof(uint16_t), 1, pFile);  
   
    if (bmpImg->channels == 1)//8??????????????   
     {  
         step = bmpImg->width;  
         offset = step%4;  
         if (offset != 4)  
         {  
             step += 4-offset;  
         }  
   
         bmpFileHeader.bfSize = 54 + 256*4 + bmpImg->width;  
         bmpFileHeader.bfReserved1 = 0;  
         bmpFileHeader.bfReserved2 = 0;  
         bmpFileHeader.bfOffBits = 54 + 256*4;  
         fwrite(&bmpFileHeader, sizeof(BitMapFileHeader), 1, pFile);  
   
         bmpInfoHeader.biSize = 40;  
         bmpInfoHeader.biWidth = bmpImg->width;  
         bmpInfoHeader.biHeight = bmpImg->height;  
         bmpInfoHeader.biPlanes = 1;  
         bmpInfoHeader.biBitCount = 8;  
         bmpInfoHeader.biCompression = 0;  
         bmpInfoHeader.biSizeImage = bmpImg->height*step;  
         bmpInfoHeader.biXPelsPerMeter = 0;  
         bmpInfoHeader.biYPelsPerMeter = 0;  
         bmpInfoHeader.biClrUsed = 256;  
         bmpInfoHeader.biClrImportant = 256;  
         fwrite(&bmpInfoHeader, sizeof(BitMapInfoHeader), 1, pFile);  
   
         quad = (RgbQuad*)malloc(sizeof(RgbQuad)*256);  
         for (i=0; i<256; i++)  
         {  
             quad[i].rgbBlue = i;  
             quad[i].rgbGreen = i;  
             quad[i].rgbRed = i;  
             quad[i].rgbReserved = 0;  
         }  
         fwrite(quad, sizeof(RgbQuad), 256, pFile);  
         free(quad);  
   
         for (i=bmpImg->height-1; i>-1; i--)  
//		 for (i=0; i < bmpImg->height; i++) 
         {  
             for (j=0; j<bmpImg->width; j++)  
             {  
                 //pixVal = bmpImg->imageData[i*bmpImg->width+j];
 				pixVal = *(*(bmpImg->imageData+i)+j);
                 fwrite(&pixVal, sizeof(unsigned char), 1, pFile);  
             }  
             if (offset!=0)  
             {  
                 for (j=0; j<offset; j++)  
                 {  
                     pixVal = 0;  
                     fwrite(&pixVal, sizeof(unsigned char), 1, pFile);  
                 }  
             }  
         }  
     }  
     fclose(pFile);  
   
     return 1;  
 } 
 
 
//unsigned char *src???????????
//int height?????
//int width??????
//char	*bmp_addr??bmp?????�????????myfinger.bmp??
void TG_SaveBmp(unsigned char *src,int height,int width,char *bmp_addr)
{
	int i,j,k;
	Image *myfingerImg;
	myfingerImg = (Image *)malloc(sizeof(Image));
	myfingerImg->channels = 1;
	myfingerImg->height = height;
	myfingerImg->width = width;
	myfingerImg->imageData = Make2DArray_uint8(height,width);

	
	k = 0;
	for(i = 0;i<height;i++)
	{
		for(j = 0;j<width;j++)
		{printf("%d %d\n", i,j);
			*(*(myfingerImg->imageData+i)+j) = src[k++];
		}
	}

	TG_SaveImage(bmp_addr,myfingerImg) ;

	Free2DArray_uint8(myfingerImg->imageData,height);
	free(myfingerImg);
	myfingerImg = NULL;
}

 
