#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include <pcosynchro/pcosemaphore.h>

#include "abstractbuffer.h"

template<typename T> class Buffer2ConsoSemaphore : public AbstractBuffer<T> {
protected:
    PcoSemaphore mutex{1};
    PcoSemaphore empty{2};
    PcoSemaphore full{0};
    T element;

public:
    Buffer2ConsoSemaphore() {}

    virtual ~Buffer2ConsoSemaphore() {}

    virtual void put(T item) {
        mutex.acquire();
        empty.acquire();
        empty.acquire();
        mutex.release();

        element = item;
 
        full.release();
        full.release();
    }

    virtual T get(void) {
        T item;
        full.acquire();
        item = element;
        empty.release();
        return item;
    }
};


#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcoconditionvariable.h>

template<typename T> class Buffer2ConsoMesa : public AbstractBuffer<T> {
protected:
    PcoMutex mutex;

public:
    Buffer2ConsoMesa() {}

    virtual ~Buffer2ConsoMesa() {}

    virtual void put(T item) {}
    virtual T get(void) {}
};


#include <pcosynchro/pcohoaremonitor.h>

template<typename T> class Buffer2ConsoHoare : public AbstractBuffer<T>, public PcoHoareMonitor {
protected:

public:
    Buffer2ConsoHoare() {}

    virtual ~Buffer2ConsoHoare() {}

    virtual void put(T item) {}
    virtual T get(void) {}
};


#endif // BUFFER2CONSO_H
