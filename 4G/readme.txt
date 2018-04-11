1、将kernel中的uImage.hdmi替换板子上的uImage.hdmi
2、将gprs中的三个文件放到板子上的 /etc/ppp/peers
3、sbin里面的5个文件放到板子上的/usr/sbin







执行命令“ pppd call gprs-dial”，拨号上网。
拨号上网后 ifconfig 会出现ppp0，以及它的ip
将ppp0的ip添加到路由，执行route add default gw
正常情况下ping百度ip能ping通


以下以ppp0的ip为10.53.78.236 为例


*-----------------------------------------------------------------------*
|									pppd call gprs-dial &																	|
|									route add default gw  10.53.78.236										|
|									ping 115.239.210.27 				// www.baidu.com					|
|																																				|
*-----------------------------------------------------------------------*