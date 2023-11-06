#include "display.h"
#include <iostream>
#include <string>

constexpr double SCENEWIDTH = 1000.0;
constexpr double SCENELENGTH = 1500.0;
constexpr double ELEMENT_WIDTH = 75.0;
constexpr double ELEMENT_WIDTH_BIG = 150.0;


static Display* theDisplay;

ExtractorItem::ExtractorItem() = default;
FactoryItem::FactoryItem() = default;
WholesalerItem::WholesalerItem() = default;
ResourceItem::ResourceItem() = default;
ProductionItem::ProductionItem() = default;


void Display::placeResources(int x, int y, int id, std::vector<bool> resources){

    QPixmap fund(QString(":images/funds_color.png"));
    QPixmap _fund = fund.scaledToWidth(ELEMENT_WIDTH / 3);
    auto fund_item = new ResourceItem();
    auto count = std::count(resources.begin(), resources.end(), true);

    int index = -1 + (resources.size() - count) / 2;
    int index2 = index - 2;
    fund_item->setPixmap(_fund);
    fund_item->setPos(x, y - 1 * (ELEMENT_WIDTH_BIG / 3) + 40);
    m_scene->addItem(fund_item);
    funds[id] = new QLabel(this);
    funds[id]->setText("Waiting...");
    int adapt = 18;
    funds[id]->setGeometry(QRect(x-50, y + (-4) * (ELEMENT_WIDTH_BIG / 3/2) + 20, 30, 30));

    if(resources[0]){
        QPixmap petrol(QString(":images/station-essence.png"));
        QPixmap _petrol = petrol.scaledToWidth(ELEMENT_WIDTH / 3);
        auto petrol_item = new ResourceItem();
        petrol_item->setPixmap(_petrol);
        petrol_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3/2));
        m_scene->addItem(petrol_item);
        petrols[id] = new QLabel(this);
        petrols[id]->setText("Waiting...");
        petrols[id]->setGeometry(QRect(x - 50 + (ELEMENT_WIDTH_BIG), y + index2++ * (ELEMENT_WIDTH_BIG / 3/2)- adapt, 30, 30));
    }

    if(resources[1]){
        QPixmap copper(QString(":images/copper.png"));
        QPixmap _copper = copper.scaledToWidth(ELEMENT_WIDTH / 3);
        auto copper_item = new ResourceItem();
        copper_item->setPixmap(_copper);
        copper_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3/2));
        m_scene->addItem(copper_item);
        coppers[id] = new QLabel(this);
        coppers[id]->setText("Waiting...");
        coppers[id]->setGeometry(QRect(x - 50 + (ELEMENT_WIDTH_BIG), y + index2++ * (ELEMENT_WIDTH_BIG / 3/2) - adapt, 30, 30));
    }

    if(resources[3]){
        QPixmap sand(QString(":images/sand.png"));
        QPixmap _sand = sand.scaledToWidth(ELEMENT_WIDTH / 3);
        auto sand_item = new ResourceItem();
        sand_item->setPixmap(_sand);
        sand_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3/2));
        m_scene->addItem(sand_item);
        sands[id] = new QLabel(this);
        sands[id]->setText("Waiting...");
        sands[id]->setGeometry(QRect(x - 50 + (ELEMENT_WIDTH_BIG), y + index2++ * (ELEMENT_WIDTH_BIG / 3/2)- adapt, 30, 30));
    }
    if(resources[2]){
        QPixmap chip(QString(":images/Microchips.png"));
        QPixmap _chip = chip.scaledToWidth(ELEMENT_WIDTH / 3);
        auto chip_item = new ResourceItem();
        chip_item->setPixmap(_chip);
        chip_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3/2));
        m_scene->addItem(chip_item);
        chips[id] = new QLabel(this);
        chips[id]->setText("Waiting...");
        chips[id]->setGeometry(QRect(x - 50 + (ELEMENT_WIDTH_BIG), y + index2++ * (ELEMENT_WIDTH_BIG / 3/2)- adapt, 30, 30));
    }
    if(resources[5]){
        QPixmap plastic(QString(":images/Plastics.png"));
        QPixmap _plastic = plastic.scaledToWidth(ELEMENT_WIDTH / 3);
        auto plastic_item = new ResourceItem();
        plastic_item->setPixmap(_plastic);
        plastic_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3/2));
        m_scene->addItem(plastic_item);
        plastics[id] = new QLabel(this);
        plastics[id]->setText("Waiting...");
        plastics[id]->setGeometry(QRect(x - 50 + (ELEMENT_WIDTH_BIG), y + index2++ * (ELEMENT_WIDTH_BIG / 3/2)- adapt, 30, 30));
    }
    if(resources[4]){
        QPixmap robot(QString(":images/Service_bots.png"));
        QPixmap _robot = robot.scaledToWidth(ELEMENT_WIDTH / 3);
        auto robot_item = new ResourceItem();
        robot_item->setPixmap(_robot);
        robot_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3/2));
        m_scene->addItem(robot_item);
        robots[id] = new QLabel(this);
        robots[id]->setText("Waiting...");
        robots[id]->setGeometry(QRect(x - 50 + (ELEMENT_WIDTH_BIG), y + index2++ * (ELEMENT_WIDTH_BIG / 3/2)- adapt, 30, 30));
    }
}

Display::Display(unsigned int nbExtractors, unsigned int nbFactories, unsigned int nbWholesalers, QWidget *parent) :
    QGraphicsView(parent)
{
    theDisplay = this;

    m_scene = new QGraphicsScene(this);
    createResourceAssociations(nbExtractors, nbWholesalers, nbFactories);

    funds = std::vector<QLabel*>(nbExtractors + nbFactories + nbWholesalers);
    petrols = std::vector<QLabel*>(nbExtractors + nbFactories + nbWholesalers);
    coppers = std::vector<QLabel*>(nbExtractors + nbFactories + nbWholesalers);
    sands = std::vector<QLabel*>(nbExtractors + nbFactories + nbWholesalers);
    chips = std::vector<QLabel*>(nbExtractors + nbFactories + nbWholesalers);
    robots = std::vector<QLabel*>(nbExtractors + nbFactories + nbWholesalers);
    plastics = std::vector<QLabel*>(nbExtractors + nbFactories + nbWholesalers);

    penColors.push_back(Qt::cyan);
    penColors.push_back(Qt::red);
    penColors.push_back(Qt::yellow);
    penColors.push_back(Qt::green);
    penColors.push_back(Qt::blue);
    penColors.push_back(Qt::magenta);
    penColors.push_back(Qt::gray);
    penColors.push_back(Qt::black);

    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->setMinimumHeight(SCENEWIDTH);
    this->setMinimumWidth(SCENELENGTH);
    this->setScene(m_scene);

    for (unsigned int i = 0; i < nbExtractors; ++i){
        QString str;
        switch(i % 3){
            case 0:
                str = ":images/sand_scaled.png";
                break;
            case 1:
                str = ":images/copper_scaled.png";
                break;
            case 2:
                str = ":images/petrol_scaled.png";
                break;
        }

        QPixmap img(str);
        QPixmap extractorPixmap;
        int x = 0 + SCENELENGTH / 6;//QRandomGenerator::system()->bounded(SCENELENGTH / 3);
        int y = (SCENEWIDTH / nbExtractors) * i + SCENEWIDTH / nbExtractors / 2;

        extractorPixmap = img.scaledToWidth(ELEMENT_WIDTH_BIG);
        auto extractor = new ProductionItem();
        extractor->setPixmap(extractorPixmap);
        extractor->setPos(x, y);
        m_scene->addItem(extractor);
        m_productItem.push_back(extractor);

        placeResources(x, y, i, resourceAssociations[i]);
    }

    for (unsigned int i = 0; i < nbWholesalers; ++i) {
        QPixmap img(QString(":images/warehouse_scaled.png"));
        QPixmap wholesalerPixmap;
        int x = (SCENELENGTH / 3) + (SCENELENGTH / 6);
//        int x = (SCENELENGTH / 3) * 2 + (SCENELENGTH / 6);//QRandomGenerator::system()->bounded(SCENELENGTH / 3);
        int y = (SCENEWIDTH / nbWholesalers) * i + SCENEWIDTH / nbWholesalers / 2;

        wholesalerPixmap = img.scaledToWidth(ELEMENT_WIDTH_BIG);
        auto wholesaler = new ProductionItem();
        wholesaler->setPixmap(wholesalerPixmap);

        wholesaler->setPos(x, y);
        m_scene->addItem(wholesaler);
        m_productItem.push_back(wholesaler);

        placeResources(x, y, i + nbExtractors, resourceAssociations[i + nbExtractors]);
    }

    for (unsigned int i = 0; i < nbFactories; ++i) {
        QPixmap img(QString(":images/factory_scaled.png"));
        QPixmap factoryPixmap;
        int x = (SCENELENGTH / 3) * 2 + (SCENELENGTH / 6);
//        int x = (SCENELENGTH / 3) + (SCENELENGTH / 6);//QRandomGenerator::system()->bounded(SCENELENGTH / 3);
        int y = (SCENEWIDTH / nbFactories) * i + SCENEWIDTH / nbFactories / 2;

        factoryPixmap = img.scaledToWidth(ELEMENT_WIDTH_BIG);
        auto factory = new ProductionItem();
        factory->setPixmap(factoryPixmap);
        factory->setPos(x, y);
        m_scene->addItem(factory);
        m_productItem.push_back(factory);

        placeResources(x, y, i + nbExtractors + nbWholesalers, resourceAssociations[i + nbWholesalers + nbExtractors]);

    }
}

void Display::createResourceAssociations(unsigned int nbExtractors, unsigned int nbWholesalers, unsigned int nbFactories) {
    unsigned int totalEntities = nbExtractors + nbWholesalers + nbFactories;
    resourceAssociations.resize(totalEntities, std::vector<bool>(6, false));



    // Remplissez le vecteur resourceAssociations en fonction du nombre d'entités de chaque type
    for (unsigned int i = 0; i < nbExtractors; ++i) {
        std::vector<bool> extractorResources(6, false);
        if (i % 3 == 0) {
            extractorResources[3] = true; // Extracteur de sable
        } else if (i % 3 == 1) {
            extractorResources[1] = true; // Extracteur de cuivre
        } else if (i % 3 == 2) {
            extractorResources[0] = true; // Extracteur de pétrole
        }
        resourceAssociations[i] = extractorResources;
    }

    for (unsigned int i = 0; i < nbWholesalers; ++i) {
        // Pour les grossistes, toutes les ressources sont associées (true)
        std::vector<bool> wholesalerResources(6, true);
        resourceAssociations[i+nbExtractors] = wholesalerResources;
    }

    for (unsigned int i = 0; i < nbFactories; ++i) {
        std::vector<bool> factoryResources(6, false);
        if (i % 3 == 0) {
            factoryResources[0] = true; // Usine à plastique
            factoryResources[5] = true; // Usine à plastique
        } else if (i % 3 == 1) {
            factoryResources[2] = true; // Usine à puce (chip)
            factoryResources[1] = true; // Usine à puce (chip)
            factoryResources[3] = true; // Usine à puce (chip)
        } else if (i % 3 == 2) {
            factoryResources[2] = true; // Usine à robot
            factoryResources[4] = true; // Usine à robot
            factoryResources[5] = true; // Usine à robot
        }
        resourceAssociations[i + nbExtractors + nbWholesalers] = factoryResources;
    }

}

void Display::update_fund(int idx, QString fund) {
    this->funds[idx]->setText(fund);
}

void Display::set_link(int from, int to){
    QPoint pFrom(m_productItem[from]->pos().x(), m_productItem[from]->pos().y());
    QPoint pTo(m_productItem[to]->pos().x(), m_productItem[to]->pos().y());
    QLine line;
    if(pFrom.x() > SCENELENGTH/2){
        /*Link Factory -> Wholesaler*/
        line = QLine(QPoint(pFrom.x(), pFrom.y() + (m_productItem[from]->pixmap().height()/2)),
                     QPoint(pTo.x() + ((ELEMENT_WIDTH)*2) + 50, pTo.y() + (m_productItem[to]->pixmap().height()/2)));
        QPen pen(penColors[6 % penColors.size()]);
        pen.setWidth(2);

        m_scene->addLine(line, pen);
    } else {
        if(pTo.x() > SCENELENGTH/2){
            /*Link Wholesaler -> Factory*/
            line = QLine(QPoint(pTo.x(), pTo.y() + (m_productItem[to]->pixmap().height()/2)),
                         QPoint(pFrom.x() + ((ELEMENT_WIDTH)*2) + 50, pFrom.y() + (m_productItem[from]->pixmap().height()/2)));
            QPen pen(penColors[from % penColors.size()]);
            pen.setWidth(2);

            m_scene->addLine(line, pen);
        } else {
            /*Link Wholesaler -> Extractor*/
            line = QLine(QPoint(pFrom.x(), pFrom.y() + (m_productItem[from]->pixmap().height()/2)),
                         QPoint(pTo.x() + ((ELEMENT_WIDTH)*2), pTo.y() + (m_productItem[to]->pixmap().height()/2)));
            QPen pen(penColors[from % penColors.size()]);
            pen.setWidth(2);

            m_scene->addLine(line, pen);
        }
    }
}

void Display::update_stocks(int idx, std::map<ItemType, int>* stocks) {

    std::vector<bool> updates = resourceAssociations[idx];

    if(updates[0]){
        this->petrols[idx]->setText(QString::number((*stocks)[ItemType::Petrol]));
    }
    if(updates[1]){
        this->coppers[idx]->setText(QString::number((*stocks)[ItemType::Copper]));
    }
    if(updates[2]){
        this->chips[idx]->setText(QString::number((*stocks)[ItemType::Chip]));
    }
    if(updates[3]){
        this->sands[idx]->setText(QString::number((*stocks)[ItemType::Sand]));
    }
    if(updates[4]){
        this->robots[idx]->setText(QString::number((*stocks)[ItemType::Robot]));
    }
    if(updates[5]){
        this->plastics[idx]->setText(QString::number((*stocks)[ItemType::Plastic]));
    }

}
