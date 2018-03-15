#ifndef _TGFINGERVEINAPI_H_
#define _TGFINGERVEINAPI_H_

/**************************************************************************************
*����ΪWindows�½ӿں������÷�ʽ
**************************************************************************************/
//#ifndef API
//#define API __declspec(dllimport) __stdcall
//#endif

/**************************************************************************************
*����Ϊlinux�½ӿں������÷�ʽ
**************************************************************************************/
#ifndef API
#define API 
#endif

/**************************************************************************************
*����ΪPCָ�����㷨�ӿں���
**************************************************************************************/
#ifdef __cplusplus 
extern "C" {
#endif

	/**************************************************************************************
	Function:
		������ȡ���Ӽ��ܵ�ָ����ͼƬ����ȡָ��������
	Input��
		unsigned char *encryptImg�����ܵ�ָ����ͼƬ���ݻ�����ָ��(������ imgWidth*imgHeight Bytes)
		int imgWidth�����ܵ�ָ����ͼƬ��ȣ����أ�
		int imgHeight�����ܵ�ָ����ͼƬ�߶ȣ����أ�
		const char *licenseDatPath�����֤�ļ�·��
	Output��
		unsigned char *feature���Ӽ��ܵ�ָ����ͼƬ����ȡ����ָ�����������ݻ�����ָ�루������ 5984 Bytes��
	Return��
		0��������ȡ�ɹ���unsigned char *feature ������Ч
		1�����֤���ڣ�������ȡʧ�ܣ�unsigned char *feature ������Ч
		2�����֤��Ч��������ȡʧ�ܣ�unsigned char *feature ������Ч
		else��������ȡʧ�ܣ�unsigned char *feature ������Ч
	Others��
		�ú�����ʹ�ñ��봫�����֤�ļ����ݣ�ʥ��Ƽ����ṩ��ʱʹ�õ�������ð����֤��
		�����汾�����֤����ѯʥ��Ƽ���
		���ܵ�ָ����ͼƬ��ָָ����ͼƬ�Ǽ��ܹ��ġ�
		һ����¼����ָ��ʱ��������θú�������ȡͬһ����ָ������ͼƬ��ָ������������
		ȡ������ָ�����������������ںϡ�
		һ������֤��ָ��ʱ�����һ�θú�������ȡһ��ָ�������������������ȶԡ�
	**************************************************************************************/
	int API TGImgExtractFeature(unsigned char *encryptImg, int imgWidth, int imgHeight, unsigned char *feature, const char *licenseDatPath);

	/**************************************************************************************
	Function:
		�����ںϣ���ͬһ����ָ�������������ݣ��ںϳɸ���ָ��ģ������
	Input��
		unsigned char *feature1�����ںϵ���������1������ָ�루������ 5984 Bytes��
		unsigned char *feature2�����ںϵ���������2������ָ�루������ 5984 Bytes��
		unsigned char *feature3�����ںϵ���������3������ָ�루������ 5984 Bytes��
	Output��
		unsigned char *tmpl�����������ںϳɵ�ģ�����ݻ�����ָ�루������17536 Bytes��
	Return��
		0�������ںϳɹ���unsigned char *tmpl ������Ч
		else�������ں�ʧ�ܣ�unsigned char *tmpl ������Ч
	Others��
		һ����¼����ָ��ʱ�����һ�θú�����������ָ���������ںϳ�ģ�壬�ںϵ�ģ������
		�����ȶԣ�һ���������ݿ��С�
	**************************************************************************************/
	int API TGFusionFeature(unsigned char *feature1, unsigned char *feature2, unsigned char *feature3, unsigned char *tmpl);

	/**************************************************************************************
	Function:
		�����ȶԣ�1:1������һ������������һ��ģ�����ݽ��бȶ�
	Input��
		unsigned char *feature�����ȶԵ��������ݻ�����ָ�루������ 5984 Bytes��
		unsigned char *tmpl��ģ�����ݻ�����ָ�루������ 17536 Bytes��
	Output��
		unsigned char *updateTmpl����������
	Return��
		0�������ȶԳɹ�
		else�������ȶ�ʧ��
	Others��
		һ��ýӿ��ʺ�1:1�������ȶԣ�������ѭ�����øýӿ���ʵ��1��N�������ȶ�
	**************************************************************************************/
	int API TGMatchFeature11(unsigned char *feature, unsigned char *tmpl, unsigned char *updateTmpl);

	/**************************************************************************************
	Function:
		�����ȶԣ�1��N������һ�������������ַ�����Ķ��ģ�����ݽ��бȶԣ��Ӷ��ڵ�ַ�����Ķ�
		��ģ���������ҵ���֮��ƥ���ģ�����ݵ�λ��
	Input��
		unsigned char *feature�����ȶԵ��������ݻ�����ָ�루������ 5984 Bytes��
		unsigned char *tmplStart����ַ�����ĵĶ��ģ�����ݻ�����ָ�루������ tmplNum*17536 Bytes��
		int tmplNum����ַ�����Ķ��ģ��ĸ���
	Output��
		int *matchIndex���ڵ�ַ�����Ķ��ģ���������ҵ���֮��ƥ���ģ��������λ�õı���ָ��
		unsigned char *updateTmpl���ȶԳɹ�����֮��ƥ���ģ��ĸ������ݣ������� 17536 Bytes��
	Return��
		0�������ȶԳɹ���int *matchIndex��unsigned char* updateTmpl������Ч
		else�������ȶ�ʧ�ܣ�int *matchIndex��unsigned char* updateTmpl������Ч
	Others��
		һ������֤��ָ��ʱ�����һ�θú��������ȶԳɹ�����鿴 int *matchIndex��
		��ַ�����Ķ��ģ��������ָ�����ݿ��е�����ģ������������һ������һ��������
		��ַ���������ģ��1����_ģ��2����...
		���ƥ�䵽����ģ��1����*matchIndexΪ1��ƥ�䵽����ģ��2����*matchIndexΪ2���Դ����ơ�
		unsigned char *updateTmpl�е����ݣ�һ���ڱȶԳɹ��󣬸������ݿ���ԭ����ģ�����ݡ�
	**************************************************************************************/
	int API TGMatchFeature1N(unsigned char *feature, unsigned char *tmplStart, int tmplNum, int *matchIndex, unsigned char *updateTmpl);

#ifdef __cplusplus 
}
#endif


#endif