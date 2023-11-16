#ifndef VOIETRAVERSEEJONCTION_H
#define VOIETRAVERSEEJONCTION_H

#include "voievariable.h"
#include "loco.h"
#include "trainsimsettings.h"

class VoieTraverseeJonction : public VoieVariable
{
    Q_OBJECT

public:
    VoieTraverseeJonction(qreal angle, qreal rayon, qreal longueur);
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
    void setNumVoieVariable(int numVoieVariable) override;

    void mousePressEvent (QGraphicsSceneMouseEvent *) override;
private:
    qreal rayon03, rayon12, angle, longueur;
    QPointF centre03;
    QPointF centre12;
    qreal lastDistDel;
};

#endif // VOIETRAVERSEEJONCTION_H
