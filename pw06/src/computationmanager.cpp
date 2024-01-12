//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteurs : Loïc Herman


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include "computationmanager.h"

#include <algorithm>
#include <experimental/list>

ComputationManager::ComputationManager(int maxQueueSize)
    : MAX_TOLERATED_QUEUE_SIZE(maxQueueSize)
{
}

int ComputationManager::requestComputation(Computation c)
{
    monitorIn();

    if (requests[c.computationType].size() >= MAX_TOLERATED_QUEUE_SIZE)
    {
        tryWait(queueFullCondition[c.computationType]);
    }

    const int id = nextId++;
    const auto req = Request(c, id);

    requests[c.computationType].push_back(req);
    results.emplace_back(id, std::optional<Result>{});

    signal(queueEmptyCondition[c.computationType]);

    monitorOut();
    return id;
}

void ComputationManager::abortComputation(int id)
{
    monitorIn();

    ComputationType type;
    bool foundRequest = false;
    for (auto& request : requests)
    {
        const auto& queue = request.second;
        const auto& reqIt = std::find_if(
            queue.begin(), queue.end(),
            [&](const auto& req) { return req.getId() == id; }
        );

        if (reqIt != queue.end())
        {
            type = request.first;
            foundRequest = true;
            requests[type].erase(reqIt);
            break;
        }
    }

    std::experimental::erase_if(
        results,
        [&](const auto& pair) { return pair.first == id; }
    );

    if (results.front().second.has_value())
    {
        // next value is available, signal it in case a thread is waiting on it
        signal(pendingResultCondition);
    }

    if (foundRequest)
    {
        // we have cleared one request so the queue is ready to accept one more
        signal(queueFullCondition[type]);
    }

    monitorOut();
}

Result ComputationManager::getNextResult()
{
    monitorIn();

    while (!stopped)
    {
        if (!results.empty())
        {
            const auto& [id, result] = results.front();
            if (result.has_value())
            {
                results.pop_front();
                monitorOut();
                return result.value();
            }
        }

        wait(pendingResultCondition);
    }

    // We should wake up other threads if they are also waiting
    signal(pendingResultCondition);
    monitorOut();
    throwStopException();
}

Request ComputationManager::getWork(ComputationType computationType)
{
    monitorIn();

    if (requests[computationType].empty())
    {
        tryWait(queueEmptyCondition[computationType]);
    }

    Request req = requests[computationType].front();
    requests[computationType].pop_front();

    signal(queueFullCondition[computationType]);

    monitorOut();
    return req;
}

bool ComputationManager::continueWork(int id)
{
    monitorIn();

    if (stopped)
    {
        monitorOut();
        return false;
    }

    const bool res = std::any_of(
        results.begin(),
        results.end(),
        [&](const auto& pair) { return pair.first == id; }
    );

    monitorOut();
    return res;
}

void ComputationManager::provideResult(Result result)
{
    monitorIn();

    const auto& it = std::find_if(
        results.begin(),
        results.end(),
        [&](const auto& pair) { return pair.first == result.getId(); }
    );

    if (it == results.end())
    {
        monitorOut();
        return;
    }

    it->second = result;
    signal(pendingResultCondition);

    monitorOut();
}

void ComputationManager::stop()
{
    monitorIn();

    stopped = true;

    std::for_each(
        queueFullCondition.begin(),
        queueFullCondition.end(),
        [this](auto& pair) { signal(pair.second); }
    );
    std::for_each(
        queueEmptyCondition.begin(),
        queueEmptyCondition.end(),
        [this](auto& pair) { signal(pair.second); }
    );
    signal(pendingResultCondition);

    monitorOut();
}

void ComputationManager::tryWait(Condition& condition)
{
    if (stopped)
    {
        monitorOut();
        throwStopException();
    }

    wait(condition);

    if (stopped)
    {
        // wake the next thread up
        signal(condition);
        monitorOut();
        throwStopException();
    }
}
