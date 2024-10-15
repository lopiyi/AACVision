#pragma once

#include <qhash.h>
#include <qstring.h>
#include <qvariant.h>
#include <qreadwritelock.h>

namespace Common {
	extern QReadWriteLock m_lock;
	struct ShareData
	{
		QHash<QString, QHash<QString, QVariant>> emitSingleData;
	};

	struct RuntimeData
	{
		QHash<QString, QVariant> singleData;
		QHash<QString, QVector<QVariant>> vectorData;
		QHash<QString, void*> imgs;
	};

	struct PicData
	{
		RuntimeData inputData;
		RuntimeData outputData;
		//*******************************************************************//
		// 图片运行参数
		int input_num;	// 输入参数个数
		QStringList inputParamNameLabels;
		QStringList inputParamValueLabels;
		QVector<QString> inputname;
		int result_num;
		bool statics;
		bool finnalPic;
		
		QVector<QString> outputname;
		int OKNG_flag;		// ok ng 参数位置
		QString ok_value_list;		// ok 值列表
		QString picname;	// 统计窗口中pic的项目名
		QString algorithmName; // 算法名
		//bool receiveData = false;	// 是否开启接收参数
		int receiveDataNum = 0;		// 接收的参数来源数目
		QStringList receiveDataCamPic;	// 接收的相机和pic，格式为3-1、2-2等
		bool isEmit = false;	// 是否发射参数
		int emitResultNum = 0;		// 发射的参数个数
		QStringList emitParamNames;						// 发射的参数名
		QStringList receiveParamNames;					// 接收的参数名
		int valueResultNum = 0;		// 结果中单值参数个数
		int stringResultNum = 0;		// 结果中字符串参数个数
		QVector<QString> stringResultNames;				// 字符串参数名
		QVector<int> stringResult2PLCAddress;			// 字符串写进PLC的地址
		QString vectorShowString;						// 屏幕上显示的参数名称数组的参数名
		QString vectorShowValue;						// 屏幕上显示的参数值的参数名
		QString imageInputName;		// 输入图像的参数名
		int imageResultNum = 0;		// 输出图片的数量
		QVector<QString> imageResultName;				// 输出图片的参数名
		//*******************************************************************//

		//*******************************************************************//
		//图片保存参数
		// 原图
		bool originalImage = true;		// 保存原图
		
		bool saveOriginalOk = true;		// 是否保存ok原图
		bool saveOriginalNg = true;		// 是否保存ng原图
		bool saveOriginalAll = true;		// 是否保存所有原图
		bool originalImageBmp = true;		// 原图是否保存为bmp
		// 结果图
		bool resultImage = true;		// 保存结果图
		int frontSize = 50;		// 字体大小
		int lineWidth = 1;		// 线宽
		double resultImageScale = 1;		// 结果图缩放系数
		
		bool showInput = true;		// 是否显示输入
		bool saveResultOk = true;		// 是否保存ok结果图
		bool saveResultNg = true;		// 是否保存ng结果图
		bool saveResultAll = true;		// 是否保存所有结果图
		bool resultImageBmp = true;		// 结果图是否保存为bmp
		//*******************************************************************//

		//*******************************************************************//
		// 写入PLC的参数
		int picAddress = 6000;		// 写入plc的首地址
		//*******************************************************************//

		//*******************************************************************//
		// 数据上传
		bool isMes = false;	// 是否数据上传
		int QR_num = 0;		// 二维码数量
		QVector<int> QR_plc_address;					// 二维码在plc中的地址
		QVector<int> QR_len;							// 二维码寄存器占用长度
		int mes_num = 0;		// 上传数据个数
		QVector<int> picPath2PLCaddress;				// 图片路径写入PLC的地址
		//*******************************************************************//
	};

	struct CameraHalconData
	{
		//************PLC相关参数*************//
		int ScanningCycle = 500;	// 定时器扫描周期
		int CamplcGrabok = 0;		// 通知PLC取图ok的寄存器地址
		int CamplcGrabmark = 0;	// 扫描PLC取图号的寄存器地址
		QString IP;		// IP
		int Port;		// 端口号
		//************PLC相关参数*************//

		//************相机相关参数*************//
		int camID = 0;	// 相机号
		QString DeviceName;		// 设备名
		int CamExposuretime = 5000;	// 曝光
		QString TriggerSource;	// 触发源Line0还是Line其他
		int cam_width = 0;		// 图像宽
		int cam_hight = 0;		// 图像高
		int picNum = 0;		// pic数目
		QString originalImageRootPath = "";		// 原图根目录
		QString resultImageRootPath = "";		// 结果图根目录
		//************相机相关参数*************//

		//************pic相关参数*************//
		QVector<PicData> MyPicDatas;
		//************pic相关参数*************//
	};
}