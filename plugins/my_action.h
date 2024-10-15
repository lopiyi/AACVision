#pragma once
#include <qaction.h>
#include <QString>

class MyAction:public QAction
{
public:
	MyAction(const QString path):path(path){}
	~MyAction(){}
	QString location() { return path; }
private:
	QString path;
};
