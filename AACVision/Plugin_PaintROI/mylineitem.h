#pragma once
#include "drawitemabstract.h"
#include <QGraphicsSceneMouseEvent>

class MyLineItem :public DrawItemAbstract
{
public:
    MyLineItem();
    MyLineItem(const QString path);
    virtual ~MyLineItem();

    virtual QString name();
    virtual void init(QPointF p) override;
    virtual bool save() override;
    virtual bool isNull() override;
    virtual void setMousePos(QPointF p) override;
    virtual void addPoint(QPointF p) override;

    int type() const override { return UserType + 2; }
private:
    enum STATE_FLAG {
        DEFAULT_FLAG = 0,
        MOVE_LINE,
        MOVE_START_POINT,
        MOVE_END_POINT
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
private:
    bool ok = true;
    double scaleFactor=1.;
    QPointF m_startPos, m_endPos, mousePoint, biasClicked;
    QPolygonF myLine;
    //    QRectF  m_oldRect;
    QPolygonF m_oldRectPolygon;

    QPolygonF m_insicedPolygon;
    //    QRectF  m_insicedRectf;

    QPolygonF m_startPolygon;
    //    QRectF  m_startRectf;

    QPolygonF m_endPolygon;
    //    QRectF  m_endRectf;
};

class LineFac : public Factory
{
public:
    DrawItemAbstract* createItem() override
    {
        return new MyLineItem;
    }
    DrawItemAbstract* createItem(const QString path) override
    {
        MyLineItem* item = new MyLineItem(path);
        if (item->isNull())
        {
            return item;
        }
        delete item;
        return nullptr;
    }
};
