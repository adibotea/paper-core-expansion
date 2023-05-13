/*
 *  deadlockrecord.h
 *  combus
 *
 *  Created by Adi Botea on 3/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DEADLOCK_RECORD_H_
#define _DEADLOCK_RECORD_H_

#include "cell.h"
#include "cellrecord.h"
#include <vector>

using namespace std;

class DeadlockRecord {
public:

    DeadlockRecord() {
    };

    ~DeadlockRecord() {
    };
    static void setStaticInfo(unsigned int nrRows, unsigned int nrCols);

    void addRecord(const CellRecord & record) {
        m_cells.push_back(record);
    };
    DeadlockRecord difference(const vector<Cell> & cellsToRemove) const;
    int operator==(const DeadlockRecord & record) const;

    static unsigned int getNrRows() {
        return m_nrRows;
    };

    static unsigned int getNrCols() {
        return m_nrCols;
    };

    const vector<CellRecord> & getCells() const {
        return m_cells;
    };

private:
    static unsigned int m_nrRows;
    static unsigned int m_nrCols;
    vector<CellRecord> m_cells;
};

#endif
