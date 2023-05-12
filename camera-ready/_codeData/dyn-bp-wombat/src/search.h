/*
 *  search.h
 *  combus
 *
 *  Created by Adi Botea on 2/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "node.h"
#include "stats.h"
#include "statscollection.h"

using namespace std;

class Search
{
public:
    Search();
    Search(const Node & root, double maxTime,
           int maxNodes, int maxSolutions, int verbosity);
    virtual ~Search();
    virtual void runSearch() = 0;
    const Node & getRoot() const {
        return m_root;
    }
protected:
    static const int NO_VALUE = -1;
    double getElapsedTime() const;
    void printSummary(ostream & os);
    void printStats(ostream & os);
    string getSummaryInfo() const;

    Node m_root;
    vector<Node> m_solutions;
    unsigned int m_expandedNodes;
    unsigned int m_allVisitedNodes;
    unsigned int m_visitedNodes;
    double m_maxTime;
    int m_maxNrNodes;
    time_t m_start;
    int m_nrSolutionsFound;
    int m_maxNrSolutions;
    int m_verbosity;
    StatsCollection m_bfStatsCol;
    StatsCollection m_timeStatsCol;
    Statistics m_depthStats;
    Statistics m_solDepthStats;
};

#endif
