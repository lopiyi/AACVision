#include "camerasettingui.h"
#include <QSettings>
#include <QTextCodec>
#include <QDateTime>
#include <QFileDialog>
#include <QShortcut>
#include <qmessagebox.h>

CameraSettingUI::CameraSettingUI(Common::CameraHalconData& data, QWidget *parent)
	: QWidget(parent),data(data)
{
	ui.setupUi(this);
	QShortcut* shortcut1 = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	QObject::connect(shortcut1, &QShortcut::activated, this, &QWidget::close);
	QShortcut* shortcut2 = new QShortcut(QKeySequence("Ctrl+S"), this);
	QObject::connect(shortcut2, &QShortcut::activated, this, &CameraSettingUI::slot_pb_save);
	setWindowTitle(QString(u8"CCD%1参数设置").arg(data.camID));
	setAttribute(Qt::WA_QuitOnClose, false);
	ui.widget->installEventFilter(this);//注册事件过滤器
	connect(ui.sb_picnum, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		picNumChanged(v);
		});
	connect(ui.pb_getDevice, &QPushButton::clicked, this, &CameraSettingUI::getDevices);
	connect(ui.pb_save, &QPushButton::clicked, this, &CameraSettingUI::slot_pb_save);
	connect(ui.pb_start, &QPushButton::clicked, this, &CameraSettingUI::slot_pb_open);
	connect(ui.pb_stop, &QPushButton::clicked, [this](){
		ui.pb_start->setEnabled(true);
		emit stopShow();
		});
	connect(ui.pb_yuantu, &QPushButton::clicked, [this]() {
		QString directory = QFileDialog::getExistingDirectory(this, u8"选择保存原图的根目录", QCoreApplication::applicationDirPath()); // 显示对话框
		if (!directory.isEmpty()) {
			ui.le_yuantu->setText(directory);
		}
		});

	connect(ui.pb_result, &QPushButton::clicked, [this]() {
		QString directory = QFileDialog::getExistingDirectory(this, u8"选择保存结果图的根目录", QCoreApplication::applicationDirPath()); // 显示对话框
		if (!directory.isEmpty()) {
			ui.le_result->setText(directory);
		}
		});
}

CameraSettingUI::~CameraSettingUI()
{
	for (size_t i = 0; i < picWidgets.size(); i++)
	{
		delete picWidgets[i];
		picWidgets[i] = nullptr;
	}
}

void CameraSettingUI::getDevices()
{
	
	// Local control variables
	HTuple  hv_Information, hv_ValueList, hv_lenght_aquipment;
	HTuple  hv_index, hv_Substrings;
	HTuple  hv_Matches_device, hv_Substrings_device, hv_Result_device;

	InfoFramegrabber("GenICamTL", "info_boards", &hv_Information, &hv_ValueList);
	hv_lenght_aquipment = hv_ValueList.TupleLength();

	QStringList devices;

	{
		HTuple end_val7 = hv_lenght_aquipment - 1;
		HTuple step_val7 = 1;
		for (hv_index = 0; hv_index.Continue(end_val7, step_val7); hv_index += step_val7)
		{
			TupleSplit(HTuple(hv_ValueList[hv_index]), "|", &hv_Substrings);
			TupleRegexpSelect(hv_Substrings, "device", &hv_Matches_device);
			TupleSplit(hv_Matches_device, ":", &hv_Substrings_device);
			TupleRegexpReplace(HTuple(hv_Substrings_device[1]), (HTuple("\\s*").Append("replace_all")),
				"", &hv_Result_device);

			devices << QString(hv_Result_device.ToString().Text()).remove("\"");
		}
	}
	if (devices.size())
	{
		ui.cb_device->clear();
		ui.cb_device->addItems(devices);
	}
}

void CameraSettingUI::slot_pb_save()
{

	saveData();
	data.MyPicDatas.clear();
	for (int i = 0; i < ui.sb_picnum->value(); i++)
	{
		data.MyPicDatas.append(picWidgets[i]->saveData());
	}
	saveConfig();
	QMessageBox::information(nullptr, u8"信息", QString(u8"CCD%1参数已保存").arg(data.camID), QMessageBox::Ok, QMessageBox::Ok);
}

void CameraSettingUI::saveConfig()
{
	const QString path = QString("./settings/cameras/CCD%1.ini").arg(data.camID);
	QSettings setting(path, QSettings::IniFormat);
	setting.clear();
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));
	
	setting.beginGroup("PLC");
	setting.setValue("IP", data.IP);
	setting.setValue("Port", data.Port);
	setting.setValue("ScanningCycle", data.ScanningCycle);
	setting.setValue("CamplcGrabmark", data.CamplcGrabmark);
	setting.setValue("CamplcGrabok", data.CamplcGrabok);
	setting.endGroup();

	setting.beginGroup("Camera");
	setting.setValue("DeviceName", data.DeviceName);
	setting.setValue("CamExposuretime", data.CamExposuretime);
	setting.setValue("TriggerSource", data.TriggerSource);
	setting.setValue("cam_hight", data.cam_hight);
	setting.setValue("cam_width", data.cam_width);
	setting.setValue("picNum", data.picNum);
	setting.setValue("originalImageRootPath", data.originalImageRootPath);
	setting.setValue("resultImageRootPath", data.resultImageRootPath);
	setting.endGroup();

	for (int j = 0; j < data.MyPicDatas.size(); j++)
	{
		Common::PicData picdata = data.MyPicDatas[j];
		setting.beginGroup(QString("PIC%1").arg(j + 1));
		setting.setValue("picname", picdata.picname);
		setting.setValue("algorithmName", picdata.algorithmName);
		setting.setValue("inputparam_num", picdata.input_num);
		setting.setValue("statics", picdata.statics);
		setting.setValue("finnalPic", picdata.finnalPic);
		setting.setValue("OKNG_flag", picdata.OKNG_flag);
		setting.setValue("ok_value_list", picdata.ok_value_list);
		for (int i = 0; i < picdata.input_num; i++)
		{
			setting.setValue(QString("inputparam%1namelabel").arg(i + 1), picdata.inputParamNameLabels[i]);
			setting.setValue(QString("inputparam%1name").arg(i + 1), picdata.inputname[i]);
			setting.setValue(QString("inputparam%1valuelabel").arg(i + 1), picdata.inputParamValueLabels[i]);
			setting.setValue(QString("inputparam%1value").arg(i + 1), picdata.inputData.singleData.value(picdata.inputname[i]));
		}

		setting.setValue("result_num", picdata.result_num);
		for (int i = 0; i < picdata.result_num; i++)
		{
			setting.setValue(QString("result%1name").arg(i + 1), picdata.outputname[i]);
		}
		setting.setValue("valueResultNum", picdata.valueResultNum);
		setting.setValue("stringResultNum", picdata.stringResultNum);

		for (int i = 0; i < picdata.stringResultNum; i++)
		{
			setting.setValue(QString("stringResul%1name").arg(i + 1), picdata.stringResultNames[i]);
			setting.setValue(QString("stringResult2PLCAddress%1").arg(i + 1), picdata.stringResult2PLCAddress[i]);
		}
		setting.setValue("vectorShowString", picdata.vectorShowString);
		setting.setValue("vectorShowValue", picdata.vectorShowValue);
		//setting.setValue("receiveData", picdata.receiveData);
		setting.setValue("receiveDataNum", picdata.receiveDataNum);
		for (int i = 0; i < picdata.receiveDataNum; i++)
		{
			setting.setValue(QString("receiveDataCamPic%1").arg(i+1), picdata.receiveDataCamPic[i]);
		}
		
		setting.setValue("isEmit", picdata.isEmit);
	
		setting.setValue("emitResultNum", picdata.emitResultNum);
		for (int i = 0; i < picdata.emitResultNum; i++)
		{
			setting.setValue(QString("emit%1name").arg(i + 1), picdata.emitParamNames[i]);
			setting.setValue(QString("receive%1name").arg(i + 1), picdata.receiveParamNames[i]);
		}

		setting.setValue("imageInputName", picdata.imageInputName);
		setting.setValue("imageResultNum", picdata.imageResultNum);
		for (int i = 0; i < picdata.imageResultNum; i++)
		{
			setting.setValue(QString("imageResult%1name").arg(i + 1), picdata.imageResultName[i]);
		}

		// 原图
		setting.setValue("originalImage", picdata.originalImage);
		
		setting.setValue("saveOriginalOk", picdata.saveOriginalOk);
		setting.setValue("saveOriginalNg", picdata.saveOriginalNg);
		setting.setValue("saveOriginalAll", picdata.saveOriginalAll);
		setting.setValue("originalImageBmp", picdata.originalImageBmp);
		// 结果图
		setting.setValue("resultImage", picdata.resultImage);
		setting.setValue("frontSize", picdata.frontSize);
		setting.setValue("lineWidth", picdata.lineWidth);
		setting.setValue("resultImageScale", picdata.resultImageScale);
		
		setting.setValue("showInput", picdata.showInput);
		setting.setValue("saveResultOk", picdata.saveResultOk);
		setting.setValue("saveResultNg", picdata.saveResultNg);
		setting.setValue("saveResultAll", picdata.saveResultAll);
		setting.setValue("resultImageBmp", picdata.resultImageBmp);

		setting.setValue("picAddress", picdata.picAddress);
		setting.setValue("isMes", picdata.isMes);
		setting.setValue("QR_num", picdata.QR_num);

		for (int var = 0; var < picdata.QR_num; ++var) {
			setting.setValue(QString("QR%1").arg(var + 1), picdata.QR_plc_address[var]);
			setting.setValue(QString("QR_len%1").arg(var + 1), picdata.QR_len[var]);
		}
		setting.setValue("mes_num", picdata.mes_num);
		for (int i = 0; i < picdata.mes_num; i++)
		{
			setting.setValue(QString("mes%1").arg(i + 1), picdata.picPath2PLCaddress[i]);
		}

		setting.endGroup();
	}
}

void CameraSettingUI::picNumChanged(int v)
{
	int bias = v - last_pic_number;
	if (bias > 0)
	{
		for (int i = last_pic_number + 1; i <= v; i++)
		{
			auto w = new PicSettingUI(data);
			w->picID = i;
			w->init();
			ui.tabWidget->addTab(w, QString("pic%1").arg(i));
			picWidgets.append(w);
		}
	}
	else {
		for (int i = last_pic_number; i > v; i--)
		{
			ui.tabWidget->removeTab(i);
			delete picWidgets[i - 1];
			picWidgets[i - 1] = nullptr;
			picWidgets.takeLast();
		}
	}
	last_pic_number = v;
}

bool CameraSettingUI::init()
{
	iniUI();
	return false;
}

void CameraSettingUI::iniUI()
{
	hwin = new QHalconWindow(ui.widget, ui.widget->width(), ui.widget->height(), ui.widget->width(), ui.widget->height());
	hwin->GetHalconBuffer()->SetWindowParam("background_color", "#dadada");
	hwin->GetHalconBuffer()->SetFont("Arial-20");
	hwin->GetHalconBuffer()->SetColor("blue");
	hwin->GetHalconBuffer()->SetDraw("margin");

	ui.cb_device->clear();
	ui.cb_device->addItem(data.DeviceName);
	ui.cb_device->setCurrentText(data.DeviceName);

	ui.sb_baoguang->setValue(data.CamExposuretime);

	ui.cb_source->setCurrentText(data.TriggerSource);

	ui.sb_w->setValue(data.cam_width);

	ui.sb_h->setValue(data.cam_hight);

	ui.le_ip->setText(data.IP);

	ui.sb_port->setValue(data.Port);

	ui.sb_T->setValue(data.ScanningCycle);

	ui.sb_picaddress->setValue(data.CamplcGrabmark);

	ui.sb_picokaddress->setValue(data.CamplcGrabok);

	ui.sb_picnum->setValue(data.picNum);

	ui.le_yuantu->setText(data.originalImageRootPath);

	ui.le_result->setText(data.resultImageRootPath);
}

void CameraSettingUI::saveData()
{
	data.DeviceName					= ui.cb_device->currentText();
	data.CamExposuretime			= ui.sb_baoguang->value();
	data.TriggerSource				= ui.cb_source->currentText();
	data.cam_width					= ui.sb_w->value();
	data.cam_hight					= ui.sb_h->value();
	data.IP							= ui.le_ip->text();
	data.Port						= ui.sb_port->value();
	data.ScanningCycle				= ui.sb_T->value();
	data.CamplcGrabmark				= ui.sb_picaddress->value();
	data.CamplcGrabok				= ui.sb_picokaddress->value();
	data.picNum						= ui.sb_picnum->value();
	data.originalImageRootPath		= ui.le_yuantu->text();
	data.resultImageRootPath		= ui.le_result->text();
}

void CameraSettingUI::showMessage(const QString str)
{
	ui.textBrowser->append(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.zzz: ") + str);
}

bool CameraSettingUI::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize) {
		hwin->resize(ui.widget->width(), ui.widget->height());
		hwin->SetImgSize(data.cam_width, data.cam_hight);
	}
	// standard event processing
	return QObject::eventFilter(watched, event);
}

void CameraSettingUI::slot_pb_open()
{
	if (showimage==nullptr)
	{
		showimage.reset(new ShowImage(hwin));
		show_thread.reset(new QThread);
		showimage->moveToThread(show_thread.get());
		connect(this, &CameraSettingUI::continueShow, showimage.get(), &ShowImage::start);
		connect(this, &CameraSettingUI::stopShow, showimage.get(), &ShowImage::stop);
		connect(showimage.get(), &ShowImage::message, this, &CameraSettingUI::showMessage);
		show_thread->start();
	}
	ui.pb_start->setEnabled(false);
	emit continueShow(ui.cb_device->currentText(), ui.sb_baoguang->value());
}

void ShowImage::start(QString device, int CamExposuretime)
{
	try
	{
		HImage image;//ME2P-1220-9GM-P-6P(00-21-49-05-B8-0F)
		OpenFramegrabber("GenICamTL", 0, 0, 0, 0, 0, 0, "progressive", -1, "default", -1,
			"false", "default", device.toStdString().c_str(), 0, -1, &hv_AcqHandle);
		SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "Off");
		SetFramegrabberParam(hv_AcqHandle, "TriggerSource", "Software");
		SetFramegrabberParam(hv_AcqHandle, "grab_timeout", 300);
		SetFramegrabberParam(hv_AcqHandle, "ExposureTime", CamExposuretime);
		show = true;
		open = true;
		while (show)
		{
			GrabImageAsync(&image, hv_AcqHandle, -1);
			//hwin->GetHalconBuffer()->ClearWindow();//清除之前的窗口内容
			hwin->GetHalconBuffer()->DispObj(image);
			hwin->GetHalconBuffer()->FlushBuffer();
			QCoreApplication::processEvents();
		}
	}
	catch (HException& except)
	{
		QString mes = QString(u8"错误代码：%1\n错误算子：%2\n错误信息：%3").arg(except.ErrorCode()).arg(except.ProcName().Text(), except.ErrorMessage().Text());
		emit message(mes);
	}
}

void ShowImage::stop()
{
	show = false;
	if (open)
	{
		CloseFramegrabber(hv_AcqHandle);
		open = false;
	}
}