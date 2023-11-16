// Implementation of class RailwayRoute
// Authors: Loïc Herman
#include "route.h"

#include <algorithm>
#include <utility>
#include <cassert>

RailwayRoute::RailwayRoute(
        const std::vector<int> &path,
        const std::vector<Junction> &junctions,
        const SharedSection &sharedSection,
        Contact station,
        int accessOffset
): m_junctions(junctions), m_station(station) {
    // Find the access and leave points in the path
    const auto sectionStart = std::find(path.begin(), path.end(), sharedSection.first);
    const auto sectionEnd = std::find(path.begin(), path.end(), sharedSection.second);
    assert(sectionStart != path.end() && sectionEnd != path.end());

    // Place the access request point 3 points before the start of the section,
    // to make sure the train has room to stop.
    m_access = *std::prev(sectionStart, accessOffset);
    m_leave = *std::next(sectionEnd, 1);
}

void RailwayRoute::awaitAccess() const {
    attendre_contact(m_access);
}

void RailwayRoute::awaitLeave() const {
    attendre_contact(m_leave);
}

void RailwayRoute::awaitStation() const {
    attendre_contact(m_station);
}

void RailwayRoute::turnJunctions() const {
    for (const auto &junction : m_junctions) {
        diriger_aiguillage(junction.first, junction.second, 0);
    }
}
