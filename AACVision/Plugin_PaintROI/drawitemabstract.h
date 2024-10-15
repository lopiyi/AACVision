#pragma once
#include <QObject>
#include <QGraphicsItem>

class DrawItemAbstract:public QGraphicsItem, public QObject
{
public:
	DrawItemAbstract() {}
	virtual ~DrawItemAbstract() {}
	virtual QString name() = 0;
	virtual void init(QPointF p) = 0;
	virtual bool isFinished() { return finished; }
	virtual bool save() = 0;
	virtual bool isNull() = 0;
	virtual void setMousePos(QPointF p) {};
	virtual void addPoint(QPointF p) {}
	virtual void emit2SceneFinished() {}
public:
	bool finished;
private:

};

class Factory
{
public:
	virtual DrawItemAbstract* createItem() = 0;
	virtual DrawItemAbstract* createItem(const QString path) = 0;
};