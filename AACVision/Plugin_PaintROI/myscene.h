#pragma once

#include <QGraphicsScene>
#include "mypointitem.h"
#include <qhash.h>
#include <memory>
#include <QKeyEvent>

class MyScene  : public QGraphicsScene
{
	Q_OBJECT

public:
	MyScene(QObject *parent);
	~MyScene();
	void setDrawItemClass(const QString itemClass);
	QStringList& getItemClasses();
	QHash<QString, Factory*> getItemFactorys();
public slots:
	void dafaultState(bool ok=true);
private:
	QHash<QString, Factory*> itemFactorys;
	QString currentItemClass;
	QStringList itemClasses;
	DrawItemAbstract* currentItem = nullptr;
protected:
	virtual void keyPressEvent(QKeyEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	//void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};
