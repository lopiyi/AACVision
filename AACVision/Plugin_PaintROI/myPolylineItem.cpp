#include "myPolylineItem.h"
#include <QPen>
#include <QPainter>
#include <QCursor>
#include <QGraphicsScene>
#include <qwidget.h>
#include <qdebug.h>
#include <QtCore/qmath.h>
#include <QStyleOptionGraphicsItem>
#include <QFileDialog>
#include <HALCONCpp/HalconCpp.h>
#include <QMessageBox>

using namespace HalconCpp;

MyPolylineItem::MyPolylineItem()
{
}

MyPolylineItem::MyPolylineItem(const QString path)
{
    ok = false;
    try
    {
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        HObject  ho_Contours;
        HTuple hv_Row, hv_Col, hv_DxfStatus;
        ReadContourXldDxf(&ho_Contours, path.toStdString().c_str(), HTuple(), HTuple(), &hv_DxfStatus);
        GetContourXld(ho_Contours, &hv_Row, &hv_Col);
        if (hv_Row.Length() <= 1)
        {
            return;
        }
        myPolygon.clear();
        for (int i = 0; i < hv_Row.Length(); i++)
        {
            myPolygon<< QPoint(hv_Col[i].D(), hv_Row[i].D());
        }
        finished = true;
        setRectSize();
        ok = true;
    }
    catch (HException& he)
    {
    }
}

MyPolylineItem::~MyPolylineItem()
{
}

QString MyPolylineItem::name()
{
	return QString(u8"折线");
}

void MyPolylineItem::init(QPointF p)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    myPolygon.append(p);
    finished = false;
    m_startPos = p;
    mousePoint = p;
    end_point = p;
}

bool MyPolylineItem::save()
{
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        u8"保存线",
        "line",
        "ROI (*.dxf)");//多组扩展名用双分号";;"隔开
    if (!fileName.isEmpty() && finished)
    {
        try
        {
            HObject  ho_Contour;
            HTuple row, col;
            for (auto& p : myPolygon) {
                row.Append(p.y());
                col.Append(p.x());
            }
            GenContourPolygonXld(&ho_Contour, row, col);
            WriteContourXldDxf(ho_Contour, fileName.toStdString().c_str());
            return true;
        }
        catch (HException& he)
        {
            HTuple  Exception;
            he.ToHTuple(&Exception);
            QString error = QString(u8"Halcon 异常:") + QString(Exception.ToString().Text());
            QMessageBox::critical(nullptr, u8"错误", error, QMessageBox::Ok, QMessageBox::Ok);
        }

    }
    return false;
}

bool MyPolylineItem::isNull()
{
	return ok;
}

void MyPolylineItem::setMousePos(QPointF p)
{
    mousePoint = p;
}

void MyPolylineItem::addPoint(QPointF p)
{
    myPolygon << p;
    mousePoint = p;
    end_point = p;
    setRectSize();
}

void MyPolylineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    const int k = myPolygon.size();
    scaleFactor = painter->transform().m11();
    const double w = 1 / scaleFactor;
    QPen mPen = QPen(QColor(112, 203, 215));
    mPen.setWidth(w);
    painter->setPen(mPen);
    switch (k)
    {
    case 0: {
        return;
    }
    case 1: {
        double s_size = size / scaleFactor;
        painter->drawRect(QRectF(myPolygon[0].x() - s_size, myPolygon[0].y() - s_size, 2 * s_size, 2 * s_size));

        painter->drawLine(myPolygon[0] + QPointF(-3 * s_size, 0), myPolygon[0] + QPointF(3 * s_size, 0));
        painter->drawLine(myPolygon[0] + QPointF(0, -3 * s_size), myPolygon[0] + QPointF(0, 3 * s_size));
        break;
    }
    default:
        QPainterPath path;
        path.addPolygon(myPolygon);
        painter->drawPath(path);
        break;
    }

    if (finished)
    {
        //if (isSelected())
        //{
        //    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        //    painter->setBrush(Qt::NoBrush);
        //    painter->drawRect(nodes[node].boundingRect()/*.adjusted(w, w, -w, -w)*/);
        //}
    }
    else {
        painter->drawLine(end_point, mousePoint);
    }
}

void MyPolylineItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
    {
        
        m_startPos = event->pos();//鼠标左击时，获取当前鼠标在图片中的坐标，
//        myPolygon.append(m_startPos);
        node = numberNode();
        //        qDebug()<<"node"<<node;
        //        qDebug()<<"m_startPos"<<m_startPos;
        if (node >= 0)//在矩形内框区域时按下鼠标，则可拖动图片
        {
            setCursor(Qt::ClosedHandCursor);   //改变光标形状,手的形状
            m_StateFlag = MOVE_NODE;//标记当前为鼠标拖动图片移动状态
            biasClicked = event->pos() - myPolygon[node];
            setSelected(true);
        }
        else
        {
            m_StateFlag = DEFAULT_FLAG;
        }
    }
    if (event->button() == Qt::RightButton)
    {
        finished = true;
        if (myPolygon.size()<=1)
        {
            emit2SceneFinished(false);
        }
        else
        {
            emit2SceneFinished(true);
        }
        qDebug() << "dddddd";
    }
    QGraphicsItem::mousePressEvent(event);
}

void MyPolylineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    setCursor(Qt::ArrowCursor);
    if (m_StateFlag != DEFAULT_FLAG)
    {
        m_StateFlag = DEFAULT_FLAG;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void MyPolylineItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_StateFlag == MOVE_NODE) {

        myPolygon[node] = event->pos() - biasClicked;
        setRectSize();
        scene()->update();
    }
}

void MyPolylineItem::setRectSize() {
    prepareGeometryChange();
    nodes.clear();
    int size = size_r/ scaleFactor;
    tem_polygon = myPolygon;
    tem_polygon << end_point;
    for (int i = 0; i < myPolygon.size(); i++) {
        QPolygonF tem;
        auto Point1 = tem_polygon[i] + QPointF(-size, -size);
        auto Point2 = tem_polygon[i] + QPointF(size, -size);
        auto Point3 = tem_polygon[i] + QPointF(size, size);
        auto Point4 = tem_polygon[i] + QPointF(-size, size);
        tem.append(Point1);
        tem.append(Point2);
        tem.append(Point3);
        tem.append(Point4);
        nodes.append(tem);

    }
    //m_oldRectPolygon.clear();
    //size = 1 * size;
    //m_oldRectPolygon.append(QPointF(limitPoint(0) - size, limitPoint(1) - size));
    //m_oldRectPolygon.append(QPointF(limitPoint(2) + size, limitPoint(1) - size));
    //m_oldRectPolygon.append(QPointF(limitPoint(2) + size, limitPoint(3) + size));
    //m_oldRectPolygon.append(QPointF(limitPoint(0) - size, limitPoint(3) + size));
    //    qDebug()<<"nodes:"<<nodes;
    //    qDebug()<<"myPolygon:"<<myPolygon;
    //    qDebug()<<"m_oldRectPolygon:"<<m_oldRectPolygon;
}

double MyPolylineItem::limitPoint(int mode) {
    if (mode == 0) { //最左边
        double left = tem_polygon[0].x();
        for (int i = 1; i < tem_polygon.size(); i++) {
            if (tem_polygon[i].x() < left) {
                left = tem_polygon[i].x();
            }
        }
        return left;
    }
    if (mode == 1) { //最上边
        double up = tem_polygon[0].y();
        for (int i = 1; i < tem_polygon.size(); i++) {
            if (tem_polygon[i].y() < up) {
                up = tem_polygon[i].y();
            }
        }
        return up;
    }
    if (mode == 2) { //最右边
        double right = tem_polygon[0].x();
        for (int i = 1; i < tem_polygon.size(); i++) {
            if (tem_polygon[i].x() > right) {
                right = tem_polygon[i].x();
            }
        }
        return right;
    }
    else { //最下边
        double down = tem_polygon[0].y();
        for (int i = 1; i < tem_polygon.size(); i++) {
            if (tem_polygon[i].y() > down) {
                down = tem_polygon[i].y();
            }
        }
        return down;
    }
}

int MyPolylineItem::numberNode() {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].containsPoint(m_startPos, Qt::WindingFill)) {
            return i;
        }
    }
    return -1;
}

QRectF MyPolylineItem::boundingRect() const {
    //QPainterPath path;
    //path.addPolygon(tem_polygon);
    //return path.boundingRect();
    if (finished)
    {
        QPainterPath path;
        path.addPolygon(tem_polygon);
        return path.boundingRect();
    }
    else {
        return QRectF(0,0,999999,999999);
    }
}