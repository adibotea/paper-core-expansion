/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CrosswordsBoard.h
 * Author: adi
 *
 * Created on 23 December 2017, 19:02
 */

#ifndef CROSSWORDSBOARD_H
#define CROSSWORDSBOARD_H

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <sstream>
#include "move.h"
#include "wombat-src/depthfirstsearch.h"
#include "wombat-src/globals.h"

static const int GRID_WIDTH = 13;
static const int GRID_WIDTH_2 = 169;


const int MAX_NR_WORDS = 300;
const int MaxMoveNumber = GRID_WIDTH_2 * 2 * MAX_NR_WORDS; // MAX ID FOR THE NUMBER OF MOVES
const int MaxLegalMoves = MaxMoveNumber;
//const int MaxPlayoutLength = GRID_WIDTH * GRID_WIDTH; // Longest sequence of moves
const int MaxPlayoutLength = GRID_WIDTH * 4; // Longest sequence of moves, after this truncate or run exaustive search 

using namespace std;

class Board {
public:
    Move rollout [MaxPlayoutLength]; // current sequence of moves
    int length; // number of moves in the rollout
    //int board[GRID_WIDTH][GRID_WIDTH][2]; // = {0}; // optional

    int completed = 0; // whether the dfs has found a solution
    int partialScore = 0; // score of the grid returned by the DFS. 

    Board() {
        length = 0;
        m_state = RandomWriteState(1);

        // On the next line, replace the 2nd argument with false to switch off
        // pruning with admissible estimates for future BPs
	//	m_moves = m_state.generateMovesNrpa(true, g_pm.getPruneWithBPs());
	m_moves = m_state.generateMovesNrpa(true, g_pm.getPruneWithBPs());
        cerr << "In Board() -- nr of moves is " << m_moves.size() << endl;
        m_terminal = (m_moves.size() == 0);
	partialScore = m_state.getNrThematicPoints();
        //cerr << "In Board() -- setting m_terminal to " << m_terminal << endl;
    }

    // Code of the move

    int code(Move m) {
        int code = m.wordId +
                MAX_NR_WORDS * m.direction +
                MAX_NR_WORDS * 2 * m.slotX +
                MAX_NR_WORDS * 2 * GRID_WIDTH * m.slotY;
        return code;
    }

    int legalMoves(Move moves [MaxLegalMoves]) {
        assert(m_moves.size() < MaxLegalMoves);
        for (int i = 0; i < m_moves.size(); i++)
            moves[i] = m_moves[i];
        return m_moves.size();
    }

    void play(Move m) {
	assert(length < MaxPlayoutLength);
	rollout[length] = m;
	length++; // don't remove !
	m_state.makeMove(m.slotY, m.slotX, m.direction, m.word);
	assert (!m_state.isDeadlock());
	// On the next line, replace the 2nd argument with false to switch off
	// pruning with admissible estimates for future BPs
	m_moves = m_state.generateMovesNrpa(true, g_pm.getPruneWithBPs());
	m_terminal = (m_moves.size() == 0);
    }


    void completeBoard(int maxnodes, int timeout) {
      if(maxnodes > 0){

        if (true) {
	  cerr << "Current (partial) state in NRPA: " << endl;
	  cerr << "(States in NRPA have only thematic words) " << endl;
	  cerr << "Branching factor for the NRPA state is " << m_moves.size() << endl;
	  cerr << m_state << endl;
        }
	
        Node root(m_state, 0, 0);
        //m_state.printVariableInfo(cout);
        DepthFirstSearch search(root,
                timeout, //overrides g_pm.getMaxTime(),
		maxnodes, //overrides g_pm.getMaxNodes(),
                1, //overrides g_pm.getMaxSolutions(),
                g_pm.getSolFileName(),
                "summary.txt",
		0,//g_pm.getVerbosity(),
                g_pm.getBackjump(),
                g_pm.getDBSize(),
                g_pm.getSolutionSampling(),
                g_pm.getNodeThresholdForRestart(),
                false, // use both thematic and non-thematic words
                g_pm.getPruneWithBPs()); // use pruning with admissible estimates of future BPs 
	search.runSearch();	
	completed = search.getNrSolsFound() > 0;
	if(completed) partialScore = search.getBestSolScore(); //number of thematic points
      }
      m_terminal = true; 

    }

    double score() {
      int score;

      if( ! completed ) {
	  m_state.performStaticAnalysis();
	  partialScore = m_state.getNrThematicPoints();
      }
      
      switch(scoring){
      case 0:
	  if ( ! completed ) 
	    score = 0; 
	else
	    score = partialScore; 
	break;
      case 1: 
	if ( ! completed )
	    score = partialScore; 
	else
	    score = partialScore + 1000; 
	break;
      case 2:
	    score = partialScore; 
	    break;
      }
      return score;
    }


    bool terminal() {
	// Adi's code, commented out.
        // // if no more legal moves
        // if (false && numPoints > best_score) {
        //     cerr << "NEW (PARTIAL) STATE: " << endl;
        //     cerr << m_state << endl;
        // }
        // if (false && m_terminal) {
        //     cerr << "Current terminal state in NRPA: " << endl;
        //     cerr << "(States in NRPA have only thematic words) " << endl;
        //     cerr << "Branching factor for the NRPA state is " << m_moves.size() << endl;
        //     cerr << m_state << endl;
        //     cerr << "Attempting to fill it out with Depth-first search (DFS)... " << endl;
        //     cerr << "(DFS uses both thematic and non-thematic words)" << endl;
        //     int score2 = this->completeWithMixedWords();
        //     //if (score2 < 0)
        //     //    score2 = -100;
        //     //cerr << "NRPA score of partial state adjusted by " << score2 << endl;
        // }        
        // if (numPoints > best_score)
        //     best_score = numPoints;
        // return m_terminal;

        if(m_terminal){
	  int score = this->score();
	  if (score > best_score) {
	    static mutex output;
	    output.lock(); 
	    if (score > best_score) {
	      cerr << "NEW BEST SCORE: " << score << endl;
	      cerr << m_state << endl;
	      best_score = score;
	    }
	    output.unlock();

	    /* store grid */
	    if(minscore != -1 && score >= minscore ){
	      thread_local int board_id = 0;
	      std::thread::id tid = std::this_thread::get_id(); 
	      std::ostringstream namegtor;
	      namegtor<<"board_"<<score<<"_"<<tid<<"_"<<board_id++; 
	      ofstream file(namegtor.str());
	      file<<m_state<<endl;
	      file.close();
	    }
	  }
	  return true;
	}
	return false; 
    }

    void print(FILE *fp) {
        fprintf(fp, "nbMovesLeft = %d, length = %d\n", 0, length);
    }

    /*
     * Return nr thematic points if a complete grid is found
     * Return -1 otherwise
     */
    int completeWithMixedWords(int maxnodes, int timeout) {

    }


     static void setScoringFunction(int x){
       scoring = x;
     }

     static void setMinscore(int x){
       minscore = x; 
     }



private:
    RandomWriteState m_state;
    bool m_terminal;
    vector<Move> m_moves;
    static atomic_int best_score;
    static int scoring;
    static int minscore;
};


#endif /* CROSSWORDSBOARD_H */

