#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QGraphicsLineItem>

class ArrowItem : public QGraphicsLineItem
{
public:
    explicit ArrowItem(QGraphicsItem *parent = 0, int ending = 0);


    QRectF boundingRect() const;
    QPainterPath shape() const;

signals:

public slots:

protected:

    QPolygonF arrowHead;
    QPolygonF arrowTail;
    QColor myColor;

    int ending;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

#endif // ARROWITEM_H
