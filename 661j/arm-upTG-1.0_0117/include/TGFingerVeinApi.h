#ifndef _TGFINGERVEINAPI_H_
#define _TGFINGERVEINAPI_H_

/**************************************************************************************
*以下为Windows下接口函数调用方式
**************************************************************************************/
//#ifndef API
//#define API __declspec(dllimport) __stdcall
//#endif

/**************************************************************************************
*以下为linux下接口函数调用方式
**************************************************************************************/
#ifndef API
#define API 
#endif

/**************************************************************************************
*以下为PC指静脉算法接口函数
**************************************************************************************/
#ifdef __cplusplus 
extern "C" {
#endif

	/**************************************************************************************
	Function:
		特征提取：从加密的指静脉图片中提取指静脉特征
	Input：
		unsigned char *encryptImg：加密的指静脉图片数据缓存区指针(缓存区 imgWidth*imgHeight Bytes)
		int imgWidth：加密的指静脉图片宽度（像素）
		int imgHeight：加密的指静脉图片高度（像素）
		const char *licenseDatPath：许可证文件路径
	Output：
		unsigned char *feature：从加密的指静脉图片中提取到的指静脉特征数据缓存区指针（缓存区 5984 Bytes）
	Return：
		0：特征提取成功，unsigned char *feature 数据有效
		1：许可证过期，特征提取失败，unsigned char *feature 数据无效
		2：许可证无效，特征提取失败，unsigned char *feature 数据无效
		else：特征提取失败，unsigned char *feature 数据无效
	Others：
		该函数的使用必须传入许可证文件数据，圣点科技已提供限时使用的免费试用版许可证，
		其他版本的许可证请咨询圣点科技。
		加密的指静脉图片是指指静脉图片是加密过的。
		一般在录入手指的时候调用三次该函数，提取同一根手指的三幅图片的指静脉特征，提
		取的三个指静脉特征用于特征融合。
		一般在验证手指的时候调用一次该函数，提取一个指静脉特征，用于特征比对。
	**************************************************************************************/
	int API TGImgExtractFeature(unsigned char *encryptImg, int imgWidth, int imgHeight, unsigned char *feature, const char *licenseDatPath);

	/**************************************************************************************
	Function:
		特征融合：将同一根手指的三个特征数据，融合成该手指的模板数据
	Input：
		unsigned char *feature1：待融合的特征数据1缓存区指针（缓存区 5984 Bytes）
		unsigned char *feature2：待融合的特征数据2缓存区指针（缓存区 5984 Bytes）
		unsigned char *feature3：待融合的特征数据3缓存区指针（缓存区 5984 Bytes）
	Output：
		unsigned char *tmpl：三个特征融合成的模板数据缓存区指针（缓存区17536 Bytes）
	Return：
		0：特征融合成功，unsigned char *tmpl 数据有效
		else：特征融合失败，unsigned char *tmpl 数据无效
	Others：
		一般在录入手指的时候调用一次该函数，将三个指静脉特征融合成模板，融合的模板用于
		特征比对，一般存放于数据库中。
	**************************************************************************************/
	int API TGFusionFeature(unsigned char *feature1, unsigned char *feature2, unsigned char *feature3, unsigned char *tmpl);

	/**************************************************************************************
	Function:
		特征比对（1:1）：将一个特征数据与一个模板数据进行比对
	Input：
		unsigned char *feature：待比对的特征数据缓存区指针（缓存区 5984 Bytes）
		unsigned char *tmpl：模板数据缓存区指针（缓存区 17536 Bytes）
	Output：
		unsigned char *updateTmpl：保留参数
	Return：
		0：特征比对成功
		else：特征比对失败
	Others：
		一般该接口适合1:1的特征比对，不建议循环调用该接口来实现1：N的特征比对
	**************************************************************************************/
	int API TGMatchFeature11(unsigned char *feature, unsigned char *tmpl, unsigned char *updateTmpl);

	/**************************************************************************************
	Function:
		特征比对（1：N）：将一个特征数据与地址连续的多个模板数据进行比对，从而在地址连续的多
		个模板数据中找到与之相匹配的模板数据的位置
	Input：
		unsigned char *feature：待比对的特征数据缓存区指针（缓存区 5984 Bytes）
		unsigned char *tmplStart：地址连续的的多个模板数据缓存区指针（缓存区 tmplNum*17536 Bytes）
		int tmplNum：地址连续的多个模板的个数
	Output：
		int *matchIndex：在地址连续的多个模板数据中找到与之相匹配的模板数据其位置的变量指针
		unsigned char *updateTmpl：比对成功后，与之相匹配的模板的更新数据（缓存区 17536 Bytes）
	Return：
		0：特征比对成功，int *matchIndex，unsigned char* updateTmpl数据有效
		else：特征比对失败，int *matchIndex，unsigned char* updateTmpl数据无效
	Others：
		一般在验证手指的时候调用一次该函数，若比对成功，则查看 int *matchIndex。
		地址连续的多个模板数据是指将数据库中的所有模板数据连接在一起，是在一块连续的
		地址中连续存放模板1数据_模板2数据...
		如果匹配到的是模板1，则*matchIndex为1；匹配到的是模板2，则*matchIndex为2，以此类推。
		unsigned char *updateTmpl中的数据，一般在比对成功后，覆盖数据库中原来的模板数据。
	**************************************************************************************/
	int API TGMatchFeature1N(unsigned char *feature, unsigned char *tmplStart, int tmplNum, int *matchIndex, unsigned char *updateTmpl);

#ifdef __cplusplus 
}
#endif


#endif