#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntoa()??????
#include <unistd.h>   
#include <pthread.h>
#include <signal.h>

#include "tgapi.h"

int net_client_connect(char *addr,int port)
{
	  int sockfd;

	  struct sockaddr_in server_addr; 
	  struct hostent *host;
	  int nbytes;
	
	  if((host=gethostbyname(addr))==NULL)
	  {
		  fprintf(stderr,"Gethostname error\n");
		  return -1;
	  }
	
	  if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	  {
		  fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
		  return -1;
	  }
	
	  bzero(&server_addr,sizeof(server_addr)); 
	  server_addr.sin_family=AF_INET; // IPV4
	  server_addr.sin_port=htons(port); 
	  server_addr.sin_addr=*((struct in_addr *)host->h_addr); // 

	  if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
	  {
		  fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
          close(sockfd);
		  return -1;
	  }

	  return sockfd;

}
int net_service_init(char *addr,int port)
{
	int sockfd;
	struct sockaddr_in server_addr; //

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
    {
        fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
        exit(1);
    }

    bzero(&server_addr,sizeof(struct sockaddr_in)); // ???,?0
    server_addr.sin_family=AF_INET; // Internet
    if(addr!=NULL)
		server_addr.sin_addr.s_addr=inet_addr(addr); //?????????IP,inet_addr?????????ip?????ip
	else
    	server_addr.sin_addr.s_addr=htonl(INADDR_ANY); // (??????long?????????long??)??????? //INADDR_ANY ????????IP?????????????IP

    server_addr.sin_port=htons(port); // (??????short?????????short??)???

	unsigned int value = 0x1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&value,sizeof(value)); 

    if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
    {
        fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
        exit(1);
    }

    if(listen(sockfd,5)==-1)
    {
        fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
        exit(1);
    }

	return sockfd;

}
int net_service_accept(int sockfd)
{
    int new_fd;
    struct sockaddr_in client_addr; 
    int sin_size;

	sin_size=sizeof(struct sockaddr_in);
    if((new_fd=accept(sockfd,NULL,NULL))==-1)
	{
		fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
		return -1;
	}
    //fprintf(stderr,"Server get connection from %s\n",inet_ntoa(client_addr.sin_addr)); // ????????.????????????
	
	return new_fd;
}
int TG_NetRecvPackage(int fd,char *heard,int *id,int *cmd,int *length,char* value,char*buf)
{
	char rx[PACKAGE_SIZE],md5[32];
	int len,ret=1;
	struct package pack;

    if(fd<0) return -5;
	memset(rx,0,sizeof(rx));
	memset(&pack,0,sizeof(pack));

	len=receivePackage(fd,rx,100);
	if(len==0) return 0;
	if(len<0) return -2;

	resolvePacgage(&pack,rx);
#ifdef DEBUG_PRINT
	printf("rx = %s,len = %d\n",rx,len);
#endif
	if(pack.cmd==21&&pack.length>0){//file or data block receive
       // if(sendPackage(fd,pack.heard,pack.id,pack.cmd,"ok")>=0) {
			receiveDataBlock(fd,buf,pack.length,500);
            Compute_string_md5((unsigned char*)buf,pack.length,md5);
			//printf("%s %s\n",pack.md5, md5);

			if(strncmp(pack.md5,md5,32)==0){//success
//				file_save(&pack,buf);
				ret=1;
			} 
			else {//fail
				ret=-3;
			}
      //  }
     //   else ret=-4;
	}
	if(pack.id==0&&pack.cmd==0)ret=-6;

	*id = pack.id;
	*cmd = pack.cmd;
	*length = pack.length;
	strcpy(value,pack.value);
	strcpy(heard,pack.heard);

	return ret;			
}


int TG_NetSendPackage(int fd,char *heard,int id,int cmd,int length,char* value,char* buf)
{
	int ret=1;
	char rx[PACKAGE_SIZE],cmdline[PACKAGE_SIZE],md5[32];
	struct package pack;

    if(fd<0) return -5;
	memset(rx,0,sizeof(rx));
	memset(&pack,0,sizeof(pack));
	memset(cmdline,0,sizeof(cmdline)); 

	if(cmd==21&&length>0){//send file or data block
        Compute_string_md5((unsigned char*)buf,length,pack.md5);
		

		sprintf(cmdline,"%s length:%d md5:%s",value,length,pack.md5);
	
		if(sendPackage(fd,heard,id,cmd,cmdline)>=0){
         /*   if(receivePackage(fd,rx,2000)<0){
				printf("receivePackage fail ack !!\n");
				ret=-2;
			}
			else{
				resolvePacgage(&pack,rx);
				if(strcmp(pack.value,"ok")==0){
					ret=sendDataPackage(fd,buf,length); 
				}
				else ret=-3;				
            }*/
            ret=sendDataPackage(fd,buf,length);
		 	printf("%s\n", pack.md5);
		}
		else ret=-4;
	}
	else{//normal cmd data
		if(value!=NULL) sprintf(cmdline,"%s",value);
		if(sendPackage(fd,heard,id,cmd,cmdline)<0)
			ret=-4;
	}

	return ret;
}
/*
int net_client_test()
{
	int fd=-1,ret;
	char filename[]="280.bmp";

	int num=0,restart=0;
	char value[64],heard[64];
	struct package pack;

	int id=0,cmd=0,length=0;


	while(1){

		sleep(1);
 		  
		if(fd<0){
			fd=net_client_connect(SERVICE_ADDR,PORT);
			if(fd<0) continue;
		}

		for(;;){
			ret=TG_NetRecvPackage(fd,heard,&id,&cmd,&length,value,recv_buf);
			if(ret==0) {//idlesse status
				length=file_size(filename);
				if(length>BUFSIZE) {printf("file length > 16MB\n");return -1;}
                read_data_hex((unsigned char*)send_buf,length,filename);

				ret=TG_NetSendPackage(fd,"TG-660",1,21,length,filename,send_buf);
				if(ret==1||ret==0) {printf("SendPackage ok %d\n",num++);}
				else if(ret==-3) {printf("Package ack fail: %d !!\n",ret);}
				else {
					printf("SendPackage fail: %d !!\n",ret);
					fd=-1;
					break;
				}	
				continue;
			}
			else if(ret==1){ //normal cmd status
			
				printf("data ok..%s %d %d %d %s %d\n",heard,id,cmd,length,value,num++);
			}
			else {//error status
				printf("SendPackage fail: %d !!\n",ret);
				close(fd);
				break;
			}
		}

	}
	
	close(fd);	
	
}

int net_service_test()
{
	int fd=-1,sockfd=-1,ret;
	char filename[]="3.3.pdf";

	int num=0,restart=0;
	char value[64],heard[64];
	struct package pack;

	int id=0,cmd=0,length=0;

	

	sockfd=net_service_init(NULL,PORT);


    while(1)
    {
    	sleep(1);
 		
    	fd=net_service_accept(sockfd);
		if(fd<0) continue;

		for(;;){//idlesse status
			ret=TG_NetRecvPackage(fd,heard,&id,&cmd,&length,value,recv_buf);
			if(ret==0) {

				continue;
			}
			else if(ret==1){//normal cmd status 
		
				printf("data ok..%s %d %d %d %s %d\n",heard,id,cmd,length,value,num++);
			}
			else {//error status
				printf("SendPackage fail: %d !!\n",ret);
				close(fd);
				break;
			}
		}
    }

	close(fd);
    close(sockfd);
}
*/






