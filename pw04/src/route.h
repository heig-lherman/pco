// Authors: Loïc Herman
#ifndef QTRAINSIM_ROUTE_H
#define QTRAINSIM_ROUTE_H

#include <utility>
#include <vector>
#include "ctrain_handler.h"

/**
 * @brief The RailwayRoute class represents a route for a train, it serves
 *        as a way to have some utilities regarding train pathing and where
 *        the train should stop.
 */
struct RailwayRoute {
    using Contact = int;
    using SharedSection = std::pair<Contact, Contact>;
    using Junction = std::pair<Contact, int>;

    /**
     * @brief RailwayRoute Construct a RailwayRoute
     *
     * @param path The sequential list of contacts the train will go through
     * @param junctions The list of junctions that need to be turned for the train to go through the shared section
     * @param sharedSection The shared section start and end, must be within path
     * @param station The contact of the station
     * @param accessOffset The offset from the start of the shared section to place the access contact
     */
    explicit RailwayRoute(
            const std::vector<int> &path,
            const std::vector<Junction> &junctions,
            const SharedSection &sharedSection,
            Contact station,
            int accessOffset = 2
    );

    /**
     * @brief awaitAccess Wait for the train to reach the access contact
     */
    void awaitAccess() const;

    /**
     * @brief awaitLeave Wait for the train to reach the leave contact
     */
    void awaitLeave() const;

    /**
     * @brief awaitStation Wait for the train to reach the station contact
     */
    void awaitStation() const;

    /**
     * @brief turnJunctions Turn the shared section junctions
     *                      to the correct position
     */
    void turnJunctions() const;

private:
    std::vector<Junction> m_junctions;

    Contact m_access;
    Contact m_leave;
    Contact m_station;
};

#endif //QTRAINSIM_ROUTE_H
