/*
 *  untitled.h
 *  combus
 *
 *  Created by Adi Botea on 23/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PARAMMANAGER_H_
#define _PARAMMANAGER_H_

#include <iostream>

using namespace std;

class ParamManager {
public:
    ParamManager();
    ~ParamManager();
    bool readParams(int argc, char * const argv[]);
    int getDBSize() const {
        return m_dbSize;
    }
    int getHeurOrder() const {
        return this->m_heurOrder;
    }
    int getMaxNodes() const {
        return m_maxNodes;
    }
    double getMaxTime() const {
        return m_maxTime;
    }
    int getMaxSolutions() const {
        return m_maxSolutions;
    }
    string getInputFileName() const {
        return m_inputFileName;
    }
    string getSolFileName() const {
        return m_solFileName;
    }
    int getPropagIters() const {
        return m_progagIters;
    }
    bool getLookahead() const {
        return m_lookahead;
    }
    bool getSymmetryInfo() const {
        return m_symmetricGrid;
    }
    int getVerbosity() const {
        return m_verbosity;
    }
    int getMaxBranchingFactor() const {
        return m_maxBranchingFactor;
    }
    int getBackjump() const {
        return m_backjump;
    }
    int getPropagationOrdering() const {
        return m_propagationOrdering;
    }
    bool getSlotStatsFlag() const {
        return (bool) m_collectSlotStats;
    }
    int getSolutionSampling() const {
        return m_solutionSampling;
    }
    int getDicPercentage() const {
        return m_dicPercentage;
    }
    int getRandomSeed() const {
        return m_randomSeed;
    }
    int getNodeThresholdForRestart() const {
        return m_nodeThresholdForRestart;
    }
    int getMinNrThemPoints() const {
        return m_minThemPts;
    }
    int getMaxNrNonThemWords() const {
        return m_maxNonThemWords;
    }
    bool getPruneWithBPs() const {
        return this->m_pruneWithBPs;
    }
    void setPruneWithBPs(bool val) {
        this->m_pruneWithBPs = val;
    }
    void setVerbosity(int val) {
        this->m_verbosity = val;
    }
    void setDBSize(int val) {
        this->m_dbSize = val;
    }
    void setDelayedStaticAnal(bool val) {
        this->m_delayedStaticAnal = val;
    }
    bool getDelayedStaticAnal() const {
        return this->m_delayedStaticAnal;
    }
    void setMaxNonThemWords(int val) {
        this->m_maxNonThemWords = val;
    }
    void setPayloadPruning(bool val) {
    	this->m_payloadPruning = val;
    }
    bool getPayloadPruning() const {
    	return this->m_payloadPruning;
    }
    void setDicSearchStyle(int val) {
    	this->m_dicSearchStyle = val;
    }
    int getDicSearchStyle() const {
    	return this->m_dicSearchStyle;
    }
    void setHeurOrder(int val) {
        this->m_heurOrder = val;
    }
    void setSubarea(int col, int hlen, int row, int vlen) {
        this->m_startCol = col;
        this->m_horizLen = hlen;
        this->m_startRow = row;
        this->m_vertLen = vlen;
    }
    int getStartCol() const {
        return this->m_startCol;
    }
    int getStartRow() const {
        return this->m_startRow;
    }
    int getHorizLen() const {
        return this->m_horizLen;
    }
    int getVertLen() const {
        return this->m_vertLen;
    }
    int getCorner() const {
        return this->m_corner;
    }
    void setCorner(int val) {
        this->m_corner = val;
    }
    void setSlotScoring(int val) {
        this->m_slot_scoring = val;
    }
    int getSlotScoring() const {
        return this->m_slot_scoring;
    }
    void setMaxNrBPMoves(int val) {
        this->m_maxNrBPMoves = val;
    }
    int getMaxNrBPMoves() const {
        return this->m_maxNrBPMoves;
    }
    void setSearch(int val) {
        this->m_search = val;
    }
    int getSearch() const {
        return this->m_search;
    }

    void displayUsage() const;
private:
    string m_inputFileName;
    string m_solFileName;
    double m_maxTime;
    int m_maxNodes;
    int m_maxSolutions;
    int m_progagIters;
    bool m_symmetricGrid;
    int m_verbosity;
    int m_maxBranchingFactor;
    int m_backjump;
    bool m_collectSlotStats;
    bool m_lookahead;
    int m_dbSize;
    int m_propagationOrdering;
    int m_solutionSampling;
    int m_dicPercentage;
    int m_randomSeed;
    int m_nodeThresholdForRestart;
    int m_minThemPts; // min # of thematic points sought in a solution
    /*
     * Max number of non-thematic words of length 5 or more,
     * accepted in a solution.
     */
    int m_maxNonThemWords;
    /*
     * Prune with admissible estimates of future BPs
     */
    bool m_pruneWithBPs;
    /*
     * True iff we perform delayed static analysis
     */ 
    bool m_delayedStaticAnal;
    /*
     * If true, prune states with the payload ratio smaller than a threshold.
     * The payload ratio is the thematic score divided by the
     * sterile load.
     * The sterile load is the sum of letters contained in sterile
     * instantiated slots.
     */
    bool m_payloadPruning;
    /*
     * 0 = old technique, with intersections of vectors
     * 1 = new technique, with going once through the min-size vector
     */
    int m_dicSearchStyle;
    /*
     * Heuristic used to order successor nodes
    */
    int m_heurOrder;

    int m_startCol;
    int m_horizLen;
    int m_startRow;
    int m_vertLen;

    int m_corner;

    /*
    Max number of moves of the kind NO-WORD-ADDED
    */
    int m_maxNrBPMoves;

    /*
     * The style of computing the score of a slot.
    */
    int m_slot_scoring;

    int m_search;

    static constexpr double MAX_CPU_TIME = -1.0;
    static constexpr int MAX_DFS_NODES = -1;
    static const int MAX_DFS_SOLUTIONS = -1;
    static const int PROPAG_ITERS = 1;
    static const bool SYMMETRIC_GRID = false;
    static const int VERBOSITY = 0;
    static const int MAX_BRANCH_FACTOR = -1;
    static const int BACKJUMP = 0;
    static const bool COLLECT_SLOT_STATS = false;
    static const bool LOOKAHEAD = false;
    static const int CONFLICT_DB_SIZE = 1400;
    static const int PROPAG_ORDER = 0;
    static const int SOL_SAMPL = 0;
    static const int DIC_PERC = 100;
    static const int RANDOM_SEED = 1;
    static const int THRESHOLD_RESTART = -1;
    static const int MIN_THEM_POINTS = 0;
    static const int MAX_NONTHEM_WORDS = 500;
    static const bool PRUNE_WITH_BPS = true;
    static const bool DELAYED_STATIC_ANAL = false;
    static const bool PAYLOAD_PRUNING = false;
    static const int DIC_SEARCH_STYLE = 0;
    static const int HEUR_ORDER = 3;

    static const int START_ROW_COL = 0;
    static const int HORIZ_VERT_LEN = 13;
    static const int CORNER = 0;

    static const int SLOT_SCORING = 0;
    static const int MAX_NR_BP_MOVES = 100;
    static const int SEARCH = 5;
};

#endif
