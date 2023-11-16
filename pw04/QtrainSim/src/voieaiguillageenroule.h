#ifndef VOIEAIGUILLAGEENROULE_H
#define VOIEAIGUILLAGEENROULE_H

#include "voievariable.h"
#include "loco.h"

class VoieAiguillageEnroule : public VoieVariable
{
    Q_OBJECT
public:
    explicit VoieAiguillageEnroule(qreal angle, qreal rayon, qreal longueur, qreal direction);
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
    qreal rayonInterieur, rayonExterieur, angle, longueur, direction;
    QPointF centreInterieur;
    QPointF centreExterieur;
    qreal lastDistDel;

};

#endif // VOIEAIGUILLAGEENROULE_H
