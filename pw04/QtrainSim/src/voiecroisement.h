#ifndef VOIECROISEMENT_H
#define VOIECROISEMENT_H

#include "voie.h"
#include "loco.h"

class VoieCroisement : public Voie
{
    Q_OBJECT

public:
    VoieCroisement(qreal angle, qreal longueur);
    void calculerAnglesEtCoordonnees(Voie *v) override;
    void calculerPositionContact() override;
    QList<QList<Voie*>*> explorationContactAContact(Voie* voieAppelante) override;
    qreal getLongueurAParcourir() override;
    Voie* getVoieSuivante(Voie* voieArrivee) override;
    void avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal, QPointF posActuelle, Voie *voieSuivante) override;
    void correctionPosition(qreal deltaX, qreal deltaY, Voie *v) override;
    void correctionPositionLoco(qreal &, qreal &) override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void setEtat(int) override;
private:
    qreal angle, longueur;
    qreal lastDistDel;
};

#endif // VOIECROISEMENT_H
