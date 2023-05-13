/*
 *  state.cpp
 *  combus
 *
 *  Created by Adi Botea on 2/07/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "state.h"
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <math.h>
#include <float.h>
#include <ext/hash_map>
#include "cellrecord.h"
#include <list>
#include <algorithm>
#include <string.h>
#include "limits.h"
#include "constants.h"
#include "globals.h"
#include "util.h"

using namespace std;
using namespace __gnu_cxx;

State::State() : m_propagationIterations(0), m_nrBlackPoints(0), m_nrThematicPoints(0), m_wordRepetition(
																							false),
				 m_complete(false), m_procFromScratch(true), m_skipDeadlockExtraction(
																 false),
				 m_0HitSlot(false), m_adjBPs(false), m_nrBPBlocks(0), m_goal(false)
{
	this->m_nrFilledCells = 0;
	m_recentCellInstantiations.clear();
	for (int i = 0; i < State::m_nrRows; i++)
	{
		for (int j = 0; j < State::m_nrCols; j++)
		{
			if (m_initGrid[i][j] != BLANK && m_initGrid[i][j] != BLACKPOINT)
			{
				m_recentCellInstantiations.push_back(Cell(i, j));
			}
			if (m_initGrid[i][j] == BLACKPOINT && this->m_grid[i][j] != BLACKPOINT)
			{
				this->addBlackPoint(i, j);
			}
			else
			{
				m_grid[i][j] = m_initGrid[i][j];
				if (m_grid[i][j] != BLANK)
					m_nrFilledCells++;
			}
		}
	}
	this->m_recentBlackPointCol = -1;
	this->m_recentBlackPointRow = -1;
	collectSlots();
	for (int i = 0; i < this->m_slots.size(); i++)
	{
		this->m_slots[i].setAllMultiChar(true);
	}
}

State::State(int iters) : m_propagationIterations(iters), m_nrBlackPoints(0), m_nrThematicPoints(
																				  0),
						  m_nrFilledCells(0), m_wordRepetition(false), m_complete(
																		   false),
						  m_procFromScratch(true), m_skipDeadlockExtraction(
													   false),
						  m_0HitSlot(false), m_adjBPs(false), m_nrBPBlocks(0)
{
	for (int i = 0; i < State::m_nrRows; i++)
	{
		for (int j = 0; j < State::m_nrCols; j++)
		{
			if (m_initGrid[i][j] != BLANK && m_initGrid[i][j] != BLACKPOINT)
				m_recentCellInstantiations.push_back(Cell(i, j));
			if (m_initGrid[i][j] == BLACKPOINT)
			{
				this->addBlackPoint(i, j);
			}
			else
			{
				m_grid[i][j] = m_initGrid[i][j];
				if (m_grid[i][j] != BLANK)
					m_nrFilledCells++;
			}
		}
	}
	collectSlots();
	for (int i = 0; i < this->m_slots.size(); i++)
	{
		this->m_slots[i].setAllMultiChar(true);
	}
}

bool State::backjumpHere(const State &deadlockState) const
{
	const vector<WordSlot> &slots = deadlockState.getDeadlockSlots();
	for (vector<WordSlot>::const_iterator it = slots.begin(); it != slots.end();
		 it++)
	{
		if (it->getDirection() != m_slots[m_idxSelSlotGen].getDirection())
			continue;
		if (it->getCol() != m_slots[m_idxSelSlotGen].getCol())
			continue;
		if (it->getRow() != m_slots[m_idxSelSlotGen].getRow())
			continue;
		return true;
	}
	return false;
}

void State::updateSlots()
{
	vector<WordSlot> new_slots;
	for (int it = 0; it < this->m_slots.size(); it++)
	{
		if (this->m_recentBlackPointRow != -1 && this->m_recentBlackPointCol != -1 && this->m_slots[it].containsCell(this->m_recentBlackPointRow, this->m_recentBlackPointCol))
		{
			vector<WordSlot> sss = this->m_slots[it].split(
				this->m_recentBlackPointRow, this->m_recentBlackPointCol);
			for (int kkk = 0; kkk < sss.size(); kkk++)
			{
				WordSlot sl = sss[kkk];
				sl.setId(new_slots.size());
				new_slots.push_back(sl);
			}
		}
		else
		{
			WordSlot sl = this->m_slots[it];
			sl.setId(new_slots.size());
			new_slots.push_back(sl);
		}
	}
	this->m_slots = new_slots;
}

void State::collectSlots()
{
	m_slots.clear();
	for (int row = 0; row < m_nrRows; row++)
	{
		collectSlotsHoriz(row);
	}
	for (int col = 0; col < m_nrCols; col++)
	{
		collectSlotsVert(col);
	}
	for (vector<WordSlot>::iterator it = m_slots.begin(); it != m_slots.end();
		 it++)
	{
		it->computeImpactAreaSize(m_slots);
	}
}

void State::collectSlotsHoriz(int row)
{
	string pattern;
	assert(pattern.size() == 0);
	int col = 0, colStart = 0;
	while (col <= m_nrCols)
	{
		if (col == m_nrCols || m_grid[row][col] == BLACKPOINT)
		{
			if (pattern.size() > 0)
			{
				assert(pattern.size() <= State::m_nrCols);
				for (int i = 0; i < pattern.size(); i++)
					assert(pattern[i] != BLACKPOINT);
				m_slots.push_back(
					WordSlot(row, colStart, HORIZONTAL, pattern,
							 m_slots.size()));
				if (m_slotStats.size() < m_slots.size())
					m_slotStats.push_back(StaticSlotStats());
			}
			pattern.clear();
			colStart = col + 1;
		}
		else
		{
			pattern += m_grid[row][col];
		}
		col++;
	}
}

void State::collectSlotsVert(int col)
{
	string pattern;
	assert(pattern.size() == 0);
	int row = 0, rowStart = 0;
	while (row <= m_nrRows)
	{
		if (row == m_nrRows || m_grid[row][col] == BLACKPOINT)
		{
			if (pattern.size() > 0)
			{
				assert(pattern.size() <= State::m_nrRows);
				m_slots.push_back(
					WordSlot(rowStart, col, VERTICAL, pattern,
							 m_slots.size()));
				if (m_slotStats.size() < m_slots.size())
					m_slotStats.push_back(StaticSlotStats());
			}
			pattern.clear();
			rowStart = row + 1;
		}
		else
		{
			pattern += m_grid[row][col];
		}
		row++;
	}
}

/*
double State::getNrpaScore() const {
	double result = 0.0;
	unsigned int kind = State::m_nrpaScoreKind;
	if (kind == 0) {
		result = 1.0 * m_nrThematicPoints
				+ 5 * (m_nrThematicPoints - m_nrFilledCells + m_nrBlackPoints)
				- 10 * this->getNrBPBlocks() - 5 * this->getNrBPs();
	} else if (kind == 1) {
		result = 1.0 * m_nrThematicPoints
				+ 10 * (m_nrThematicPoints - m_nrFilledCells + m_nrBlackPoints)
				- 5 * this->getNrBPBlocks();
	} else if (kind == 2) {
		result = 1.0 * m_nrThematicPoints
				+ 5 * (m_nrThematicPoints - m_nrFilledCells + m_nrBlackPoints)
				- 10 * this->getNrBPBlocks();
	} else if (kind == 3) {
		result = 1.0 * m_nrThematicPoints / this->m_nrBPBlocks;
	} else if (kind == 4) {
		result = 1.0 * m_nrThematicPoints - this->getNrBPBlocks();
	} else if (kind == 5) {
		result = 1.0 * m_nrThematicPoints / (1 + this->getNrBPs());
	} else if (kind == 6) {
		result = 1.0 * m_nrThematicPoints / (1 + countBlanksAdjToLetters());
	} else if (kind == 7) {
		result = 1.0 * m_nrThematicPoints;
	}

	//if (this->isDeadlock())
	//    result -= 10;
	return result;
}
*/

// This method is used when a deadlock is detected.
// Starting from the slot with 0 successors,
// we gradually enlarge the area using slots
// that are not fully instantiated.
// The method returns the set of fully instantiated
// slots that intersect this area.

void State::computeDeadlockInfo(int seedSlotIdx, BooleanMap &boolMap)
{
	if (seedSlotIdx == -1)
		return;
	//    bool boolMapInitiallyEmpty = (boolMap.getNrTrueValues() == 0);
	int iter = 0;
	vector<int> area;
	list<int> newlyAddedSlots;
	hash_map<int, int> deadlockSlotIndexes;
	m_deadlockSlots.clear();
	newlyAddedSlots.push_front(seedSlotIdx);
	while (newlyAddedSlots.size() > 0)
	{
		iter++;
		int slotIdx = newlyAddedSlots.back();
		newlyAddedSlots.pop_back();
		boolMap.markCells(m_slots[slotIdx].getCells(), false);
		if (find(area.begin(), area.end(), slotIdx) == area.end())
		{
			area.push_back(slotIdx);
			//            boolMap.markCells(m_slots[slotIdx].getCells(), false);
		}
		vector<int> slots = getIntersectingSlots(slotIdx);
		for (vector<int>::const_iterator it = slots.begin(); it != slots.end();
			 it++)
		{
			if (m_slots[*it].getPattern().isInstantiated())
			{
				deadlockSlotIndexes[*it] = *it;
			}
			else
			{
				if ((iter > 6) && m_slots[*it].getPattern().isBlank() && m_slots[*it].getPattern().hasNoConstraints())
					continue;
				if (find(area.begin(), area.end(), *it) != area.end())
					continue;
				if (find(newlyAddedSlots.begin(), newlyAddedSlots.end(), *it) != newlyAddedSlots.end())
					continue;
				newlyAddedSlots.push_front(*it);
			}
		}
	}
	for (hash_map<int, int>::const_iterator it = deadlockSlotIndexes.begin();
		 it != deadlockSlotIndexes.end(); it++)
		m_deadlockSlots.push_back(m_slots[it->second]);
	computeDeadlockCells(area);
}

void State::computeDeadlockCells(const vector<int> &slots)
{
	hash_map<int, CellRecord> cells;
	for (vector<int>::const_iterator it = slots.begin(); it != slots.end();
		 it++)
	{
		const WordSlot &slot = m_slots[*it];
		for (int pos = -1; pos <= slot.getLength(); pos++)
		{
			if (pos == -1 && !slot.cellBeforeExists())
				continue;
			if (pos == slot.getLength() && !slot.cellAfterExists(m_nrRows, m_nrCols))
				continue;
			if (slot.getLetterExt(pos) != BLANK)
			{
				int index = slot.getCellRow(pos) * m_nrCols + slot.getCellCol(pos);
				cells[index] = CellRecord(slot.getCellRow(pos),
										  slot.getCellCol(pos), slot.getLetterExt(pos));
			}
		}
	}
	for (hash_map<int, CellRecord>::const_iterator it = cells.begin();
		 it != cells.end(); it++)
	{
		assert(it->second.m_letter != BLANK);
		m_deadlockCells.addRecord(it->second);
	}
}

void State::computeDeadlockInfo(BooleanMap &boolMap)
{
	computeDeadlockInfo(m_idxSelSlotGen, boolMap);
}

double State::computeSlotScore(int it) const
{
	return m_slots[it].getStaticScore();
	if (!g_pm.getSlotStatsFlag())
		return m_slots[it].getStaticScore();
	if (m_slots[it].getStaticScore() <= 1)
		return m_slots[it].getStaticScore();
	if (m_slotStats[it].getStatisticVolume() == 0)
		return m_slots[it].getStaticScore();
	return .1 * m_slots[it].getStaticScore() + .9 * m_slotStats[it].getStatisticScore();
}

vector<int> State::getIntersectingSlots(int slotIdx) const
{
	vector<int> result;
	for (int it = 0; it < m_slots.size(); it++)
	{
		if (it == slotIdx)
			continue;
		if (m_slots[slotIdx].slotsCrossEachOther(m_slots[it]))
			result.push_back(it);
	}
	return result;
}

string State::selPatAsString() const
{
	char tmp[200];
	assert(m_idxSelSlotGen == -1 || m_idxSelSlotGen < m_slots.size());
	if (m_idxSelSlotGen == -1)
		return "<>";
	string str = "<" + m_slots[m_idxSelSlotGen].getSimplePattern() + ">";
	for (int it = 0; it < str.length(); it++)
		if (str[it] == BLANK)
			str[it] = '.';
	assert(m_slots[m_idxSelSlotGen].getCol() <= m_nrCols);
	assert(m_slots[m_idxSelSlotGen].getRow() <= m_nrRows);
	sprintf(tmp, " [row %d, col %d] -- [estimated branch fact %d]",
			m_slots[m_idxSelSlotGen].getRow(),
			m_slots[m_idxSelSlotGen].getCol(),
			m_slots[m_idxSelSlotGen].getNrMatchingWords());
	assert(strlen(tmp) < 200);
	str += tmp;
	return str;
}

int State::countMatchingWords(const WordSlot &slot) const
{
	int result = 0;
	assert(
		slot.getDirection() == HORIZONTAL || slot.getDirection() == VERTICAL);
	vector<Dictionary> &dics =
		(slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
	for (vector<Dictionary>::const_iterator it1 = (dics).begin();
		 it1 != (dics).end(); it1++)
	{
		result += it1->getNrWordsMatchingPattern(slot.getPattern());
	}
	return result;
}

bool State::hasMatchingWord(const WordSlot &slot) const
{
	if (slot.getLength() <= 2)
		return true;
	vector<Dictionary> &dics =
		(slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
	for (vector<Dictionary>::const_iterator it1 = dics.begin();
		 it1 != dics.end(); it1++)
	{
		if (this->m_nrNonThemWords >= g_pm.getMaxNrNonThemWords() && slot.getLength() >= MIN_LENGTH_NTW && !it1->isThematic())
			continue;
		/*
		if (len == 2 && this->m_nrSlots[2] >= 10)
			continue;
		if (len == 3 && this->m_nrSlots[3] >= 10)
			continue;
		if (len == 4 && this->m_nrSlots[4] >= 4)
			continue;
		if (len == 5 && this->m_nrSlots[5] >= 4)
			continue;
			*/
		if (it1->atLeast1Hit(slot.getPattern()))
		{
			return true;
		}
	}
	return false;
}

bool State::matchesDeadlock(const vector<CellRecord> &deadlock) const
{
	for (vector<CellRecord>::const_iterator it = deadlock.begin();
		 it != deadlock.end(); it++)
	{
		if (this->m_grid[it->m_row][it->m_col] != it->m_letter)
			return false;
	}
	return true;
}

bool State::wordIsThematic(const WordSlot &slot) const
{
	assert(
		slot.getDirection() == HORIZONTAL || slot.getDirection() == VERTICAL);
	vector<Dictionary> &dics =
		(slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
	for (vector<Dictionary>::const_iterator it1 = (dics).begin();
		 it1 != (dics).end(); it1++)
	{
		if (!it1->isThematic())
			continue;
		if (it1->getNrWordsMatchingPattern(slot.getPattern()) == 1)
			return true;
	}
	return false;
}

/*
 Iterate through all slots affected by the last move
 and compute the multi-char map.
 */
void State::updateMultiCharMap()
{
	for (int it = 0; it < m_slots.size(); it++)
	{
		if (m_slots[it].getLength() <= 2)
			continue;
		if (!slotContainsRecentlyUpdatedCell(m_slots[it], INSTANTIATION))
			continue;
		vector<Dictionary> &dics =
			(m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
		m_slots[it].updateMultiCharPatternFromGridMap(m_multiCharMap);
		m_slots[it].updateMultiCharPatternFromDictionaries(dics);
		m_multiCharMap.updateFromSlot(m_slots[it]);
	}
}

void State::propConstDBP(int nrIterations)
{
	vector<int> slotStack;
	int iter = 0;
	bool deadlock = false;
	bool bp_allowed = this->exceedBlackPoints();

	m_multiCharMap.clearRecentlyConstrainedCells();
	this->m_maxFuturePoints = 0;
	if (m_procFromScratch)
	{
		// first get the domain (list of words) for each slot
		for (int it = 0; it < m_slots.size(); it++)
		{
			if (m_slots[it].getLength() <= 2)
			{
				continue;
			}
			vector<Dictionary> &dics =
				(m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
			vector<int> legal_bps = this->getLegalBPPos(m_slots[it]);
			bool non_them_ok = (this->m_nrNonThemWords < g_pm.getMaxNrNonThemWords());
			m_slots[it].computeDomainDBP(dics, non_them_ok, legal_bps,
										 bp_allowed);
		}
	}
	else
	{
		// first get the domain (list of words) for each slot
		for (int it = 0; it < m_slots.size(); it++)
		{
			if (m_slots[it].getLength() <= 2)
			{
				continue;
			}
			if (m_slots[it].getPattern().isInstantiated())
				continue;
			if (!slotContainsRecentlyUpdatedCell(m_slots[it], INSTANTIATION) && m_slots[it].domainIsComputed())
				continue;
			vector<Dictionary> &dics =
				(m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
			// Synchronize the multi-char map of the slot with that of the grid map
			m_slots[it].updateMultiCharPatternFromGridMap(m_multiCharMap);
			vector<int> legal_bps = this->getLegalBPPos(m_slots[it]);
			bool non_them_ok = (this->m_nrNonThemWords < g_pm.getMaxNrNonThemWords());
			m_slots[it].computeDomainDBP(dics, non_them_ok, legal_bps,
										 bp_allowed);
			if (find(slotStack.begin(), slotStack.end(), it) == slotStack.end())
			{
				// cerr << "adding slot " << it << " to stack" << endl;
				slotStack.push_back(it);
			}
		}
	}
	// now start iterative propagation of constraints
	while (!slotStack.empty())
	{
		int index = slotStack.back();
		slotStack.pop_back();
		// Synchronize the multi-char map of the slot with that of the grid map
		m_slots[index].updateMultiCharPatternFromGridMap(m_multiCharMap);
		vector<Dictionary> &dics =
			(m_slots[index].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
		vector<int> legal_bps = this->getLegalBPPos(m_slots[index]);
		if (!m_slots[index].domainIsComputed())
		{
			bool non_them_ok = (this->m_nrNonThemWords < g_pm.getMaxNrNonThemWords());
			m_slots[index].computeDomainDBP(dics, non_them_ok, legal_bps,
											bp_allowed);
		}
		else
		{
			if (m_slots[index].getPattern().isBlank())
			{
				// no constraints likely to be added here
				//cerr << "Domain size for blank pattern: " << m_slots[index].getNrMatchingWords() << endl;
				continue;
			}
			else
				m_slots[index].upwardsPropagation(dics);
		}
		if (m_slots[index].getNrMatchingWords() == 0)
			deadlock = true;
		if (m_slots[index].getPattern().isBlank())
		{
			//cerr << "Domain size for blank pattern: " << m_slots[index].getNrMatchingWords() << endl;
			continue;
		}
		else
		{
			int first_blank_pos = m_slots[index].getFirstBP();
			if (legal_bps.size() > 0)
				first_blank_pos = legal_bps[0];
			m_slots[index].downwardsPropBPS(dics, first_blank_pos);
		}
		m_multiCharMap.clearRecentlyConstrainedCells();
		m_multiCharMap.updateFromSlot(m_slots[index]);
		m_recentCellConstraints = m_multiCharMap.getRecentlyConstrainedCells();
		// add intersecting slots to stack
		for (int it = 0; it < m_slots.size(); it++)
		{
			if (m_slots[it].getLength() <= 2)
				continue;
			if (!slotContainsRecentlyUpdatedCell(m_slots[it], CONSTRAINT))
				continue;
			if (m_slots[it].getPattern().isInstantiated())
				continue;
			if (find(slotStack.begin(), slotStack.end(), it) == slotStack.end())
			{
				slotStack.push_back(it);
			}
		}
		iter++;
		if (iter == nrIterations)
		{
			break;
		}
		if (deadlock)
		{
			break;
		}
	}
	this->m_maxFuturePoints = 0;
	for (int it = 0; it < m_slots.size(); it++)
	{
		if (m_slots[it].getLength() <= 2)
		{
			continue;
		}
		// THE PART BELOW IS BUGGY. WE SHOULD ALSO TRY SUBSLOTS OF THE ACTUAL SLOT.
		//cerr << "Setting nr matching words for slot [" << m_slots[it].getSimplePattern() << "]" << endl;
		m_slots[it].setNrMatchWordsToDom();
		if (!m_slots[it].getPattern().isInstantiated() && m_slots[it].getNrThemWords() > 0)
		{
			this->m_maxFuturePoints += m_slots[it].getLength();
		}

		// The reason we clear the domains is to reduce the memory footprint
		// of states and therefore of the full program.
		m_slots[it].clearDomain();
		// Note: Clearing the dictionaries might be part of the reason
		// why incremental propagation doesn't work currently
	}
	m_propIterationStats.addDataPoint(iter);
}

void State::computeRecentlyConstrainedCells(
	const int m1[MAX_NR_ROWS][MAX_NR_COLS],
	const int m2[MAX_NR_ROWS][MAX_NR_COLS])
{
	m_recentCellConstraints.clear();
	for (int row = 0; row < m_nrRows; row++)
	{
		for (int col = 0; col < m_nrCols; col++)
		{
			if (m1[row][col] != m2[row][col])
				m_recentCellConstraints.push_back(Cell(row, col));
		}
	}
}

int State::countBlanksAdjToLetters() const
{
	int result = 0;
	for (int row = 0; row < m_nrRows; row++)
	{
		for (int col = 0; col < m_nrCols; col++)
		{
			if (m_grid[row][col] >= 'A' && m_grid[row][col] <= 'Z')
			{
				if (row > 0 && m_grid[row - 1][col] == BLANK)
					result++;
				if (col > 0 && m_grid[row][col - 1] == BLANK)
					result++;
				if (row < m_nrRows - 1 && m_grid[row + 1][col] == BLANK)
					result++;
				if (col < m_nrCols - 1 && m_grid[row][col + 1] == BLANK)
					result++;
			}
		}
	}
	return result;
}

double State::weightedSumLetters(int slot_idx) const
{
	double result = 0;
	double ratio = 1.0;
	for (int cell_idx = 0; cell_idx < m_slots[slot_idx].getLength();
		 cell_idx++)
	{
		int row = m_slots[slot_idx].getCellRow(cell_idx);
		int col = m_slots[slot_idx].getCellCol(cell_idx);
		if (m_grid[row][col] != BLANK)
			result += ratio;
		ratio *= .9;
	}
	return result;
}

int State::countBlanksAdjToSlot(int slot_idx) const
{
	int result = 0;
	for (int cell_idx = 0; cell_idx < m_slots[slot_idx].getLength();
		 cell_idx++)
	{
		int row = m_slots[slot_idx].getCellRow(cell_idx);
		int col = m_slots[slot_idx].getCellCol(cell_idx);
		if (row > 0 && m_grid[row - 1][col] == BLANK && m_slots[slot_idx].getDirection() == HORIZONTAL)
			result++;
		if (col > 0 && m_grid[row][col - 1] == BLANK && m_slots[slot_idx].getDirection() == VERTICAL)
			result++;
		if (row < m_nrRows - 1 && m_grid[row + 1][col] == BLANK && m_slots[slot_idx].getDirection() == HORIZONTAL)
			result++;
		if (col < m_nrCols - 1 && m_grid[row][col + 1] == BLANK && m_slots[slot_idx].getDirection() == VERTICAL)
			result++;
	}
	return result;
}

void State::markBPFreeSlots()
{
	int nr_horiz = 0;
	int nr_vert = 0;
	vector<int> horiz_to_mark; // Horizontal slots that could be marked as forbidding BPs
	vector<int> vert_to_mark;  // Vertical slots that could be marked as forbidding BPs
	//cerr << "ENTER METHOD comptueHeurBPs" << endl;
	for (int it = 0; it < this->m_slots.size(); it++)
	{
		m_slots[it].setAllowBPs(true);
	}
	for (int it = 0; it < this->m_slots.size(); it++)
	{
		const WordSlot &slot = m_slots[it];
		const WordPattern &pattern = slot.getPattern();
		//cerr << "Slot with index " << it << endl;
		int len = slot.getLength();
		if (len < 3)
			continue;
		if (pattern.isInstantiated())
			continue;
		bool slot_has_word = false;
		vector<Dictionary> &dics =
			(slot.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
		for (vector<Dictionary>::const_iterator it1 = dics.begin();
			 it1 != dics.end(); it1++)
		{
			if (slot_has_word)
				break;
			if (this->m_nrNonThemWords >= g_pm.getMaxNrNonThemWords() && len >= MIN_LENGTH_NTW && !it1->isThematic())
				continue;
			/*
			if (len == 2 && this->m_nrSlots[2] >= 10)
				continue;
			if (len == 3 && this->m_nrSlots[3] >= 10)
				continue;
			if (len == 4 && this->m_nrSlots[4] >= 4)
				continue;
			if (len == 5 && this->m_nrSlots[5] >= 4)
				continue;
				*/
			if (it1->atLeast1Hit(pattern))
			{
				slot_has_word = true;
				continue;
			}
		}
		if (slot.getDirection() == HORIZONTAL)
		{
			if (!slot_has_word)
			{
				nr_horiz += this->getEstimBPs(slot);
				//cerr << "Horizontal slot with no matches: " << slot.getSimplePattern() << endl;
			}
			else
				horiz_to_mark.push_back(it);
		}
		if (slot.getDirection() == VERTICAL)
		{
			if (!slot_has_word)
				nr_vert += this->getEstimBPs(slot);
			else
				vert_to_mark.push_back(it);
		}
	}
	int val = (nr_horiz > nr_vert) ? nr_horiz : nr_vert;
	if (this->getNrBPs() + val == MAX_BPS)
	{
		if (val == nr_horiz)
		{
			for (int kk = 0; kk < horiz_to_mark.size(); kk++)
				m_slots[horiz_to_mark[kk]].setAllowBPs(false);
		}
		else
			for (int kk = 0; kk < vert_to_mark.size(); kk++)
				m_slots[vert_to_mark[kk]].setAllowBPs(false);
		if (g_pm.getVerbosity() >= 3)
		{
			cerr << "Have forbidden BPs in the following slots" << endl;
			for (int it = 0; it < m_slots.size(); it++)
			{
				if (!m_slots[it].getAllowBPs())
				{
					cerr << m_slots[it].getPattern().getSimplePatternDots() << endl;
				}
			}
			cerr << "in state" << endl;
			cerr << *this << endl;
		}
	}
	this->m_heurBPs = val;
}

void State::collectSlotStatistics()
{
	m_0HitSlot = false;
	m_idxSelSlotGen = -1;
	this->analyseInstantiatedSlots();
	this->markBPFreeSlots();
	this->analyseUninstantiatedSlots();
	// apply a search that is consistent in exploring one area
	WordSlot &slot = m_slots[m_idxSelSlotGen];
	if (m_idxSelSlotGen >= 0)
		m_slotStats[m_idxSelSlotGen].addScoreRecord(slot.getStaticScore());
}

int State::computeMaxPatternLength() const
{
	int result = 0;
	for (int it = 0; it < m_slots.size(); it++)
	{
		if (m_slots[it].getPattern().isInstantiated())
			continue;
		if (m_slots[it].getLength() > result)
			result = m_slots[it].getLength();
	}
	return result;
}

int State::getFirstHorizBlank(int row) const
{
	for (int colIt = 0; colIt < m_nrCols; colIt++)
	{
		if (m_grid[row][colIt] == BLANK)
			return colIt;
	}
	return NOTHING;
}

int State::getFirstVertBlank(int col) const
{
	for (int rowIt = 0; rowIt < m_nrRows; rowIt++)
	{
		if (m_grid[rowIt][col] == BLANK)
			return rowIt;
	}
	return NOTHING;
}

string State::getRow(int i) const
{
	string res;
	for (int j = 0; j < State::m_nrCols; j++)
	{
		res += m_grid[i][j];
	}
	return res;
}

string State::getCol(int j) const
{
	string res;
	for (int i = 0; i < State::m_nrRows; i++)
	{
		res += m_grid[i][j];
	}
	return res;
}

bool State::isGridComplete() const
{
	for (int it = 0; it < m_nrRows; it++)
	{
		if (getFirstHorizBlank(it) != NOTHING)
			return false;
	}
	return true;
}

void State::makeHorizMove(int row, int col, string word)
{
	if (col + word.length() > m_nrCols)
	{
		cerr << "Error: (" << row << ", " << col << ", " << word;
		cerr << ")" << std::endl
			 << this;
	}
	//cerr << "Word to add: " << word << "[" << row << "," << col << "]" << endl;
	//cerr << "Slot in grid: ";
	//for (int i = 0; i < word.length(); i++)
	//    cerr << m_grid[row][col + i];
	//cerr << endl;

	assert(col + word.length() <= m_nrCols);
	for (int i = 0; i < word.length(); i++)
	{
		assert(
			m_grid[row][col + i] == BLANK || m_grid[row][col + i] == word[i]);
		if (m_grid[row][col + i] == BLANK)
		{
			m_grid[row][col + i] = word[i];
			m_recentCellInstantiations.push_back(Cell(row, col + i));
			this->m_nrFilledCells++;
		}
	}
	if (col + word.length() < m_nrCols && m_grid[row][col + word.length()] != BLACKPOINT)
	{
		this->addBlackPoint(row, col + word.length());
	}
	if (col - 1 >= 0 && m_grid[row][col - 1] != BLACKPOINT)
	{
		this->addBlackPoint(row, col - 1);
	}
}

void State::makeMove(const WordSlot &slot, string word)
{
	if (false && m_slots.size() > 0)
	{
		assert(slot.getId() >= 0 && slot.getId() < m_slots.size());
		m_slots[slot.getId()].setPattern(word);
	}
	this->setLastWord(word);
	makeMove(slot.getRow(), slot.getCol(), slot.getDirection(), word);
}

void State::makeMove(int row, int col, bool direction, string word)
{
	m_recentCellInstantiations.clear();
	this->m_recentBlackPointCol = -1;
	this->m_recentBlackPointRow = -1;
	if (g_pm.getSymmetryInfo())
	{
		if (direction == HORIZONTAL)
		{
			makeHorizMove(row, col, word);
			makeVertMove(col, row, word);
		}
		else
		{
			makeHorizMove(col, row, word);
			makeVertMove(row, col, word);
		};
	}
	else if (direction == HORIZONTAL)
	{
		makeHorizMove(row, col, word);
	}
	else
	{
		makeVertMove(row, col, word);
	}
	if (m_wordsInGrid.find(word) != m_wordsInGrid.end())
	{
		m_wordRepetition = true;
	}
	if (this->m_procFromScratch)
		collectSlots();
	else
		updateSlots();
	//    updateSlotsFromGrid();
	//performStaticAnalysis();
}

void State::makeVertMove(int row, int col, string word)
{
	if (row + word.length() > m_nrRows)
	{
		cerr << "Error: (" << row << ", " << col << ", " << word;
		cerr << ")" << endl
			 << this;
	}
	for (int i = 0; i < word.length(); i++)
	{
		assert(
			m_grid[row + i][col] == BLANK || m_grid[row + i][col] == word[i]);
		if (m_grid[row + i][col] == BLANK)
		{
			m_grid[row + i][col] = word[i];
			m_recentCellInstantiations.push_back(Cell(row + i, col));
			this->m_nrFilledCells++;
		}
	}
	if (row + word.length() < m_nrRows && m_grid[row + word.length()][col] != BLACKPOINT)
	{
		this->addBlackPoint(row + word.length(), col);
	}
	if (row - 1 >= 0 && m_grid[row - 1][col] != BLACKPOINT)
	{
		this->addBlackPoint(row - 1, col);
	}
}

void State::performStaticAnalysis()
{
	// As a side effect, updateCharacterMap computes the number
	// of matching words for the slots that were affected by the
	// most recent move. For all other slots, that number is computed
	// in collectPatternStatistics(). This trick saves some computation time.
	// TODO: Fix the propagation for the new setting with dynamic black points
	this->propConstDBP(m_propagationIterations);
	this->addForcedBPs();
	collectSlotStatistics();
	if (this->m_idxSelSlotGen == -1) {
		this->m_goal = true;
	}
}

bool State::slotContainsRecentlyUpdatedCell(const WordSlot &slot,
											cellUpdateType updateType) const
{
	const vector<Cell> &recentChanges =
		(updateType == INSTANTIATION) ? m_recentCellInstantiations : m_recentCellConstraints;
	for (vector<Cell>::const_iterator it = recentChanges.begin();
		 it != recentChanges.end(); ++it)
	{
		if (slot.getDirection() == HORIZONTAL)
			if (it->m_row == slot.getRow() && it->m_col >= slot.getCol() && it->m_col < slot.getCol() + slot.getLength())
				return true;
		if (slot.getDirection() == VERTICAL)
			if (it->m_col == slot.getCol() && it->m_row >= slot.getRow() && it->m_row < slot.getRow() + slot.getLength())
				return true;
	}
	return false;
}

void State::printVariableInfo(ostream &os) const
{
	int vars[50];
	int nrCons = 0;
	for (int it = 0; it < 50; it++)
		vars[it] = 0;
	for (int it = 0; it < m_slots.size(); it++)
	{
		const WordSlot &slot = m_slots[it];
		vars[slot.getLength()]++;
		nrCons += slot.getLength();
	}
	os << "Nr constraints: " << nrCons << endl;
	for (int it = 0; it < 50; it++)
	{
		if (vars[it] != 0)
		{
			os << "Slots of length " << it << " : " << vars[it] << endl;
		}
	}
}

void State::readDictionaries(ifstream &file)
{
	string line;
	getline(file, line);
	int nrDics = atoi(line.c_str());
	int maxRowCol = (m_nrCols > m_nrRows) ? (m_nrCols) : (m_nrRows);
	for (int it = 0; it < nrDics; it++)
	{
		assert(!file.eof());
		getline(file, line);
		bool isThematic = (line == "1") ? true : false;
		assert(!file.eof());
		getline(file, line);
		Dictionary dictionary(line, maxRowCol, isThematic,
							  g_pm.getDicPercentage());
		assert(!file.eof());
		getline(file, line);
		if (line == "1")
			m_horizDics.push_back(dictionary);
		assert(!file.eof());
		getline(file, line);
		if (line == "1")
			m_vertDics.push_back(dictionary);
	}
}

void State::readHeader(ifstream &file)
{
	string line;
	assert(!file.eof());
	getline(file, line);
	State::m_nrRows = atoi(line.c_str());
	assert(!file.eof());
	getline(file, line);
	State::m_nrCols = atoi(line.c_str());
	assert(!file.eof());
	getline(file, line);
	State::m_allowNewBlackPoints = atoi(line.c_str());
	assert(!file.eof());
	getline(file, line);
	State::m_allowTwoChars[HORIZONTAL] = atoi(line.c_str());
	assert(!file.eof());
	getline(file, line);
	State::m_allowTwoChars[VERTICAL] = atoi(line.c_str());
	MultiCharMap::setStaticInfo(State::m_nrRows, State::m_nrCols);
}

void State::readInitGrid(ifstream &file)
{
	string line;
	for (int rowIt = 0; rowIt < m_nrRows; rowIt++)
	{
		assert(!file.eof());
		getline(file, line);
		for (int colIt = 0; colIt < line.size(); colIt += 2)
		{
			if (colIt >= 2 * MAX_NR_COLS)
				break;
			m_initGrid[rowIt][colIt / 2] = toupper(line[colIt]);
		}
	}
}

void State::readStaticInfo(const string &filename)
{
	ifstream file(filename.c_str());
	if (file.is_open())
	{
		readHeader(file);
		readInitGrid(file);
		readDictionaries(file);
		file.close();
	}
	else
	{
		cerr << "Cannot open file " << filename << endl;
	}
}

string State::getNrpaScoreKind()
{
	string result;
	unsigned int kind = State::m_nrpaScoreKind;
	if (kind == 0)
	{
		result =
			"2*(# thematic points) + 0*(# double-point cells) - 10*(# BP blocks) - 5*(# BPs)";
	}
	else if (kind == 1)
	{
		result =
			"# thematic points + 10*(# double-point cells) - 5*(# BP blocks)";
	}
	else if (kind == 2)
	{
		result =
			"# thematic points + 5*(# double-point cells) - 10*(# BP blocks)";
	}
	else if (kind == 3)
	{
		result = "# thematic points divided by # BP blocks";
	}
	else if (kind == 4)
	{
		result = "# thematic points - # BP blocks";
	}
	return result;
}

bool State::semiClosure() const
{
	return false;
}

void State::updateSlotsFromGrid()
{
	for (int it = 0; it < m_slots.size(); it++)
	{
		WordSlot &slot = m_slots[it];
		updateSlotFromGrid(slot);
	}
}

bool State::isBPLegal(int row, int col) const
{
	if (this->m_grid[row][col] != BLANK)
		return false;
	if (row > 0 && this->m_grid[row - 1][col] == BLACKPOINT)
		return false;
	if (row < this->m_nrRows && this->m_grid[row + 1][col] == BLACKPOINT)
		return false;
	if (col > 0 && this->m_grid[row][col - 1] == BLACKPOINT)
		return false;
	if (col < this->m_nrCols && this->m_grid[row][col + 1] == BLACKPOINT)
		return false;
	return true;
}

bool State::isDeadlock() const
{
	//if (m_branchingFactor == 0) {
	//    cerr << "Branching factor = 0" << endl;
	//    cerr << "Most constrained pattern: " << this->getMostConstraintPattern() << endl;
	//}
	string reason = "";
	bool result = false;
	if (false)
	{ // && m_branchingFactor == 0) {
		reason = "BF == 0";
		result = true;
	}
	if (m_multiCharMap.containsConflict())
	{
		reason = "conflict";
		result = true;
	}
	if (this->m_0HitSlot)
	{
		//cerr << "0-hit slot detected" << endl;
		//cerr << *this << endl << "---" << endl;
		reason = "0-hit slot";
		result = true;
	}
	if (result && g_pm.getVerbosity() >= 3)
	{
		cerr << "Successor pruned away as deadlocked:" << endl;
		cerr << "Reason: " << reason << endl;
		cerr << *this;
		cerr << endl;
	}
	return result;
}

vector<int> State::getLegalBPPos(const WordSlot &slot) const
{
	int bpc = slot.getCol();
	int bpr = slot.getRow();
	vector<int> result;
#ifdef NO_BPS
	return result;
#endif
	for (unsigned int len = 0; len < slot.getLength(); len++)
	{
		if (slot.getDirection() == HORIZONTAL)
			bpc = slot.getCol() + len;
		else
			bpr = slot.getRow() + len;
		if (this->isBPLegal(bpr, bpc))
			result.push_back(len);
	}
	return result;
}

void State::updateSlotFromGrid(WordSlot &slot)
{
	string pattern = "";
	for (int pos = 0; pos < slot.getLength(); pos++)
	{
		pattern += m_grid[slot.getCellRow(pos)][slot.getCellCol(pos)];
	}
	slot.setPattern(pattern);
	slot.updateMultiCharPatternFromGridMap(m_multiCharMap);
}

int State::computeSterileLoad() const
{
	int result = 0;
	for (int it = 0; it < m_slots.size(); it++)
	{
		if (m_slots[it].getLength() < 3)
		{
			result += m_slots[it].getLength();
			continue;
		}
		if (!m_slots[it].getPattern().isInstantiated())
			continue;
		if (!wordIsThematic(m_slots[it]))
			result += m_slots[it].getLength();
	}
	return result;
}

void State::analyseInstantiatedSlots()
{
	this->m_nrThematicPoints = 0;
	this->m_nrNonThemWords = 0;
	m_wordsInGrid.clear();
	this->m_nrSlots.clear();
	for (int kk = 0; kk < 6; kk++)
		this->m_nrSlots.push_back(0);
	for (int it = 0; it < m_slots.size(); it++)
	{
		if (m_slots[it].getLength() < 3)
		{
			//	this->m_nrSlots[m_slots[it].getLength()]++;
			continue;
		}
		if (!m_slots[it].getPattern().isInstantiated())
			continue;
		if (m_slots[it].getLength() < 6)
		{
			if (!this->wordIsThematic(m_slots[it]))
				this->m_nrSlots[m_slots[it].getLength()]++;
		}
		m_slots[it].setNrMatchingWords(1);
		string word = m_slots[it].getSimplePattern();
		if (m_wordsInGrid.find(word) != m_wordsInGrid.end())
		{
			m_wordRepetition = true;
			m_skipDeadlockExtraction = true;
		}
		m_wordsInGrid[word] = word;
		if (wordIsThematic(m_slots[it]))
			m_nrThematicPoints += word.size();
		else
		{
			if (word.size() > MIN_LENGTH_NTW)
				this->m_nrNonThemWords++;
		}
	}
}

void State::analyseUninstantiatedSlots()
{
	this->m_themSlotOrder.clear();
	this->m_idxSelSlotGen = -1;
	this->m_minSlotScore = DBL_MAX;
	vector<IDPair> tmp_vect;
	for (int it = 0; it < m_slots.size(); it++)
	{
		if (m_slots[it].getLength() < 3)
			continue;
		if (m_slots[it].getPattern().isInstantiated())
			continue;
		vector<Dictionary> &dics =
			(m_slots[it].getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
		vector<int> allowed_bps = this->getLegalBPPos(m_slots[it]);
		bool non_them_ok =
			(this->m_nrNonThemWords < g_pm.getMaxNrNonThemWords());
		if (true || m_slots[it].getNrMatchingWords() == -1)
		{
			bool allow_bp = (this->getNrBPs() != MAX_BPS);
			if (!m_slots[it].getAllowBPs())
				allow_bp = false;
			m_slots[it].setNrMatchingWordsFromDics(dics, non_them_ok,
												   allowed_bps, allow_bp);
		}
		if (m_slots[it].isPrefixDeadlocked(allowed_bps))
			this->m_0HitSlot = true;
		else
			perform0HitTest(m_slots[it], dics, non_them_ok, allowed_bps);
		if (m_slots[it].getCol() < g_pm.getStartCol() ||
			m_slots[it].getCol() >= g_pm.getStartCol() + g_pm.getHorizLen())
			continue;
		if (m_slots[it].getRow() < g_pm.getStartRow() ||
			m_slots[it].getRow() >= g_pm.getStartRow() + g_pm.getVertLen())
			continue;
		//if (m_slots[it].getCol() > 0 && m_slots[it].getRow() > 0)
		//	continue;
		if (!m_slots[it].compatWithCornerSel())
			continue;

		// order uninstantiated slots heuristically
		//double bal = this->countBlanksAdjToSlot(it);
		//double score = this->getSlotThemScore(it) / (1 + bal * bal);
		double score = 15 - this->weightedSumLetters(it);
		score = computeSlotScore(it);
		int block_start, nr_words;
		m_slots[it].getMinMatchingWords(block_start, nr_words, dics);
		assert(block_start != -1);
		//cerr << "Slot: <" << m_slots[it].getSimplePattern() << "> ; ";
		//cerr << "Score: " << score << endl;
		IDPair pair;
		pair.int_member = it;
		pair.dbl_member = score + (.05*m_slots[it].getMinDistToBorder());
		tmp_vect.push_back(pair);
		// check if this is the best slot in the generic sense
		//if (!m_slots[it].needsBP(dics))
		//    score += 20;
		//		if (nr_words < this->m_minSlotScore && m_slots[it].getCol() < 6 && m_slots[it].getRow() < 6) {
		double slot_score = nr_words + (.05*m_slots[it].getMinDistToBorder());
		if (slot_score < this->m_minSlotScore)
		{
			m_idxSelSlotGen = it;
			m_posInSelSlot = block_start;
			this->m_minSlotScore = slot_score;
			//m_branchingFactor = m_slots[it].getNrMatchingWords();
		}
	}
	//if (!m_slots[0].getPattern().isInstantiated())
	//	m_idxSelSlotGen = 0;
	//if (this->m_idxSelSlotGen != -1)
	//    this->m_minSlotScore = m_slots[m_idxSelSlotGen].getNrMatchingWords();
	if (this->m_idxSelSlotGen != -1)
		this->m_themSlotOrder.push_back(this->m_idxSelSlotGen);
	sort(tmp_vect.begin(), tmp_vect.end());
	for (int it = 0; it < tmp_vect.size(); it++)
	{
		//cerr << "Pair " << tmp_vect[it].dbl_member << " ; " << tmp_vect[it].int_member << endl;
		this->m_themSlotOrder.push_back(tmp_vect[it].int_member);
	}
}

int State::getEstimBPs(const WordSlot &slot)
{
	string pat = slot.getSimplePattern();
	int length = slot.getLength();
	for (int p = 0; p < length - 1; p++)
	{
		if (pat[p] == BLANK)
		{
			int row = slot.getRow();
			int col = slot.getCol();
			if (slot.getDirection() == HORIZONTAL)
				col += p;
			else
				row += p;
			if (!this->isBPLegal(row, col))
				continue;
		}
		WordSlot subslot1 = slot.getSubslot(0, p);
		WordSlot subslot2 = slot.getSubslot(p + 1, length - p - 1);
		if (this->hasMatchingWord(subslot1) && this->hasMatchingWord(subslot2))
			return 1;
	}
	if (g_pm.getVerbosity() >= 3)
	{
		cerr << "Slot that requires two or more BPs:" << endl;
		cerr << "<" << slot.getSimplePattern() << ">" << endl;
		cerr << *this << endl;
	}
	return 2;
}

void State::perform0HitTest(WordSlot &slot, vector<Dictionary> &dics,
							bool non_them_ok, vector<int> &allowed_bps)
{
#ifndef NO_BPS
	if (slot.hasSolidBlockDeadlock(dics, non_them_ok, allowed_bps))
		this->m_0HitSlot = true;
#endif
}

bool State::addForcedBPs()
{
	bool changes_made = false;
	while (true)
	{
		bool new_change = this->addAtMostOneForcedBP();
		if (new_change)
			changes_made = true;
		else
			break;
	}
	return changes_made;
}

bool State::addAtMostOneForcedBP()
{
	for (int it = 0; it < m_slots.size(); it++)
	{
		const WordSlot &sl = m_slots[it];
		if (sl.getLength() <= 2)
			continue;
		vector<Dictionary> &dics =
			(sl.getDirection() == HORIZONTAL) ? (m_horizDics) : (m_vertDics);
		vector<int> allowed_bps = this->getLegalBPPos(sl);
		//        for (int k = 0; k < allowed_bps.size(); k++) {
		//            int idx = allowed_bps[k];
		for (int idx = 0; idx < sl.getLength(); idx++)
		{
			if (sl.getLetter(idx) != BLANK)
				continue;
			if (sl.isForcedBP(dics, allowed_bps, idx))
			{
				if (g_pm.getVerbosity() >= 3)
				{
					cerr << "Forced BP at row " << sl.getCellRow(idx);
					cerr << " ; col " << sl.getCellCol(idx) << endl;
					cerr << "Slot <" << sl.getPattern().getSimplePattern()
						 << ">" << endl;
					cerr << *this << endl;
				}
				assert(m_grid[sl.getCellRow(idx)][sl.getCellCol(idx)] != BLACKPOINT);
				if (m_grid[sl.getCellRow(idx)][sl.getCellCol(idx)] != BLACKPOINT)
				{
					this->addBlackPoint(sl.getCellRow(idx), sl.getCellCol(idx));
					if (this->m_procFromScratch)
						collectSlots();
					else
						updateSlots();
					return true;
				}
			}
		}
	}
	return false;
}

unsigned int State::m_nrpaScoreKind;
unsigned int State::m_nrRows;
unsigned int State::m_nrCols;
vector<Dictionary> State::m_horizDics;
vector<Dictionary> State::m_vertDics;
bool State::m_allowNewBlackPoints;
bool State::m_allowTwoChars[2];
char State::m_initGrid[MAX_NR_ROWS][MAX_NR_COLS];
//unsigned int State::m_conflictDetection;
vector<StaticSlotStats> State::m_slotStats;
int State::m_propagationOrdering;
Statistics State::m_propIterationStats;
