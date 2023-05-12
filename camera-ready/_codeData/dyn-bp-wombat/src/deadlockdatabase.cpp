/*
 *  deadlockdatabase.cpp
 *  combus
 *
 *  Created by Adi Botea on 1/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "deadlockdatabase.h"
#include "util.h"

bool DeadlockDB::nodeMatchesRecord(const Node & node) {
    const RandomWriteState & state = node.getData();
    for (list<DeadlockRecord>::const_iterator it = m_db.begin();
         it != m_db.end(); it++) {
//    for (int it = 0; it < m_db.size(); it++) {
        if (state.matchesDeadlock(it->getCells())) {
            //cerr << "State matches deadlock record" << endl;
            //cerr << *it << endl;
            m_mostRecentHit = *it;
            return true;
        }
    }
    return false;
}
