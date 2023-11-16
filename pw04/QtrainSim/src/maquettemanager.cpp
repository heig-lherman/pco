
#include <QStringList>
#include <QDir>
#include <QApplication>

#include "maquettemanager.h"
#include "general.h"


MaquetteManager::~MaquetteManager()
{
    while (!maquettes.isEmpty()) {
        MaquetteDesc *desc = maquettes.at(0);
        delete desc;
        maquettes.pop_front();
    }
}

QList<MaquetteDesc*> MaquetteManager::listeMaquettes() const
{
    return maquettes;
}

bool MaquetteManager::maquetteExiste(QString nomMaquette)
{
    if (!isCharge)
        chargerListeMaquettes();
    foreach (MaquetteDesc *desc,maquettes)
    {
        if (desc->nomMaquette.compare(nomMaquette)==0)
            return true;
    }
    return false;
}

QStringList MaquetteManager::nomMaquettes()
{
    QStringList res;
    if (!isCharge)
        chargerListeMaquettes();
    foreach (MaquetteDesc *desc,maquettes)
        res << desc->nomMaquette;
    return res;
}

QString MaquetteManager::fichierMaquette(QString nomMaquette)
{
    if (!isCharge)
        chargerListeMaquettes();
    foreach (MaquetteDesc *desc,maquettes)
    {
        if (desc->nomMaquette.compare(nomMaquette)==0)
            return desc->nomFichier;
    }
    return "";
}

QString MaquetteManager::dossierMaquette()
{
    QDir dir(MAQUETTE_DIR);
    return dir.absolutePath();
}

bool MaquetteManager::chargerListeMaquettes()
{
    QDir dir(MAQUETTE_DIR);
    dir.setFilter(QDir::Files);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        MaquetteDesc *desc=new MaquetteDesc();
        desc->nomFichier=fileInfo.absoluteFilePath();
        QString s=fileInfo.fileName();
        if (s.indexOf("Maquet_")==0)
            s=s.right(s.length()-7);
        s=s.left(s.length()-4);
        desc->nomMaquette=s;
        maquettes << desc;
//        std::cout << "Maquette. Nom: " << qPrintable(desc->nomMaquette) << ". Fichier: " << qPrintable(desc->nomFichier) << std::endl;
    }
    if (maquettes.size()==0)
        return false;
    isCharge=true;
    return true;
}
