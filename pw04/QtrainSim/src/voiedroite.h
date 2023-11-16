#ifndef VOIEDROITE_H
#define VOIEDROITE_H

#include "voie.h"
#include "loco.h"

class VoieDroite : public Voie
{
    Q_OBJECT

public:
    VoieDroite(qreal longueur);
    void calculerAnglesEtCoordonnees(Voie *v = nullptr) override;
    void calculerPositionContact() override;
    QList<QList<Voie*>*> explorationContactAContact(Voie* voieAppelante) override;
    qreal getLongueurAParcourir() override;
    Voie* getVoieSuivante(Voie* voieArrivee) override;
    void avanceLoco(qreal &dist, qreal &, qreal &, qreal, QPointF posActuelle, Voie *voieSuivante) override;
    void correctionPosition(qreal deltaX, qreal deltaY, Voie *v) override;
    void correctionPositionLoco(qreal &x, qreal &y) override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void setEtat(int) override;
private:
    qreal longueur;
    qreal lastDistDel;
};

#endif // VOIEDROITE_H
