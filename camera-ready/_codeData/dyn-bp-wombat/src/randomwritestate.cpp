/*
 *  randomwritestate.cpp
 *  combus
 *
 *  Created by Adi Botea on 7/10/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <queue>
#include <algorithm>
#include "randomwritestate.h"
#include "wordslot.h"
#include "constants.h"
#include "parammanager.h"
#include "globals.h"
#include "util.h"

using namespace std;

RandomWriteState::RandomWriteState() :
		State(), m_nrAllSuccessors(0), m_nrBPMoves(0) {
	staticAnalAndTest(0, g_pm.getPruneWithBPs());
}

RandomWriteState::RandomWriteState(int iters) :
		State(iters), m_nrAllSuccessors(0), m_nrBPMoves(0) {
	staticAnalAndTest(0, g_pm.getPruneWithBPs());
}

RandomWriteState::~RandomWriteState() {
}

vector<RandomWriteState> RandomWriteState::generateSuccessors(int bestSolScore,
		bool only_them_words, bool prune_with_bps) {
	vector<RandomWriteState> result;
	if (false && only_them_words) {
		//assert(false);
		for (int it = 0; it < this->m_themSlotOrder.size(); it++) {
			int idx = this->m_themSlotOrder[it];
			vector<RandomWriteState> tr = genSuccsAL(m_slots[idx], bestSolScore,
					only_them_words, prune_with_bps);
			result.insert(result.end(), tr.begin(), tr.end());
			//if (result.size() >= 20)
			//	break;
		}
		return result;
	} else {
		if (this->m_idxSelSlotGen == -1) {
			return result;
		} else {
			return this->genSuccsAL2(m_slots[this->m_idxSelSlotGen], this->m_posInSelSlot,
					bestSolScore, only_them_words, prune_with_bps);
		}
	}
}

vector<RandomWriteState> RandomWriteState::genSuccsAL2(const WordSlot & sel_slot, int sel_pos,
		int bestSolScore, bool only_them_words, bool prune_with_bps) {
	//assert(!only_them_words);
	m_nrAllSuccessors = 0;
	int counter = 0;
	vector<RandomWriteState> result;
	vector<WordSlot> subslots = sel_slot.getSubslotsAroundPos(sel_pos);
	for (int slot_idx = 0; slot_idx < subslots.size(); slot_idx++) {
		const WordSlot & slot = subslots[slot_idx];
		if (slot.getLength() <= 2 && !only_them_words) {
			if (this->m_nrBPMoves >= g_pm.getMaxNrBPMoves())
				continue;
			RandomWriteState succ;
			bool success = this->makeBPOnlySucc(slot, bestSolScore, prune_with_bps, succ);
			if (success)
				result.push_back(succ);
			continue;
		}
		const WordPattern pat = slot.getPattern();
		assert(
				slot.getDirection() == HORIZONTAL || slot.getDirection() == VERTICAL);
		vector<Dictionary> & dics =
				(slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
		this->m_skipDeadlockExtraction = false;
		// Generate words of variable lengths within this slot.
		// TODO: Currently inefficient because we don't check *early enough*
		// if some of these would introduce adjacent black points
		int len = pat.getLength();
		WordPattern this_pat = pat.getPrefix(len);
		for (vector<Dictionary>::const_iterator it1 = dics.begin();
				it1 != dics.end(); it1++) {
			if (only_them_words && !it1->isThematic())
				continue;
			if (this->m_nrNonThemWords >= g_pm.getMaxNrNonThemWords()
					&& len > MIN_LENGTH_NTW && !it1->isThematic())
				continue;
			if (false) {
				if (!it1->isThematic() && len == 2 && this->m_nrSlots[2] >= 10) {
					//cerr << "Exceeding slots of length 2: " << endl;
					//cerr << *this << endl;
					continue;
				}
				if (!it1->isThematic() && len == 3 && this->m_nrSlots[3] >= 10) {
					//cerr << "Exceeding slots of length 3: " << endl;
					//cerr << *this << endl;
					continue;
				}
				if (!it1->isThematic() && len == 4 && this->m_nrSlots[4] >= 5) {
					//cerr << "Exceeding slots of length 4: " << endl;
					//cerr << *this << endl;
					continue;
				}
				if (!it1->isThematic() && len == 5 && this->m_nrSlots[5] >= 4) {
					//cerr << "Exceeding slots of length 5: " << endl;
					//cerr << *this << endl;
					continue;
				}
			}
			vector<string> words = it1->expandPattern(this_pat);
			for (vector<string>::iterator it2 = words.begin();
					it2 != words.end(); it2++) {
				counter++;
				if (g_pm.getMaxBranchingFactor() >= 0
						&& counter > g_pm.getMaxBranchingFactor())
					break;
				RandomWriteState state = *this;
				state.setPropagFromScratch(true);
				state.makeMove(slot, *it2);
				if (!g_pm.getDelayedStaticAnal())
					if (!state.staticAnalAndTest(bestSolScore, prune_with_bps))
						continue;
				result.push_back(state);
				if (g_pm.getVerbosity() >= 3) {
					cerr << "ADDING SUCCESSOR " << endl;
					cerr << state << endl;
				}
			}
		}
	}
	if (result.size() > 0) {
		//cerr << "Before sorting" << endl;
		//for (auto xx : result) {
		//	cerr << xx << endl;
		//}
		sort(result.begin(), result.end());
		//cerr << "After sorting" << endl;
		//for (auto xx : result) {
		//	cerr << xx << endl;
		//}
		reverse(result.begin(), result.end());
		//cerr << "After reverting" << endl;
		//for (auto xx : result) {
		//	cerr << xx << endl;
		//}
		//cerr << "cucu" << endl;
	}
	return result;
}

bool RandomWriteState::makeBPOnlySucc(const WordSlot & slot, int bestSolScore,
		bool prune_with_bps,
		RandomWriteState & result) {
	assert (slot.getLength() <= 2);
	int row = slot.getRow();
	int col = slot.getCol();
	if (slot.getDirection() == HORIZONTAL)
		col += slot.getLength();
	else
		row += slot.getLength();
	if (row >= this->m_nrRows || col >= this->m_nrCols)
		return false;
	if (!this->isBPLegal(row, col))
		return false;
	result = *this;
	result.setPropagFromScratch(true);
	result.addBlackPoint(row, col);
	result.setLastWord("NO-WORD-ADDED");
	result.m_nrBPMoves++;
	result.collectSlots();
	if (!g_pm.getDelayedStaticAnal()) {
		if (!result.staticAnalAndTest(bestSolScore, prune_with_bps))
			return false;
	}
	return true;
}

vector<RandomWriteState> RandomWriteState::genSuccsAL(const WordSlot & slot,
		int bestSolScore, bool only_them_words, bool prune_with_bps) {
	//assert(!only_them_words);
	m_nrAllSuccessors = 0;
	int counter = 0;
	vector<RandomWriteState> result;
	const WordPattern & pat = slot.getPattern();
	assert(
			slot.getDirection() == HORIZONTAL || slot.getDirection() == VERTICAL);
	vector<Dictionary> & dics =
			(slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
	this->m_skipDeadlockExtraction = false;
	// Generate words of variable lengths within this slot.
	// TODO: Currently inefficient because we don't check *early enough*
	// if some of these would introduce adjacent black points
	if (slot.getAllowBPs() && pat.getLength() > 2) {
		if (!only_them_words) {
			for (int p = 0; p <= 2; p++)
				if (pat.getSimplePattern()[p] == BLANK) {
					int row = slot.getRow();
					int col = slot.getCol();
					if (slot.getDirection() == HORIZONTAL)
						col += p;
					else
						row += p;
					if (!this->isBPLegal(row, col))
						continue;
					RandomWriteState state = *this;
					state.setPropagFromScratch(true);
					state.addBlackPoint(row, col);
					state.setLastWord("NO-WORD-ADDED");
					state.collectSlots();
					if (!g_pm.getDelayedStaticAnal()) {
						if (!state.staticAnalAndTest(bestSolScore, prune_with_bps))
							continue;
					}
					result.push_back(state);
					if (g_pm.getVerbosity() >= 3) {
						cerr << "ADDING BLACK POINT SUCCESSOR "
								<< slot.getSimplePattern() << endl;
						cerr << state << endl;
					}
				}
		}
	}
	int min_len = (this->getNrBPs() == MAX_BPS) ? pat.getLength() : 3;
#ifdef NO_BPS
	min_len = pat.getLength();
#endif
	if (!slot.getAllowBPs())
		min_len = pat.getLength();
	for (unsigned int len = min_len; len <= pat.getLength(); len++) {
		if (len < pat.getLength()) {
			int bpc = slot.getCol();
			int bpr = slot.getRow();
			if (slot.getDirection() == HORIZONTAL)
				bpc += len;
			else
				bpr += len;
			if (!this->isBPLegal(bpr, bpc))
				continue;
		}
		WordPattern this_pat = pat.getPrefix(len);
		for (vector<Dictionary>::const_iterator it1 = dics.begin();
				it1 != dics.end(); it1++) {
			if (only_them_words && !it1->isThematic())
				continue;
			if (this->m_nrNonThemWords >= g_pm.getMaxNrNonThemWords()
					&& len >= MIN_LENGTH_NTW && !it1->isThematic())
				continue;
			if (!it1->isThematic() && len == 2 && this->m_nrSlots[2] >= 10) {
				//cerr << "Exceeding slots of length 2: " << endl;
				//cerr << *this << endl;
				continue;
			}
			if (!it1->isThematic() && len == 3 && this->m_nrSlots[3] >= 10) {
				//cerr << "Exceeding slots of length 3: " << endl;
				//cerr << *this << endl;
				continue;
			}
			if (!it1->isThematic() && len == 4 && this->m_nrSlots[4] >= 5) {
				//cerr << "Exceeding slots of length 4: " << endl;
				//cerr << *this << endl;
				continue;
			}
			if (!it1->isThematic() && len == 5 && this->m_nrSlots[5] >= 4) {
				//cerr << "Exceeding slots of length 5: " << endl;
				//cerr << *this << endl;
				continue;
			}
			vector<string> words = it1->expandPattern(this_pat);
			for (vector<string>::iterator it2 = words.begin();
					it2 != words.end(); it2++) {
				counter++;
				if (g_pm.getMaxBranchingFactor() >= 0
						&& counter > g_pm.getMaxBranchingFactor())
					break;
				RandomWriteState state = *this;
				state.setPropagFromScratch(true);
				state.makeMove(slot, *it2);
				if (!g_pm.getDelayedStaticAnal())
					if (!state.staticAnalAndTest(bestSolScore, prune_with_bps))
						continue;
				result.push_back(state);
				if (g_pm.getVerbosity() >= 3) {
					cerr << "ADDING SUCCESSOR " << endl;
					cerr << state << endl;
				}
			}
		}
	}
	if (result.size() > 0) {
		sort(result.begin(), result.end());
		reverse(result.begin(), result.end());
	}
	return result;
}

bool RandomWriteState::staticAnalAndTest(double bestSolScore,
		bool prune_with_bps) {
	this->setPropagFromScratch(true);
	this->performStaticAnalysis();
	if (!this->isPromising())
		return false;
	if (!this->isLegal()) {
		return false;
	}
	if (this->isDeadlock()) {
		//cerr << "Successor deadlocked ( move ";
		//cerr << *it2 << " )" << endl;
		return false;
	}
	if (this->cannotReachThemPoints(bestSolScore) && g_pm.getCorner() == 0)
		return false;
	if (prune_with_bps && this->willExceedBPs())
		return false;
	return true;
}

int RandomWriteState::computeHeurBPsSlow() const {
	int horiz_result = 0;
	int vert_result = 0;
	//cerr << "ENTER METHOD comptueHeurBPs" << endl;
	for (int it = 0; it < this->m_slots.size(); it++) {
		const WordSlot & slot = m_slots[it];
		const WordPattern & pattern = slot.getPattern();
		//cerr << "Slot with index " << it << endl;
		int len = slot.getLength();
		if (len < 3)
			continue;
		if (pattern.isInstantiated())
			continue;
		bool slot_has_word = false;
		vector<Dictionary> & dics =
				(slot.getDirection() == HORIZONTAL) ?
						(m_horizDics) : (m_vertDics);
		for (vector<Dictionary>::const_iterator it1 = dics.begin();
				it1 != dics.end(); it1++) {
			if (slot_has_word)
				break;
			if (this->m_nrNonThemWords >= g_pm.getMaxNrNonThemWords()
					&& len >= MIN_LENGTH_NTW && !it1->isThematic())
				continue;
			vector<string> words = it1->expandPattern(pattern);
			int iter = 0;
			if (words.size() > 0) {
				slot_has_word = true;
				continue;
			}
			for (vector<string>::iterator it2 = words.begin();
					it2 != words.end(); it2++) {
				//cerr << "Word " << *it2;
				//cerr << " -- " << iter++ << " out of " << words.size() << endl;
				RandomWriteState state = *this;
				state.setPropagFromScratch(true);
				state.makeMove(slot, *it2);
				if (!state.isPromising())
					continue;
				if (!state.isLegal()) {
					continue;
				}
				if (state.isDeadlock()) {
					continue;
				}
				slot_has_word = true;
				break; // we found a word matching this slot
			}
		}
		if (!slot_has_word && slot.getDirection() == HORIZONTAL) {
			//cerr << "Horizontal slot with no matches: " << slot.getSimplePattern() << endl;
			horiz_result++; // no word matches this slot
		}
		if (!slot_has_word && slot.getDirection() == VERTICAL) {
			//cerr << "Vertical slot with no matches: " << slot.getSimplePattern() << endl;
			vert_result++; // no word matches this slot
		}
	}
	return (horiz_result > vert_result) ? (horiz_result) : (vert_result);
}

bool RandomWriteState::isPromising() const {
	if (!g_pm.getPayloadPruning())
		return true;
	bool result = true;
	if (this->getNrFilledCells() > 30) {
		bool condition = (getNrThematicPoints() < 1.05*computeSterileLoad());
		if (condition) {
			result = false;
			//cerr << "Successor pruned away as not promising" << endl;
			//cerr << *this;
			//cerr << endl;
		}
	}
	if (false && this->getNrFilledCells() > 60) {
		bool cond2 = 1.0*this->getNrBPs()/this->getNrFilledCells() > 0.2;
		if (cond2) {
			result = false;
			cerr << "Successor pruned away as not promising" << endl;
			cerr << *this;
			cerr << endl;
		}
	}
	return result;
}

bool RandomWriteState::isConnected() const {
	BooleanMap bm;
	queue<Cell> q;
	for (int col = 0; col < this->m_nrCols; col++) {
		if (m_grid[0][col] != BLACKPOINT) {
			q.push(Cell(0, col));
			bm.setValue(0, col, true);
			break;
		}
	}
	assert(!q.empty());
	while (!q.empty()) {
		Cell c = q.front();
		q.pop();
		assert(bm.getValue(c.m_row, c.m_col));
		for (int co = -1; co <= 1; co++) {
			for (int ro = -1; ro <= 1; ro++) {
				if (co != 0 && ro != 0)
					continue;
				if (co == 0 && ro == 0)
					continue;
				int col = c.m_col + co;
				int row = c.m_row + ro;
				if (row < 0 || row >= this->m_nrRows)
					continue;
				if (col < 0 || col >= this->m_nrCols)
					continue;
				if (m_grid[row][col] != BLACKPOINT && !bm.getValue(row, col)) {
					q.push(Cell(row, col));
					bm.setValue(row, col, true);
				}
			}
		}
	}
	bool result = (bm.getNrTrueValues() == m_nrRows * m_nrCols - getNrBPs());
	return result;
}

bool RandomWriteState::hasIllegalWords() const {
	for (vector<WordSlot>::const_iterator it = m_slots.begin();
			it != m_slots.end(); it++) {
		if (!it->getPattern().isInstantiated())
			continue;
		if (it->getLength() < 3)
			continue;
		if (countMatchingWords(*it) == 0) {
			if (g_pm.getVerbosity() >= 3)
				cerr << "Illegal word in slot: " << it->getSimplePattern()
						<< endl;
			return true;
		}
	}
	return false;
}

bool RandomWriteState::cannotReachThemPoints(int score) const {
	bool result = (this->getMaxOptScore() < score);
	if (result && g_pm.getVerbosity() >= 3) {
		cerr << "Successor pruned as unable to exceed best current score:"
				<< endl;
		cerr << *this;
		cerr << endl;
	}
	return result;
}

bool RandomWriteState::isLegal() const {
	bool result = true;
	string reason = "";
	if (adjacentBlackPoints()) {
		result = false;
		reason = "adjacent BPs";
	}
	if (hasIllegalWords()) {
		result = false;
		reason = "illegal word";
	}
	if ((!g_pm.getSymmetryInfo() && wordRepetition())) {
		reason = "word repetition";
		result = false;
	}
	if (exceedBlackPoints()) {
		result = false;
		reason = "exceed max BPs";
	}
	if (!this->isConnected()) {
		result = false;
		reason = "grid disconnected";
		//cerr << "Successor pruned away as illegal" << endl;
		//cerr << "Reason: " << reason << endl;
		//cerr << *this;
		//cerr << endl;
	}
	if (!result) {
		if (g_pm.getVerbosity() >= 3) {
			cerr << "Successor pruned away as illegal" << endl;
			cerr << "Reason: " << reason << endl;
			cerr << *this;
			cerr << endl;
		}
		return false;
	} else
		return true;
}

bool RandomWriteState::willExceedBPs() {
	int heurBPs = this->getHeurBPs();
	if (this->m_nrBlackPoints + heurBPs > MAX_BPS) {
		if (g_pm.getVerbosity() >= 3) {
			cerr << "Successor pruned with admissible estimates for future BPs:"
					<< endl;
			cerr << *this;
			cerr << endl << "Heur BPs: " << heurBPs << endl;
			cerr << endl;
		}
		return true;
	}
	return false;
}

int RandomWriteState::operator==(const RandomWriteState &rhs) const {
	if (this->getNrThematicPoints() != rhs.getNrThematicPoints())
		return 0;
	else
		return 1;
}

int RandomWriteState::operator<(const RandomWriteState &rhs) const {
	if (g_pm.getHeurOrder() == 0) {
		double this_ratio = 1.0 * this->getNrFilledCells() / (1 + this->getNrBPs());
		double other_ratio = 1.0 * rhs.getNrFilledCells() / (1 + rhs.getNrBPs());
		if (this_ratio < other_ratio)
			return 1;
		return 0;
	} else if (g_pm.getHeurOrder() == 1) {
		double this_ratio = (1.0*this->getMinSlotScore())/(1+this->getNrBPs());
		double other_ratio = (1.0*rhs.getMinSlotScore()/(1+rhs.getNrBPs()));
		if (this_ratio < other_ratio)
			return 1;
		return 0;
	} else if (g_pm.getHeurOrder() == 2) {
		double this_ratio = (1.0*this->getNrThematicPoints());
		double other_ratio = (1.0*rhs.getNrThematicPoints());
		if (this_ratio < other_ratio)
			return 1;
		return 0;
	} else if (g_pm.getHeurOrder() == 3) {
		double this_ratio = (1.0*this->getNrThematicPoints()/(1+this->getNrBPs()));
		double other_ratio = (1.0*rhs.getNrThematicPoints()/(1+rhs.getNrBPs()));
		if (this_ratio < other_ratio)
			return 1;
		return 0;
	} else if (g_pm.getHeurOrder() == 4) {
		double this_ratio = (1.0*this->getNrThematicPoints()*this->getMinSlotScore())/(1+this->getNrBPs());
		double other_ratio = (1.0*rhs.getNrThematicPoints()*rhs.getMinSlotScore())/(1+rhs.getNrBPs());
		if (this_ratio < other_ratio)
			return 1;
		return 0;
	}
	assert (false);
	return 0;
	/*
	if (this->getMaxOptScore() < rhs.getMaxOptScore())
		return 1;
	if (this->getMaxOptScore() > rhs.getMaxOptScore())
		return 0;
	if (this->getNrThematicPoints() < rhs.getNrThematicPoints())
		return 1;
	if (this->getNrThematicPoints() > rhs.getNrThematicPoints())
		return 0;
	*/
}

vector<Move> RandomWriteState::extractPlayedThemMoves() const {
	vector<Move> result;
	for (int slot_idx = 0; slot_idx < this->m_slots.size(); slot_idx++) {
		const WordSlot & slot = this->m_slots[slot_idx];
		if (slot.getLength() <= 2)
			continue;
		if (!slot.getPattern().isInstantiated())
			continue;
		vector<Dictionary> & dics =
				(slot.getDirection() == HORIZONTAL) ?
						(m_horizDics) : (m_vertDics);
		for (vector<Dictionary>::const_iterator it1 = dics.begin();
				it1 != dics.end(); it1++) {
			if (!it1->isThematic())
				continue;
			int word_id = it1->getWordId(slot.getSimplePattern());
			if (word_id == -1)
				continue;
			Move move;
			move.direction = slot.getDirection();
			move.slotX = slot.getCol();
			move.slotY = slot.getRow();
			move.wordId = word_id;
			move.word = slot.getSimplePattern();
			result.push_back(move);
		}
	}
	return result;
}
