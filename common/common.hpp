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
		// ͼƬ���в���
		int input_num;	// �����������
		QStringList inputParamNameLabels;
		QStringList inputParamValueLabels;
		QVector<QString> inputname;
		int result_num;
		bool statics;
		bool finnalPic;
		
		QVector<QString> outputname;
		int OKNG_flag;		// ok ng ����λ��
		QString ok_value_list;		// ok ֵ�б�
		QString picname;	// ͳ�ƴ�����pic����Ŀ��
		QString algorithmName; // �㷨��
		//bool receiveData = false;	// �Ƿ������ղ���
		int receiveDataNum = 0;		// ���յĲ�����Դ��Ŀ
		QStringList receiveDataCamPic;	// ���յ������pic����ʽΪ3-1��2-2��
		bool isEmit = false;	// �Ƿ������
		int emitResultNum = 0;		// ����Ĳ�������
		QStringList emitParamNames;						// ����Ĳ�����
		QStringList receiveParamNames;					// ���յĲ�����
		int valueResultNum = 0;		// ����е�ֵ��������
		int stringResultNum = 0;		// ������ַ�����������
		QVector<QString> stringResultNames;				// �ַ���������
		QVector<int> stringResult2PLCAddress;			// �ַ���д��PLC�ĵ�ַ
		QString vectorShowString;						// ��Ļ����ʾ�Ĳ�����������Ĳ�����
		QString vectorShowValue;						// ��Ļ����ʾ�Ĳ���ֵ�Ĳ�����
		QString imageInputName;		// ����ͼ��Ĳ�����
		int imageResultNum = 0;		// ���ͼƬ������
		QVector<QString> imageResultName;				// ���ͼƬ�Ĳ�����
		//*******************************************************************//

		//*******************************************************************//
		//ͼƬ�������
		// ԭͼ
		bool originalImage = true;		// ����ԭͼ
		
		bool saveOriginalOk = true;		// �Ƿ񱣴�okԭͼ
		bool saveOriginalNg = true;		// �Ƿ񱣴�ngԭͼ
		bool saveOriginalAll = true;		// �Ƿ񱣴�����ԭͼ
		bool originalImageBmp = true;		// ԭͼ�Ƿ񱣴�Ϊbmp
		// ���ͼ
		bool resultImage = true;		// ������ͼ
		int frontSize = 50;		// �����С
		int lineWidth = 1;		// �߿�
		double resultImageScale = 1;		// ���ͼ����ϵ��
		
		bool showInput = true;		// �Ƿ���ʾ����
		bool saveResultOk = true;		// �Ƿ񱣴�ok���ͼ
		bool saveResultNg = true;		// �Ƿ񱣴�ng���ͼ
		bool saveResultAll = true;		// �Ƿ񱣴����н��ͼ
		bool resultImageBmp = true;		// ���ͼ�Ƿ񱣴�Ϊbmp
		//*******************************************************************//

		//*******************************************************************//
		// д��PLC�Ĳ���
		int picAddress = 6000;		// д��plc���׵�ַ
		//*******************************************************************//

		//*******************************************************************//
		// �����ϴ�
		bool isMes = false;	// �Ƿ������ϴ�
		int QR_num = 0;		// ��ά������
		QVector<int> QR_plc_address;					// ��ά����plc�еĵ�ַ
		QVector<int> QR_len;							// ��ά��Ĵ���ռ�ó���
		int mes_num = 0;		// �ϴ����ݸ���
		QVector<int> picPath2PLCaddress;				// ͼƬ·��д��PLC�ĵ�ַ
		//*******************************************************************//
	};

	struct CameraHalconData
	{
		//************PLC��ز���*************//
		int ScanningCycle = 500;	// ��ʱ��ɨ������
		int CamplcGrabok = 0;		// ֪ͨPLCȡͼok�ļĴ�����ַ
		int CamplcGrabmark = 0;	// ɨ��PLCȡͼ�ŵļĴ�����ַ
		QString IP;		// IP
		int Port;		// �˿ں�
		//************PLC��ز���*************//

		//************�����ز���*************//
		int camID = 0;	// �����
		QString DeviceName;		// �豸��
		int CamExposuretime = 5000;	// �ع�
		QString TriggerSource;	// ����ԴLine0����Line����
		int cam_width = 0;		// ͼ���
		int cam_hight = 0;		// ͼ���
		int picNum = 0;		// pic��Ŀ
		QString originalImageRootPath = "";		// ԭͼ��Ŀ¼
		QString resultImageRootPath = "";		// ���ͼ��Ŀ¼
		//************�����ز���*************//

		//************pic��ز���*************//
		QVector<PicData> MyPicDatas;
		//************pic��ز���*************//
	};
}