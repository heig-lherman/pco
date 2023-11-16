#include "segment.h"

Segment::Segment(Contact *c1, Contact *c2, QList<Voie *> lv, QObject *parent) :
    QObject(parent)
{
    this->contact1 = c1;
    this->contact2 = c2;
    this->voies = lv;
}

Voie* Segment::getMilieu()
{
    int indiceMilieu = this->voies.length() / 2;
    int delta = 1;

    while(true)
    {
        if(voies.at(indiceMilieu)->getNbreLiaisons() == 2)
        {
            return voies.at(indiceMilieu);
        }
        else
        {
            indiceMilieu += delta;

            if(indiceMilieu < 0 || indiceMilieu >= voies.length())
                return nullptr;

            delta *= -1;
            if(delta < 0)
                delta -= 1;
            else
                delta += 1;
        }
    }
}

Voie* Segment::getSuivantMilieu()
{
    int indiceMilieu = voies.indexOf(getMilieu());

    return voies.at(indiceMilieu +1); //pas de garde fou! probablement un peu risque...
}

Voie* Segment::getPrecedentMilieu()
{
    int indiceMilieu = voies.indexOf(getMilieu());

    return voies.at(indiceMilieu -1); //pas de garde fou! probablement un peu risque...
}

bool Segment::relie(Contact *c1, Contact *c2)
{
    if(contact1 == c1 && contact2 == c2)
        return true;
    return false;
}
