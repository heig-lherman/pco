#ifndef VOIEAIGUILLAGE_H
#define VOIEAIGUILLAGE_H

#include "voievariable.h"
#include "loco.h"

class VoieAiguillage : public VoieVariable
{
    Q_OBJECT

public:
    VoieAiguillage(qreal angle, qreal rayon, qreal longueur, qreal direction);
    void setNumVoieVariable(int numVoieVariable) override;
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

    void mousePressEvent (QGraphicsSceneMouseEvent *) override;
private:
    qreal rayon, angle, longueur, direction;
    QPointF centre;
    qreal lastDistDel;
};

#endif // VOIEAIGUILLAGE_H
