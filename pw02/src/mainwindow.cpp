#include <QMessageBox>
#include <QtConcurrent>

#include "mainwindow.h"
#include "threadmanager.h"

#include "ui_mainwindow.h"

const QString MainWindow::validChars = QString("abcdefghijklmnopqrstuvwxyz"
                                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                               "1234567890!$~*");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    threadManager(new ThreadManager(parent))
{
    /*
     * ui est l'instance de la classe représentant notre interface graphique
     */
    ui->setupUi(this);

    /*
     * On connecte les signaux sur les slots:
     *
     * - L'appui du bouton btnCrack déclenche notre méthode prepareHacking
     * - Le signal finished du hackingWatcher (indiquant que la fonction
     *   startHacking de notre threadManager
     *   a retourné) déclenche notre méthode endHacking
     * - Le signal sig_incrementPercentComputed du threadManager déclenche notre
     *   méthode incrementProgressBar.
     *   Ce signal transmet en paramètre un double
     */
    connect(
                ui->btnCrack,
                SIGNAL(clicked()),
                this,
                SLOT(prepareHacking()));
    connect(
                &hackingWatcher,
                SIGNAL(finished()),
                this,
                SLOT(endHacking()));
    connect(
                threadManager,
                SIGNAL(sig_incrementPercentComputed(double)),
                this,
                SLOT(incrementProgressBar(double)));

    /*
     * On prépare une expression régulière pour valider le hash
     */
    hashValidationRegExp.setPattern("\\b[0-9a-f]{32}\\b");
    hashValidationRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    hashValidationRegExp.setPatternSyntax(QRegExp::RegExp);

    isHacking = false;
}
/*
 * La fonction ci-dessous est exécutée à chaque appui sur le bouton "Crack!"
 */
void MainWindow::prepareHacking()
{
    /*
     * Un meme évènement sous QT est traité de manière séquentielle dans tous
     * les cas. Le test ci-dessous permet donc d'éviter qu'on lance deux groupes
     * de threads, en cliquant deux fois sur le bouton.
     */
    if (isHacking)
        return;

    QString inputError("");

    /*
     * Controle de saisie
     */
    if (!hashValidationRegExp.exactMatch(ui->inputHash->text()))
       inputError =    "Error: invalid hash. A MD5 hash is 32 chars long, with"
                        "chars in following set: 0 to 9 or a to f";

    if (ui->inputThreads->text().toInt() <= 0)
       inputError =     "Error: the number of threads must be greather than 0.";

    if (ui->inputNbChars->text().toInt() <= 0)
       inputError =     "Error: the password's number of characters must be"
                        "greather than 0.";

    if (inputError.length()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Erreur");
        msgBox.setText(inputError);
        msgBox.exec();
        return;
    }

    /*
     * Si le controle de saisie est passé, on met le flag isHacking à true et on
     * initialise la progression à zéro.
     */
    isHacking       = true;
    progress        = 0;

    /*
     * Désactivation des champs du formulaire
     */
    ui->btnCrack->setEnabled(false);
    ui->inputSalt->setEnabled(false);
    ui->inputHash->setEnabled(false);
    ui->inputNbChars->setEnabled(false);
    ui->inputThreads->setEnabled(false);

    /*
     * Démarrage du chronomètre
     */
    chronometer.start();
    /*
     * Appel de la fonction startHacking du threadManager de manière non
     * bloquante.
     */
    hackingAsync    =  QtConcurrent::run(
                threadManager,
                &ThreadManager::startHacking,
                validChars,
                ui->inputSalt->text(),
                ui->inputHash->text().toLower(),/* force le hash en minuscule */
                ui->inputNbChars->text().toInt(),
                ui->inputThreads->text().toInt());
    /*
     * Ajout d'un watcher permettant de signaler à MainWindow la fin de la tache
     * asynchrone
     */
    hackingWatcher.setFuture(hackingAsync);
}
/*
 * La fonction ci-dessous est exécutée à chaque réception d'un signal
 * incrementPercentComputed en provenance de threadManager
 */
void MainWindow::incrementProgressBar(double percent)
{
    progress += percent;
    ui->progressBar->setValue(100*progress);
}
/*
 * La fonction ci-dessous est exécutée à la réception réception d'un signal
 * finished en provenance du hackingWatcher, indiquant que la fonction
 * startHacking du hackingManger a retourné.
 */
void MainWindow::endHacking()
{
    QMessageBox msgBox;

    msgBox.setWindowTitle("Results");

    if (hackingAsync.result().length() > 0) {
        msgBox.setText("Found! \nPassword: "+ hackingAsync.result() +
                       "\nElapsed Time: "+
                       QString::number(chronometer.elapsed())+" ms");
    } else {
        msgBox.setText("Not found... \nElapsed time: " +
                       QString::number(chronometer.elapsed())+" ms");
    }

    /*
     * Affichage de la boite de dialogue. L'appel est bloquant et la
     * fenetre est modale.
     */
    msgBox.exec();

    ui->progressBar->setValue(0);
    ui->btnCrack->setEnabled(true);
    ui->inputSalt->setEnabled(true);
    ui->inputHash->setEnabled(true);
    ui->inputNbChars->setEnabled(true);
    ui->inputThreads->setEnabled(true);

    isHacking = false;
}
