1����kernel�е�uImage.hdmi�滻�����ϵ�uImage.hdmi
2����gprs�е������ļ��ŵ������ϵ� /etc/ppp/peers
3��sbin�����5���ļ��ŵ������ϵ�/usr/sbin







ִ����� pppd call gprs-dial��������������
���������� ifconfig �����ppp0���Լ�����ip
��ppp0��ip��ӵ�·�ɣ�ִ��route add default gw
���������ping�ٶ�ip��pingͨ


������ppp0��ipΪ10.53.78.236 Ϊ��


*-----------------------------------------------------------------------*
|									pppd call gprs-dial &																	|
|									route add default gw  10.53.78.236										|
|									ping 115.239.210.27 				// www.baidu.com					|
|																																				|
*-----------------------------------------------------------------------*