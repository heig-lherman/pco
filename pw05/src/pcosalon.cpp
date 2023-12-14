/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 * Authors: Loïc Herman
 */
// Modifications à faire dans le fichier

#include "pcosalon.h"

#include <pcosynchro/pcothread.h>

#include <iostream>

PcoSalon::PcoSalon(GraphicSalonInterface* interface, unsigned int capacity)
    : _interface(interface),
      _capacity(capacity),
      _chairCount(0),
      _clientSeated(false),
      _inService(true)
{
}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
bool PcoSalon::accessSalon(unsigned clientId)
{
    _mutex.lock();
    if (_clients.size() > _capacity)
    {
        // The salon is full
        _mutex.unlock();
        return false;
    }

    // Track the client in the queue
    _clients.push(clientId);

    animationClientAccessEntrance(clientId);

    // notify the barber that a client is waiting
    _wakeUp.notifyOne();

    if (_clients.front() != clientId)
    {
        // There is already a client in the salon, sit down on a chair
        const unsigned chair = _chairCount++;
        if (_chairCount >= _capacity)
            _chairCount = 0;

        animationClientSitOnChair(clientId, chair);
    }

    _mutex.unlock();
    return true;
}

void PcoSalon::goForHairCut(unsigned clientId)
{
    _mutex.lock();

    // Wait for the chair to be empty, the barber will call us
    while (_clients.front() != clientId)
        _chairEmpty.wait(&_mutex);

    _interface->consoleAppendTextClient(clientId, "[WORK] goto workchair");
    animationClientSitOnWorkChair(clientId);
    // notify the barber we are ready
    _clientSeated = true;
    _clientReady.notifyAll();

    // Wait for the barber to finish
    _haircut.wait(&_mutex);

    _mutex.unlock();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId)
{
    _mutex.lock();
    animationClientWaitForHairToGrow(clientId);
    _mutex.unlock();
}

void PcoSalon::walkAround(unsigned clientId)
{
    _mutex.lock();
    animationClientWalkAround(clientId);
    _mutex.unlock();
}

void PcoSalon::goHome(unsigned clientId)
{
    _mutex.lock();
    animationClientGoHome(clientId);
    _mutex.unlock();
}

/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
unsigned int PcoSalon::getNbClient()
{
    _mutex.lock();
    const unsigned res = _clients.size();
    _mutex.unlock();
    return res;
}

void PcoSalon::goToSleep()
{
    _mutex.lock();
    animationBarberGoToSleep();

    // Wait for a client to wake the barber up
    _wakeUp.wait(&_mutex);

    animationWakeUpBarber();
    _mutex.unlock();
}

void PcoSalon::pickNextClient()
{
    _mutex.lock();
    // We will pick the next client but we need to notify all pending clients as
    // the order of the wait can be random
    _interface->consoleAppendTextBarber(QString("[PICK] next client %1").arg(_clients.front()));
    _chairEmpty.notifyAll();
    _mutex.unlock();
}

void PcoSalon::waitClientAtChair()
{
    _mutex.lock();
    // We will wait till the client is seated
    while (!_clientSeated)
        _clientReady.wait(&_mutex);

    _clientSeated = false;
    _mutex.unlock();
}

void PcoSalon::beautifyClient()
{
    _mutex.lock();

    animationBarberCuttingHair();

    // let the client go and remove them from the queue
    _clients.pop();
    _haircut.notifyOne();

    _mutex.unlock();
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService()
{
    _mutex.lock();
    const bool res = _inService;
    _mutex.unlock();
    return res;
}

void PcoSalon::endService()
{
    _mutex.lock();
    _inService = false;
    _wakeUp.notifyOne();
    _mutex.unlock();
}

/********************************************
 *   Méthodes privées pour les animations   *
 *******************************************/
void PcoSalon::animationClientAccessEntrance(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientAccessEntrance(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnChair(unsigned clientId, unsigned clientSitNb)
{
    _mutex.unlock();
    _interface->clientSitOnChair(clientId, clientSitNb);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnWorkChair(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientSitOnWorkChair(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientWaitForHairToGrow(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, true);
    _mutex.lock();
}

void PcoSalon::animationClientWalkAround(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWalkAround(clientId);
    _mutex.lock();
}

void PcoSalon::animationBarberGoToSleep()
{
    _mutex.unlock();
    _interface->barberGoToSleep();
    _mutex.lock();
}

void PcoSalon::animationWakeUpBarber()
{
    _mutex.unlock();
    _interface->clientWakeUpBarber();
    _mutex.lock();
}

void PcoSalon::animationBarberCuttingHair()
{
    _mutex.unlock();
    _interface->barberCuttingHair();
    _mutex.lock();
}

void PcoSalon::animationClientGoHome(unsigned clientId){
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, false);
    _mutex.lock();
}
