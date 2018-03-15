#ifndef BMP_LOADSAVE_H   
#define BMP_LOADSAVE_H   
#include <stdint.h>
#define bool int
#define true 1
#define false 0
//�ļ���Ϣͷ  
typedef struct  
{  
    //unsigned short    bfType; //bfType�����ļ���ʽ,���ǡ�BM������ʮ�����Ƶ�����ʾ��0x4d42 
    uint32_t    bfSize;  //bfSizebfSize������Ǹ�λͼ�ļ��Ĵ�С�������ļ�ͷ����Ϣͷ����ɫ�壨����У����������ݣ���ռ4���ֽ�
    uint16_t    bfReserved1;//bfReserved1��bfReserved2���Ǳ��������������Ӧ�ö�Ϊ0  
    uint16_t    bfReserved2;  
    uint32_t    bfOffBits;//bfOffBits��������������ݾ����ļ�ͷ��λ�ã�Ҳ����ƫ�Ƶ�ַ  
} BitMapFileHeader;  
//λͼ��Ϣͷ  
typedef struct  
{  
    uint32_t  biSize;//biSize������Ǹýṹ��Ĵ�С����40�ֽ�   
    uint32_t   biWidth;//biWidth�������λͼ�Ŀ��   
    uint32_t   biHeight;//biHeight�������λͼ�ĸ߶�   
    uint16_t   biPlanes; //biPlanes�������λͼ��ƽ����  
    uint16_t   biBitCount; //biBitCount�������λͼ��λ������1��16��8��24��32��
    uint32_t  biCompression;//biCompression�������λͼ�������ݵĴ�С������BMPλͼ��û��ѹ���ģ�����ֵΪ0  
    uint32_t  biSizeImage;//biSizeImage�������λͼ�������ݵĴ�С�����߶�*ÿ��������ռ���ֽ���   
    uint32_t   biXPelsPerMeter;   
    uint32_t   biYPelsPerMeter;   
    uint32_t   biClrUsed;   
    uint32_t   biClrImportant;   
} BitMapInfoHeader;  
//RGB��ɫ���� 
typedef struct   
{  
    unsigned char rgbBlue; //����ɫ����ɫ����   
    unsigned char rgbGreen; //����ɫ����ɫ����   
    unsigned char rgbRed; //����ɫ�ĺ�ɫ����   
    unsigned char rgbReserved; //����ֵ   
} RgbQuad;  
//ͼ������  
typedef struct  
{  
    int width;  
    int height;  
    int channels;  
	//unsigned char * imageData;
    unsigned char ** imageData;  
} Image;  

//Image* LoadImage(char* path); 
void TG_LoadImage(Image*bmpImg,char* path);

int TG_SaveImage(char* path, Image* bmpImg);  

void TG_SaveBmp(unsigned char *src,int height,int width,char *bmp_addr);

#endif   
