#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>
#include <linux/watchdog.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include "tgapi.h"
#include "md5.h"
#include "Bmp.h"

char recv_buf[BUFSIZE];
struct package p;

pthread_t pthread_recv_tid;
pthread_mutex_t mutex_thread;

int fd_hid = -1;
int recv_flag = 0;
int get_fw_flag = 0;
int get_sn_flag = 0;
int get_camera_flag = 0;
int get_feature_flag = 0;

char sn_str[100];
char fw_str[100];
//unsigned char camera_data[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT] = {0};
unsigned char camera_data[LABELIMG_SIZE] = {0};
unsigned char feature[CAMERA_ROI_WIDTH*CAMERA_ROI_HEIGHT] = {0};

TAG_HULL tag;

void printf_tag()
{
	printf("herad:%s length:%d\n",tag.heard,tag.length);
	printf("fw_tag:%x%x fw_value:%s\n",tag.fw_tag[0],tag.fw_tag[1],tag.fw_value);
	printf("sn_tag:%x%x sn_value:%s\n",tag.sn_tag[0],tag.sn_tag[1],tag.sn_value);
	printf("type_tag:%x%x type_value:%x\n",tag.type_tag[0],tag.type_tag[1],tag.type_value);
	printf("encode_tag:%x%x encode_value:%x\n",tag.encode_tag[0],tag.encode_tag[1],tag.encode_value);
	printf("encrypt_tag:%x%x encrypt_value:%x\n",tag.encrypt_tag[0],tag.encrypt_tag[1],tag.encrypt_value);
	printf("key_tag:%x%x key_value:%x\n",tag.key_tag[0],tag.key_tag[1],tag.key_value);
	printf("tgc_tag:%x%x tgc_value:%s\n",tag.tgc_tag[0],tag.tgc_tag[1],tag.tgc_value);
	printf("tgf_tag:%x%x tgf_value:%s\n",tag.tgf_tag[0],tag.tgf_tag[1],tag.tgf_value);
	printf("sub_tag:%x%x sub_value:%d\n",tag.sub_tag[0],tag.sub_tag[1],tag.sub_value);
	printf("time_tag:%x%x time_value:%s\n",tag.time_tag[0],tag.time_tag[1],tag.time_value);
	printf("env_tag:%x%x env_value:%d\n",tag.env_tag[0],tag.env_tag[1],tag.env_value);
	printf("data_tag:%x%x data_length:%d data_width:%d data_height:%d\n",tag.data_tag[0],tag.data_tag[1],tag.data_length,tag.data_width,tag.data_height);

	
}
void* RecvThread(void *arg)
{
    int  ret,sockfd;
	recv_flag=0;
#if defined(HID_DEV_H3)||defined(HID_DEV_JZ)
	while(!recv_flag)
	{

		fd_hid=init_Device();		
		if(fd_hid<0) {
			usleep(100*1000);
			continue;
		}
#endif

#ifdef NET_DEV
    sockfd=net_service_init(NULL,PORT);
    while(!recv_flag)
    {
        usleep(50*1000);
	//	printf("sockfd = %d\n",sockfd);
	//	printf("net_service_accept\n");
        fd_hid=net_service_accept(sockfd);
	//	printf("fd = %d\n",fd_hid);
        if(fd_hid<0) continue;
#endif

        while(!recv_flag)
        {
            usleep(1000);
            memset(&p,'\n',sizeof(p));
         //   mutex_pack.lock();
            ret=TG_NetRecvPackage(fd_hid,p.heard,&p.id,&p.cmd,&p.length,p.value,recv_buf);
         //   mutex_pack.unlock();
        // printf("ret %d\n",ret);
            if(ret==0)
            {
                continue;
            }
            if(ret==1)
            {

                switch(p.cmd)
                {
                    case 450:
                      //  printf("didn't recv camera data\n");
						ret=-450;
                        break;
                    case 451:
                       // printf("encrypt/decrypt failed\n");
						//ret=RET_CRYPT_FAIL;
                        break;
                    case 21:
                        switch(p.id)
                        {
                        	case 0:
								//printf("this is tag hull data ,id = 1.  len=%d\n",p.length);
								//memcpy((char *)&tag,recv_buf,p.length);
								//printf_tag();
								//printf("*************************data_Length:%d\n",tag.data_length);
                                //memcpy(camera_data,tag.data_value,tag.data_length);
                                //memcpy(camera_data,&tag,tag.length);
                                memcpy(camera_data,recv_buf,p.length);
                               	
								//get_camera_flag = 1;
								get_camera_flag = p.length;
								break;
                            case 1:
                                //printf("this is feature data ,id = 1.  len %d\n",p.length);
								memcpy(feature,recv_buf,p.length);
								get_feature_flag=1;
                               // if(localPath.isEmpty()) localPath=p.value;
                                //write_data_hex((unsigned char*)recv_buf,p.length,p.value);
                                break;

                            case 2:
                              //  printf("this is camera data ,id = 2.\n");
#ifdef DEBUG_PRINT
                                printf("camera data recv_length =%d \n",p.length);
#endif
                                memcpy(camera_data,recv_buf,p.length);
								get_camera_flag = 1;
                                break;

                            case 3:
                              //  printf("this is H3 recv file done\n");
                              //  write_data_hex((unsigned char*)recv_buf,p.length,p.value);
								//ret=RECV_CMD_SAVE_H3_FILE_DATA;
                                break;
                            case 4:
                               // printf("this is TF recv file done\n");
                               // write_data_hex((unsigned char*)recv_buf,p.length,p.value);
								//ret=RECV_CMD_SAVE_TF_FILE_DATA;
                                break;
                            case 5:
                              //  printf("this is encrypt/decrypt done\n");
                              //  write_data_hex((unsigned char*)recv_buf,p.length,"./code/out.dat");
								//ret=RECV_CMD_SAVE_CRYPT_FILE_DATA;
                                break;
							case 6:
                              //  printf("this is fw done\n");
                             //   printf("fw recv_length =%d %s\n",p.length,recv_buf);
                                memcpy(fw_str,recv_buf,p.length);
								get_fw_flag = 1;;
                                break;
							case 7:
								//	printf("this is sn done\n");
								//printf("sn recv_length =%d %s\n",p.length,recv_buf);
								memcpy(sn_str,recv_buf,p.length);
								get_sn_flag = 1;;
								break;

                            case 500://beat~~
#ifdef DEBUG_PRINT
								printf("beat~~\n");
#endif
	                            continue;
                            default:
                                printf("error id =%d\n",p.id);
                                break;
                        }
                        break;
                    default:
					    printf("error cmd =%d\n",p.cmd);
                        break;
                }
            }
            else
            {
#ifdef DEBUG_PRINT
                printf("RecvThread  :RecvPackage fail: %d\n",ret);
#endif
                close(fd_hid);
				fd_hid=-1;
                break;
            }
        }
    }
	close(fd_hid);
	fd_hid=-1;
	recv_flag=2;
//	close(sockfd);
#ifdef DEBUG_PRINT
	printf("recv_service stop ..\n");
#endif
}

int initComm(char* ttyDir,unsigned speed)
{
  int retry = 60;


  int mfd = open(ttyDir, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
  if(mfd < 0)
  {
    //perror(ttyDir);
    return -1;
  }


  while(lockf(mfd, F_TLOCK, 0) < 0)
  {
    sleep(1);
    retry--;
    if(retry <= 0)
    {
      printf("Devcie %s locked.\n", ttyDir);
      close(mfd);
      return -1;
    }
  }
    switch(speed)
    {
        case 4800:speed=B4800;break;
        case 9600:speed=B9600;break;
        case 19200:speed=B19200;break;
        case 38400:speed=B38400;break;
        case 57600:speed=B57600;break;
        case 115200:speed=B115200;break;
        case 230400:speed=B230400;break;
        case 460800:speed=B460800;break;
        default:printf("speed eer");return -1;
    }
  struct termios Opt;
  tcgetattr(mfd, &Opt);
  cfsetispeed(&Opt,speed);
  cfsetospeed(&Opt,speed);
  tcsetattr(mfd, TCSANOW, &Opt);
  tcflush(mfd, TCIOFLUSH);

  tcgetattr(mfd, &Opt);

  Opt.c_cflag &= ~CSIZE;
  Opt.c_cflag |= CS8;
 // Opt.c_cflag |=CBAUD;
  Opt.c_cflag   |= IXON|IXOFF|IXANY;                                 /* ?????? 8 */
  Opt.c_cflag &= ~PARENB;                               /* Clear parity enable */
  Opt.c_cflag &= ~CSTOPB;                               /* ?????? 1 */
  Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  Opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  Opt.c_oflag  &= ~OPOST;

  Opt.c_cc[VTIME] = 150;                                /* ????????15 seconds*/
  Opt.c_cc[VMIN] = 0;                                   /* Update the options and do it NOW */

  if(tcsetattr(mfd, TCSANOW, &Opt) < 0)
  {
    perror(ttyDir);
    close(mfd);
    return -1;
  }
  tcflush(mfd, TCIOFLUSH);

  return mfd;
}

int writeComm(int fd, char* buf, int len)
{
#ifdef HID_DEV_H3
	int ret,packSize=4088,skip=8,length=0,block;
	char heard[8];
	
	memset(heard,0,8);
	heard[0]=1;

	length=len;
	while(length>0){
		if(length>packSize) block=packSize+skip;
		else block=length+skip;

		
		
		write(fd,heard,block);
		ret=write(fd,&buf[len-length],block);

		length-=ret;
		length+=skip;
		//printf("%d %d %d %d %d\n",len,length,ret,len-length,block);	
		if(length<0||ret<0)break;	
		
		if(skip)usleep(20000);
	}
#endif
#ifdef HID_DEV_JZ
		int ret,packSize=4088,skip=0,length=0,block;

		length=len;
		while(length>0){
			if(length>packSize) block=packSize+skip;
			else block=length+skip;

			ret=write(fd,&buf[len-length],block);
	
			length-=ret;
			length+=skip;
			//printf("%d %d %d %d %d\n",len,length,ret,len-length,block);	
			if(length<0||ret<0)break;	
			
			if(skip)usleep(20000);
		}
#endif

#ifdef NET_DEV
	int rs = write(fd, buf, len);
	if(rs == -1)
	{
	  perror("writeComm:");
	  return -1;
	}

#endif

  return 0;
}

int readComm(int fd, char* buf, int len, int timeout /* ???? */)
{
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  struct timeval tv = {0, 1000*timeout};
  int total = len,err_count=len/1000,rs=0;
  char* startRev = buf;
  while(total > 0)
  {
    int res = select(fd+1, &fds, NULL, NULL, &tv);
    if(res == -1)
    {
      if(errno == EINTR)
        continue;
      else
      {
        printf("socket error %d with select", errno);
        return -1;
      }
    }
    else if(res == 0)
    {err_count=0;
      buf[len - total] = '\0';
      return len - total;
    }

    if(FD_ISSET(fd, &fds))
    {
      rs = read(fd, startRev, len);
      if(rs < 0)
      {
        buf[len - total] = '\0';
        perror("readComm:");
        return -1;
      }
       //printf("total: %d %d\n", total,rs);
      total -= rs;
      startRev += rs;
    }
	if(!rs) return -1;

    if(err_count--<0)
    {
        //qDebug()<<res<<len/1000<<err_count--;
      //  usleep(100000);
        return len - total;
    }
  }

 // printf("readComm: %s\n", buf);
  return len - total;
}
void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
    unsigned char h1,h2;
    unsigned char s1,s2;
    int i;

    for (i=0; i<nLen; i++)
    {
        h1 = pbSrc[2*i];
        h2 = pbSrc[2*i+1];

        s1 = toupper(h1) - 0x30;
        if (s1 > 9)
        s1 -= 7;

        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
        s2 -= 7;

        pbDest[i] = s1*16 + s2;
    }
}
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
    unsigned char ddl,ddh;
    int i;

    for (i=0; i<nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        pbDest[i*2] = ddh;
        pbDest[i*2+1] = ddl;
    }

    pbDest[nLen*2] = '\0';
}
int read_file(char *buf,int length,char *path)
{
    int fd,ret;
    fd=open(path,O_RDONLY);
    if(fd<0) return -1;
        ret=read(fd,buf,length);
        close(fd);
        return ret;
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
/**
 * compute the value of a string
 * @param  dest_str
 * @param  dest_len
 * @param  md5_str
 */
int Compute_string_md5(unsigned char *dest_str, unsigned int dest_len, char *md5_str)
{
    int i;
    unsigned char md5_value[MD5_SIZE];
    MD5_CTX md5;

    // init md5
    MD5Init(&md5);

    MD5Update(&md5, dest_str, dest_len);

    MD5Final(&md5, md5_value);

    // convert md5 value to md5 string
    for(i = 0; i < MD5_SIZE; i++)
    {
        snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
    }

    return 0;
}
int file_size(char* filename)
{
    //int fd,r;
        /*FILE *fp=fopen(filename,"r");
        if(!fp) return -1;
        fseek(fp,0L,SEEK_END);
        int size=ftell(fp);
        fclose(fp);
          */
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;
/*
    fd=open(filename,O_RDONLY);
    if(fd<0) return -1;
    read(fd,str_to_pc,size);
    printf("[%d %d]\n",size,r);
    close(fd);*/
    return size;
}
int file_save(struct package *pack,char *buf)
{
    int fd;

    fd=open(pack->value,O_RDWR|O_CREAT);
    if(fd<0) return -1;
    write(fd,buf,pack->length);
    close(fd);
    chmod(pack->value, S_IRWXU|S_IRWXG|S_IRWXO);
}
int sendPackage(int fd,char *heard,int id,int cmd,char *value)
{
    char dat[PACKAGE_SIZE];
    memset(dat,'\0',sizeof(dat));
    sprintf(dat,"writeComm = device:%s id:%d cmd:%d value:%s",heard,id,cmd,value);
	//printf("%s\n",dat);
   	trans_encrypt(dat,strlen(dat));
    return writeComm(fd,dat,strlen(dat));
}
int sendDataPackage(int fd,char *data,int len)
{
	usleep(1000*250);
//	trans_encrypt(data,len);
   	return writeComm(fd,data,len);
}
int receivePackage(int fd,char *pack,int timeout)
{
	int ret;
	ret = readComm(fd,pack,PACKAGE_SIZE,timeout);
//	printf("receivePackage = %s\n",pack);
//	printf("receivePackage = %s  ret = %d\n",pack,ret);
	trans_encrypt(pack,strlen(pack));
#ifdef DEBUG_PRINT
	printf("receivePackage = %s  ret = %d\n",pack,ret);
#endif
	return ret;
}
int receiveDataBlock(int fd,char *data,int len,int timeout)
{
    int r,length=0,try1=30;
    while(try1--){
        r=readComm(fd,&data[length],len-length,timeout);
        if(r>0) 
			length+=r;
#ifdef DEBUG_PRINT
        printf("--num:%d---block:%d--%d---%d-\n",try1,r,length,len);
#endif
        if(length>=len||r<0) break;
    }
	//printf("%s\n[%d][%d]\n",data,strlen(data),length);
//	trans_encrypt(data,length);
    return length;
}
int resolvePacgage(struct package *pack,char *dat)
{
        char *p,n;
        memset(pack,0,sizeof(struct package));
        p=strstr(dat,"device:");
        if(p!=NULL){
            n=0;p+=7;
            while(*(p+n)!=' '&&n<32) n++;
            memcpy(pack->heard,p,n+1);
        }else return -1;
        p=strstr(dat,"id:");
        if(p!=NULL){
            n=0;p+=3;
            while(*(p+n)!=' '&&n<32) n++;
            pack->id=atoi(p);
        }else return -2;
        p=strstr(dat,"cmd:");
        if(p!=NULL){
            n=0;p+=4;
            while(*(p+n)!=' '&&n<32) n++;
            pack->cmd=atoi(p);
        }else return -3;
        p=strstr(dat,"value:");
        if(p!=NULL){
            n=0;p+=6;
            while(*(p+n)!=' '&&n<32) n++;
            memcpy(pack->value,p,n);
        }else return -4;
        p=strstr(dat,"length:");
        if(p!=NULL){
            n=0;p+=7;
            while(*(p+n)!=' '&&n<32) n++;
            pack->length=atoi(p);
        }
        p=strstr(dat,"md5:");
        if(p!=NULL){
            n=0;p+=4;
            while(*(p+n)!=' '&&n<32) n++;
            memcpy(pack->md5,p,n);
        }
      //  printf("client connetct %d --> device:%s id:%d cmd:%d value:%s length:%d\n",
      //  num++,pack->heard,pack->id,pack->cmd,pack->value,pack->length);
}

int TG_RecvPackage(int fd,char *heard,int *id,int *cmd,int *length,char* value,char*buf)
{
    char rx[PACKAGE_SIZE],md5[32],*tmp=NULL;
    int len,ret=1;
    struct package pack;

    if(fd<0) return -5;

//    mutex_pack.lock();

    memset(rx,0,sizeof(rx));
    memset(&pack,0,sizeof(pack));

    len=receivePackage(fd,rx,200);
    if(len==0) return 0;
    if(len<0) return -2;

    resolvePacgage(&pack,rx);

    if(pack.cmd==21&&pack.length>0){//file or data block receive
      //  if(sendPackage(fd,pack.heard,pack.id,pack.cmd,"ok")>=0) {

            tmp=(char *)malloc(pack.length*2+2);if(tmp==NULL) return -1;

            receiveDataBlock(fd,tmp,pack.length,200);
            StrToHex((unsigned char*)buf,(unsigned char*)tmp,pack.length);

            pack.length=(pack.length>>1);
           // buf++;//skip 1B data
            Compute_string_md5((unsigned char*)buf,pack.length,md5);
            printf("%s ", md5);

            if(strncmp(pack.md5,md5,32)==0){//success
                //file_save(&pack,buf);
               // TG_SaveBmp((unsigned char*)buf,480,640,pack.value);
                ret=1;
            }
            else {//fail
                ret=-3;
            }
            //printf("%s %d %d %s %d %d %s %d\n",pack.heard,pack.id,pack.cmd,pack.value,pack.length,len,md5,++num);
      //  }
     //   else ret=-4;
    }
    if(pack.id==0&&pack.cmd==0)ret=-6;

    *id = pack.id;
    *cmd = pack.cmd;
    *length = pack.length;
    strcpy(value,pack.value);
    strcpy(heard,pack.heard);

    if(tmp!=NULL) free(tmp);

//    mutex_pack.unlock();

    return ret;
}


int TG_SendPackage(int fd,char *heard,int id,int cmd,int length,char* value,char* buf)
{
    int ret=1;
    char rx[PACKAGE_SIZE],cmdline[PACKAGE_SIZE],*tmp=NULL;
    struct package pack;

    if(fd<0) return -5;

   // mutex_pack.lock();

    memset(rx,0,sizeof(rx));
    memset(&pack,0,sizeof(pack));
    memset(cmdline,0,sizeof(cmdline));

    if(cmd==21&&length>0){//send file or data block
        tmp=(char *)malloc(length*2+2);if(tmp==NULL) return -1;
        Compute_string_md5((unsigned char*)buf,length,pack.md5);
        //printf("%s\n", pack.md5);

        HexToStr((unsigned char*)tmp,(unsigned char*)buf,length);
        length*=2;

        sprintf(cmdline,"%s length:%d md5:%s",value,length,pack.md5);
        if(sendPackage(fd,heard,id,cmd,cmdline)>=0){
           /* if(receivePackage(fd,rx,2000)<0){
                printf("-----------receivePackage fail ack !!---------------\n");
                ret=-2;
            }
            else{
                resolvePacgage(&pack,rx);
                if(strcmp(pack.value,"ok")==0){
                    ret=sendDataPackage(fd,tmp,length);
                }
                else ret=-3;
            }*/
            ret=sendDataPackage(fd,tmp,length);
        }
        else ret=-4;
    }
    else{//normal cmd data
        if(value!=NULL) sprintf(cmdline,"%s length:%d",value,length);
        if(sendPackage(fd,heard,id,cmd,cmdline)<0)
            ret=-4;
    }


    if(tmp!=NULL) free(tmp);
   // mutex_pack.unlock();
    return ret;
}
int init_Device()
{
    int fd,i,res;
    char dev[64];

#if defined(HID_DEV_H3)||defined(HID_DEV_JZ)
	for(i=0;i<5;i++){
		int tmp[32];
		memset(tmp,0,sizeof(tmp));
		struct hidraw_devinfo info;
		memset(dev,0,sizeof(dev));
		sprintf(dev,"/dev/hidraw%d",i);
		fd = open(dev, O_RDWR/*|O_NONBLOCK*/);
		if(fd>0) {
			/* Get Raw Info */
			res = ioctl(fd, HIDIOCGRAWINFO, &info);
			if (res < 0) {
				perror("HIDIOCGRAWINFO");
			} else {
			//	printf("Raw Info:\n");
			//	printf("\tvendor: 0x%04hx\n", info.vendor);
			//	printf("\tproduct: 0x%04hx\n", info.product);
				sprintf(tmp,"0x%04hx 0x%04hx",info.vendor,info.product);
				if(strstr(tmp,"0x0525")!=NULL&&strstr(tmp,"0xa4ac")!=NULL)
					return fd;
			}			
		}
	}
#endif

#ifdef COM_DEV


    for(i=0;i<5;i++){
        memset(dev,0,sizeof(dev));
        sprintf(dev,"/dev/ttyACM%d",i);
        fd=initComm(dev,9600);
        if(fd>0) return fd;

        memset(dev,0,sizeof(dev));
        sprintf(dev,"/dev/ttyGS%d",i);
        fd=initComm(dev,115200);
        if(fd>0) return fd;

    }

#endif
    return -1;
}
int release_Device(int fd)
{

    close(fd);
}
/*int wd_fd;
int init_watchdog(int timeout)
{
    wd_fd = open("/dev/watchdog",O_RDWR);
    if(wd_fd < 0)
    {
        printf("wd open failed\n");
        return -1;
    }

    ioctl(wd_fd, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);
    ioctl(wd_fd, WDIOC_SETTIMEOUT, &timeout);
}
int feed_watchdog()
{
    ioctl(wd_fd,WDIOC_KEEPALIVE,NULL);
}
int release_watchdog()
{
    close(wd_fd);
}
*/

