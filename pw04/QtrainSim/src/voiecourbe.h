#ifndef VOIECOURBE_H
#define VOIECOURBE_H

#include "voie.h"
#include "loco.h"

class VoieCourbe : public Voie
{
    Q_OBJECT

public:
    VoieCourbe(qreal angle, qreal rayon, int direction);
    void calculerAnglesEtCoordonnees(Voie *v) override;
    void calculerPositionContact() override;
    QList<QList<Voie*>*> explorationContactAContact(Voie* voieAppelante) override;
    qreal getLongueurAParcourir() override;
    Voie* getVoieSuivante(Voie* voieArrivee) override;
    void avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal angleCumule, QPointF posActuelle, Voie *voieSuivante) override;
    void correctionPosition(qreal deltaX, qreal deltaY, Voie *v) override;
    void correctionPositionLoco(qreal &, qreal &) override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void setEtat(int) override;

private:
    QPointF centre;
    qreal rayon, angle;
    int direction;
    qreal lastDistDel;
};

#endif // VOIECOURBE_H
