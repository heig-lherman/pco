
#include <iostream>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QSettings>
#include <QCoreApplication>
#include <QDockWidget>
#include <QCloseEvent>
#include <QLineEdit>
#include <QtGlobal>

#include "commandetrain.h"
#include "mainwindow.h"
#include "trainsimsettings.h"
#include "maquettemanager.h"

// Define a compatibility symbol due to "QString::SkipEmptyParts" being
// deprecated in newer versions of Qt
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define SkipEmptyParts      Qt::SkipEmptyParts
#else
#define SkipEmptyParts      QString::SkipEmptyParts
#endif

 void outcallback( const char* ptr, std::streamsize count, void* pTextBox )
 {
   (void) count;
   static QString to;
   static QString towrite;
   for(int i=0;i<count;i++)
   {
       if (ptr[i]=='\n')
       {
           QTextEdit* p = static_cast< QTextEdit* >( pTextBox );
           p->append( towrite );
           towrite.clear();
       }
       else
           towrite+=ptr[i];
   }
 }

#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    generalConsole = new QTextEdit(this);
    dockGeneralConsole = new QDockWidget("Console generale",this);
    dockGeneralConsole->setWidget(generalConsole);
    addDockWidget(Qt::BottomDockWidgetArea,dockGeneralConsole,Qt::Horizontal);

    inputWidget = new QLineEdit("",this);
    inputDock = new QDockWidget("Input", this);
    inputDock->setWidget(inputWidget);
    addDockWidget(Qt::TopDockWidgetArea, inputDock, Qt::Horizontal);
    CONNECT(inputWidget, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
    CommandeTrain* ct = CommandeTrain::getInstance();
    CONNECT(this, SIGNAL(commandSent(QString)), ct, SLOT(commandSent(QString)))

    myRedirector = new StdRedirector<>( std::cout, outcallback, generalConsole );

    //Lecture des informations des voies.
    QFile fichierInfosVoies(DATADIR+"/infosVoies.txt");
    if (!fichierInfosVoies.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0,"Erreur",QString("Le fichier de description des voies ne peut être trouvé. Vérifiez qu'il est bien présent dans le répertoire parent de l'exécutable.\n Le nom du fichier est: %1.\nAvez-vous effectué un \"make install\"?").arg(fichierInfosVoies.fileName()));
        exit(0);
    }
    QTextStream lecture(&fichierInfosVoies);

    QString ligne;

    QStringList ligneDecoupee;

    QList<double>* description;

    ligne = lecture.readLine();

    while(!ligne.startsWith("EOF"))
    {
        ligneDecoupee = ligne.split(QRegExp("\\s+"), SkipEmptyParts);

        description = new QList<double>();

        /* En l'etat, le programme gere 6 types de voies differentes :
         * - droite : caracterisees par leur longueur.
         * - courbe : caracterisees par leur rayon de courbure, et l'angle parcouru.
         * - aiguillage : caracterisees par leur rayon de courbure et l'angle parcouru (pour la partie courbe)
         *                et par leur longueur (pour la partie droite).
         * - croisement : caracterisees par leur longueur (pour les deux parties droites) et l'angle aigu entre les deux parties droites.
         *                Les parties droites se croisent toujours en leur milieu.
         * - traversee-jonction : caracterisees par leur longueur (pour les deux parties droites, le rayon de courbure des parties courbes,
         *                        et l'angle parcouru.
         * - buttoir : caracterisees par leur longueur (utile uniquement pour le dessin.
         *
         * Il est possible d'ajouter des types de voies. Referez-vous a la documentation.
         */
        if(ligneDecoupee.at(1) == "droite")
            description->append(1.0);
        else if(ligneDecoupee.at(1) == "courbe")
            description->append(2.0);
        else if(ligneDecoupee.at(1) == "aiguillage")
            description->append(3.0);
        else if(ligneDecoupee.at(1) == "croisement")
            description->append(4.0);
        else if(ligneDecoupee.at(1) == "traversee-jonction")
            description->append(5.0);
        else if(ligneDecoupee.at(1) == "buttoir")
            description->append(6.0);
        else if(ligneDecoupee.at(1) == "aiguillageEnroule")
            description->append(7.0);
        else if(ligneDecoupee.at(1) == "aiguillageTriple")
            description->append(8.0);


        for(int i =2; i < ligneDecoupee.length(); i++)
        {
            description->append(ligneDecoupee.at(i).toDouble());
        }

        //chargement des informations des voies dans la QMap idoine.
        infosVoies.insert(ligneDecoupee.at(0).toInt(), description);

        ligne = lecture.readLine();

    }


    m_state=PAUSE;

    setGeometry(50,50,530,580);
    locoSignalMapper = new QSignalMapper(this);
    CONNECT(locoSignalMapper, SIGNAL(mapped(QObject *)),
            this, SLOT(toggleLoco(QObject *)));

    setGeometry(0,0,530,580);

    simView = new SimView(this);

    setCentralWidget(simView);

    createActions();
    createMenus();
    createToolbar();
    updateMenus();


    statusLabel=new QLabel(this);
    QStatusBar *status=new QStatusBar(this);
    setStatusBar(status);
    status->addPermanentWidget(statusLabel);
    status->showMessage(tr("Pret a l'utilisation"));

    readSettings();
    toggleSimulation();
    toggleSimulation();
}

MainWindow::~MainWindow()
{
    delete myRedirector;
}


#define APPNAME "QTrainSim"
#define ORG     "REDS"

void MainWindow::readSettings()
{
    QSettings settings(ORG,APPNAME);
    this->setGeometry(settings.value("geometry",QRect(100,100,700,600)).toRect());
    TrainSimSettings::getInstance()->setViewAiguillageNumber(settings.value("viewAiguillageNb",false).toBool());
    viewAiguillageNumberAct->setChecked(TrainSimSettings::getInstance()->getViewAiguillageNumber());
    TrainSimSettings::getInstance()->setViewContactNumber(settings.value("viewContactNb",false).toBool());
    viewContactNumberAct->setChecked(TrainSimSettings::getInstance()->getViewContactNumber());
    TrainSimSettings::getInstance()->setViewLocoLog(settings.value("viewLocoLog",false).toBool());
    viewLocoLogAct->setChecked(TrainSimSettings::getInstance()->getViewLocoLog());
    TrainSimSettings::getInstance()->setInertie(settings.value("inertie",true).toBool());
    inertieAct->setChecked(TrainSimSettings::getInstance()->getInertie());

}

void MainWindow::writeSettings() const
{
    QSettings settings(ORG,APPNAME);
    settings.setValue("geometry",geometry());
    settings.setValue("viewAiguillageNb",TrainSimSettings::getInstance()->getViewAiguillageNumber());
    settings.setValue("viewContactNb",TrainSimSettings::getInstance()->getViewContactNumber());
    settings.setValue("viewLocoLog",TrainSimSettings::getInstance()->getViewLocoLog());
    settings.setValue("inertie",TrainSimSettings::getInstance()->getInertie());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::finishedAnimation()
{
}

void MainWindow::simulationStep()
{
    //cout << "Simulation step " << m_simStep << endl;
}

void MainWindow::zoomFit()
{
    simView->zoomFit();
}

void MainWindow::zoomOut()
{
    simView->zoomOut();
}

void MainWindow::zoomIn()
{
    simView->zoomIn();
}

void MainWindow::createToolbar()
{
    toolBar=this->addToolBar("Tools");
//    toolBar->addAction(this->chargerMaquetteAct);
#ifndef MAQUETTE
    toolBar->addAction(this->toggleSimAct);
#endif
    toolBar->addAction(this->emergencyStopAct);
    toolBar->addSeparator();
    toolBar->addAction(zoomInAct);
    toolBar->addAction(zoomOutAct);
}

void MainWindow::updateMenus()
{
    toggleSimAct->setEnabled(true);
}


void MainWindow::toggleSimulation()
{

    if (m_state==PAUSE)
    {
        this->simView->animationStart();
        toggleSimAct->setText(tr("&Pause"));
        toggleSimAct->setShortcut(tr("Ctrl+P"));
        toggleSimAct->setStatusTip(tr("Pause the simulation"));
        toggleSimAct->setIcon(QIcon(QPixmap(":images/simulate_break.png")));
        statusLabel->setText(tr("Running"));
        m_state=RUNNING;
    }
    else
    {
        this->simView->animationStop();
        toggleSimAct->setText(tr("&Resume"));
        toggleSimAct->setShortcut(tr("Ctrl+R"));
        toggleSimAct->setStatusTip(tr("Resume the simulation"));
        toggleSimAct->setIcon(QIcon(QPixmap(":images/simulate_start.png")));
        m_state=PAUSE;
        statusLabel->setText(tr("Pause"));
    }

    updateMenus();
}

#ifdef CDEVELOP
extern "C" {
void emergency_stop();
}
#else
void emergency_stop();
#endif

void MainWindow::emergencyStop()
{
    emergency_stop();
}

void MainWindow::rotatePlus()
{
    simView->rotate(10);
}

void MainWindow::rotateMinus()
{
    simView->rotate(-10);
}

void MainWindow::setLocoState(LocoCtrl *loco,LocoCtrl::STATE state)
{
    switch (state)
        {
        case LocoCtrl::PAUSE: {
            loco->state=LocoCtrl::PAUSE;
            loco->ptrLoco->setActive(false);
            loco->toggle->setText(tr("Restart the loco %1").arg(loco->loco));
            loco->toggle->setStatusTip(tr("Restart the loco %1").arg(loco->loco));
            loco->toggle->setIcon(QIcon(QPixmap(":images/simulate_start.png")));
    } break;
        case LocoCtrl::RUNNING: {
            loco->state=LocoCtrl::RUNNING;
            loco->ptrLoco->setActive(true);
            loco->toggle->setText(QString("Pause loco %1").arg(loco->loco));
            loco->toggle->setStatusTip(tr("Pause the loco"));
            loco->toggle->setIcon(QIcon(QPixmap(":images/simulate_break.png")));
    } break;
    }
}

void MainWindow::toggleLoco(QObject *locoCtrl)
{
    LocoCtrl *locoC=dynamic_cast<LocoCtrl*>(locoCtrl);
    statusBar()->showMessage("Toggle");

    switch(locoC->state)
        {
        case LocoCtrl::RUNNING: {
            setLocoState(locoC,LocoCtrl::PAUSE);
        } break;
        case LocoCtrl::PAUSE: {
            setLocoState(locoC,LocoCtrl::RUNNING);
        } break;
    };
}

#include <QMessageBox>

void MainWindow::afficherMessageLoco(int numLoco,QString message)
{
    for(int i=0;i<locoCtrls.size();i++)
        if (locoCtrls.at(i)->loco==numLoco)
        {
            locoCtrls.at(i)->console->append(message);
            return;
        }
    QMessageBox::warning(this,"Numéro de loco",QString(
                             "Attention, pour l'affichage dans la console, le\
                             numero de loco %1 n'est pas valide").arg(numLoco));
}

void MainWindow::addLoco(int no_loco)
{
    Loco* l = new Loco(no_loco);
    simView->addLoco(l, no_loco);

    LocoCtrl *c=new LocoCtrl;
    QString dockName=QString("Loco %1").arg(no_loco);
    c->dock = new QDockWidget(dockName);
    addDockWidget(Qt::RightDockWidgetArea,c->dock,Qt::Vertical);

    c->console = new QTextEdit(this);
    c->dock->setWidget(c->console);
    c->state=LocoCtrl::RUNNING;
    c->loco=no_loco;
    c->ptrLoco = l;
    l->controller=c;
    c->toolBar=new QToolBar(this);
    QString s=QString("Loco %1: ").arg(no_loco);
    c->toolBar->addWidget(new QLabel(s));
    c->toggle= new QAction(tr("&Pause"), this);
    setLocoState(c,LocoCtrl::RUNNING);

    CONNECT(c->toggle, SIGNAL(triggered()), locoSignalMapper, SLOT(map()));
    locoSignalMapper->setMapping(c->toggle, c);

    locoCtrls << c;

#ifndef MAQUETTE
    c->toolBar->addAction(c->toggle);
    actionMenu->addAction(c->toggle);
    toolBar->addWidget(c->toolBar);
#endif // MAQUETTE
}

void MainWindow::createActions()
{

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setIcon(QIcon(QPixmap(":images/simulate_exit.png")));
    CONNECT(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    printAct = new QAction(tr("&Print"), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setStatusTip(tr("Print the map"));
    CONNECT(printAct, SIGNAL(triggered()), this, SLOT(print()));

    toggleSimAct = new QAction(tr("&Pause"), this);
    toggleSimAct->setShortcut(tr("Ctrl+P"));
    toggleSimAct->setStatusTip(tr("Pause the simulation"));
    toggleSimAct->setIcon(QIcon(QPixmap(":images/simulate_break.png")));
    CONNECT(toggleSimAct, SIGNAL(triggered()), this, SLOT(toggleSimulation()));

    emergencyStopAct = new QAction(tr("&Emergency stop"),this);
    emergencyStopAct->setShortcut(tr("Ctrl+E"));
    emergencyStopAct->setStatusTip(tr("Executes an emergency stop. Has to be implemented by the students"));
    emergencyStopAct->setIcon(QIcon(QPixmap(":images/simulate_end.png")));
    CONNECT(emergencyStopAct, SIGNAL(triggered()), this, SLOT(emergencyStop()));


/*    QPixmap pix(":images/simulate_start.png");
    QPainter painter(&pix);
    painter.drawText(QRectF(26,0,7,12),"1");

    resumeAct = new QAction(tr("&Resume"), this);
    resumeAct->setShortcut(tr("Ctrl+R"));
    resumeAct->setStatusTip(tr("Resume the simulation"));
    resumeAct->setIcon(QIcon(pix));
    CONNECT(resumeAct, SIGNAL(triggered()), this, SLOT(resumeSimulation()));
*/


    chargerMaquetteAct = new QAction(tr("Load circuit"), this);
    chargerMaquetteAct->setShortcut(tr("Ctrl+C"));
    chargerMaquetteAct->setStatusTip(tr("Charger une maquette"));
    CONNECT(chargerMaquetteAct, SIGNAL(triggered()), this, SLOT(on_actionCharger_Maquette_triggered()));


    zoomInAct = new QAction(tr("Zoom &in"), this);
    zoomInAct->setIcon(QIcon(QPixmap(":/images/zoomin.png")));
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
//    zoomInAct->setShortcut(tr("Ctrl+K"));
    zoomInAct->setStatusTip(tr("Zoom in"));
    CONNECT(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));


    zoomOutAct = new QAction(tr("Zoom &out"), this);
    zoomOutAct->setIcon(QIcon(QPixmap(":/images/zoomout.png")));
    zoomOutAct->setShortcut(tr("Ctrl+L"));
    zoomOutAct->setStatusTip(tr("Zoom out"));
    CONNECT(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));


    zoomFitAct = new QAction(tr("Zoom &fit"), this);
    //     zoomFitAct->setShortcut(tr("Ctrl+L"));
    zoomFitAct->setStatusTip(tr("Zoom fit"));
    CONNECT(zoomFitAct, SIGNAL(triggered()), this, SLOT(zoomFit()));

    rotatePlusAct = new QAction(tr("Rotate +"), this);
    rotatePlusAct->setShortcut(tr("Ctrl+R"));
    rotatePlusAct->setStatusTip(tr("Rotate posivite"));
    CONNECT(rotatePlusAct, SIGNAL(triggered()), this, SLOT(rotatePlus()));

    rotateMinusAct = new QAction(tr("Rotate -"), this);
    rotateMinusAct->setShortcut(tr("Ctrl+T"));
    rotateMinusAct->setStatusTip(tr("Rotate negative"));
    CONNECT(rotateMinusAct, SIGNAL(triggered()), this, SLOT(rotateMinus()));


    viewContactNumberAct = new QAction(tr("View contact number"), this);
    //viewContactNumberAct->setShortcut(tr("Ctrl+T"));
    viewContactNumberAct->setStatusTip(tr("View contact number"));
    viewContactNumberAct->setCheckable(true);
    CONNECT(viewContactNumberAct, SIGNAL(triggered()), this, SLOT(viewContactNumber()));

    viewAiguillageNumberAct = new QAction(tr("View aiguillage number"), this);
    //viewAiguillageNumberAct->setShortcut(tr("Ctrl+T"));
    viewAiguillageNumberAct->setStatusTip(tr("View contact number"));
    viewAiguillageNumberAct->setCheckable(true);
    CONNECT(viewAiguillageNumberAct, SIGNAL(triggered()), this, SLOT(viewAiguillageNumber()));

    viewLocoLogAct = new QAction(tr("View Loco logs"), this);
    //viewLocoLogAct->setShortcut(tr("Ctrl+T"));
    viewLocoLogAct->setStatusTip(tr("View contact number"));
    viewLocoLogAct->setCheckable(true);
    CONNECT(viewLocoLogAct, SIGNAL(triggered()), this, SLOT(viewLocoLog()));

    viewInputAct = inputDock->toggleViewAction();

    inertieAct = new QAction(tr("Inertia"), this);
    inertieAct->setShortcut(tr("Ctrl+I"));
    inertieAct->setStatusTip(tr("Enable inertia"));
    inertieAct->setCheckable(true);
    CONNECT(inertieAct, SIGNAL(triggered()), this, SLOT(toggleInertie()));
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(chargerMaquetteAct);
    fileMenu->addAction(printAct);
    fileMenu->addAction(exitAct);

    actionMenu = menuBar()->addMenu(tr("&Actions"));
    actionMenu->addSeparator();
#ifndef MAQUETTE
    actionMenu->addAction(toggleSimAct);
#endif // MAQUETTE
    actionMenu->addAction(emergencyStopAct);
    actionMenu->addSeparator();

    QMenu *view=menuBar()->addMenu(tr("&View"));
    view->addAction(zoomInAct);
    view->addAction(zoomOutAct);
    view->addAction(zoomFitAct);
    view->addAction(rotatePlusAct);
    view->addAction(rotateMinusAct);
    view->addSeparator();
    view->addAction(viewLocoLogAct);
    view->addAction(viewContactNumberAct);
    view->addAction(viewAiguillageNumberAct);
    view->addAction(viewInputAct);

    QMenu *settings=menuBar()->addMenu(tr("&Settings"));
    settings->addAction(inertieAct);
}

#include <QPrintDialog>
#include <QPrinter>

void MainWindow::print()
{
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
 //   if (editor->textCursor().hasSelection())
 //       dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (dialog->exec() != QDialog::Accepted)
        return;

//    QPrinter printer(QPrinter::HighResolution);
//    printer.setPageSize(QPrinter::A4);
    QPainter painter(&printer);

    // print, fitting the viewport contents into a full page
    simView->render(&painter);

    // print the upper half of the viewport into the lower.
    // half of the page.
    QRect viewport = simView->viewport()->rect();
    simView->render(&painter,QRectF(0,0,printer.width(),printer.height()),
                    viewport);
//                QRectF(0, printer.height() / 2,
//                       printer.width(), printer.height() / 2),
//                viewport.adjusted(0, 0, 0, -viewport.height() / 2));

}

void MainWindow::viewContactNumber()
{
    TrainSimSettings::getInstance()->setViewContactNumber(viewContactNumberAct->isChecked());
    simView->redraw();
}

void MainWindow::viewAiguillageNumber()
{
    TrainSimSettings::getInstance()->setViewAiguillageNumber(viewAiguillageNumberAct->isChecked());
    simView->redraw();
}

void MainWindow::viewLocoLog()
{
    TrainSimSettings::getInstance()->setViewLocoLog(viewLocoLogAct->isChecked());
}

void MainWindow::toggleInertie()
{
    TrainSimSettings::getInstance()->setInertie(inertieAct->isChecked());
}

SimView* MainWindow::getSimView()
{
    return simView;
}

void MainWindow::on_actionCharger_Maquette_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier Maquette"), DATADIR+"/Maquettes", tr("Text Files (*.txt)"));
    chargerMaquette(filename);
}

void MainWindow::chargerMaquette(QString filename)
{
    this->simView->viderMaquette();

    // stockage temporaire des voies, avec les identifiants des voies a lier.
    QMap <Voie*, QList<int>*> voiesALier;
    // stockage temporaire des voies, indexees par identifiants.
    QMap <int, Voie*> IDVoies;

    QStringList listeTemporaire;
    QList<qreal>* infosVoieEnTraitement;
    int IDvoie;
    qreal directionVoieEnTraitement;

    QFile fichier(filename);

    if(!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //declaration d'erreur.
    }

    QTextStream lecture(&fichier);
    QString ligne;
    bool premiereInfoValide;
    int limite;

    //avance rapide pour passer une eventuelle introduction.

    ligne = lecture.readLine();

    listeTemporaire = ligne.split(" ", SkipEmptyParts);

    limite = listeTemporaire.at(0).toInt(&premiereInfoValide);

    while((listeTemporaire.length() != 1) && !premiereInfoValide)
    {
        if(lecture.atEnd())
            qDebug() << "Erreur de lecture de fichier : fichier non standard. (nombre de voies mal indique)";
        ligne = lecture.readLine();

        listeTemporaire = ligne.split(" ", SkipEmptyParts);

        limite = listeTemporaire.at(0).toInt(&premiereInfoValide);

    }

    // lecture des informations relatives aux voies, creation des voies.

    VoieDroite* vd;
    VoieCourbe* vc;
    VoieAiguillage* va;
    VoieAiguillageEnroule* vae;
    VoieAiguillageTriple* vat;
    VoieCroisement* vcr;
    VoieTraverseeJonction* vt;
    VoieButtoir* vb;

    for(int i =0; i < limite; i++)
    {
        ligne = lecture.readLine();

        listeTemporaire = ligne.split(" ", SkipEmptyParts);

        IDvoie = listeTemporaire.at(0).toInt();

        //recuperation des infos de la voie en traitement.
        infosVoieEnTraitement = infosVoies[listeTemporaire.at(1).toInt()];

        if(infosVoieEnTraitement->at(0) == 1.0)//voie Droite
        {
            //Creation et insertion de la voie dans les stockages temporaires.
            vd = new VoieDroite(infosVoieEnTraitement->at(1));
            vd->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, vd);
            voiesALier.insert(vd, new QList<int>());
            voiesALier[vd]->append(listeTemporaire.at(2).toInt());
            voiesALier[vd]->append(listeTemporaire.at(3).toInt());
            this->simView->addVoie(vd, listeTemporaire.at(0).toInt());
        }
        else if(infosVoieEnTraitement->at(0) == 2.0)//voie Courbe
        {
            // les valeurs numeriques choisies pour representer gauche et droite sont utiles pour les calculs trigonometriques lors du placement des voies.
            // NE CHANGER SOUS AUCUN PRETEXTE.
            if(listeTemporaire.at(4).toLower() == "gauche")
            {
                directionVoieEnTraitement = 1.0;
            }
            else if(listeTemporaire.at(4).toLower() == "droite")
            {
                directionVoieEnTraitement = -1.0;
            }
            else //en cas d'erreur dans le fichier...
                qDebug() << "Erreur de lecture de fichier : fichier non standard (direction de courbe). ";

            //Creation et insertion de la voie dans les stockages temporaires.
            vc = new VoieCourbe(infosVoieEnTraitement->at(1), infosVoieEnTraitement->at(2), directionVoieEnTraitement);
            vc->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, vc);
            voiesALier.insert(vc, new QList<int>());
            voiesALier[vc]->append(listeTemporaire.at(2).toInt());
            voiesALier[vc]->append(listeTemporaire.at(3).toInt());
            this->simView->addVoie(vc, listeTemporaire.at(0).toInt());
        }
        else if(infosVoieEnTraitement->at(0) == 3.0)//voie Aiguillage
        {
            // les valeurs numeriques choisies pour representer gauche et droite sont utiles pour les calculs trigonometriques lors du placement des voies.
            // NE CHANGER SOUS AUCUN PRETEXTE.
            if(listeTemporaire.at(5).toLower() == "gauche")
                directionVoieEnTraitement = 1.0;
            else if(listeTemporaire.at(5).toLower() == "droite")
                directionVoieEnTraitement = -1.0;
            else
                qDebug() << "Erreur de lecture de fichier : fichier non standard (direction d'aiguillage). ";

            //Creation et insertion de la voie dans les stockages temporaires.
            va = new VoieAiguillage(infosVoieEnTraitement->at(1), infosVoieEnTraitement->at(2), infosVoieEnTraitement->at(3), directionVoieEnTraitement);
            va->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, va);
            voiesALier.insert(va, new QList<int>());
            voiesALier[va]->append(listeTemporaire.at(2).toInt());
            voiesALier[va]->append(listeTemporaire.at(3).toInt());
            voiesALier[va]->append(listeTemporaire.at(4).toInt());
            this->simView->addVoie(va, listeTemporaire.at(0).toInt());
        }
        else if(infosVoieEnTraitement->at(0) == 4.0)//voie Croisement
        {
            //Creation et insertion de la voie dans les stockages temporaires.
            vcr = new VoieCroisement(infosVoieEnTraitement->at(1), infosVoieEnTraitement->at(2));
            vcr->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, vcr);
            voiesALier.insert(vcr, new QList<int>());
            voiesALier[vcr]->append(listeTemporaire.at(2).toInt());
            voiesALier[vcr]->append(listeTemporaire.at(3).toInt());
            voiesALier[vcr]->append(listeTemporaire.at(4).toInt());
            voiesALier[vcr]->append(listeTemporaire.at(5).toInt());
            this->simView->addVoie(vcr, listeTemporaire.at(0).toInt());
        }
        else if(infosVoieEnTraitement->at(0) == 5.0)//voie Traversee-Jonction
        {
            //Creation et insertion de la voie dans les stockages temporaires.
            vt= new VoieTraverseeJonction(infosVoieEnTraitement->at(1), infosVoieEnTraitement->at(2), infosVoieEnTraitement->at(3));
            vt->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, vt);
            voiesALier.insert(vt, new QList<int>());
            voiesALier[vt]->append(listeTemporaire.at(2).toInt());
            voiesALier[vt]->append(listeTemporaire.at(3).toInt());
            voiesALier[vt]->append(listeTemporaire.at(4).toInt());
            voiesALier[vt]->append(listeTemporaire.at(5).toInt());
            this->simView->addVoie(vt, listeTemporaire.at(0).toInt());
        }
        else if(infosVoieEnTraitement->at(0) == 6.0)//voie Buttoir
        {
            //Creation et insertion de la voie dans les stockages temporaires.
            vb = new VoieButtoir(infosVoieEnTraitement->at(1));
            vb->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, vb);
            voiesALier.insert(vb, new QList<int>());
            voiesALier[vb]->append(listeTemporaire.at(2).toInt());
            this->simView->addVoie(vb, listeTemporaire.at(0).toInt());
        }
        else if(infosVoieEnTraitement->at(0) == 7.0)//voie Aiguillage Enroule
        {
            // les valeurs numeriques choisies pour representer gauche et droite sont utiles pour les calculs trigonometriques lors du placement des voies.
            // NE CHANGER SOUS AUCUN PRETEXTE.
            if(listeTemporaire.at(5).toLower() == "gauche")
                directionVoieEnTraitement = 1.0;
            else if(listeTemporaire.at(5).toLower() == "droite")
                directionVoieEnTraitement = -1.0;
            else
                qDebug() << "Erreur de lecture de fichier : fichier non standard (direction d'aiguillage). ";

            //Creation et insertion de la voie dans les stockages temporaires.
            vae = new VoieAiguillageEnroule(infosVoieEnTraitement->at(1), infosVoieEnTraitement->at(2), infosVoieEnTraitement->at(3), directionVoieEnTraitement);
            vae->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, vae);
            voiesALier.insert(vae, new QList<int>());
            voiesALier[vae]->append(listeTemporaire.at(2).toInt());
            voiesALier[vae]->append(listeTemporaire.at(4).toInt()); //ordre inversé, pour la cohérence du code...
            voiesALier[vae]->append(listeTemporaire.at(3).toInt());
            this->simView->addVoie(vae, listeTemporaire.at(0).toInt());
        }
        else if(infosVoieEnTraitement->at(0) == 8.0)//voie Aiguillage Triple
        {
            //Creation et insertion de la voie dans les stockages temporaires.
            vat = new VoieAiguillageTriple(infosVoieEnTraitement->at(1), infosVoieEnTraitement->at(2), infosVoieEnTraitement->at(3));
            vat->setIdVoie(IDvoie);
            IDVoies.insert(IDvoie, vat);
            voiesALier.insert(vat, new QList<int>());
            voiesALier[vat]->append(listeTemporaire.at(2).toInt());
            voiesALier[vat]->append(listeTemporaire.at(3).toInt());
            voiesALier[vat]->append(listeTemporaire.at(4).toInt());
            voiesALier[vat]->append(listeTemporaire.at(5).toInt());
            this->simView->addVoie(vat, listeTemporaire.at(0).toInt());
        }
    }
    //finalisation de la creation des voies.

    for(int i = 1; i <= IDVoies.size(); i++)
    {
        for(int j = 0; j < voiesALier[IDVoies[i]]->length(); j++)
        {
            IDVoies[i]->lier(IDVoies[voiesALier[IDVoies[i]]->at(j)], j);
        }
    }

    //debut de la lecture des contacts.

    limite = lecture.readLine().toInt();

    Contact* c;

    for(int i=0; i < limite;i++)
    {
        ligne = lecture.readLine();

        listeTemporaire = ligne.split(" ", SkipEmptyParts);

        //creation des contacts.
        c = new Contact(listeTemporaire.at(0).toInt(), listeTemporaire.at(1).toInt());

        IDVoies[listeTemporaire.at(1).toInt()]->setContact(c);
        this->simView->addContact(c, listeTemporaire.at(0).toInt());
    }

    //debut de la lecture des aiguillages.

    limite = lecture.readLine().toInt();

    for(int i=0; i < limite;i++)
    {
        ligne = lecture.readLine();

        listeTemporaire = ligne.split(" ", SkipEmptyParts);

        VoieVariable *v=dynamic_cast<VoieVariable *>(IDVoies[listeTemporaire.at(1).toInt()]);

        this->simView->addVoieVariable(v, listeTemporaire.at(0).toInt());

        v->setNumVoieVariable(listeTemporaire.at(0).toInt());

    }

    //indication de la premiere voie a poser.

    Voie * premiereVoie = IDVoies[lecture.readLine().toInt()];

    this->simView->setPremiereVoie(premiereVoie);

    this->simView->construireMaquette();

    this->simView->genererSegments();

    this->simView->zoomFit();

    this->simView->repaint();

    // On détruit la map qui contient des pointeurs sur des QList
    QMapIterator<Voie*, QList<int>*> it(voiesALier);
    while (it.hasNext()) {
        it.next();
        delete it.value();
    }

    this->maquetteFinie.release();
}

void MainWindow::afficherMessage(QString message)
{
    this->generalConsole->append(message);
}


void MainWindow::selectionMaquette(QString maquette)
{

    MaquetteManager manager;

    QStringList list=manager.nomMaquettes();
    if (!list.contains(maquette))
    {
        QString message=QString("La maquette \"%1\" n'existe pas.\n").arg(maquette);
        if (list.size()==0)
        {
            message+="Aucune maquette n'est disponible. Elles devraient se trouver dans le repertoire \""+manager.dossierMaquette()+"\". Verifiez votre installation";
        }
        else
        {
            message+="Les maquettes valides sont:";
            foreach(QString maq,list)
                message+=QString("\n\t%1").arg(maq);
        }
        QMessageBox::warning(0,"La maquette n'existe pas",message);
        exit(1);
    }
    chargerMaquette(manager.fichierMaquette(maquette));
    semWaitMaquette.release();
}

void MainWindow::onReturnPressed()
{
    commandSent(inputWidget->text());
    inputWidget->setText("");
}
