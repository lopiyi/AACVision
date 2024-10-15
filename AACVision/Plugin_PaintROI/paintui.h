#pragma once

#include <QMainWindow>
#include "ui_paintui.h"
#include "myview.h"
#include <qlabel.h>

class PaintUI : public QMainWindow
{
	Q_OBJECT

public:
	PaintUI(QWidget *parent = nullptr);
	~PaintUI();
public slots:
	void slot_pb_open();
private:
	Ui::PaintUIClass ui;
	MyView* myview;
	MyScene* myscene;
	QLabel* pathInfo = nullptr;
	QLabel* stateInfo = nullptr;
	QLabel* corInfo = nullptr;
	QLabel* pixValue = nullptr;
};
