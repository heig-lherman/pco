#ifndef MAQUETTEMANAGER_H
#define MAQUETTEMANAGER_H

#include <QString>
#include <QList>

/**
 * Descripteur de maquette.
 * Contient uniquement le nom de la maquette ainsi que le nom du fichier
 * correspondant.
 */
class MaquetteDesc
{
public:
    QString nomMaquette; //!> Nom de la maquette
    QString nomFichier; //!> Fichier contenant la maquette
};

/**
 * Gestionnaire des fichiers Maquette.
 * Il permet de charger la liste des maquettes disponibles, et de
 * récupérer le nom de fichier d'une maquette désignée par son nom.
 */
class MaquetteManager
{
public:
    MaquetteManager(){isCharge=false;};

    ~MaquetteManager();

    bool chargerListeMaquettes();

    QList<MaquetteDesc*> listeMaquettes() const;

    bool maquetteExiste(QString nomMaquette);
    QString fichierMaquette(QString nomMaquette);

    QStringList nomMaquettes();

    QString dossierMaquette();

private:
    bool isCharge;
    QList<MaquetteDesc*> maquettes;
};


#endif // MAQUETTEMANAGER_H
