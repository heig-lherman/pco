#include "simview.h"

SimView::SimView(QWidget */*parent*/)
    : QGraphicsView()
{
    scene = new QGraphicsScene();
    this->setScene(scene);
    this->setRenderHints(QPainter::Antialiasing);
    timer = new QTimer(this);
    CONNECT(timer, SIGNAL(timeout()), this, SLOT(animationStep()));
}

void SimView::redraw()
{
    scene->update(sceneRect());
}

void SimView::addVoie(Voie *v, int ID)
{
    this->Voies.insert(ID, v);
    this->scene->addItem(v);
    v->setVisible(true);
}

void SimView::addContact(Contact *c, int ID)
{
    this->contacts.insert(ID, c);
}

void SimView::addVoieVariable(VoieVariable *vv, int ID)
{
    this->VoiesVariables.insert(ID, vv);
    connect(vv, SIGNAL(etatModifie(Voie*)), this, SLOT(voieVariableModifiee(Voie*)));
}

void SimView::setPremiereVoie(Voie *v)
{
    this->premiereVoie = v;
}

void SimView::modifierAiguillage(int n, int v)
{
    this->VoiesVariables[n]->setEtat(v);
}

void SimView::construireMaquette()
{
    this->premiereVoie->calculerAnglesEtCoordonnees();

    this->premiereVoie->calculerPosition();
}

void SimView::viderMaquette()
{
    foreach(Voie* v, this->Voies)
        delete v;

    this->Voies.clear();
}

void SimView::genererSegments()
{
    for(int i = 1; i <= this->contacts.size(); i++)
    {
        QList<QList<Voie*>*> parcours;
        parcours = this->Voies.value(contacts.value(i)->getNumVoiePorteuse())->startExplorationContactAContact();

        foreach(QList<Voie*>* lv, parcours)
        {
            if(lv->last()->getContact() != nullptr)
            {
                if(contacts.key(lv->first()->getContact()) < contacts.key(lv->last()->getContact()))
                {
                    segments.append(new Segment(lv->first()->getContact(), lv->last()->getContact(), *lv));
                }
            }
            else
            {
                //gestion de segments entre un contact et une voie buttoir...
                segments.append(new Segment(lv->first()->getContact(), nullptr, *lv));
            }
        }

        // On détruit les QList*
        while (!parcours.isEmpty()) {
            QList<Voie*> *l = parcours.at(0);
            delete l;
            parcours.pop_front();
        }
    }
}

void SimView::addLoco(Loco *l, int ID)
{
    this->Locos.insert(ID, l);
    this->scene->addItem(l);

    CONNECT(l, SIGNAL(nouveauSegment(Contact*,Contact*,Loco*)), this, SLOT(locoSurNouveauSegment(Contact*,Contact*,Loco*)));
    CONNECT(this, SIGNAL(locoSurSegment(Segment*)), l, SLOT(locoSurSegment(Segment*)));
    CONNECT(this, SIGNAL(notificationVoieVariableModifiee(Voie*)), l, SLOT(voieVariableModifiee(Voie*)));

    peintLocos();
}

void SimView::peintLocos()
{
    int nbreLocos = this->Locos.size();

    int sigmaCouleur = 255 * 6 / nbreLocos;

    int indiceCouleur;

    int r, g, b;

    QList<Loco*> listeLocos = Locos.values();

    for(int i=0; i < listeLocos.length(); i++)
    {
        indiceCouleur = i * sigmaCouleur;

        if(indiceCouleur < 255)
        {
            r = 255;
            g = indiceCouleur;
            b = 0;
        }
        else if(indiceCouleur < 2*255)
        {
            r = 2 * 255 - indiceCouleur;
            g = 255;
            b = 0;
        }
        else if(indiceCouleur < 3*255)
        {
            r = 0;
            g = 255;
            b = indiceCouleur % 255;
        }
        else if(indiceCouleur < 4*255)
        {
            r = 0;
            g = 4*255 - indiceCouleur;
            b = 255;
        }
        else if(indiceCouleur < 5*255)
        {
            r = indiceCouleur % 255;
            g = 0;
            b = 255;
        }
        else if(indiceCouleur < 6*255)
        {
            r = 255;
            g = 0;
            b = 6*255 - indiceCouleur;
        }

        listeLocos.value(i)->setCouleur(r,g,b);
    }
}

void SimView::zoomIn()
{
    scale(1.1,1.1);
}

void SimView::zoomOut()
{
    scale(1/1.1,1/1.1);
}

void SimView::zoomFit()
{
    fitInView(scene->itemsBoundingRect(),Qt::KeepAspectRatio);
}


Contact* SimView::getContact(int n)
{
    return this->contacts.value(n);
}

Segment* SimView::getSegmentByContacts(int contactA, int contactB)
{
    int min = contactA < contactB ? contactA : contactB;
    int max = contactA < contactB ? contactB : contactA;

    foreach(Segment* s, this->segments)
    {
        if(s->relie(this->contacts.value(min), this->contacts.value(max)))
            return s;
    }
    return nullptr;
}

void SimView::animationStart()
{
    timer->start(1000/FRAME_RATE);
}



#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QThread>
#include <QApplication>

#ifdef WITHSOUND
#include <QSound>


class SoundThread : public QThread
{
public:
    SoundThread(QObject *parent=nullptr): QThread(parent){

    }

protected:
    void run()
    {
        QSound *sound=new QSound(QApplication::applicationDirPath()+"/../sound/explosion2.wav", nullptr);
        sound->setParent(this);
        sound->play();
  //      QSound::play(QApplication::applicationDirPath()+"/../sound/explosion2.wav");
//        QSound::play(":/sound/explosion.wav");
    }
};

#endif // WITHSOUND

void SimView::animationStep()
{
    QList<Loco*> listeLocos = this->Locos.values();

    bool tropProche;

    QList<Voie*> prochainesVoies;

    foreach(Loco* l, listeLocos)
    {
        if(l->getActive() && l->getVoie() != nullptr)
        {
            if(l->getVitesse() != 0)
                l->avancer((l->getVitesse() * 1000.0 / FRAME_RATE) * FACTEUR_VITESSE);

            QPolygonF contourLoco = l->getContour();
            QPolygonF contourAutreLoco;
            //test de collision
            foreach(Loco* otherLoco, listeLocos)
            {
                if(l != otherLoco)
                {
                    contourAutreLoco = otherLoco->getContour();

                    if(contourLoco.subtracted(contourAutreLoco) != contourLoco)
                    {
                        animationStop();
                        l->setActive(false);
                        otherLoco->setActive(false);
                        ExplosionItem *item=new ExplosionItem();
                        QPixmap img(":images/explosion.png");
                        item->setPixmap(img);
                        scene->addItem(item);
                        QPointF debPoint((l->pos().x()+otherLoco->pos().x())/2,
                                    (l->pos().y()+otherLoco->pos().y())/2);
                        QPointF endPoint((l->pos().x()+otherLoco->pos().x())/2-256,
                                    (l->pos().y()+otherLoco->pos().y())/2-256);
                        item->setPos(endPoint);

                        QPropertyAnimation *animation1=new QPropertyAnimation(item, "pos");
                        animation1->setDuration(500);
                        animation1->setStartValue(debPoint);
                        animation1->setEndValue(endPoint);

                        QPropertyAnimation *animation2=new QPropertyAnimation(item, "scale");
                        animation2->setDuration(500);
                        animation2->setStartValue(0.0);
                        animation2->setEndValue(1.0);

                        QParallelAnimationGroup *animationGroup=new QParallelAnimationGroup();

                        animationGroup->addAnimation(animation1);
                        animationGroup->addAnimation(animation2);

                        item->setZValue(ZVAL_EXPLOSION);
                        item->show();
                        animationGroup->start();
#ifdef WITHSOUND
                        SoundThread *thread=new SoundThread(this);
                        thread->start();
#endif // WITHSOUND
                    }
                }
            }


            //alerte proximite. Pas encore optimal.
            qreal distanceSecurite = l->getVitesse() * 2000.0 * FACTEUR_VITESSE;

            prochainesVoies.append(l->getVoie());
            prochainesVoies.append(l->getVoieSuivante());
            distanceSecurite -= prochainesVoies.last()->getLongueurAParcourir();

            while(distanceSecurite > 0)
            {
                prochainesVoies.append(prochainesVoies.last()->getVoieSuivante(prochainesVoies.at(prochainesVoies.length()-2)));
                distanceSecurite -= prochainesVoies.last()->getLongueurAParcourir();
            }

            tropProche = false;

            foreach(Voie* v, prochainesVoies)
            {
                foreach(Loco* autreLoco, listeLocos)
                {
                    if(autreLoco != l && v == autreLoco->getVoie())
                    {
                        tropProche = true;
                    }
                }
            }
            if(tropProche)
                l->setAlerteProximite(true);
            else
                l->setAlerteProximite(false);

            prochainesVoies.clear();
        }
    }
}

void SimView::animationStop()
{
    timer->stop();
}

void SimView::setLoco(int contactA, int contactB, int numLoco, int vitesseLoco)
{
    Segment* s = getSegmentByContacts(contactA, contactB);

    if (s == nullptr)
    {
        QMessageBox::warning(this,"Error",QString("Les numéros de contact (%1,%2) entre lesquels se trouve la loco ne sont pas valides. Ils doivent être directement voisins.\nL'application va se terminer.").arg(contactA).arg(contactB));
        exit(-1);
    }

    Voie* v = s->getMilieu();


    Loco* l = this->Locos.value(numLoco);

    this->Locos.value(numLoco)->setVitesse(vitesseLoco);

    l->setVoie(v);

    l->setVoieSuivante(contactA > contactB ? s->getSuivantMilieu() : s->getPrecedentMilieu());

    l->setPos(v->pos());

    if(l->getVoieSuivante() == l->getVoie()->getVoieVoisineDOrdre(0))
    {
        l->setRotation(l->rotation() - v->getAngleDeg(0));
        l->setAngleCumule(l->getAngleCumule() + v->getAngleDeg(0));
    }
    else
    {
        l->setRotation(l->rotation() + (- v->getAngleDeg(0) - 180.0) < 0.0 ? (- v->getAngleDeg(0) + 180.0) : (- v->getAngleDeg(0) - 180.0));
        l->setAngleCumule(l->getAngleCumule() + ((v->getAngleDeg(0) - 180.0) < 0.0 ? (v->getAngleDeg(0) + 180.0) : (v->getAngleDeg(0) - 180.0)));
    }
}

void SimView::askLoco(int /*contactA*/, int /*contactB*/)
{
    //prompter un message demandant le numLoco et la vitesse.
}

void SimView::setVitesseLoco(int numLoco, int vitesseLoco)
{
    if (!checkLoco(numLoco))
        return;
    this->Locos.value(numLoco)->setVitesse(vitesseLoco);
}

void SimView::reverseLoco(int numLoco)
{
    if (!checkLoco(numLoco))
        return;
    this->Locos.value(numLoco)->inverserSens();
}

void SimView::setVitesseProgressiveLoco(int numLoco, int vitesseLoco)
{
    if (!checkLoco(numLoco))
        return;
    this->Locos.value(numLoco)->setVitesse(vitesseLoco); //similaire à setVitesseLoco!
}

void SimView::stopLoco(int numLoco)
{
    if (!checkLoco(numLoco))
        return;
    this->Locos.value(numLoco)->setVitesse(0);
}

void SimView::setVoieVariable(int numVoieVariable, int direction)
{
    if (!checkVoieVariable(numVoieVariable))
        return;
    this->VoiesVariables.value(numVoieVariable)->setEtat(direction);
}

void SimView::locoSurNouveauSegment(Contact *ctc1, Contact *ctc2, Loco *l)
{
    l->setSegmentActuel(getSegmentByContacts(contacts.key(ctc1), contacts.key(ctc2)));
}

void SimView::voieVariableModifiee(Voie *v)
{
    notificationVoieVariableModifiee(v);
}


bool SimView::checkLoco(int numLoco)
{
    if (!this->Locos.contains(numLoco))
    {
        QMessageBox::critical(this,"Erreur",QString("La loco %1 n'existe pas!\nL'application va se terminer.").arg(numLoco));
        exit(-1);
    }
    return true;
}

bool SimView::checkVoieVariable(int numVoie)
{
    if (!this->VoiesVariables.contains(numVoie))
    {
        QMessageBox::critical(this,"Erreur",QString("La voie variable %1 n'existe pas sur la maquette sélectionnée!\nL'application va se terminer.").arg(numVoie));
        exit(-1);
    }
    return true;
}
