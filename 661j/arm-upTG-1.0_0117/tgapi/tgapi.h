#ifndef TGAPI_H
#define TGAPI_H

//#define HID_DEV_H3
#define HID_DEV_JZ
//#define NET_DEV

#define CAM_7725
//#define CAM_9034
//#define MODULE_701





#ifdef CAM_7725
#define DEV_TYPE "661"
#define CAMERA_ROI_WIDTH	500
#define CAMERA_ROI_HEIGHT         200
//#define CUT_ROW_START 0
//#define CUT_COL_START 0
#endif

#ifdef CAM_9034
#define DEV_TYPE "660H3"
#define CAMERA_ROI_WIDTH	425
#define CAMERA_ROI_HEIGHT	180
#define CUT_ROW_START 31
#define CUT_COL_START 40
#endif

#ifdef MODULE_701
#define DEV_TYPE "701"
#define FINGERPRINT_WIDTH 256
#define FINGERPRINT_HEIGHT 288
#define CAMERA_ROI_WIDTH	500
#define CAMERA_ROI_HEIGHT	220
#endif


//#define DEBUG_PRINT

#define SAVE_PIC
#define ID_TABLE_ADDR "./database/id.txt"

#define FINGER_701_PATH "./701/finger/"
#define DAT_701_PATH "./701/dat/FGChar_"
#define DAT_ADDR "./dat/"
#define SERVICE_ADDR "127.0.0.1"
#define PORT 3333

#define BUFSIZE 1024*1024*2
#define PACKAGE_SIZE 1024
#define LABELIMG_SIZE    1024*1024

/******************************************************************************
//Add Label Img Pack
******************************************************************************/

typedef struct{
//head msg
unsigned char heard[16];
//total length of pack
unsigned int  length;

unsigned char fw_tag[4];
unsigned int  fw_length;
unsigned char fw_value[16];

unsigned char sn_tag[4];
unsigned int  sn_length;
unsigned char sn_value[16];

unsigned char type_tag[4];
unsigned int  type_length;
unsigned int type_value;

unsigned char encode_tag[4];
unsigned int  encode_length;
unsigned int encode_value;

unsigned char wh_tag[4];
unsigned int  wh_length;
unsigned short  data_width;
unsigned short  data_height;

//safe define
unsigned char encrypt_tag[4];
unsigned int  encrypt_length;
unsigned int encrypt_value;

unsigned char key_tag[4];
unsigned int  key_length;
unsigned int key_value;

//collect env
unsigned char tgc_tag[4];
unsigned int  tgc_length;
unsigned char tgc_value[4];

unsigned char tgf_tag[4];
unsigned int  tgf_length;
unsigned char tgf_value[4];

unsigned char sub_tag[4];
unsigned int  sub_length;
unsigned int sub_value;

unsigned char time_tag[4];
unsigned int  time_length;
unsigned char time_value[16];

unsigned char env_tag[4];
unsigned int  env_length;
unsigned int env_value;
//image data
unsigned char data_tag[4];
unsigned int  data_length;
unsigned char data_value[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT];

}TAG_HULL;

struct package {
	char	heard[16];
	int 	id;
	int		cmd;
	int 	length;
	char	md5[32];
	char	value[128];
};

typedef struct{
	int num;
	char addr[50];
}stu_usr;


void* RecvThread(void *arg);

int write_id_table(char *file,int num,int *id_table);

void trans_encrypt(unsigned char * data,int length);

void XorEncryptDecrypt(unsigned char * Veindata, int length, int flag);

unsigned char **test_Make2DArray_uint8(int row,int col);

void test_Free2DArray_uint8(unsigned char **a,int row);

int tg_get_usr_num(char* addr);

int tg_get_usr_dat(char* addr,stu_usr *usr_addr,unsigned char *dat);

int init_watchdog(int timeout);

int feed_watchdog();

int init_Device();

int release_Device(int fd);
int initComm(char* ttyDir,unsigned speed);
//int writeComm(int fd, char* buf, int len);
//int readComm(int fd, char* buf, int len, int timeout);
void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
int read_file(char *buf,int length,char *path);
int read_data_hex(unsigned char *buf,int length,char *string);
int write_data_hex(unsigned char * array,int length,char *string);
int Compute_string_md5(unsigned char *dest_str, unsigned int dest_len, char *md5_str)
;
int file_size(char* filename)  ;
int file_save(struct package *pack,char *buf);
int sendPackage(int fd,char *heard,int id,int cmd,char *value);
int sendDataPackage(int fd,char *data,int len);
int receivePackage(int fd,char *pack,int timeout);
int receiveDataBlock(int fd,char *data,int len,int timeout);
int resolvePacgage(struct package *pack,char *dat);

void base64_encode(unsigned char *src, int src_len, unsigned char *dst);
void base64_decode(unsigned char *src, int src_len, unsigned char *dst);

int request_buf(char *buf,char *tmp,int size);
int free_buf(char *buf,char *tmp);

int TG_RecvPackage(int fd,char *heard,int *id,int *cmd,int *length,char* value,char*buf);
int TG_SendPackage(int fd,char *heard,int id,int cmd,int length,char* value,char* buf);

void TG_SaveBmp(unsigned char *src,int height,int width,char *bmp_addr);

//int net_service_run();
//int net_client_run();
int TG_NetRecvPackage(int fd,char *heard,int *id,int *cmd,int *length,char* value,char*buf);
int TG_NetSendPackage(int fd,char *heard,int id,int cmd,int length,char* value,char* buf);
int net_client_connect(char *addr,int port);
int net_service_init(char *addr,int port);
int net_service_accept(int sockfd);



#endif
