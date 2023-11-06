#ifndef DISPLAY_H
#define DISPLAY_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <pcosynchro/pcosemaphore.h>
#include <QLabel>
#include <QLine>

#include "seller.h"

class ResourceItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        ResourceItem();
        PcoSemaphore sem;
};

class ExtractorItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        ExtractorItem();
        PcoSemaphore sem;
};

class FactoryItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        FactoryItem();
        PcoSemaphore sem;
};

class WholesalerItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        WholesalerItem();
        PcoSemaphore sem;
};

class ProductionItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        ProductionItem();
        PcoSemaphore sem;
};

class Display : public QGraphicsView
{
    Q_OBJECT
public:
    Display(unsigned int nbExtractors, unsigned int nbFactories, unsigned int nbWholesalers, QWidget *parent);
    //~Display();

    std::vector<QLabel*> funds;
    std::vector<QLabel*> petrols;
    std::vector<QLabel*> coppers;
    std::vector<QLabel*> chips;
    std::vector<QLabel*> sands;
    std::vector<QLabel*> robots;
    std::vector<QLabel*> plastics;

    std::vector<QColor> penColors;

    std::vector<ProductionItem*> m_productItem;


    void update_stocks(int idx, std::map<ItemType, int>* stocks);
    void update_fund(int idx, QString fund);

    void set_link(int from, int to);

private:
    QGraphicsScene *m_scene;

    std::vector<std::vector<bool>> resourceAssociations;

    void placeResources(int x, int y, int id, std::vector<bool> resources);
    void createResourceAssociations(unsigned int nbExtractors, unsigned int nbWholesalers, unsigned int nbFactories);
public slots:

};

#endif // DISPLAY_H
