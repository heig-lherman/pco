#ifndef VOIEBUTTOIR_H
#define VOIEBUTTOIR_H

#include "voie.h"
#include "loco.h"

class VoieButtoir : public Voie
{
    Q_OBJECT

public:
    VoieButtoir(qreal longueur);
    void calculerAnglesEtCoordonnees(Voie *v) override;
    void calculerPositionContact() override;
    QList<QList<Voie*>*> explorationContactAContact(Voie *) override;
    qreal getLongueurAParcourir() override;
    Voie* getVoieSuivante(Voie*) override;
    void avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal, QPointF posActuelle, Voie *voieSuivante) override;
    void correctionPosition(qreal deltaX, qreal deltaY, Voie *) override;
    void correctionPositionLoco(qreal &, qreal &) override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void setEtat(int) override;

private:
    qreal longueur;
};

#endif // VOIEBUTTOIR_H
