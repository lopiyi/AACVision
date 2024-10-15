#include "myrect1item.h"

MyRect1Item::MyRect1Item()
{}

MyRect1Item::~MyRect1Item()
{}

QString MyRect1Item::name()
{
	return QString(u8"����");
}

void MyRect1Item::init(QPointF p)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    leftTopPoint = p;
    mousePoint = p;
    finished = false;
}

bool MyRect1Item::save()
{
	return false;
}

bool MyRect1Item::isNull()
{
	return false;
}

void MyRect1Item::setMousePos(QPointF p)
{
}

void MyRect1Item::addPoint(QPointF p)
{
    mousePoint = p;
    rigntBottomPoint = p;
    rect = QRectF(leftTopPoint, p);

    switchPoint();
    finished = true;
}

void MyRect1Item::switchPoint()
{
    leftTopPoint = QPoint(qMin(leftTopPoint.x(), rigntBottomPoint.x()), qMin(leftTopPoint.y(), rigntBottomPoint.y()));
    rigntBottomPoint = QPoint(qMax(leftTopPoint.x(), rigntBottomPoint.x()), qMax(leftTopPoint.y(), rigntBottomPoint.y()));
}

void MyRect1Item::mousePressEvent(QGraphicsSceneMouseEvent* event)
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
                biasClicked = mousePoint - (myLine[0] + myLine[1]) / 2;
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
