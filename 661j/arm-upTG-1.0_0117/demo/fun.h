#ifndef FUN_H 
#define FUN_H 



#include <dirent.h>

#define FEATURE_SIZE 5984
#define FEATURE_SIZE_X3 17536

typedef struct{
	int num;
	char addr[260];
}stu_usr;

void XorEncryptDecrypt(unsigned char * Veindata, int length, int flag);

void TGSaveImage(char *file,unsigned char* bmp,int w ,int h)  ;

/********************************  dat  ********************************/
int tg_get_usr_num(char* addr);

int tg_get_usr_dat(char* addr,stu_usr *usr_addr,unsigned char *dat);

/********************************  other fun  ********************************/
int write_id_table(char *file,int num,int *id_table);

unsigned char **test_Make2DArray_unsigned(int row,int col);

void test_Free2DArray_unsigned(unsigned char **a,int row);

int read_data_hex(unsigned char *buf,int length,char *string);

int write_data_hex(unsigned char * array,int length,char *string);
//void send_ret_resolve(int *fd,int ret);

//void recv_ret_resolve(int *fd,int ret);
#endif 

