/**
  \file mainwindows.h
  \author Yann Thoma
  \date 24.02.2017
  \brief Définition de l'interface graphique de l'application qui permet
  d'attaquer le hash md5 d'un mot de passe.


  Ce fichier contient la définition de la classe MainWindow. Cette classe est
  une interface graphique qui permet à l'utilisateur de proposer le hash md5
  d'un mot de passe à retrouver avec plusieurs threads.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QElapsedTimer>
#include <QTimer>
#include <QRegExp>
#include <QtCore>

#include "threadmanager.h"

namespace Ui {
class MainWindow;
}

/**
 * \brief The MainWindow class
 *
 * Cette classe est une interface graphique qui permet à l'utilisateur de
 * proposer le hash md5 d'un mot de passe à retrouver avec plusieurs threads.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:

    //! Caractères acceptés pour le mot de passe.
    static const QString validChars;

    //! Instance de l'interface graphique.
    Ui::MainWindow  *ui;

    //! Instance du thread "Manager" qui gère les autres threads responsable du
    //! hack du mot de passe stocké dans le hash md5.
    ThreadManager   *threadManager;

    //! Chronnomètre qui mesure le temps nécessaire au reverse du hash md5.
    QElapsedTimer   chronometer;

    //! Expression régulière pour valider les entrèes de l'utilisateur.
    QRegExp         hashValidationRegExp;

    //! Tâche non-bloquante qui exécute les threads responsables du hack.
    QFuture<QString>        hackingAsync;

    //! Utiliser pour signaler la fin de l'exécution des threads responsables
    //! du hack.
    QFutureWatcher<QString> hackingWatcher;

    //! La valeur de la barre de progression.
    double  progress;

    //! Flag qui indique si le programme a reversé le hash md5.
    bool    isHacking;

public:
    /**
     * \brief MainWindow Constructeur simple
     * \param parent Objet parent de l'interface
     */
    explicit MainWindow(QWidget *parent = 0);

signals:
    
public slots:
    /**
     * \brief prepareHacking Méthode qui récupère les paramètres entrés par
     * l'utilisateur pour reverser le hash md5.
     */
    void prepareHacking();
    /**
     * \brief endHacking Méthode pour cleaner et terminer le programme.
     */
    void endHacking();
    /**
     * \brief run Méthode qui incrémente la barre de progression affichée dans
     * l'interface. Cette barre de progression est partagée entre tous les
     * threads.
     */
    void incrementProgressBar(double percent);
};

#endif // MAINWINDOW_H
