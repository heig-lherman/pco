/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#ifndef SALON_H
#define SALON_H

#include "salonbarberinterface.h"
#include "salonclientinterface.h"

class Salon : public SalonBarberInterface, public SalonClientInterface
{
public:
    /*!
     * \brief endService Permet au barbier de fermer le salon
     */
    virtual void endService() = 0;
};

#endif // SALON_H
