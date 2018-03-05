#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/time.h>
#include <dirent.h> 

#define WLANLIST "/etc/WlanList"
#define STR_MAX_LEN 256

/*****************************************************************  
** @brief       get file's line num
** @author      xuhc
** @param1      <in>  <char *>          path of file
** @exception
** @return     -1:fail
**             else:line num
** @note       
** @see 
** @date       2017/09/21
******************************************************************/
static int get_line_num(char *path)
{
	FILE *fp;
	int i = 0,c;
	fp=fopen(path ,"r"); 
	if (fp!=NULL)
	{
		while ((c=fgetc(fp))!=EOF)
		{ 
			if(c=='\n')
				i++;
		}
	}
	else
	{
		return -1;
		fclose(fp);
	}
	fclose(fp);
	return i;
}

/*****************************************************************  
** @brief       get wlan num
** @author      xuhc
** @exception
** @return     -1    : fail
**             else  : wlan num,from 0
** @note       
** @see 
** @date       2018/03/02
******************************************************************/
static int get_wlan_num(void)
{
	int wlan_num;
	wlan_num = get_line_num(WLANLIST);
	wlan_num = wlan_num/2;
	printf("wlan_num = %d\n",wlan_num);
	return wlan_num;
}

/*****************************************************************  
** @brief       get wlan name
** @author      xuhc
** @param1      <out>  <char *>         wlan name
** @param2      <in>  <int>          	wlan number,from 1
** @exception
** @return     -1 : fail
**             0  : success
** @note       
** @see 
** @date       2018/02/26
******************************************************************/
static int get_wlan_name(char *wlan_name,int wlan_num)
{
	FILE *fp=fopen(WLANLIST, "r+");
	int i;
	int length;
	char str[STR_MAX_LEN]; 
	char *pstr = NULL;
	int line_num;
	line_num = (wlan_num - 1)*2;
	if (fp==NULL )
		return -1;
	for(i = 0;i<line_num;i++ ) 
	{
		fgets(str,sizeof(str),fp);
	}
	fgets(str,sizeof(str),fp);
	pstr = strstr(str,"\"");
	pstr++;
	length = strlen(pstr);
	*(pstr+length-2) = '\0';
	strcpy(wlan_name,pstr);
	fclose(fp);
	return 0;
}


/*****************************************************************  
** @brief       get wlan quality
** @author      xuhc
** @param1      <in>  <int>          wlan number,from 1 
** @exception
** @return     -1    : fail
**             else  : wlan quality
** @note       
** @see 
** @date       2018/02/26
******************************************************************/
static int get_wlan_quality(int wlan_num)
{
	FILE *fp=fopen(WLANLIST, "r+");
	int i;
	int length;
	char str[STR_MAX_LEN]; 
	char *pstr = NULL;
	int quality;
	int line_num;
	line_num = (wlan_num - 1) * 2 + 1;
	if (fp==NULL )
		return -1;
	for(i = 0;i<line_num;i++ ) 
	{
		fgets(str,sizeof(str),fp);
	}
	fgets(str,sizeof(str),fp);
//	printf("str=%s.\n",str);
	pstr = strstr(str,"Quality=");
	pstr += 8;
//	printf("pstr = %s\n",pstr);
	length = strlen(pstr);
	for(i = 0;i<length-1;i++ ) 
	{
		if(*(pstr+i) == '/')
			*(pstr+i) = '\0';
	}
//	printf("pstr = %s\n",pstr);
	quality = atoi(pstr);
//	printf("Quality = %d\n",quality);
	fclose(fp);
	return quality;
}

/*****************************************************************  
** @brief       modify wlan
** @author      xuhc
** @param1      <in>  <char *>          wlan name
** @param2      <in>  <char *>          wlan password
** @param3      <in>  <int>         	wlan number,from 1
** @exception
** @return     -1    :  fail
**             0     :  success
** @note       
** @see 
** @date       2018/02/26
******************************************************************/
static int modify_wlan(char *wlan_name,char *wlan_passwd,int wlan_num)
{
	
	char command[STR_MAX_LEN];
	sprintf(command,"wpa_cli -i wlan1 set_network %d ssid '\"%s\"'",wlan_num,wlan_name);
	printf("modify_wlan command = %s\n",command);
	system(command);
	sprintf(command,"wpa_cli -i wlan1 set_network %d psk '\"%s\"'",wlan_num,wlan_passwd);
	printf("modify_wlan command = %s\n",command);
	system(command);
	sprintf(command,"wpa_cli -i wlan1 enable_network %d",wlan_num);
	printf("modify_wlan command = %s\n",command);
	system(command);
	sprintf(command,"wpa_cli -i wlan1 save_config");
	printf("add_wlan command = %s\n",command);
	system(command);
}

/*****************************************************************  
** @brief       add a new wlan
** @author      xuhc
** @param1      <in>  <char *>          new wlan name
** @param2      <in>  <char *>          new wlan password
** @param3      <in>  <int>         	new wlan number
** @exception
** @return     -1    :  fail
**             else     :  new_wlan_num
** @note       
** @see 
** @date       2018/02/28
******************************************************************/
static int add_wlan(char *new_wlan_name,char *new_wlan_passwd)
{
	char command[STR_MAX_LEN];
	char wlan_num_str[STR_MAX_LEN];
	int wlan_num;
	FILE *temp_stream;
	temp_stream = popen("wpa_cli -i wlan1 add_network","r");

	fread(wlan_num_str, sizeof(char), sizeof(wlan_num_str), temp_stream);
	wlan_num = atoi(wlan_num_str);

	sprintf(command,"wpa_cli -i wlan1 set_network %d ssid '\"%s\"'",wlan_num,new_wlan_name);
	printf("add_wlan command = %s\n",command);
	system(command);
	sprintf(command,"wpa_cli -i wlan1 set_network %d psk '\"%s\"'",wlan_num,new_wlan_passwd);
	printf("add_wlan command = %s\n",command);
	system(command);
	sprintf(command,"wpa_cli -i wlan1 enable_network %d",wlan_num);
	printf("add_wlan command = %s\n",command);
	system(command);
	sprintf(command,"wpa_cli -i wlan1 save_config");
	printf("add_wlan command = %s\n",command);
	system(command);

	pclose(temp_stream);
	return wlan_num;
}

/*****************************************************************  
** @brief       turn off wlan1
** @author      xuhc
** @exception
** @return     void
** @note       
** @see 
** @date       2018/03/02
******************************************************************/
static void down_wlan()
{
	system("ifconfig wlan1 down");

}

/*****************************************************************  
** @brief       turn on wlan1
** @author      xuhc
** @exception
** @return     void
** @note       
** @see 
** @date       2018/03/02
******************************************************************/
static void up_wlan()
{
	system("ifconfig wlan1 up");
}

int main( int argc, char** argv )
{
	int wlan_num;
	int i;
	int new_wlan_num;
	char wlan_name[STR_MAX_LEN];
	char command[STR_MAX_LEN];
	int connect_name_id;
	char connect_name[STR_MAX_LEN] = "asd";
	char connect_passwd[STR_MAX_LEN] = "jkl";
	int wlan_quality;
	system("iwlist wlan1 scan > /etc/WlanList ");
	system("grep -E \"SSID|Quality\" /etc/WlanList > /etc/grepWlanList");
	system("grep -v \"x00\" /etc/grepWlanList > /etc/WlanList");

	wlan_num = get_wlan_num();
	printf("wlan_num = %d\n",wlan_num);
	printf("\n-----------wlan list------------\n");

	for(i = 1;i<= wlan_num;i++)
	{
		get_wlan_name(wlan_name,i);
		wlan_quality = get_wlan_quality(i);
		printf("%d\t%s\t%d\n",i,wlan_name,wlan_quality);
	}
	printf("--------------------------------\n\n");


//---------------------------------------add_wlan------------------------------------------------
	printf("plesae input which wifi to connect:\n");
	scanf("%d",&connect_name_id);
	get_wlan_name(connect_name,connect_name_id);
	printf("plesae input wifi passwd:\n");
	scanf("%s",connect_passwd);
	new_wlan_num = add_wlan(connect_name,connect_passwd);
	printf("new_wlan_num  =  %d\n",new_wlan_num);
//-----------------------------------------------------------------------------------------------

	return 0;
}
