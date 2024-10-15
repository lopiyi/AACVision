#pragma once

#include "drawitemabstract.h"
#include <QGraphicsSceneMouseEvent>

class MyRect1Item  : public DrawItemAbstract
{
	Q_OBJECT

public:
	MyRect1Item();
    MyRect1Item(const QString path);
    virtual ~MyRect1Item();

    virtual QString name();
    virtual void init(QPointF p) override;
    virtual bool save() override;
    virtual bool isNull() override;
    virtual void setMousePos(QPointF p) override;
    virtual void addPoint(QPointF p) override;
private:
    enum STATE_FLAG {
        DEFAULT_FLAG = 0,
        MOVE_RECT,
        SCALE_RECT
    };
    STATE_FLAG m_StateFlag;
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    QPainterPath shape() const;
    QRectF  boundingRect() const;

    QPolygonF getRectPolygonLine(QPointF start, QPointF end, double bias = 50.);
    void setRectSize();
    QPolygonF centerRect(QPointF point, int size);
    void switchPoint();
private:
    QPointF leftTopPoint, rigntBottomPoint, mousePoint;
    QPointF leftTop, leftBottom;
    QRectF rect;
};

class Rect1Fac : public Factory
{
public:
    DrawItemAbstract* createItem() override
    {
        return new MyRect1Item;
    }
    DrawItemAbstract* createItem(const QString path) override
    {
        MyRect1Item* item = new MyRect1Item(path);
        if (item->isNull())
        {
            return item;
        }
        delete item;
        return nullptr;
    }
};