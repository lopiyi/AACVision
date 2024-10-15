#include "mypointitem.h"
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

MyPointItem::MyPointItem()
{
    
}

MyPointItem::MyPointItem(const QString path)
{
    ok = false;
    try
    {
        HObject  ho_Contours;
        HTuple hv_Row, hv_Col, hv_DxfStatus;
        ReadContourXldDxf(&ho_Contours, path.toStdString().c_str(), HTuple(), HTuple(), &hv_DxfStatus);
        GetContourXld(ho_Contours, &hv_Row, &hv_Col);
        if (hv_Row.Length()!=1)
        {
            return;
        }
        init(QPoint(hv_Col.D(), hv_Row.D()));
        ok = true;
    }
    catch (HException& he)
    {
    }
}

MyPointItem::~MyPointItem()
{
}

QString MyPointItem::name()
{
    return QString(u8"点");
}

void MyPointItem::init(QPointF p)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    myPoint = p;
    finished = true;
    setRectSize();
}

bool MyPointItem::save()
{
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        u8"保存点",
        "point",
        "ROI (*.dxf)");//多组扩展名用双分号";;"隔开
    if (!fileName.isEmpty())
    {
        try
        {
            HObject  ho_Contour;
            GenContourPolygonXld(&ho_Contour, myPoint.y(), myPoint.x());
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

bool MyPointItem::isNull()
{
    return ok;
}

void MyPointItem::setMousePos(QPointF p)
{
}

void MyPointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    double scaleFactor = painter->transform().m11();
    QPen mPen = QPen(QColor(212,203,55));
    const double w = 1 / scaleFactor;
    mPen.setWidth(w);
    painter->setPen(mPen);
    s_size = size / scaleFactor;
    painter->drawRect(QRectF(myPoint.x()-s_size, myPoint.y()-s_size, 2 * s_size, 2 * s_size));
    //painter->drawLine(myPoint + QPointF(-3*s_size, 0), myPoint + QPointF(-s_size, 0));
    //painter->drawLine(myPoint + QPointF(0, -3 * s_size), myPoint + QPointF(0, -s_size));
    //painter->drawLine(myPoint + QPointF(3 * s_size, 0), myPoint + QPointF(s_size, 0));
    //painter->drawLine(myPoint + QPointF(0, s_size), myPoint + QPointF(0, 3* s_size));

    painter->drawLine(myPoint + QPointF(-3 * s_size, 0), myPoint + QPointF(3 * s_size, 0));
    painter->drawLine(myPoint + QPointF(0, -3 * s_size), myPoint + QPointF(0, 3 * s_size));
    if (isSelected())
    {
        painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(this->boundingRect()/*.adjusted(w, w, -w, -w)*/);
    }
}

void MyPointItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
    {
        //setSelected(true);
        m_startPos = event->pos();//鼠标左击时，获取当前鼠标在图片中的坐标，
        if (m_oldRectPolygon.containsPoint(m_startPos, Qt::WindingFill))//在矩形内框区域时按下鼠标，则可拖动图片
        {
            setCursor(Qt::ClosedHandCursor);   //改变光标形状,手的形状
            m_StateFlag = MOVE_POINT;//标记当前为鼠标拖动图片移动状态
            biasClicked = event->pos() - myPoint;
            setSelected(true);
        }
        else
        {
            m_StateFlag = DEFAULT_FLAG;
        }
    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

void MyPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    setCursor(Qt::ArrowCursor);
    if (m_StateFlag != DEFAULT_FLAG)
    {
        m_StateFlag = DEFAULT_FLAG;
    }
    else {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

void MyPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_StateFlag == MOVE_POINT) {
        myPoint = event->pos() - biasClicked;
        setRectSize();
        scene()->update();
    }
}

void MyPointItem::setRectSize() {
    prepareGeometryChange();
    m_oldRectPolygon.clear();
    auto Point1 = myPoint + QPointF(-3*s_size, -3*s_size);
    auto Point2 = myPoint + QPointF(3*s_size, -3*s_size);
    auto Point3 = myPoint + QPointF(3*s_size, 3*s_size);
    auto Point4 = myPoint + QPointF(-3*s_size, 3*s_size);
    m_oldRectPolygon.append(Point1);
    m_oldRectPolygon.append(Point2);
    m_oldRectPolygon.append(Point3);
    m_oldRectPolygon.append(Point4);
}

QRectF MyPointItem::boundingRect() const {
    QPainterPath path;
    path.addPolygon(m_oldRectPolygon);
    return path.boundingRect();
}
