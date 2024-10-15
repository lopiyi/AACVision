#pragma once
#include "camera_plugin.h"
#include <QTimer>
#include <memory>
#include "TcpSocket.h"
#include "trigger.h"
#include "takepichalcon.h"
#include "imageprocess.h"
#include "camera.h"
#include "camerasettingui.h"
#include "camstatistics.h"

class CameraHalconInstance:public CameraPluginAbstract
{
	Q_OBJECT
public:
	CameraHalconInstance(int k);
	~CameraHalconInstance();
	virtual MyWidget* getCameraUI() override;
	virtual QWidget* getStatisticsUI() override;
	virtual QVector<MyAction*> getActions() override;
	virtual bool isOk() override;
	virtual void init() override;
	virtual void start() override;
	virtual void stop() override;
	virtual void setValue(const QString flag, void*v) override;
public slots:
	virtual void slotFromUI(const QString flag, void*v) override;
signals:
	void signalReflection(const QString) override;
	void emitMessagesToUI(const QString str) override;
	void emitValueToUI(const QString flag, void*v) override;
	void emitState2CameraUI(bool ok = true, int kind=0);
	void emitStatics(const int pic, const bool okng, const bool finnalPic);
private:
	void loadConfig();
	void genActions();
	void reloadData();
	void reconnectTCP();
	void reloadAlgorithm();
	//bool initHalconEngine();
private slots:
	void slotReceiveMessage(const QString str);
	void slotFromTriggerPicNumber(const int pic);
	void slotFromCameraUIContinueShow(const int state);
	virtual void slotReflection(const QString flag) override;
	void slotFromImagePrecessToUpdataStatistics(const int pic, const bool okng, const bool finnalPic);
	void Client_Disconnected();//��Ϊ�ͻ���ʱ��������Ͽ�����(����)
	void onError(QAbstractSocket::SocketError error);//���ӷ�������ʱ����������������
private:
	QString info;
	QVector< ImageProcess*> imgprecess;
	Common::CameraHalconData data;
	//std::shared_ptr<HDevEngine> m_engine; //halcon����
	QTcpSocket* qsocket = nullptr;	// ��PLCͨѶ��tcp
	TcpSocket* socket = nullptr;	// tcp
	std::shared_ptr<Trigger> trigger = nullptr;	// ���ڴ������յ���
	std::shared_ptr<TakePicHalcon> takepic = nullptr;	// ���ڴ����ȡͼ����
	
	QHash<int, ImageProcess*> picManage;	// ������pic
	//QVector<ImageProcess> picManage;	// ������pic
	HImage image;
	bool ok;
	bool free_run = false;
private:
	QVector<MyAction*> actions;
	Camera* mycamera = nullptr;
	CameraSettingUI* camera_setting_ui = nullptr;
	QHalconWindow* hwin=nullptr;
	std::shared_ptr<CamStatistics> statistics = nullptr;
};

