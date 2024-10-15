#pragma once

#include "drawitemabstract.h"
#include <QGraphicsSceneMouseEvent>

class MyPointItem:public DrawItemAbstract
{
public:
	MyPointItem();
    MyPointItem(const QString path);
	virtual ~MyPointItem();

    virtual QString name();
    virtual void init(QPointF p) override;
    virtual bool save() override;
    virtual bool isNull() override;
    virtual void setMousePos(QPointF p) override;

    int type() const override { return UserType + 1; }
private:
    enum STATE_FLAG {
        DEFAULT_FLAG = 0,
        MOVE_POINT
    };
    STATE_FLAG m_StateFlag;
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=nullptr);
    //    QPainterPath shape() const;
    QRectF  boundingRect() const;
    void setRectSize();
private:
    bool ok=true;
    double size = 7;
    double s_size=9999999;
    QPointF myPoint, biasClicked;
    QPolygonF m_oldRectPolygon;
    QPointF m_startPos;
};

class PointFac : public Factory
{
public:
    DrawItemAbstract* createItem() override
    {
        return new MyPointItem;
    }
    DrawItemAbstract* createItem(const QString path) override
    {
        MyPointItem* item = new MyPointItem(path);
        if (item->isNull())
        {
            return item;
        }
        delete item;
        return nullptr;
    }
};