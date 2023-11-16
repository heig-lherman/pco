#ifndef VOIEAIGUILLAGETRIPLE_H
#define VOIEAIGUILLAGETRIPLE_H

#include "voievariable.h"
#include "loco.h"

class VoieAiguillageTriple : public VoieVariable
{
public:
    VoieAiguillageTriple(qreal angle, qreal rayon, qreal longueur);
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
    qreal rayonGauche, rayonDroite, angle, longueur;
    QPointF centreGauche;
    QPointF centreDroite;
    qreal lastDistDel;
};

#endif // VOIEAIGUILLAGETRIPLE_H
