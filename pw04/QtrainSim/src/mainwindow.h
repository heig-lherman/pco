#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QSignalMapper>
#include <QTextEdit>
#include <QSemaphore>
#include <ios>

#include "voieaiguillage.h"
#include "voieaiguillageenroule.h"
#include "voieaiguillagetriple.h"
#include "voiebuttoir.h"
#include "voiecourbe.h"
#include "voiecroisement.h"
#include "voiedroite.h"
#include "voietraverseejonction.h"
#include "simview.h"
#include "contact.h"
#include "connect.h"

template< class Elem = char, class Tr = std::char_traits< Elem > >
 class StdRedirector : public std::basic_streambuf< Elem, Tr >
 {
     /**
       * Callback Function.
       */
   typedef void (*pfncb) ( const Elem*, std::streamsize _Count, void* pUsrData );

 public:
     /**
       * Constructor.
       * @param a_Stream the stream to redirect
       * @param a_Cb the callback function
       * @param a_pUsrData user data passed to callback
       */
   StdRedirector( std::ostream& a_Stream, pfncb a_Cb, void* a_pUsrData ) :
     m_Stream( a_Stream ),
     m_pCbFunc( a_Cb ),
     m_pUserData( a_pUsrData )
   {
       //redirect stream
     m_pBuf = m_Stream.rdbuf( this );
   };

     /**
       * Destructor.
       * Restores the original stream.
       */
   ~StdRedirector()
   {
     m_Stream.rdbuf( m_pBuf );
   }

     /**
       * Override xsputn and make it forward data to the callback function.
       */
   std::streamsize xsputn( const Elem* _Ptr, std::streamsize _Count )
   {
     m_pCbFunc( _Ptr, _Count, m_pUserData );
     return _Count;
   }

     /**
       * Override overflow and make it forward data to the callback function.
       */
   typename Tr::int_type overflow( typename Tr::int_type v )
   {
     Elem ch = Tr::to_char_type( v );
     m_pCbFunc( &ch, 1, m_pUserData );
     return Tr::not_eof( v );
   }

protected:
   std::basic_ostream<Elem, Tr>& m_Stream;
   std::streambuf*               m_pBuf;
   pfncb                         m_pCbFunc;
   void*                         m_pUserData;
 };

class LocoCtrl : public QObject
{
public:
    int loco;
    Loco* ptrLoco;
    enum STATE {RUNNING=0,PAUSE=1} state;
    QAction *toggle;
    QToolBar *toolBar;
    QTextEdit *console;
    QDockWidget *dock;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /** Constructeur de classe.
      *
      */
    explicit MainWindow(QWidget *parent = 0);

    /** Destructeur de classe.
      *
      */
    ~MainWindow();

    QSemaphore semWaitMaquette;
    QSemaphore maquetteFinie;

    /** retourne un pointeur vers le SiMView contenant la simulation.
      * \return le SimView contenant la simulation.
      */
    SimView* getSimView();

    void setLocoState(LocoCtrl *loco,LocoCtrl::STATE state);

    QDockWidget *dockGeneralConsole;
    QTextEdit *generalConsole;
    StdRedirector<>* myRedirector;
    StdRedirector<>* myOtherRedirector;

    QDockWidget* inputDock;
    QLineEdit* inputWidget;

    void readSettings();
    void writeSettings() const;

    /** Charge et construit la maquette dont le nom est filename
      * \param filename le nom de la maquette à charger.
      */
    void chargerMaquette(QString filename);

    void createActions();
    void createMenus();
    void updateMenus();
    void createToolbar();


    enum {NOTSTARTED,RUNNING,PAUSE} m_state;
    int m_simStep;

    QAction *chargerMaquetteAct;
    QAction *exitAct;
    QAction *toggleSimAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *zoomFitAct;
    QAction *rotatePlusAct;
    QAction *rotateMinusAct;
    QAction *viewContactNumberAct;
    QAction *viewAiguillageNumberAct;
    QAction *viewLocoLogAct;
    QAction *viewInputAct;
    QAction *inertieAct;
    QAction *emergencyStopAct;
    QAction *printAct;

    QMenu *actionMenu;
    QToolBar *toolBar;

    QSignalMapper* locoSignalMapper;

    QList<LocoCtrl *> locoCtrls;

    QLabel *statusLabel;

signals:
    void commandSent(QString command);

private slots:
    void on_actionCharger_Maquette_triggered();

private:
    SimView *simView;
    QMap <int, QList<double>*> infosVoies;

public slots:
    void selectionMaquette(QString maquette);
    void addLoco(int no_loco);
    void closeEvent(QCloseEvent *event);
    void toggleSimulation();
    void emergencyStop();
    void simulationStep();
    void finishedAnimation();
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void rotatePlus();
    void rotateMinus();
    void viewContactNumber();
    void viewAiguillageNumber();
    void viewLocoLog();
    void toggleLoco(QObject *locoCtrls);
    void toggleInertie();
    void afficherMessage(QString message);
    void afficherMessageLoco(int numLoco,QString message);
    void print();
    void onReturnPressed();
};

#endif // MAINWINDOW_H
