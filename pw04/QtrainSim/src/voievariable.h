#ifndef VOIEVARIABLE_H
#define VOIEVARIABLE_H

#include "voie.h"
#include "loco.h"

class VoieVariable : public Voie
{
    Q_OBJECT

public:
    VoieVariable();

    void setEtat(int nouvelEtat) override;
    /** permet d'indiquer à la voie variable quel est son numéro.
      * \param numVoieVariable le numéro de la voie variable.
      */
    virtual void setNumVoieVariable(int numVoieVariable) = 0;
signals:
    /** signale que la voie variable a été modifiée.
      * \param v la voie modifiée.
      */
    void etatModifie(Voie* v);
public slots:

protected:
    int etat;
    int numVoieVariable;
};

#endif // VOIEVARIABLE_H
