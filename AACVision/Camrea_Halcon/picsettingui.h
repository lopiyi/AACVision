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
	// �����������
	int last_input_num = 0;
	//QVector<std::shared_ptr<FormWidget>> inputWidgets;
	QVector<std::shared_ptr<FormWidgetCustom>> inputWidgets;
	//// ���ղ�������
	//int last_receive_num = 0;
	//QVector<std::shared_ptr<NumWidget>> receiveWidgets;
	// ��ά���������
	int last_QR_num = 0;
	QVector<std::shared_ptr<FormWidget>> QRWidgets;
	// �ϴ����ݿ���
	int last_emit_num = 0;
	QVector<std::shared_ptr<NumWidget>> emitWidgets;
	// �����������
	int last_output_num = 0;
	QVector<std::shared_ptr<NumWidget>> outputWidgets;
	// �����������
	int last_transmit_num = 0;
	QVector<std::shared_ptr<FormWidget>> transmitWidgets;
	// ���ͼƬ��������
	int last_outputpic_num = 0;
	QVector<std::shared_ptr<NumWidget>> outputpicWidgets;
	// ����ַ���������
	int last_outputstr_num = 0;
	QVector<std::shared_ptr<FormWidget>> outputstrWidgets;
	// ���ղ�������
	int last_receive_num = 0;
	QVector<std::shared_ptr<NumWidget>> receiveWidgets;
};
