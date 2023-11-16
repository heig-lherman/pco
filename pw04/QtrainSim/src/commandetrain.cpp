
#include <iostream>
#include <QApplication>
#include <QThread>

#include "commandetrain.h"
#include "mainwindow.h"



static MainWindow *mainwindow;
static SimView* simView;





CommandeTrain::CommandeTrain()
{
    command = "";
    mutex = new QMutex();
    VarCond = new QWaitCondition();
    waitingOn=false;
}

CommandeTrain* CommandeTrain::getInstance()
{
    static CommandeTrain instance;
    return &instance;
}

void CommandeTrain::init_maquette(void)
{
    mainwindow=new MainWindow();
    mainwindow->show();

    simView = mainwindow->getSimView();

    CONNECT(this, SIGNAL(setLoco(int,int,int,int)), simView, SLOT(setLoco(int,int,int,int)));
    CONNECT(this, SIGNAL(askLoco(int,int)), simView, SLOT(askLoco(int,int)));
    CONNECT(this, SIGNAL(setVitesseLoco(int,int)), simView, SLOT(setVitesseLoco(int,int)));
    CONNECT(this, SIGNAL(reverseLoco(int)), simView, SLOT(reverseLoco(int)));
    CONNECT(this, SIGNAL(setVitesseProgressiveLoco(int,int)), simView, SLOT(setVitesseProgressiveLoco(int,int)));
    CONNECT(this, SIGNAL(setVoieVariable(int,int)), simView, SLOT(setVoieVariable(int,int)));
    CONNECT(this, SIGNAL(addLoco(int)),mainwindow,SLOT(addLoco(int)));
    CONNECT(this, SIGNAL(selectMaquette(QString)),mainwindow,SLOT(selectionMaquette(QString)));
    CONNECT(this, SIGNAL(afficheMessage(QString)),mainwindow,SLOT(afficherMessage(QString)));
    CONNECT(this, SIGNAL(afficheMessageLoco(int,QString)),mainwindow,SLOT(afficherMessageLoco(int,QString)));

    QTimer::singleShot(10, this, SLOT(timerTrigger()));
}


#ifdef CDEVELOP
extern "C" int cmain();
#else
int cmain();
#endif

class UserThread : public QThread
{
public:

    // L'ouverture de dialogues est autorisee dans cette fonction
    virtual bool initialize() {
//        QMessageBox::warning(0,"coucou","Hello les amis");


        return true;


    }
protected:

    // L'ouverture de dialogues n'est pas autorisee dans cette fonction
    virtual void run() {

        cmain();
    }
};

static UserThread *userThread = nullptr;

CommandeTrain::~CommandeTrain()
{
    if (userThread != nullptr) {
        userThread->terminate();
        userThread->wait();
        delete userThread;
    }
}

void CommandeTrain::timerTrigger()
{


    userThread=new UserThread();
    if (!userThread->initialize()) {
        exit(0);
    }
    userThread->start();

}

void CommandeTrain::mettre_maquette_hors_service(void)
{

}

void CommandeTrain::mettre_maquette_en_service(void)
{

}

void CommandeTrain::ajouter_loco(int no_loco)
{
    emit addLoco(no_loco);
}

void CommandeTrain::diriger_aiguillage(int no_aiguillage, int direction, int /*temps_alim*/)
{
    emit setVoieVariable(no_aiguillage, direction);
}

void CommandeTrain::attendre_contact(int no_contact)
{
    Contact *c=simView->getContact(no_contact);
    if (c == nullptr)
    {
        QMessageBox::warning(nullptr,"Error",QString("Attention, le numéro de contact %1 n'est pas valide").arg(no_contact));
    }
    else
        c->attendContact();
}

void CommandeTrain::arreter_loco(int no_loco)
{
    emit setVitesseLoco(no_loco, 0);
}

void CommandeTrain::mettre_vitesse_progressive(int no_loco, int vitesse_future)
{
    emit setVitesseProgressiveLoco(no_loco, vitesse_future);
}

void CommandeTrain::mettre_fonction_loco(int /*no_loco*/, char /*etat*/)
{
    //sans effets sur le simulateur.
}

void CommandeTrain::inverser_sens_loco(int no_loco)
{
    emit reverseLoco(no_loco);
}

void CommandeTrain::mettre_vitesse_loco(int no_loco, int vitesse)
{
    emit setVitesseLoco(no_loco, vitesse);
}

void CommandeTrain::demander_loco(int contact_a, int contact_b, int */*no_loco*/, int */*vitesse*/)
{
    askLoco(contact_a, contact_b); //a refaire... pas adapte!
}

void CommandeTrain::assigner_loco(int contact_a,int contact_b,int no_loco,int vitesse)
{
    emit addLoco(no_loco);
    emit setLoco(contact_a, contact_b, no_loco, vitesse);
}

void CommandeTrain::selection_maquette(QString maquette)
{
    emit selectMaquette(maquette);
    mainwindow->semWaitMaquette.acquire();
    mainwindow->maquetteFinie.acquire();
}

void CommandeTrain::afficher_message(const char *message)
{
    QString mess=QString("%1").arg(message);
    emit afficheMessage(mess);
}


void CommandeTrain::afficher_message_loco(int numLoco,const char *message)
{
    QString mess=QString("%1").arg(message);
    emit afficheMessageLoco(numLoco,mess);
}

void CommandeTrain::commandSent(QString command)
{
    this->command = command;
    VarCond->wakeAll();
}

QString CommandeTrain::getCommand()
{
    mutex->lock();
    waitingOn=true;
    VarCond->wait(mutex);
    QString tmp = command;
    command = "";
    waitingOn=false;
    mutex->unlock();
    return tmp;
}
