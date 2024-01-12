#include "arrowitem.h"

ArrowItem::ArrowItem(QGraphicsItem *parent, int ending) :
    QGraphicsLineItem(parent), ending(ending)
{
    myColor = QColor(0,0,0);
}

#include <QPen>
#include <QPainter>
#include <math.h>


const qreal Pi = 3.14;

const qreal arrowSize = 5;


QRectF ArrowItem::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath ArrowItem::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}


void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen myPen = pen();
    myPen.setColor(myColor);
//    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(myColor);


    if (ending & 2)
    {
        double angle = ::acos(line().dx() / line().length());
        if (line().dy() >= 0)
            angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                                cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                                cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << line().p1() << arrowP1 << arrowP2;
        painter->drawPolygon(arrowHead);
    }

    if (ending & 1)
    {
        double angle = ::acos(line().dx() / line().length());
        if (line().dy() >= 0)
            angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p2() - QPointF(sin(angle + Pi / 3) * arrowSize,
                                                cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p2() - QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                                cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << line().p2() << arrowP1 << arrowP2;
        painter->drawPolygon(arrowHead);
    }
    painter->setRenderHint(QPainter::Antialiasing, false);

    painter->drawLine(line());

}
