#pragma once
#include <QWidget>

class MyWidget:public QWidget
{
public:
	MyWidget() {};
	virtual ~MyWidget() {};
	virtual void setValue(const QString flag, void* v) = 0;
};
