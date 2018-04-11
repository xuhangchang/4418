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

#define IFCONFIGLIST "/etc/IfconfigList"
#define PPPLIST "/etc/PppList"

#define STR_MAX_LEN 256


/*****************************************************************  
** @brief       get ppp ip
** @author      xuhc
** @param1      <out>  <char *>         ppp ip
** @exception
** @return     -1 : fail
**             0  : success
** @note       
** @see 
** @date       2018/04/04
******************************************************************/
static int get_ppp_ip(char *ppp_ip)
{
	FILE *fp=fopen(PPPLIST, "r+");
	int i;
	int length;
	char str[STR_MAX_LEN]; 
	char *pstr1 = NULL;
	char *pstr2 = NULL;
	if (fp==NULL )
		return -1;

	fgets(str,sizeof(str),fp);
	pstr1 = strstr(str,":");
	pstr1++;
	pstr2 = pstr1;
	while(pstr2++)
	{
		if(' ' == *pstr2)
		{
			*pstr2 = '\0';
			break;
		}
	}
	strcpy(ppp_ip,pstr1);
	fclose(fp);
	return 0;
}


/*****************************************************************  
** @brief       route add default gw
** @author      xuhc
** @param1      <in>  <char *>         ppp ip
** @exception
** @return     -1 : fail
**             0  : success
** @note       
** @see 
** @date       2018/04/04
******************************************************************/
static int route_add(char *ppp_ip)
{
	char str[STR_MAX_LEN] = "route add default gw "; 
	strcat(str,ppp_ip);
	system(str);
	printf("cmd = %s\n",str);
	return 0;
}


int main( int argc, char** argv )
{
	int i;
	char ppp_ip_addr[STR_MAX_LEN];


//	system("ifconfig > /etc/IfconfigList ");
//	system("grep -E "P-t-P" /etc/IfconfigList  > /etc/PppList");

	get_ppp_ip(ppp_ip_addr);
	printf("ppp_ip_addr = %s\n",ppp_ip_addr);

	route_add(ppp_ip_addr);
	return 0;
}
