#pragma once

#include <QWidget>
#include "ui_picsettingui.h"
#include "SmallUI.h"
#include "common.hpp"

class PicSettingUI : public QWidget
{
	Q_OBJECT

public:
	PicSettingUI(Common::CameraHalconData& data, QWidget *parent = nullptr);
	~PicSettingUI();
	bool init();
	Common::PicData saveData();
	int picID;
private slots:
	void inputNumChanged(int v);
	//void receiveNumChanged(int v);
	void QRNumChanged(int v);
	void emitNumChanged(int v);
	void outputNumChanged(int v);
	void transmitNumChanged(int v);
	void outputpicNumChanged(int v);
	void outputstrNumChanged(int v);
	void receiveDataNumChanged(int v);
private:
	void readDafaultConfig();
private:
	Ui::PicSettingUIClass ui;
	Common::CameraHalconData& data;
	Common::PicData picdata;
	// 输入参数控制
	int last_input_num = 0;
	//QVector<std::shared_ptr<FormWidget>> inputWidgets;
	QVector<std::shared_ptr<FormWidgetCustom>> inputWidgets;
	//// 接收参数控制
	//int last_receive_num = 0;
	//QVector<std::shared_ptr<NumWidget>> receiveWidgets;
	// 二维码参数控制
	int last_QR_num = 0;
	QVector<std::shared_ptr<FormWidget>> QRWidgets;
	// 上传数据控制
	int last_emit_num = 0;
	QVector<std::shared_ptr<NumWidget>> emitWidgets;
	// 输出参数控制
	int last_output_num = 0;
	QVector<std::shared_ptr<NumWidget>> outputWidgets;
	// 发射参数控制
	int last_transmit_num = 0;
	QVector<std::shared_ptr<FormWidget>> transmitWidgets;
	// 输出图片参数控制
	int last_outputpic_num = 0;
	QVector<std::shared_ptr<NumWidget>> outputpicWidgets;
	// 输出字符参数控制
	int last_outputstr_num = 0;
	QVector<std::shared_ptr<FormWidget>> outputstrWidgets;
	// 接收参数控制
	int last_receive_num = 0;
	QVector<std::shared_ptr<NumWidget>> receiveWidgets;
};
