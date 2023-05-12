/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Experiment.h
 * Author: adi
 *
 * Created on 23 December 2017, 18:42
 */

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "randomwritestate.h"

class Experiment {
public:
    Experiment();
    Experiment(const Experiment& orig);
    virtual ~Experiment();

    void evalDelayedStaticAnal();
    void evalPruneWithBPs();
    void evalDeadlockDB();
    void evalILD();
    void runDFS();
    void staticRootAnal();
    void runDFSCorner(int corner);
    void runDFS100Times();
    /*
     * Payload pruning prunes away states whose ratio between the
     * thematic score and the sterile load is below a given threshold.
     * The sterile load is the total length of instantiated words from
     * the non-thematic dictionary.
     */
    void evalPayloadPruning();
    void evalDicSearch();
    void runWithDetailedLog();
private:
    void makeRndThemMoves(RandomWriteState & state, int nr_moves);
};

#endif /* EXPERIMENT_H */

