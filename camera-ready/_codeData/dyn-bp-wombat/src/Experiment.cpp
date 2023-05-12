/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Experiment.cpp
 * Author: adi
 * 
 * Created on 23 December 2017, 18:42
 */

//#include <c++/4.8.2/functional> // does not compile for me

#include "Experiment.h"
#include "parammanager.h"

#include "randomwritestate.h"
#include "deadlockrecord.h"
#include "util.h"
#include "depthfirstsearch.h"
#include "globals.h"
#include "limiteddiscrsearch.h"

Experiment::Experiment()
{
}

Experiment::Experiment(const Experiment &orig)
{
}

Experiment::~Experiment()
{
}

void Experiment::staticRootAnal()
{
    string inputfile = g_pm.getInputFileName();
    State::readStaticInfo(inputfile);
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    //state.printVariableInfo(cout);
    g_pm.setSlotScoring(5);
    g_pm.setHeurOrder(3);
    //g_pm.setVerbosity(2);
    //g_pm.setDBSize(0);
    g_pm.setPayloadPruning(false);
    g_pm.setMaxNonThemWords(14);
    g_pm.setPruneWithBPs(false);
    RandomWriteState state(1);

    bool legal = true;
    while (state.isLegal()) {
        Node node(state, 0, 0);
        vector<Node> successors = node.generateSuccessors(0, false, true);
        if (successors.size() == 1) {
            state = successors[0].getData();
            cerr << "Performed forced move in initial state:" << endl;
            cerr << state << endl;
        } else
        {
            if (successors.size() == 0)
                legal = false;
            break;
        }
    }
    if (!state.isLegal())
        legal = false;

    if (legal) {
        cerr << "LEGAL STATE:" << endl;
        state.writeToFilePzl("legal-" + inputfile);
    } else {
        cerr << "ILLEGAL STATE:" << endl;
        state.writeToFilePzl("dead-" + inputfile);
    }
    cerr << state << endl;

}

void Experiment::runDFS()
{
    string inputfile = g_pm.getInputFileName();
    State::readStaticInfo(inputfile);
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    //state.printVariableInfo(cout);
    g_pm.setSlotScoring(5);
    g_pm.setHeurOrder(3);
    //g_pm.setVerbosity(2);
    //g_pm.setDBSize(0);
    g_pm.setPayloadPruning(false);
    g_pm.setMaxNonThemWords(14);
    g_pm.setPruneWithBPs(false);
    //g_pm.setDelayedStaticAnal(true);
    //cerr << "Running DFS with pruning turned on" << endl;
    RandomWriteState state(1);
    Node root(state, 0, 0);
    DepthFirstSearch
        search(root,
               g_pm.getMaxTime(),
               g_pm.getMaxNodes(),
               1, //g_pm.getMaxSolutions(),
               g_pm.getSolFileName(),
               "summary.txt",
               g_pm.getVerbosity(),
               g_pm.getBackjump(),
               g_pm.getDBSize(),
               g_pm.getSolutionSampling(),
               g_pm.getNodeThresholdForRestart(),
               false, // false = use both thematic and non-thematic words
               g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs
    search.runSearch();
    if (search.getNrSolsFound() >=  0) {
        cerr << "STATE ALIVE:" << endl;
        state.writeToFilePzl("alive-0-" + inputfile);
        RandomWriteState ss = search.getBestPartSol().getData();
        ss.writeToFilePzl("alive-1-" + inputfile);
    } else {
        cerr << "DEAD STATE:" << endl;
        state.writeToFilePzl("dead-" + inputfile);
    }
}

void Experiment::runDFSCorner(int corner)
{
    g_pm.setCorner(corner);
    g_pm.setHeurOrder(3);
    g_pm.setVerbosity(1);
    g_pm.setDBSize(0);
    g_pm.setMaxNrBPMoves(0);
    g_pm.setPayloadPruning(false);
    g_pm.setSubarea(0, 13, 0, 13);
    g_pm.setMaxNonThemWords(2);
    g_pm.setPruneWithBPs(false);
    State::readStaticInfo("input-crosswords.pzl");
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    RandomWriteState state(1);
    //state.addCharacter(12, 0, 'a');
    Node root(state, 0, 0);
    //state.printVariableInfo(cout);
    //g_pm.setMaxNonThemWords(7);
    //g_pm.setDelayedStaticAnal(true);
    //cerr << "Running DFS with pruning turned on" << endl;
    DepthFirstSearch
        search(root,
               g_pm.getMaxTime(),
               -1, //g_pm.getMaxNodes(),
               -1,  //g_pm.getMaxSolutions(),
               g_pm.getSolFileName(),
               "summary.txt",
               g_pm.getVerbosity(),
               g_pm.getBackjump(),
               g_pm.getDBSize(),
               g_pm.getSolutionSampling(),
               g_pm.getNodeThresholdForRestart(),
               false, // false = use both thematic and non-thematic words
               g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs
    search.runSearch();
}

void Experiment::evalPayloadPruning()
{
    State::readStaticInfo("input-crosswords.pzl");
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    RandomWriteState state(1);
    Node root(state, 0, 0);
    //state.printVariableInfo(cout);
    g_pm.setVerbosity(0);
    g_pm.setDBSize(1400);
    g_pm.setPayloadPruning(true);
    g_pm.setMaxNonThemWords(4);
    //g_pm.setMaxNonThemWords(7);
    //g_pm.setDelayedStaticAnal(true);
    //cerr << "Running DFS with pruning turned on" << endl;
    DepthFirstSearch
        search(root,
               g_pm.getMaxTime(),
               g_pm.getMaxNodes(),
               1, //g_pm.getMaxSolutions(),
               g_pm.getSolFileName(),
               "summary.txt",
               g_pm.getVerbosity(),
               g_pm.getBackjump(),
               g_pm.getDBSize(),
               g_pm.getSolutionSampling(),
               g_pm.getNodeThresholdForRestart(),
               false, // false = use both thematic and non-thematic words
               true); //g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs
    search.runSearch();
}

void Experiment::runWithDetailedLog()
{
    State::readStaticInfo("input-crosswords.pzl");
    DeadlockRecord::setStaticInfo(State::getNrRows(), State::getNrCols());
    RandomWriteState state(1);
    Node root(state, 0, 0);
    //state.printVariableInfo(cout);
    g_pm.setVerbosity(3);
    g_pm.setDBSize(1400);
    g_pm.setPayloadPruning(true);
    //g_pm.setMaxNonThemWords(4);
    //g_pm.setMaxNonThemWords(7);
    //g_pm.setDelayedStaticAnal(true);
    //cerr << "Running DFS with pruning turned on" << endl;
    DepthFirstSearch
        search(root,
               g_pm.getMaxTime(),
               g_pm.getMaxNodes(),
               1, //g_pm.getMaxSolutions(),
               g_pm.getSolFileName(),
               "summary.txt",
               g_pm.getVerbosity(),
               g_pm.getBackjump(),
               g_pm.getDBSize(),
               g_pm.getSolutionSampling(),
               g_pm.getNodeThresholdForRestart(),
               false, // false = use both thematic and non-thematic words
               true); //g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs
    search.runSearch();
}
