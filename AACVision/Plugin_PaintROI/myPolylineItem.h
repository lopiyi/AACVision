#pragma once
#include "drawitemabstract.h"
#include <QGraphicsSceneMouseEvent>
class MyPolylineItem :public DrawItemAbstract
{
    Q_OBJECT
public:
    MyPolylineItem();
    MyPolylineItem(const QString path);
    virtual ~MyPolylineItem();

    virtual QString name();
    virtual void init(QPointF p) override;
    virtual bool save() override;
    virtual bool isNull() override;
    virtual void setMousePos(QPointF p) override;
    virtual void addPoint(QPointF p) override;

    int type() const override { return UserType + 3; }
signals:
    void emit2SceneFinished(bool ok);
private:
    enum STATE_FLAG {
        DEFAULT_FLAG = 0,
        MOVE_POLYGON,
        MOVE_NODE
    };
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    //    QPainterPath shape() const;
    QRectF  boundingRect() const;
    void setRectSize();
    int numberNode();
    double limitPoint(int mode);
public:
    bool ok = true;
    STATE_FLAG m_StateFlag;
    QPolygonF myPolygon, tem_polygon;
    QVector<QPolygonF> nodes;
    int node;
    QPointF m_startPos, mousePoint, end_point, biasClicked;
    QPolygonF m_oldRectPolygon;
    double scaleFactor;
    double size = 7;
    //double s_size = 999999;
    double size_r = 30;
};


class PolygonFac : public Factory
{
public:
    DrawItemAbstract* createItem() override
    {
        return new MyPolylineItem;
    }
    DrawItemAbstract* createItem(const QString path) override
    {
        MyPolylineItem* item = new MyPolylineItem(path);
        if (item->isNull())
        {
            return item;
        }
        delete item;
        return nullptr;
    }
};

