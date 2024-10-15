#include "myscene.h"
#include "mypointitem.h"
#include "mylineitem.h"
#include "myPolylineItem.h"
#include <QDebug>

MyScene::MyScene(QObject *parent)
	: QGraphicsScene(parent)
{
	itemFactorys.insert(u8"����", new PointFac);
	itemClasses << u8"����";

	itemFactorys.insert(u8"����", new LineFac);
	itemClasses << u8"����";

	itemFactorys.insert(u8"������", new PolygonFac);
	itemClasses << u8"������";
}

MyScene::~MyScene()
{}

void MyScene::setDrawItemClass(const QString itemClass)
{
	if (itemClasses.contains(itemClass))
	{
		this->currentItemClass = itemClass;
	}
	else {
		dafaultState();
	}
}

QStringList& MyScene::getItemClasses()
{
	return itemClasses;
}

QHash<QString, Factory*> MyScene::getItemFactorys()
{
	// TODO: �ڴ˴����� return ���
	return itemFactorys;
}

void MyScene::dafaultState(bool ok)
{
	this->currentItemClass.clear();
	if (ok)
	{
		if (this->currentItem)
		{
			this->currentItem = nullptr;
		}
	}
	else
	{
		if (this->currentItem)
		{
			removeItem(currentItem);
			this->currentItem = nullptr;
		}
	}
	update();
}

void MyScene::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape) {
		if (currentItem!=nullptr)
		{
			dafaultState(false);
		}
	}
	QGraphicsScene::keyPressEvent(event);
}

void MyScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->buttons() == Qt::LeftButton && !currentItemClass.isEmpty()) {
		if (currentItem == nullptr)
		{
			currentItem = itemFactorys.value(currentItemClass)->createItem();
			connect(currentItem, SIGNAL(emit2SceneFinished()), this, SLOT(dafaultState()));
			currentItem->init(event->scenePos());
			//currentItem->setRectSize();
			addItem(currentItem);
			return QGraphicsScene::mousePressEvent(event);
		}
		if (currentItem->isFinished())
		{
			dafaultState();
		}
		else {
			//currentItem->setMousePos(event->scenePos());
			//update();
			currentItem->addPoint(event->scenePos());
			if (currentItem->isFinished())
			{
				dafaultState();
			}
		}
	}
	if (event->buttons() == Qt::RightButton)
	{
		if (currentItem!=nullptr)
		{
			//dafaultState();
			qDebug() << "3333333333333333";
			return QGraphicsScene::mousePressEvent(event);
		}
	}
	
	QGraphicsScene::mousePressEvent(event);
}

void MyScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (currentItem != nullptr)
	{
		currentItem->setMousePos(event->scenePos());
		update();
	}
	else
	{
		QGraphicsScene::mouseMoveEvent(event);
	}
	
}
//
//void MyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
//{
//}
