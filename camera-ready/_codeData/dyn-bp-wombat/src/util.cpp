/*
 *  util.cpp
 *  combus
 *
 *  Created by Adi Botea on 7/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


#include "util.h"
#include "colors.h"
#include <map>

ostream & operator << (ostream &os, const State & state)
{
    os << state.getNrRows() << ", " << state.getNrCols() << endl;
    os << "Selected slot for DFS expansion: " << state.selPatAsString() << endl;
    os << "Top three slots to generate NRPA moves: ";
    os << state.getTopSlots4NrpaMoves(3) << endl;
    //os << "NRPA score: " << state.getNrpaScore() << endl;
    os << "Nr BP blocks: " << state.getNrBPBlocks() << endl;
    os << "Filled cells: " << state.getNrFilledCells() << endl;
    os << "Thematic letters: " << state.getNrThematicPoints();
    if (state.getNrFilledCells() > 0) {
        os << " ( ";
        os << 0.5*100*state.getNrThematicPoints()/state.getNrFilledCells();
        os << " % of the area filled out )";
    }
    os << endl;
    //os << "Max future points: " << state.getMaxFuturePoints() << endl;
    //os << "Optimistic max score: " <<
    //        state.getNrThematicPoints() + state.getMaxFuturePoints() << endl;
    int empty_cells = state.getNrCols()*state.getNrCols() - state.getNrFilledCells();
    os << "Empty cells: " << empty_cells << endl;
    os << "Black points: " << state.getNrBPs();
    if (state.getNrFilledCells() > 0) {
        os << " ( ";
        os << 100*state.getNrBPs()/state.getNrFilledCells();
        os << " % of the area filled out )";
    }
    os << endl;
    //os << "Score style: " << State::getNrpaScoreKind() << endl;
    for (int j = 0; j <= state.getNrCols() + 1; j++) {
        os << ". ";
    }
    os << endl;
    for (int i = 0; i < state.getNrRows(); i++) {
        string row = state.getRow(i);
        os << ". ";
        for (int k = 0; k < row.length(); k++) {
            if (row[k] == '@')
                os << red << (char) row[k] << reset << " ";
            else
                os << (char) toupper(row[k]) << " ";
        }
        os << "." << endl;
    }
    for (int j = 0; j <= state.getNrCols() + 1; j++) {
        os << ". ";
    }
    return os;
}
  
bool operator== (const WordSlot &s1, const WordSlot &s2)
{
    return s1.getSimplePattern() == s2.getSimplePattern();
}

bool operator== (WordSlot &s1, WordSlot &s2)
{
    return s1.getSimplePattern() == s2.getSimplePattern();
}

ostream & operator << (ostream &os, const Node & node)
{
    os << endl << "Depth: " << node.getDepth() << endl;
    os << "Path info: " << node.getPathInfo() << endl;
    os << node.getData() << endl;
    return os;
}

ostream & operator << (ostream &os, const Statistics & stats)
{
    os << " " << stats.getName() << " ";
    os << stats.getVolume() << " ";
    os << stats.getMin() << " " << stats.getMax() << " ";
    os << stats.getAverage() << " " << stats.getVariance();
    return os;
}

ostream & operator << (ostream &os, Statistics & stats)
{
    os << " " << stats.getName() << " ";
    os << stats.getVolume() << " ";
    os << stats.getMin() << " " << stats.getMax() << " ";
    os << stats.getAverage() << " " << stats.getVariance() << " ";
    return os;
}

ostream & operator << (ostream &os, const StatsCollection & col)
{
    const map<int, Statistics> & map = col.getMap();
    os << map.size() << " ";
    std::map<int, Statistics>::const_iterator iter;
    for (iter=map.begin(); iter != map.end(); ++iter) {
        os << "[ " << iter->first << " : " << iter->second << " ] ";
    }
    os << " ";
    return os;
}

ostream & operator << (ostream &os, const DeadlockRecord & record)
{
    char grid[50][50];
    for (int row = 0; row < DeadlockRecord::getNrRows(); row++) {
        for (int col = 0; col < DeadlockRecord::getNrCols(); col++) {
            grid[row][col] = ' ';
        }
    }
    const vector<CellRecord> & cells = record.getCells();
    for (vector<CellRecord>::const_iterator it = cells.begin();
         it != cells.end(); it++) {
        grid[it->m_row][it->m_col] = it->m_letter;
    }
    for (int row = 0; row < DeadlockRecord::getNrRows(); row++) {
        for (int col = 0; col < DeadlockRecord::getNrCols(); col++) {
            os << grid[row][col] << ' ';
        }
        os << endl;
    }
    return os;
}
