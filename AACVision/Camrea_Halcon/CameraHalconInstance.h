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
	void Client_Disconnected();//作为客户端时与服务器断开连接(掉线)
	void onError(QAbstractSocket::SocketError error);//连接发生错误时输出，输出错误类型
private:
	QString info;
	QVector< ImageProcess*> imgprecess;
	Common::CameraHalconData data;
	//std::shared_ptr<HDevEngine> m_engine; //halcon引擎
	QTcpSocket* qsocket = nullptr;	// 与PLC通讯的tcp
	TcpSocket* socket = nullptr;	// tcp
	std::shared_ptr<Trigger> trigger = nullptr;	// 用于触发拍照的类
	std::shared_ptr<TakePicHalcon> takepic = nullptr;	// 用于从相机取图的类
	
	QHash<int, ImageProcess*> picManage;	// 管理多个pic
	//QVector<ImageProcess> picManage;	// 管理多个pic
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

