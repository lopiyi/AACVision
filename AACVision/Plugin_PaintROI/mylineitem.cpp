#include "mylineitem.h"
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

MyLineItem::MyLineItem()
{
}

MyLineItem::MyLineItem(const QString path)
{
    ok = false;
    try
    {
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        HObject  ho_Contours;
        HTuple hv_Row, hv_Col, hv_DxfStatus;
        ReadContourXldDxf(&ho_Contours, path.toStdString().c_str(), HTuple(), HTuple(), &hv_DxfStatus);
        GetContourXld(ho_Contours, &hv_Row, &hv_Col);
        if (hv_Row.Length() != 2)
        {
            return;
        }
        myLine.clear();
        m_startPos = QPoint(hv_Col[0].D(), hv_Row[0].D());
        m_endPos = QPoint(hv_Col[1].D(), hv_Row[1].D());
        myLine << m_startPos << m_endPos;
        finished = true;
        setRectSize();
        ok = true;
    }
    catch (HException& he)
    {
    }
}

MyLineItem::~MyLineItem()
{
}

QString MyLineItem::name()
{
    return QString(u8"��");
}

void MyLineItem::init(QPointF p)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    myLine.clear();
    myLine << p;
    m_startPos = p;
    mousePoint = p;
    finished = false;
}

bool MyLineItem::save()
{
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        u8"������",
        "line",
        "ROI (*.dxf)");//������չ����˫�ֺ�";;"����
    if (!fileName.isEmpty() && finished)
    {
        try
        {
            HObject  ho_Contour;
            HTuple row, col;
            row[0] = myLine[0].y();
            col[0] = myLine[0].x();
            row[1] = myLine[1].y();
            col[1] = myLine[1].x();
            GenContourPolygonXld(&ho_Contour, row, col);
            WriteContourXldDxf(ho_Contour, fileName.toStdString().c_str());
            return true;
        }
        catch (HException& he)
        {
            HTuple  Exception;
            he.ToHTuple(&Exception);
            QString error = QString(u8"Halcon �쳣:") + QString(Exception.ToString().Text());
            QMessageBox::critical(nullptr, u8"����", error, QMessageBox::Ok, QMessageBox::Ok);
        }

    }
    return false;
}

bool MyLineItem::isNull()
{
    return ok;
}

void MyLineItem::setMousePos(QPointF p)
{
    mousePoint = p;
}

void MyLineItem::addPoint(QPointF p)
{
    m_endPos = p;
    finished = true;
    myLine << p;
    setRectSize();
}

void MyLineItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (finished)
        {
            //setSelected(true);
            mousePoint = event->pos();//������ʱ����ȡ��ǰ�����ͼƬ�е����꣬
            
            if (m_startPolygon.containsPoint(mousePoint, Qt::WindingFill))
            {
                setCursor(Qt::SizeHorCursor);
                m_StateFlag = MOVE_START_POINT;//��ǵ�ǰΪ�û�������ʼ�㸽��������
                biasClicked = mousePoint - myLine[0];
            }
            else if (m_endPolygon.containsPoint(mousePoint, Qt::WindingFill))
            {
                setCursor(Qt::SizeHorCursor);
                m_StateFlag = MOVE_END_POINT;//��ǵ�ǰΪ�û����¾��ε��ұ߽�����
                biasClicked = mousePoint - myLine[1];
            }
            else if (m_insicedPolygon.containsPoint(mousePoint, Qt::WindingFill))//�ھ����ڿ�����ʱ������꣬����϶�ͼƬ
            {
                setCursor(Qt::ClosedHandCursor);   //�ı�����״,�ֵ���״
                m_StateFlag = MOVE_LINE;//��ǵ�ǰΪ����϶�ͼƬ�ƶ�״̬
                biasClicked = mousePoint - (myLine[0]+myLine[1])/2;
                setSelected(true);
            }
            else
            {
                m_StateFlag = DEFAULT_FLAG;
            }
            //        qDebug()<<"m_StateFlag:"<<m_StateFlag;
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void MyLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    setCursor(Qt::ArrowCursor);
    if (m_StateFlag != DEFAULT_FLAG)
    {
        m_StateFlag = DEFAULT_FLAG;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void MyLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    scaleFactor = painter->transform().m11();
    const double w = 1 / scaleFactor;
    const int k = myLine.size();
    //size /= scaleFactor;
    QPen mPen = QPen(Qt::blue);
    mPen.setWidth(w);
    switch (k)
    {
    case 1:
    {
        painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(QRectF(m_startPos, mousePoint));
        
        painter->setPen(mPen);
        //QPainterPath path;
        //        path.addPolygon(myLine);
        //        path.addPolygon(m_oldRectPolygon);
        //        painter->drawPath(path);
        painter->drawLine(m_startPos, mousePoint);
        break;
    }
    case 2:
    {
        painter->setPen(mPen);
        painter->drawLine(myLine.at(0), myLine.at(1));
        break;
    }
    default:
        
        break;
    }
    if (isSelected())
    {
        painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawPolygon(m_insicedPolygon);
        //painter->drawRect(this->boundingRect()/*.adjusted(w, w, -w, -w)*/);
    }
}

QRectF MyLineItem::boundingRect() const {
    const int k = myLine.size();
    switch (k)
    {
    case 1:
        return QRectF(m_startPos, mousePoint);
        break;
    default:
        QPainterPath path;
        path.addPolygon(m_oldRectPolygon);
        return path.boundingRect();
        break;
    }
}

QPolygonF MyLineItem::getRectPolygonLine(QPointF start, QPointF end, double bias) {
    bias /= scaleFactor;
    auto mediaPoint = (start + end) / 2;
    // ���㵥λ����
    auto len = sqrt(pow(start.x() - end.x(), 2) + pow(start.y() - end.y(), 2)) / 2;
    auto xiangLiang = (end - start) / 2;
    auto danWeiXiangLiang = xiangLiang / len;
    // ��λ����ת90��
    auto danWeiXiangLiangAdd90 = QPointF(-danWeiXiangLiang.y(), danWeiXiangLiang.x());
    // ��λ����ת-90��
    auto danWeiXiangLiangSub90 = QPointF(danWeiXiangLiang.y(), -danWeiXiangLiang.x());
    // ������ʼ�������ƫ������
    auto start_bias_point = (-xiangLiang + -danWeiXiangLiang * bias) + mediaPoint; //��ֱ�߷���ƫ��bias������
    // ת90��
    auto Point1 = start_bias_point + danWeiXiangLiangAdd90 * bias;
    // ת-90��
    auto Point2 = start_bias_point + danWeiXiangLiangSub90 * bias;
    // �����յ������ƫ������
    auto end_bias_point = (xiangLiang + danWeiXiangLiang * bias) + mediaPoint; //��ֱ�߷���ƫ��bias������
    // ת90��
    auto Point3 = end_bias_point + danWeiXiangLiangAdd90 * bias;
    // ת-90��
    auto Point4 = end_bias_point + danWeiXiangLiangSub90 * bias;
    QPolygonF RectPolygon;
    RectPolygon.append(Point1);
    RectPolygon.append(Point2);
    RectPolygon.append(Point4);
    RectPolygon.append(Point3);
    //    qDebug()<<"RectPolygon:"<<RectPolygon;
    //    qDebug()<<"start:"<<start;
    //    qDebug()<<"end:"<<end;
    //    qDebug()<<"xiangLiang:"<<xiangLiang;
    //    qDebug()<<"len:"<<len;
    //    qDebug()<<"danWeiXiangLiang:"<<danWeiXiangLiang;
    return RectPolygon;
}

void MyLineItem::setRectSize() {
    prepareGeometryChange();
    m_oldRectPolygon = getRectPolygonLine(myLine[0], myLine[1]);
    auto centerPoint = (myLine.at(0) + myLine.at(1)) / 2;
    //m_insicedPolygon = centerRect(centerPoint, 20/ scaleFactor);
    m_insicedPolygon = m_oldRectPolygon;
    m_startPolygon = centerRect(myLine.at(0), 20 / scaleFactor);

    m_endPolygon = centerRect(myLine.at(1), 20 / scaleFactor);
}

QPolygonF MyLineItem::centerRect(QPointF point, int size) {

    QPolygonF RectPolygon;
    RectPolygon.append(point + QPointF(-size, -size));
    RectPolygon.append(point + QPointF(size, -size));
    RectPolygon.append(point + QPointF(size, size));
    RectPolygon.append(point + QPointF(-size, size));
    return RectPolygon;
}

QPainterPath MyLineItem::shape() const//�������Ƽ����ײcollide�������hit��Ӧ����
{
    QPainterPath path;
    path.addPolygon(m_oldRectPolygon);
    return path;
}

void MyLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (finished)
    {
        if (m_StateFlag == MOVE_LINE) {
            auto xiangLiang = (myLine[1] - myLine[0]) / 2;
            myLine[0] = event->pos() - xiangLiang - biasClicked;
            myLine[1] = event->pos() + xiangLiang - biasClicked;
            setRectSize();
            scene()->update();
        }
        else if (m_StateFlag == MOVE_START_POINT) {
            myLine[0] = event->pos() - biasClicked;
            setRectSize();
            scene()->update();
        }
        else if (m_StateFlag == MOVE_END_POINT) {
            myLine[1] = event->pos() - biasClicked;
            setRectSize();
            scene()->update();
        }
        //    qDebug()<<myLine;
    }
    else
    {
        mousePoint = event->pos();
        //scene()->update();
    }
}


