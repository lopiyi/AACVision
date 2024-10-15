#include "picsettingui.h"
#include <qgridlayout.h>
#include <qdebug.h>
#include <QSettings>
#include <QTextCodec>
#include <QFileDialog>

PicSettingUI::PicSettingUI(Common::CameraHalconData& data, QWidget *parent)
	: QWidget(parent), data(data)
{
	ui.setupUi(this);

	// 设置比例
	QList<int> sizes;
	// 在保持比例的情况下，绝对值要尽量大
	sizes << 10000 << 20000 << 20000;
	ui.splitter->setSizes(sizes);

	QGridLayout* gridlayout1 = new QGridLayout(ui.scrollArea);
	ui.scrollArea->widget()->setLayout(gridlayout1);
	connect(ui.sb_input_num, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		inputNumChanged(v);
		});
	
	//QGridLayout* gridlayout2 = new QGridLayout(ui.scrollArea_2);
	//ui.scrollArea_2->widget()->setLayout(gridlayout2);
	//connect(ui.sb_receive_num, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
	//	receiveNumChanged(v);
	//	});

	QGridLayout* gridlayout3 = new QGridLayout(ui.scrollArea_7);
	ui.scrollArea_7->widget()->setLayout(gridlayout3);
	connect(ui.sb_QR_num, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		QRNumChanged(v);
		});

	QGridLayout* gridlayout4 = new QGridLayout(ui.scrollArea_8);
	ui.scrollArea_8->widget()->setLayout(gridlayout4);
	connect(ui.sb_mes_num, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		emitNumChanged(v);
		});

	QGridLayout* gridlayout5 = new QGridLayout(ui.scrollArea_3);
	ui.scrollArea_3->widget()->setLayout(gridlayout5);
	connect(ui.sb_output_num, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		outputNumChanged(v);
		});

	QGridLayout* gridlayout6 = new QGridLayout(ui.scrollArea_4);
	ui.scrollArea_4->widget()->setLayout(gridlayout6);
	connect(ui.sb_emit_num, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		transmitNumChanged(v);
		});

	QGridLayout* gridlayout7 = new QGridLayout(ui.scrollArea_5);
	ui.scrollArea_5->widget()->setLayout(gridlayout7);
	connect(ui.sb_output_picnum, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		outputpicNumChanged(v);
		});

	QGridLayout* gridlayout8 = new QGridLayout(ui.scrollArea_8);
	ui.scrollArea_6->widget()->setLayout(gridlayout8);
	connect(ui.sb_output_string_num, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		outputstrNumChanged(v);
		});

	QGridLayout* gridlayout9 = new QGridLayout(ui.scrollArea_2);
	ui.scrollArea_2->widget()->setLayout(gridlayout9);
	connect(ui.sb_receiveData, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
		receiveDataNumChanged(v);
		});

	
}

PicSettingUI::~PicSettingUI()
{}

bool PicSettingUI::init()
{
	if (picID>data.MyPicDatas.size())
	{
		readDafaultConfig();
	}
	else {
		picdata = data.MyPicDatas[picID - 1];
	}

	ui.le_picname					->setText(picdata.picname);
	ui.le_algorithmName				->setText(picdata.algorithmName);
	ui.le_vectorShowString			->setText(picdata.vectorShowString);
	ui.le_vectorShowValue			->setText(picdata.vectorShowValue);
	ui.le_imageInputName			->setText(picdata.imageInputName);
	ui.le_OKNG_value				->setText(picdata.ok_value_list);
	ui.sb_valueNum					->setValue(picdata.valueResultNum);
	ui.sb_firstAddress				->setValue(picdata.picAddress);
	ui.cb_stastics					->setChecked(picdata.statics);
	ui.cb_finnalPic					->setChecked(picdata.finnalPic);
	ui.cb_originalImage				->setChecked(picdata.originalImage);
	ui.cb_saveOriginalOk			->setChecked(picdata.saveOriginalOk);
	ui.cb_saveOriginalNg			->setChecked(picdata.saveOriginalNg);
	ui.cb_saveOriginalAll			->setChecked(picdata.saveOriginalAll);
	ui.cb_originalImageBmp			->setChecked(picdata.originalImageBmp);
	
	ui.cb_resultImage				->setChecked(picdata.resultImage);
	ui.cb_saveResultOk				->setChecked(picdata.saveResultOk);
	ui.cb_saveResultNg				->setChecked(picdata.saveOriginalNg);
	ui.cb_saveResultAll				->setChecked(picdata.saveResultAll);
	ui.cb_resultImageBmp			->setChecked(picdata.resultImageBmp);
	
	ui.sb_frontSize					->setValue(picdata.frontSize);
	ui.sb_lineWidth					->setValue(picdata.lineWidth);
	ui.sb_okng_flag					->setValue(picdata.OKNG_flag);
	ui.dsb_resultImageScale			->setValue(picdata.resultImageScale);
	ui.cb_showInput					->setChecked(picdata.showInput);

	///*******************************************************************///
	ui.sb_input_num->setValue(picdata.input_num);
	for (int i = 0; i < picdata.input_num; i++)
	{
		//inputWidgets[i]->setLineText(1, picdata.inputname[i]);
		//inputWidgets[i]->setLineText(2, QString("%1").arg(picdata.inputData.singleData.value(picdata.inputname[i]).toDouble()));
		inputWidgets[i]->setLineText(1, picdata.inputParamNameLabels[i]);
		inputWidgets[i]->setLineText(2, picdata.inputname[i]);
		inputWidgets[i]->setLineText(3, picdata.inputParamValueLabels[i]);
		inputWidgets[i]->setLineText(4, QString::number(picdata.inputData.singleData.value(picdata.inputname[i]).toDouble()));
	}

	ui.sb_receiveData->setValue(picdata.receiveDataNum);
	for (int i = 0; i < picdata.receiveDataNum; i++)
	{
		receiveWidgets[i]->setLineText(picdata.receiveDataCamPic[i]);
	}
	//ui.cb_receiveData->setChecked(picdata.receiveData);
	//auto cam_pic = picdata.receiveDataCamPic.split("-");
	//ui.sb_camID->setValue(cam_pic.first().toInt());
	//ui.sb_picID->setValue(cam_pic.last().toInt());
	
	ui.cb_isMes->setChecked(picdata.isMes);
	ui.sb_QR_num->setValue(picdata.QR_num);
	for (int i = 0; i < picdata.QR_num; i++)
	{
		QRWidgets[i]->setLineText(1, QString::number(picdata.QR_plc_address[i]));
		QRWidgets[i]->setLineText(2, QString::number(picdata.QR_len[i]));
	}
	ui.sb_mes_num->setValue(picdata.mes_num);
	for (int i = 0; i < picdata.mes_num; i++)
	{
		emitWidgets[i]->setLineText(QString::number(picdata.picPath2PLCaddress[i]));
	}

	///*******************************************************************///
	ui.sb_output_num->setValue(picdata.result_num);
	for (int i = 0; i < picdata.result_num; i++)
	{
		outputWidgets[i]->setLineText(picdata.outputname[i]);
	}
	ui.cb_isEmit->setChecked(picdata.isEmit);
	ui.sb_emit_num->setValue(picdata.emitResultNum);
	for (int i = 0; i < picdata.emitResultNum; i++)
	{
		transmitWidgets[i]->setLineText(1, picdata.emitParamNames[i]);
		transmitWidgets[i]->setLineText(2, picdata.receiveParamNames[i]);
	}
	ui.sb_output_picnum->setValue(picdata.imageResultNum);
	for (int i = 0; i < picdata.imageResultNum; i++)
	{
		outputpicWidgets[i]->setLineText(picdata.imageResultName[i]);
	}
	ui.sb_output_string_num->setValue(picdata.stringResultNum);
	for (int i = 0; i < picdata.stringResultNum; i++)
	{
		outputstrWidgets[i]->setLineText(1, picdata.stringResultNames[i]);
		outputstrWidgets[i]->setLineText(2, QString::number(picdata.stringResult2PLCAddress[i]));
	}
	return false;
}

Common::PicData PicSettingUI::saveData()
{
	picdata.picname						= ui.le_picname->text();
	picdata.algorithmName				= ui.le_algorithmName->text();
	picdata.vectorShowString			= ui.le_vectorShowString->text();
	picdata.vectorShowValue				= ui.le_vectorShowValue->text();
	picdata.imageInputName				= ui.le_imageInputName->text();
	picdata.ok_value_list				= ui.le_OKNG_value->text();
	picdata.valueResultNum				= ui.sb_valueNum->value();
	picdata.picAddress					= ui.sb_firstAddress->value();
	picdata.OKNG_flag					= ui.sb_okng_flag->value();
	picdata.statics						= ui.cb_stastics->isChecked();
	picdata.finnalPic					= ui.cb_finnalPic->isChecked();
	picdata.originalImage				= ui.cb_originalImage->isChecked();
	picdata.saveOriginalOk				= ui.cb_saveOriginalOk->isChecked();
	picdata.saveOriginalNg				= ui.cb_saveOriginalNg->isChecked();
	picdata.saveOriginalAll				= ui.cb_saveOriginalAll->isChecked();
	picdata.originalImageBmp			= ui.cb_originalImageBmp->isChecked();
	
	picdata.resultImage					= ui.cb_resultImage->isChecked();
	picdata.saveResultOk				= ui.cb_saveResultOk->isChecked();
	picdata.saveOriginalNg				= ui.cb_saveResultNg->isChecked();
	picdata.saveResultAll				= ui.cb_saveResultAll->isChecked();
	picdata.resultImageBmp				= ui.cb_resultImageBmp->isChecked();
	
	picdata.frontSize					= ui.sb_frontSize->value();
	picdata.lineWidth					= ui.sb_lineWidth->value();
	picdata.resultImageScale			= ui.dsb_resultImageScale->value();
	picdata.showInput					= ui.cb_showInput->isChecked();

	///*******************************************************************///
	picdata.input_num = ui.sb_input_num->value();
	picdata.inputname.clear();
	picdata.inputData.singleData.clear();
	picdata.inputParamNameLabels.clear();
	picdata.inputParamValueLabels.clear();
	for (int i = 0; i < ui.sb_input_num->value(); i++)
	{
		picdata.inputParamNameLabels.append(inputWidgets[i]->getLineText(1));
		picdata.inputname.append(inputWidgets[i]->getLineText(2));
		picdata.inputParamValueLabels.append(inputWidgets[i]->getLineText(3));
		picdata.inputData.singleData.insert(inputWidgets[i]->getLineText(2), inputWidgets[i]->getLineText(4).toDouble());
	}

	picdata.receiveDataNum = ui.sb_receiveData->value();
	picdata.receiveDataCamPic.clear();
	for (int i = 0; i < ui.sb_receiveData->value(); i++)
	{
		picdata.receiveDataCamPic << receiveWidgets[i]->getLineText();
	}

	//picdata.receiveData = ui.cb_receiveData->isChecked();
	//picdata.receiveDataCamPic = QString("%1-%2").arg(ui.sb_camID->value()).arg(ui.sb_picID->value());

	picdata.isMes = ui.cb_isMes->isChecked();
	picdata.QR_num = ui.sb_QR_num->value();
	picdata.QR_plc_address.clear();
	picdata.QR_len.clear();
	for (int i = 0; i < ui.sb_QR_num->value(); i++)
	{
		picdata.QR_plc_address.append(QRWidgets[i]->getLineText(1).toInt());
		picdata.QR_len.append(QRWidgets[i]->getLineText(2).toInt());
	}
	picdata.mes_num = ui.sb_mes_num->value();
	picdata.picPath2PLCaddress.clear();
	for (int i = 0; i < picdata.mes_num; i++)
	{
		picdata.picPath2PLCaddress.append(emitWidgets[i]->getLineText().toInt());
	}

	///*******************************************************************///
	picdata.result_num = ui.sb_output_num->value();
	picdata.outputname.clear();
	for (int i = 0; i < ui.sb_output_num->value(); i++)
	{
		picdata.outputname.append(outputWidgets[i]->getLineText());
	}
	picdata.isEmit = ui.cb_isEmit->isChecked();
	picdata.emitResultNum = ui.sb_emit_num->value();
	picdata.emitParamNames.clear();
	picdata.receiveParamNames.clear();
	for (int i = 0; i < picdata.emitResultNum; i++)
	{
		picdata.emitParamNames.append(transmitWidgets[i]->getLineText(1));
		picdata.receiveParamNames.append(transmitWidgets[i]->getLineText(2));
	}
	picdata.imageResultNum = ui.sb_output_picnum->value();
	picdata.imageResultName.clear();
	for (int i = 0; i < picdata.imageResultNum; i++)
	{
		picdata.imageResultName.append(outputpicWidgets[i]->getLineText());
	}
	picdata.stringResultNum = ui.sb_output_string_num->value();
	picdata.stringResultNames.clear();
	picdata.stringResult2PLCAddress.clear();
	for (int i = 0; i < picdata.stringResultNum; i++)
	{
		picdata.stringResultNames.append(outputstrWidgets[i]->getLineText(1));
		picdata.stringResult2PLCAddress.append(outputstrWidgets[i]->getLineText(2).toInt());
	}

	return picdata;
}

//void PicSettingUI::inputNumChanged(int v)
//{
//	int bias = v - last_input_num;
//	int vertical = 2;
//	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea->widget()->layout());
//	if (bias>0)
//	{
//		for (int i = last_input_num+1; i <= v; i++)
//		{
//			int Row = (i + vertical - 1) / vertical - 1;
//			int Col = i - Row * vertical - 1;
//			QStringList l;
//			l << QString(u8"输入参数%1名：").arg(i) << QString(u8"参数%1默认值：").arg(i);
//			std::shared_ptr<FormWidget> w = std::make_shared<FormWidget>(l);
//			layout->addWidget(w.get(), Row, Col);
//			inputWidgets.append(w);
//		}
//	}
//	else {
//		for (int i = last_input_num; i > v; i--)
//		{
//			layout->removeWidget(inputWidgets[i - 1].get());
//			inputWidgets.takeLast();
//		}
//	}
//	last_input_num = v;
//}
void PicSettingUI::inputNumChanged(int v)
{
	int bias = v - last_input_num;
	int vertical = 1;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea->widget()->layout());
	if (bias>0)
	{
		for (int i = last_input_num+1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			QStringList l;
			l << "" << "" << "" << "";
			std::shared_ptr<FormWidgetCustom> w = std::make_shared<FormWidgetCustom>(l);
			layout->addWidget(w.get(), Row, Col);
			inputWidgets.append(w);
		}
	}
	else {
		for (int i = last_input_num; i > v; i--)
		{
			layout->removeWidget(inputWidgets[i - 1].get());
			inputWidgets.takeLast();
		}
	}
	last_input_num = v;
}

void PicSettingUI::readDafaultConfig()
{
	QString path = QString("./settings/pic_default.ini");
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));
	setting.beginGroup("PIC");
	picdata.picname = setting.value("picname").toString();														// pic名称
	picdata.algorithmName = setting.value("algorithmName").toString();											// 算法名称
	picdata.input_num = setting.value("inputparam_num").toInt();														// 输入参数个数
	picdata.statics = setting.value("statics").toBool();															// 本pic是否参数okng统计
	picdata.finnalPic = setting.value("finnalPic").toBool();														// 是否为最后一个pic，用于统计相机的ok数
	picdata.OKNG_flag = setting.value("OKNG_flag").toInt();
	picdata.ok_value_list = setting.value("ok_value_list").toString();
	picdata.inputData.singleData.clear();
	picdata.inputname.clear();
	picdata.inputParamNameLabels.clear();
	picdata.inputParamValueLabels.clear();
	for (int i = 0; i < picdata.input_num; i++)
	{
		picdata.inputParamNameLabels.append(setting.value(QString("inputparam%1namelabel").arg(i + 1)).toString());			// 输入参数标签
		picdata.inputname.append(setting.value(QString("inputparam%1name").arg(i + 1)).toString());						// 输入参数名
		picdata.inputParamValueLabels.append(setting.value(QString("inputparam%1valuelabel").arg(i + 1)).toString());	// 输入参数值标签
		picdata.inputData.singleData.insert(picdata.inputname[i], setting.value(QString("inputparam%1value").arg(i + 1)));		// 输入参数默认值
	}
	picdata.result_num = setting.value("result_num").toInt();															// 输出参数个数
	picdata.outputname.clear();

	for (int i = 0; i < picdata.result_num; i++)
	{
		picdata.outputname.append(setting.value(QString("result%1name").arg(i + 1)).toString());						// 输出参数名
	}
	picdata.valueResultNum = setting.value("valueResultNum").toInt();													// 输出结果中单个值参数的个数

	picdata.stringResultNum = setting.value("stringResultNum").toInt();													// 输出结果中字符串值参数的个数
	picdata.stringResultNames.clear();
	picdata.stringResult2PLCAddress.clear();
	for (int i = 0; i < picdata.stringResultNum; i++)
	{
		picdata.stringResultNames.append(setting.value(QString("stringResul%1name").arg(i + 1)).toString());			// 输出结果中字符串值参数的名称
		picdata.stringResult2PLCAddress.append(setting.value(QString("stringResult2PLCAddress%1").arg(i + 1)).toInt());	// 输出结果中字符串值参数写入PLC的地址
	}
	picdata.vectorShowString = setting.value("vectorShowString").toString();											// 输出结果中用于屏幕显示字符串参数名称
	picdata.vectorShowValue = setting.value("vectorShowValue").toString();												// 输出结果中用于屏幕显示值参数名称
	//picdata.receiveData = setting.value("receiveData").toBool();														// 是否开启接收参数
	picdata.receiveDataNum = setting.value("receiveDataNum").toInt();													// 接收的参数来源数目
	for (int i = 0; i < picdata.receiveDataNum; i++)
	{
		picdata.receiveDataCamPic << setting.value(QString("receiveDataCamPic%1").arg(i + 1)).toString();				// 接收的相机和pic，格式为3-1、2-2等
	}
	picdata.isEmit = setting.value("isEmit").toBool();																	// 是否开启相机间数据传递

	picdata.emitResultNum = setting.value("emitResultNum").toInt();														// 传递数据的个数
	picdata.emitParamNames.clear();
	picdata.receiveParamNames.clear();
	for (int i = 0; i < picdata.emitResultNum; i++)
	{
		picdata.emitParamNames.append(setting.value(QString("emit%1name").arg(i + 1)).toString());						// 传递数据的名称
		picdata.receiveParamNames.append(setting.value(QString("receive%1name").arg(i + 1)).toString());				// 接受者的数据名称
	}

	picdata.imageInputName = setting.value("imageInputName").toString();

	picdata.imageResultNum = setting.value("imageResultNum").toInt();													// 输出图片的数量
	for (int i = 0; i < picdata.imageResultNum; i++)
	{
		picdata.imageResultName.append(setting.value(QString("imageResult%1name").arg(i + 1)).toString());				// 输出图片所在参数名
	}

	// 原图
	picdata.originalImage = setting.value("originalImage").toBool();
	
	picdata.saveOriginalOk = setting.value("saveOriginalOk").toBool();
	picdata.saveOriginalNg = setting.value("saveOriginalNg").toBool();
	picdata.saveOriginalAll = setting.value("saveOriginalAll").toBool();
	picdata.originalImageBmp = setting.value("originalImageBmp").toBool();
	// 结果图
	picdata.resultImage = setting.value("resultImage").toBool();
	picdata.frontSize = setting.value("frontSize").toInt();
	picdata.lineWidth = setting.value("lineWidth").toInt();
	picdata.resultImageScale = setting.value("resultImageScale").toDouble();
	
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
}

void PicSettingUI::QRNumChanged(int v)
{
	int bias = v - last_QR_num;
	int vertical = 1;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea_7->widget()->layout());
	if (bias > 0)
	{
		for (int i = last_QR_num + 1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			QStringList l;
			l << QString(u8"二维码%1地址：").arg(i) << QString(u8"二维码%1长度：").arg(i);
			std::shared_ptr<FormWidget> w = std::make_shared<FormWidget>(l);
			layout->addWidget(w.get(), Row, Col);
			QRWidgets.append(w);
		}
	}
	else {
		for (int i = last_QR_num; i > v; i--)
		{
			layout->removeWidget(QRWidgets[i - 1].get());
			QRWidgets.takeLast();
		}
	}
	last_QR_num = v;
}

void PicSettingUI::emitNumChanged(int v)
{
	int bias = v - last_emit_num;
	int vertical = 1;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea_8->widget()->layout());
	if (bias > 0)
	{
		for (int i = last_emit_num + 1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			std::shared_ptr<NumWidget> w = std::make_shared<NumWidget>(QString(u8"数据%1地址：").arg(i));
			layout->addWidget(w.get(), Row, Col);
			emitWidgets.append(w);
		}
	}
	else {
		for (int i = last_emit_num; i > v; i--)
		{
			layout->removeWidget(emitWidgets[i - 1].get());
			emitWidgets.takeLast();
		}
	}
	last_emit_num = v;
}

void PicSettingUI::outputNumChanged(int v)
{
	int bias = v - last_output_num;
	int vertical = 2;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea_3->widget()->layout());
	if (bias > 0)
	{
		for (int i = last_output_num + 1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			std::shared_ptr<NumWidget> w = std::make_shared<NumWidget>(QString(u8"输出参数%1：").arg(i));
			layout->addWidget(w.get(), Row, Col);
			outputWidgets.append(w);
		}
	}
	else {
		for (int i = last_output_num; i > v; i--)
		{
			layout->removeWidget(outputWidgets[i - 1].get());
			outputWidgets.takeLast();
		}
	}
	last_output_num = v;
}

void PicSettingUI::transmitNumChanged(int v)
{
	int bias = v - last_transmit_num;
	int vertical = 1;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea_4->widget()->layout());
	if (bias > 0)
	{
		for (int i = last_transmit_num + 1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			QStringList l;
			l << QString(u8"发射参数%1：").arg(i) << QString(u8"接收参数%1：").arg(i);
			std::shared_ptr<FormWidget> w = std::make_shared<FormWidget>(l);
			layout->addWidget(w.get(), Row, Col);
			transmitWidgets.append(w);
		}
	}
	else {
		for (int i = last_transmit_num; i > v; i--)
		{
			layout->removeWidget(transmitWidgets[i - 1].get());
			transmitWidgets.takeLast();
		}
	}
	last_transmit_num = v;
}

void PicSettingUI::outputpicNumChanged(int v)
{
	int bias = v - last_outputpic_num;
	int vertical = 2;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea_5->widget()->layout());
	if (bias > 0)
	{
		for (int i = last_outputpic_num + 1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			std::shared_ptr<NumWidget> w = std::make_shared<NumWidget>(QString(u8"图片参数%1：").arg(i));
			layout->addWidget(w.get(), Row, Col);
			outputpicWidgets.append(w);
		}
	}
	else {
		for (int i = last_outputpic_num; i > v; i--)
		{
			layout->removeWidget(outputpicWidgets[i - 1].get());
			outputpicWidgets.takeLast();
		}
	}
	last_outputpic_num = v;
}

void PicSettingUI::outputstrNumChanged(int v)
{
	int bias = v - last_outputstr_num;
	int vertical = 1;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea_6->widget()->layout());
	if (bias > 0)
	{
		for (int i = last_outputstr_num + 1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			QStringList l;
			l << QString(u8"字符参数%1：").arg(i) << QString(u8"写入地址%1：").arg(i);
			std::shared_ptr<FormWidget> w = std::make_shared<FormWidget>(l);
			layout->addWidget(w.get(), Row, Col);
			outputstrWidgets.append(w);
		}
	}
	else {
		for (int i = last_outputstr_num; i > v; i--)
		{
			layout->removeWidget(outputstrWidgets[i - 1].get());
			outputstrWidgets.takeLast();
		}
	}
	last_outputstr_num = v;
}

void PicSettingUI::receiveDataNumChanged(int v)
{
	int bias = v - last_receive_num;
	int vertical = 2;
	QGridLayout* layout = qobject_cast<QGridLayout*>(ui.scrollArea_2->widget()->layout());
	if (bias > 0)
	{
		for (int i = last_receive_num + 1; i <= v; i++)
		{
			int Row = (i + vertical - 1) / vertical - 1;
			int Col = i - Row * vertical - 1;
			std::shared_ptr<NumWidget> w = std::make_shared<NumWidget>(QString(u8"接收参数%1：").arg(i));
			layout->addWidget(w.get(), Row, Col);
			receiveWidgets.append(w);
		}
	}
	else {
		for (int i = last_receive_num; i > v; i--)
		{
			layout->removeWidget(receiveWidgets[i - 1].get());
			receiveWidgets.takeLast();
		}
	}
	last_receive_num = v;
}