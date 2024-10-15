#include "camerasettingui.h"
#include <qcombobox.h>
#include <QSettings>
#include <QTextCodec>
#include <QDir>
#include <QShortcut>

CameraSettingUI::CameraSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle(u8"相机系统配置");
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	QShortcut* shortcut1 = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	QObject::connect(shortcut1, &QShortcut::activated, this, &QWidget::close);
	layout = new QFormLayout(ui.scrollArea->widget());
	ui.scrollArea->widget()->setLayout(layout);
	connect(ui.pb_add, &QPushButton::clicked, this, &CameraSettingUI::addCamera);
	connect(ui.pb_sub, &QPushButton::clicked, this, &CameraSettingUI::subCamera);
	connect(ui.pb_save, &QPushButton::clicked, this, &CameraSettingUI::pb_save_clicked);
	connect(ui.spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CameraSettingUI::sb_value_changed);
	loadConfig();
	initUI();
}

CameraSettingUI::~CameraSettingUI()
{}

void CameraSettingUI::addCamera()
{
	if (cam_num<=ui.spinBox->maximum())
	{
		cameraChanged(true);
		ui.spinBox->setValue(cam_num);
	}
}

void CameraSettingUI::subCamera()
{
	cameraChanged(false);
	ui.spinBox->setValue(cam_num);
}

void CameraSettingUI::pb_save_clicked()
{
	const QString path = QString(u8"config.ini");
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));
	setting.beginGroup("Main");
	setting.setValue("cam_num", cam_num);
	setting.setValue("vertical", ui.spinBox_2->value());
	for (int i = 0; i < layout->rowCount(); i++)
	{
		QLayoutItem* item = layout->itemAt(i, QFormLayout::FieldRole);  // itemAt() FieldRole 返回列中的对象(列表布局中每一列的右边的元素)，没有则返回0
		QComboBox* b = (QComboBox*)item->widget(); // 将item的窗口部件强制转换为QLineEdit类型
		setting.setValue(QString("cam%1_sdk").arg(i + 1), b->currentText());
	}
	setting.endGroup();
}

void CameraSettingUI::cameraChanged(const bool add)
{
	if (add)
	{
		cam_num += 1;
		auto* comb = new QComboBox(this);
		comb->addItems(camSDK);
		layout->addRow(QString("CCD%1 SDK:").arg(cam_num), comb);
	}
	else {
		if (cam_num <= 0)
		{
			return;
		}
		cam_num -= 1;
		layout->removeRow(cam_num);
	}
}

void CameraSettingUI::sb_value_changed(int v)
{
	bool add = true;
	if (v<cam_num)
	{
		add = false;
	}
	int bias = qAbs(v - cam_num);
	for (int i = 0; i < bias; i++)
	{
		cameraChanged(add);
	}
}

void CameraSettingUI::loadConfig()
{
	camSDK.clear();
	QDir pluginsDir(QApplication::instance()->applicationDirPath() + QString("/plugins/cameras/"));
	QStringList nameFilters;
	nameFilters << "*.dll";
	QFileInfoList loaders = pluginsDir.entryInfoList(nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
	for (QFileInfo& loader : loaders)
	{
		camSDK << loader.baseName();
	}
	const QString path = QString(u8"config.ini");
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));
	setting.beginGroup("Main");
	int tem_cam_num = setting.value("cam_num").toInt();
	ui.spinBox->setValue(tem_cam_num);
	cam_num = tem_cam_num;
	vertical = setting.value("vertical").toInt();
	sdks.clear();
	for (int i = 0; i < cam_num; i++)
	{
		sdks << setting.value(QString("cam%1_sdk").arg(i + 1)).toString();
	}
	setting.endGroup();
}

void CameraSettingUI::initUI()
{
	//ui.spinBox->setValue(cam_num);
	for (int i = 0; i < layout->rowCount(); i++)
	{
		QLayoutItem* item = layout->itemAt(i, QFormLayout::FieldRole);  // itemAt() FieldRole 返回列中的对象(列表布局中每一列的右边的元素)，没有则返回0
		QComboBox* b = (QComboBox*)item->widget(); // 将item的窗口部件强制转换为QLineEdit类型
		b->setCurrentText(sdks.at(i));
	}
	ui.spinBox_2->setValue(vertical);
}