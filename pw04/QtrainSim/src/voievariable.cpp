#include "voievariable.h"

VoieVariable::VoieVariable()
    :Voie()
{
}

void VoieVariable::setEtat(int nouvelEtat)
{
    this->etat = nouvelEtat;
    this->update(boundingRect());
    etatModifie(this);
}
