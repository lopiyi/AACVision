#include "CameraHalconInstance.h"
#include <QSettings>
#include <QTextCodec>
#include "device_communication.h"
#include <QDir>
#include "simpleQtLogger.h"

Common::ShareData CameraPluginAbstract::share_data;
QReadWriteLock Common::m_lock;

CameraHalconInstance::CameraHalconInstance(int k)
{
	camID = k;
	loadConfig();
	genActions();
}

CameraHalconInstance::~CameraHalconInstance()
{
	if (qsocket)
	{
		qsocket->close();
		delete qsocket;
		qsocket = nullptr;
	}
	if (socket)
	{
		delete socket;
		socket = nullptr;
	}
}

bool CameraHalconInstance::isOk()
{
	return ok;
}

MyWidget* CameraHalconInstance::getCameraUI()
{
	if (!mycamera)
	{
		mycamera = new Camera(data);
		hwin = mycamera->getHwin();
		connect(mycamera, &Camera::signal2camera, this, &CameraHalconInstance::slotFromCameraUIContinueShow, Qt::QueuedConnection);
		connect(this, &CameraHalconInstance::emitState2CameraUI, mycamera, &Camera::setBackground, Qt::QueuedConnection);
	}
	return mycamera;
}

QWidget* CameraHalconInstance::getStatisticsUI()
{
	return statistics.get();
}

QVector<MyAction*> CameraHalconInstance::getActions()
{
	return actions;
}

void CameraHalconInstance::init()
{
	info = QString("CCD%1:").arg(camID);
	ok = true;
	if (!qsocket)
	{
		qsocket = new QTcpSocket(this);
		qsocket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
	}
	qsocket->abort();        //���ӷ�����,�˺��������ر��׽��֣�����д�������е��κι������ݡ�
	qsocket->connectToHost(data.IP, data.Port); //���ӷ�����
	bool plcOk = qsocket->waitForConnected(3000);
	if (!plcOk) {
		ok = false;
		emit emitMessagesToUI(info + u8"TCP����ʧ��");
	}
	else {
		connect(qsocket, &QTcpSocket::disconnected, this, &CameraHalconInstance::Client_Disconnected, Qt::QueuedConnection);//��ĳ���źźͲ��Ѿ�����ʱ���ٽ����ظ������Ӿͻ�ʧ�ܡ�Ҳ���Ǳ������ظ����ӡ�
		connect(qsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)), Qt::QueuedConnection);//�������
		QString mes1 = info + QString(u8"TCP���ӳɹ�");
		emit emitMessagesToUI(mes1);
	}
	if (!socket)
	{
		socket = new TcpSocket(data, qsocket);
	}
	emit emitState2CameraUI(plcOk, 1);

	trigger.reset(new Trigger(data, socket, this));
	connect(trigger.get(), &Trigger::emitMessageToCamera, this, &CameraHalconInstance::slotReceiveMessage);
	connect(trigger.get(), &Trigger::emitPicNumberToCamera, this, &CameraHalconInstance::slotFromTriggerPicNumber, Qt::QueuedConnection);
	if (!trigger->init()) {
		ok = false;
	}

	//initHalconEngine();
	bool camOk = true;

	takepic.reset(new TakePicHalcon(data, this));
	connect(takepic.get(), &TakePicHalcon::emitMessageToCamera, this, &CameraHalconInstance::slotReceiveMessage);
	bool picOk = takepic->init();
	if (!picOk)
	{
		camOk = false;
	}
	

	for (int i = 0; i < picManage.size(); i++)
	{
		delete picManage.value(i + 1);
	}
	picManage.clear();
	for (int i = 0; i < data.picNum; i++)
	{
		ImageProcess* imagePrecess = new ImageProcess(data, socket, hwin, &CameraPluginAbstract::share_data, this);
		imgprecess << imagePrecess;
		imagePrecess->picID = i + 1;
		connect(imagePrecess, &ImageProcess::emitMessageToCamera, this, &CameraHalconInstance::slotReceiveMessage);
		connect(imagePrecess, &ImageProcess::emitToUpdataStatistics, this, &CameraHalconInstance::slotFromImagePrecessToUpdataStatistics, Qt::QueuedConnection);
		if (imagePrecess->init()) {
			picManage.insert(i + 1, imagePrecess);
		}
		else {
			camOk = false;
		}
	}
	emit emitState2CameraUI(camOk, 0);
	ok = ok & camOk;
	emit emitValueToUI(u8"��ʼ��", &ok);
}

void CameraHalconInstance::start()
{
	trigger->start();
}

void CameraHalconInstance::stop()
{
	trigger->stop();
}

void CameraHalconInstance::setValue(const QString flag, void* v)
{
	
}

void CameraHalconInstance::slotFromUI(const QString flag, void* v)
{
	if (flag == u8"��ʼ��")
	{
		init();
		return;
	}
}

void CameraHalconInstance::reconnectTCP()
{
	LS_INFO(info + QString::fromUtf8(u8"��ʼ����TCP"));
	loadConfig();
	trigger->stop();
	if (qsocket)
	{
		qsocket->abort();
	}
	qsocket->connectToHost(data.IP, data.Port); //���ӷ�����
	bool ok = qsocket->waitForConnected(3000);
	if (!ok) {
		LS_INFO(info + QString::fromUtf8(u8"TCP�ؽ�ʧ��"));
		emit emitMessagesToUI(info + u8"TCP�ؽ�ʧ��");
	}
	else {
		LS_INFO(info + QString::fromUtf8(u8"TCP�ؽӳɹ�"));
		QString mes1 = info + QString(u8"TCP�ؽӳɹ�");
		emit emitMessagesToUI(mes1);
	}
	emit emitState2CameraUI(ok, 1);
	//trigger->start();
}

void CameraHalconInstance::reloadData()
{
	//loadConfig();
	socket->updateData(data);
	trigger->updateData(data);
	trigger->init();
	takepic->updateData(data);
	for (int i = 0; i < data.picNum; i++)
	{
		imgprecess[i]->updateData(data);
	}
}

void CameraHalconInstance::reloadAlgorithm()
{
	for (int i = 0; i < data.picNum; i++)
	{
		imgprecess[i]->init();
	}
}

void CameraHalconInstance::genActions()
{
	MyAction* setting = new MyAction(QString(u8"����/�������/���%1").arg(camID));
	connect(setting, &QAction::triggered, [this]() {
		if (camera_setting_ui == nullptr)
		{
			camera_setting_ui = new CameraSettingUI(data);
			camera_setting_ui->init();
		}
		camera_setting_ui->showNormal();
		camera_setting_ui->raise();
		});
	actions.append(setting);

	MyAction* re_data = new MyAction(u8"���¼���/��������");
	connect(re_data, &QAction::triggered, [this]() {
		emit signalReflection(u8"��������");
		});
	actions.append(re_data);

	MyAction* re_Algorithm = new MyAction(u8"���¼���/�����㷨");
	connect(re_Algorithm, &QAction::triggered, [this]() {
		emit signalReflection(u8"�����㷨");
		});
	actions.append(re_Algorithm);

	MyAction* re_TCP = new MyAction(u8"���¼���/����PLC");
	connect(re_TCP, &QAction::triggered, [this]() {
		emit signalReflection(u8"����PLC");
		});
	actions.append(re_TCP);

	statistics.reset(new CamStatistics(data));
	MyAction* clearStatistics = new MyAction(u8"����/���ͳ��");
	connect(clearStatistics, &QAction::triggered, statistics.get(), &CamStatistics::clearStatistics);
	connect(this, &CameraHalconInstance::emitStatics, statistics.get(), &CamStatistics::updataStatistic, Qt::QueuedConnection);
	actions.append(clearStatistics);
}

void CameraHalconInstance::slotReflection(const QString flag)
{
	if (flag==u8"��������")
	{
		reloadData();
		emit emitMessagesToUI(info+u8"����������ɡ�");
		return;
	}
	if (flag==u8"�����㷨")
	{
		QThread::sleep(1);
		reloadData();
		reloadAlgorithm();
		emit emitMessagesToUI(info+u8"�㷨������ɡ�");
		return;
	}
	if (flag==u8"����PLC")
	{
		disconnect(qsocket, &QTcpSocket::disconnected, this, &CameraHalconInstance::Client_Disconnected);//��ĳ���źźͲ��Ѿ�����ʱ���ٽ����ظ������Ӿͻ�ʧ�ܡ�Ҳ���Ǳ������ظ����ӡ�
		disconnect(qsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));//�������
		reconnectTCP();
		connect(qsocket, &QTcpSocket::disconnected, this, &CameraHalconInstance::Client_Disconnected, Qt::QueuedConnection);//��ĳ���źźͲ��Ѿ�����ʱ���ٽ����ظ������Ӿͻ�ʧ�ܡ�Ҳ���Ǳ������ظ����ӡ�
		connect(qsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)), Qt::QueuedConnection);//�������
	}
}

void CameraHalconInstance::slotFromImagePrecessToUpdataStatistics(const int pic, const bool okng, const bool finnalPic)
{
	//statistics->updataStatistic(pic, okng, finnalPic);
	emit emitStatics(pic, okng, finnalPic);
}

void CameraHalconInstance::loadConfig()
{
	Common::m_lock.lockForWrite();
	QString path = QString("./settings/cameras/CCD%1.ini").arg(camID);
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));

	setting.beginGroup("PLC");
	data.IP = setting.value("IP").toString();
	data.Port = setting.value("Port").toInt();
	data.ScanningCycle			= setting.value("ScanningCycle").toInt();
	data.CamplcGrabmark			= setting.value("CamplcGrabmark").toInt();
	data.CamplcGrabok			= setting.value("CamplcGrabok").toInt();
	setting.endGroup();

	setting.beginGroup("Camera");
	data.camID = camID;
	data.DeviceName = setting.value("DeviceName").toString();
	data.CamExposuretime = setting.value("CamExposuretime").toInt();
	data.TriggerSource = setting.value("TriggerSource").toString();
	data.cam_hight = setting.value("cam_hight").toInt();
	data.cam_width = setting.value("cam_width").toInt();
	data.picNum = setting.value("picNum").toInt();
	data.originalImageRootPath = setting.value("originalImageRootPath").toString();
	data.resultImageRootPath = setting.value("resultImageRootPath").toString();
	setting.endGroup();

	data.MyPicDatas.clear();
	for (int i = 0; i < data.picNum; i++)
	{
		Common::PicData picdata;
		setting.beginGroup(QString("PIC%1").arg(i+1));
		picdata.picname = setting.value("picname").toString();														// pic����
		picdata.algorithmName = setting.value("algorithmName").toString();											// �㷨����
		picdata.input_num = setting.value("inputparam_num").toInt();														// �����������
		picdata.statics = setting.value("statics").toBool();															// ��pic�Ƿ����okngͳ��
		picdata.finnalPic = setting.value("finnalPic").toBool();														// �Ƿ�Ϊ���һ��pic������ͳ�������ok��
		picdata.OKNG_flag = setting.value("OKNG_flag").toInt();														// okng���������������λ��
		picdata.ok_value_list = setting.value("ok_value_list").toString();											// �����ж�okng��ֵ
		picdata.inputData.singleData.clear();
		picdata.inputname.clear();
		picdata.inputParamNameLabels.clear();
		picdata.inputParamValueLabels.clear();
		for (int i = 0; i < picdata.input_num; i++)
		{
			picdata.inputParamNameLabels.append(setting.value(QString("inputparam%1namelabel").arg(i + 1)).toString());			// ���������ǩ
			picdata.inputname.append(setting.value(QString("inputparam%1name").arg(i + 1)).toString());						// ���������
			picdata.inputParamValueLabels.append(setting.value(QString("inputparam%1valuelabel").arg(i + 1)).toString());	// �������ֵ��ǩ
			picdata.inputData.singleData.insert(picdata.inputname[i], setting.value(QString("inputparam%1value").arg(i + 1)));		// �������Ĭ��ֵ
		}
		picdata.result_num = setting.value("result_num").toInt();															// �����������
		picdata.outputname.clear();

		for (int i = 0; i < picdata.result_num; i++)
		{
			picdata.outputname.append(setting.value(QString("result%1name").arg(i + 1)).toString());						// ���������
		}
		picdata.valueResultNum = setting.value("valueResultNum").toInt();													// �������е���ֵ�����ĸ���

		picdata.stringResultNum = setting.value("stringResultNum").toInt();													// ���������ַ���ֵ�����ĸ���
		picdata.stringResultNames.clear();
		picdata.stringResult2PLCAddress.clear();
		for (int i = 0; i < picdata.stringResultNum; i++)
		{
			picdata.stringResultNames.append(setting.value(QString("stringResul%1name").arg(i + 1)).toString());			// ���������ַ���ֵ����������
			picdata.stringResult2PLCAddress.append(setting.value(QString("stringResult2PLCAddress%1").arg(i + 1)).toInt());	// ���������ַ���ֵ����д��PLC�ĵ�ַ
		}
		picdata.vectorShowString = setting.value("vectorShowString").toString();											// ��������������Ļ��ʾ�ַ�����������
		picdata.vectorShowValue = setting.value("vectorShowValue").toString();												// ��������������Ļ��ʾֵ��������
		//picdata.receiveData = setting.value("receiveData").toBool();														// �Ƿ������ղ���
		picdata.receiveDataNum = setting.value("receiveDataNum").toInt();													// ���յĲ�����Դ��Ŀ
		for (int i = 0; i < picdata.receiveDataNum; i++)
		{
			picdata.receiveDataCamPic << setting.value(QString("receiveDataCamPic%1").arg(i + 1)).toString();				// ���յ������pic����ʽΪ3-1��2-2��
		}
		picdata.isEmit = setting.value("isEmit").toBool();																	// �Ƿ�����������ݴ���

		picdata.emitResultNum = setting.value("emitResultNum").toInt();														// �������ݵĸ���
		picdata.emitParamNames.clear();
		picdata.receiveParamNames.clear();
		for (int i = 0; i < picdata.emitResultNum; i++)
		{
			picdata.emitParamNames.append(setting.value(QString("emit%1name").arg(i + 1)).toString());						// �������ݵ�����
			picdata.receiveParamNames.append(setting.value(QString("receive%1name").arg(i + 1)).toString());				// �����ߵ���������
		}

		picdata.imageInputName=setting.value("imageInputName").toString();

		picdata.imageResultNum = setting.value("imageResultNum").toInt();													// ���ͼƬ������
		for (int i = 0; i < picdata.imageResultNum; i++)
		{
			picdata.imageResultName.append(setting.value(QString("imageResult%1name").arg(i + 1)).toString());				// ���ͼƬ���ڲ�����
		}

		// ԭͼ
		picdata.originalImage = setting.value("originalImage").toBool();
		//picdata.originalImageRootPath = setting.value("originalImageRootPath").toString();
		picdata.saveOriginalOk = setting.value("saveOriginalOk").toBool();
		picdata.saveOriginalNg = setting.value("saveOriginalNg").toBool();
		picdata.saveOriginalAll = setting.value("saveOriginalAll").toBool();
		picdata.originalImageBmp = setting.value("originalImageBmp").toBool();
		// ���ͼ
		picdata.resultImage = setting.value("resultImage").toBool();
		picdata.frontSize = setting.value("frontSize").toInt();
		picdata.lineWidth = setting.value("lineWidth").toInt();
		picdata.resultImageScale = setting.value("resultImageScale").toDouble();
		//picdata.resultImageRootPath = setting.value("resultImageRootPath").toString();
		picdata.showInput = setting.value("showInput").toBool();
		picdata.saveResultOk = setting.value("saveResultOk").toBool();
		picdata.saveResultNg = setting.value("saveResultNg").toBool();
		picdata.saveResultAll = setting.value("saveResultAll").toBool();
		picdata.resultImageBmp = setting.value("resultImageBmp").toBool();

		picdata.picAddress = setting.value("picAddress").toInt();
		picdata.isMes = setting.value("isMes").toBool();
		picdata.QR_num = setting.value("QR_num").toInt();
		for (int var = 0; var < picdata.QR_num; ++var) {
			picdata.QR_plc_address.append(setting.value(QString("QR%1").arg(var + 1)).toInt());
			picdata.QR_len.append(setting.value(QString("QR_len%1").arg(var + 1)).toInt());
		}
		picdata.mes_num = setting.value("mes_num").toInt();
		for (int i = 0; i < picdata.mes_num; i++)
		{
			picdata.picPath2PLCaddress.append(setting.value(QString("mes%1").arg(i + 1)).toInt());
		}

		setting.endGroup();

		data.MyPicDatas.append(picdata);
	}
	Common::m_lock.unlock();
}

void CameraHalconInstance::Client_Disconnected()
{
	try
	{
		Common::m_lock.lockForWrite();
		qsocket->abort();
		//��������
		qsocket->connectToHost(data.IP, data.Port); //���ӷ�����
		if (!qsocket->waitForConnected(3000))
		{
			QString mes = info + data.IP + "   " + QString::number(data.Port) + u8"TCP���ӶϿ�";
			emitMessagesToUI(mes);
			Common::m_lock.unlock();
			return;
		}
		else
		{
			QString mes = info + data.IP + " " + QString::number(data.Port) +" " + u8"TCP�����ɹ�";
			emitMessagesToUI(mes);
			Common::m_lock.unlock();
			return;
		}
	}
	catch (QException& e)
	{
		QString mes = info + data.IP + "   " + QString::number(data.Port) + u8"TCP��������";
		emitMessagesToUI(mes);
		return;
	}
}

void CameraHalconInstance::onError(QAbstractSocket::SocketError error)
{
	QString mes = info + QString(u8"TCP����%1").arg(error);
	emit emitMessagesToUI(mes);
}

void CameraHalconInstance::slotReceiveMessage(const QString str)
{
	emit emitMessagesToUI(info + str);
}

void CameraHalconInstance::slotFromTriggerPicNumber(const int pic)
{
	try
	{
		if (!picManage.keys().contains(pic))
		{
			QString mes = info + QString(u8"�����ڵ�ͼ�ţ�%1").arg(pic);
			emit emitMessagesToUI(mes);
		}
		else {
			if (takepic->take(image))
			{
				picManage.value(pic)->run(image);
			}
			//else {
			//	mes1 = info + QString(u8"ȡͼʧ��");
			//	emit signal2mainui(u8"��ʾ��Ϣ", &mes1);
			//}
		}
	}
	catch (HException& he)
	{
		HTuple  Exception;
		he.ToHTuple(&Exception);
		QString mes1 = info + QString(u8"Halcon �㷨�쳣:") + QString(Exception.ToString().Text());
		emit emitMessagesToUI(mes1);
	}
	catch (HDevEngineException& m_halconEngineException)
	{
		QString error = m_halconEngineException.Message();
		error = QString(u8"Halcon �����쳣:") + error;
		emit emitMessagesToUI(info + error);
	}
	catch (QException& e)
	{
		QString mes1 = info + info + "QT" + e.what();
		emit emitMessagesToUI(mes1);
	}
	catch (const std::exception& e)
	{
		QString mes1 = info + "std" + e.what();
		emit emitMessagesToUI(mes1);
	}
	catch (...)
	{
		QString mes1 = info + QString(u8"����%1").arg(pic);
		emit emitMessagesToUI(mes1);
	}
	//LS_INFO(info + QString::fromUtf8(u8"�ٴ�����ɨ��"));
	trigger->start();
}

void CameraHalconInstance::slotFromCameraUIContinueShow(const int state)
{
	free_run = state;
	while (free_run)
	{
		if (takepic->takeContinueShow(image)) {
			hwin->GetHalconBuffer()->ClearWindow();//���֮ǰ�Ĵ�������
			hwin->GetHalconBuffer()->DispObj(image);
			hwin->GetHalconBuffer()->FlushBuffer();
		}
		else {
			break;
		}
		QCoreApplication::processEvents();
	}
}