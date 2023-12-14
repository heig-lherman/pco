/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#ifndef SALONBARBERINTERFACE_H
#define SALONBARBERINTERFACE_H


class SalonBarberInterface
{
public:
    /*!
     * \brief getNbClient Permet de savoir le nombre de clients qui attendent
     * \return retourne le nombre de client dans la salle d'attente
     */
    virtual unsigned int getNbClient() = 0;

    /*!
     *  \brief goToSleep va envoyer le barbier dormir lorsqu'il n'y a pas
     *  de client dans son salon et qu'il a terminé d'embellir le dernier client
     */
    virtual void goToSleep() = 0;

    /*!
     *  \brief pickNextClient permet au barbier de prendre le client suivant
     */
    virtual void pickNextClient() = 0;

    /*!
     *  \brief waitClientAtChair permet au barbier d'attendre l'arrivée du client
     *  au siège de travail
     */
    virtual void waitClientAtChair() = 0;

    /*!
     *  \brief beautifyClient permet au barbier de réaliser sa tâche sur le client
     */
    virtual void beautifyClient() = 0;

    /*!
     * \brief isInService Permet de savoir si le salon est ouvert
     * \return retourne vrai si le salon est ouvert
     */
    virtual bool isInService() = 0;
};

#endif // SALONBARBERINTERFACE_H
