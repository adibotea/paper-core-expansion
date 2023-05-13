/*
 *  wordslot.cpp
 *  combus
 *
 *  Created by Adi Botea on 22/04/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "wordslot.h"
#include "state.h"
#include <algorithm>

using namespace std;

WordSlot::WordSlot(int row, int col, bool dir, const string &regex, int id)
: m_pattern(regex), m_row(row), m_col(col), m_direction(dir),
m_nrMatchingWords(-1), m_nrThemWords(-1), m_impactAreaSize(0), m_domainIsComputed(false), m_id(id),
m_allowBPs(true) {
    if (dir == HORIZONTAL) {
        for (int it = 0; it < regex.length(); it++) {
            m_cells.push_back(Cell(row, col + it));
        }
    } else {
        for (int it = 0; it < regex.length(); it++) {
            m_cells.push_back(Cell(row + it, col));
        }
    }
}

void WordSlot::computeDomainDBP(const vector<Dictionary> &dics,
        bool non_them_ok,
        vector<int> & legal_bps, bool bp_allowed) {
    m_domain.clear();
    this->m_hasThemWords = false;
    this->m_first_bp = this->getLength();
    for (vector<Dictionary>::const_iterator it = dics.begin(); it != dics.end(); it++) {
        vector<int> domain;
        vector<int> word_length;
        const WordPattern & pattern = m_pattern;
        int first_blank_pos = (legal_bps.size() > 0) ? (legal_bps[0]) : this->getLength();
        //first_blank_pos = pattern.firstBlankPos();
        int min_len = (bp_allowed) ? first_blank_pos : this->getLength();
        if (min_len <= 2)
            this->m_first_bp = min_len;
        for (int len = min_len; len <= pattern.getLength(); len++) {
            if (len <= 2) {
                continue;
            }
            if (len < pattern.getLength() &&
                    (find(legal_bps.begin(), legal_bps.end(), len) == legal_bps.end()))
                continue;
            if (len >= MIN_LENGTH_NTW && !non_them_ok &&
                    !it->isThematic())
                continue;
            WordPattern this_pat = pattern.getPrefix(len);
            vector<int> this_dom = it->expandPatternIndex(this_pat);
            if (this_dom.size() > 0 && it->isThematic())
                this->m_hasThemWords = true;
            if (this_dom.size() > 0 && len < this->m_first_bp)
                this->m_first_bp = len;
            domain.insert(domain.end(), this_dom.begin(), this_dom.end());
            for (int kk = 0; kk < this_dom.size(); kk++)
                word_length.push_back(len);
        }
        m_domain.push_back(domain);
        this->m_word_length.push_back(word_length);
    }
    m_domainIsComputed = true;
    setNrMatchWordsToDom();
    //cerr << "Slot: " << this->m_pattern.getSimplePattern() << " ; domain size: ";
    //cerr << m_domain[0].size() << "  " << m_domain[1].size() << endl;
    //cerr << endl;
}

void WordSlot::computeImpactAreaSize(const vector<WordSlot> & slots) {
    m_impactAreaSize = 0;
    for (vector<WordSlot>::const_iterator it = slots.begin();
            it != slots.end(); it++) {
        if (slotsCrossEachOther(*it))
            m_impactAreaSize += it->getLength();
    }
}

void WordSlot::downwardsPropBPS(const vector<Dictionary> & dics, int first_blank_pos) {
    assert(dics.size() == m_domain.size());
    if (first_blank_pos <= 2)
        return;
    // Reset only the prefix, so that we recompute the domains of these prefix cells
    m_pattern.setMultiCharPrefix(first_blank_pos, false);
    for (int it = 0; it < dics.size(); it++) {
        for (int idx = 0; idx < m_domain[it].size(); idx++) {
            string word = dics[it].getWord((m_domain[it])[idx], (m_word_length[it])[idx]);
            for (int pos = 0; pos < first_blank_pos; pos++) {
                char charIdx = toupper(word[pos]) - 'A';
                m_pattern.setMultiCharValue(pos, charIdx, true);
            }
        }
    }
}

bool WordSlot::slotsCrossEachOther(const WordSlot & slot) const {
    if (this->getDirection() == slot.getDirection())
        return false;
    const WordSlot & horizSlot = (m_direction == HORIZONTAL) ? (*this) : slot;
    const WordSlot & vertSlot = (m_direction == VERTICAL) ? (*this) : slot;

    if (horizSlot.getRow() < vertSlot.getRow() ||
            horizSlot.getRow() >= vertSlot.getRow() + vertSlot.getLength())
        return false;

    if (vertSlot.getCol() < horizSlot.getCol() ||
            vertSlot.getCol() >= horizSlot.getCol() + horizSlot.getLength())
        return false;

    return true;
}

/*
Given a pattern, this method tells what characters are legal on
 the blank cells of the pattern.
 */
void WordSlot::updateMultiCharPatternFromDictionaries(const vector<Dictionary> & dics) {
    WordPattern finalPattern(m_pattern.getLength());
    finalPattern.setAllMultiChar(false);
    m_nrMatchingWords = 0;
    for (vector<Dictionary>::const_iterator it1 = (dics).begin();
            it1 != (dics).end(); it1++) {
        WordPattern pattern(m_pattern);
        m_nrMatchingWords += it1->fillMultiCharPattern(pattern);
        finalPattern.addMultiCharValues(pattern);
    }
    m_pattern.setAllMultiChar(false);
    m_pattern.addMultiCharValues(finalPattern);
}

void WordSlot::updateMultiCharPatternFromGridMap(const MultiCharMap & map) {
    m_pattern.updateMultiCharPatternFromGridMap(map, m_row, m_col,
            m_direction, getLength());
}

int WordSlot::getFirstBP() const {
    if (this->getLength() <= 2) {
        // Not sure what exactly should I do here.
        // Returning 0 is correct, but not sure if more effective values
        // can be set.
        return 0;
    } else {
        return this->m_first_bp;
    }
}

int WordSlot::getNrBlanks() const {
    int result = 0;
    for (int i = 0; i < this->getLength(); i++) {
        if (this->getLetter(i) == BLANK)
            result++;
    }
    return result;
}

bool WordSlot::good4TopLeftCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row > 3)
            return false;
        if (m_col > 0)
            return false;
    } else {
        if (m_col > 3)
            return false;
        if (m_row > 0)
            return false;
    }
    return true;
}

bool WordSlot::good4TopRightCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row > 3)
            return false;
        if (m_col + this->getLength() < 12)
            return false;
    } else {
        if (m_col < 10)
            return false;
        if (m_row > 0)
            return false;
    }
    return true;
}

bool WordSlot::good4BottomRightCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row < 7)
            return false;
        if (m_col + this->getLength() < 12)
            return false;
    } else {
        if (m_col < 7)
            return false;
        if (m_row + this->getLength() < 12)
            return false;
    }
    return true;
}

bool WordSlot::good4BottomLeftCorner() const {
    if (this->m_direction == HORIZONTAL) {
        if (m_row < 9)
            return false;
        if (m_col > 0)
            return false;
    } else {
        if (m_col >= 4)
            return false;
        if (m_row + this->getLength() < 12)
            return false;
    }
    return true;
}

bool WordSlot::needsBP(vector<Dictionary> & dics) const {
    const WordPattern & pattern = this->getPattern();
    //cerr << "Slot with index " << it << endl;
    int len = this->getLength();
    if (len < 3)
        return false;
    if (pattern.isInstantiated())
        return false;
    for (vector<Dictionary>::const_iterator it1 = dics.begin();
            it1 != dics.end(); it1++) {
        if (it1->atLeast1Hit(pattern)) {
            return false;
        }
    }
    return true;
}

WordSlot WordSlot::getSubslot(int start, int length) const {
    WordSlot res;
    res.m_direction = this->m_direction;
    res.m_pattern = this->m_pattern.getSubpattern(start, length);
    if (this->m_direction == HORIZONTAL) {
        res.m_col = this->m_col + start;
        res.m_row = this->m_row;
        for (int ii = 0; ii < this->m_cells.size(); ii++) {
            assert(m_cells[ii].m_col == m_col + ii);
            if (start <= ii && ii < start + length)
                res.m_cells.push_back(m_cells[ii]);
        }
    } else {
        assert(this->m_direction == VERTICAL);
        res.m_col = this->m_col;
        res.m_row = this->m_row + start;
        for (int ii = 0; ii < this->m_cells.size(); ii++) {
            assert(m_cells[ii].m_row == m_row + ii);
            if (start <= ii && ii < start + length)
                res.m_cells.push_back(m_cells[ii]);
        }
    }
    res.m_domain.clear();
    res.m_domainIsComputed = false;
    res.m_word_length.clear();
    return res;
}

vector<WordSlot> WordSlot::split(int bp_row, int bp_col) const {
    vector<WordSlot> result;
    if (this->m_direction == HORIZONTAL) {
        assert(bp_row == this->m_row);
        assert(this->m_col <= bp_col && bp_col < this->m_col + this->getLength());
        int length1 = bp_col - m_col;
        int length2 = m_col + this->getLength() - bp_col - 1;
        if (length1 >= 1)
            result.push_back(this->getSubslot(0, length1));
        if (length2 >= 1)
            result.push_back(this->getSubslot(bp_col - this->m_col + 1, length2));
    }
    if (this->m_direction == VERTICAL) {
        assert(bp_col == this->m_col);
        assert(this->m_row <= bp_row && bp_row < this->m_row + this->getLength());
        int length1 = bp_row - m_row;
        int length2 = m_row + this->getLength() - bp_row - 1;
        if (length1 >= 1)
            result.push_back(this->getSubslot(0, length1));
        if (length2 >= 1)
            result.push_back(this->getSubslot(bp_row - this->m_row + 1, length2));
    }
    return result;
}

void WordSlot::setNrMatchingWords(int value) {
    m_nrMatchingWords = value;
}

void WordSlot::setNrMatchingWordsFromDics(const vector<Dictionary> & dics,
        bool non_them_ok, vector<int> & legal_bps,
        bool bp_allowed) {
    const WordPattern & pat = this->getPattern();
    this->m_nrThemWords = 0;
    this->m_nrMatchingWords = 0;
    // Generate words of variable lengths within this slot.
    // TODO: Currently inefficient because we don't check *early enough* 
    // if some of these would introduce adjacent black points 
    int min_len = (bp_allowed) ? 3 : pat.getLength();
    for (unsigned int len = min_len; len <= pat.getLength(); len++) {
        if (len < pat.getLength() &&
                find(legal_bps.begin(), legal_bps.end(), len) == legal_bps.end())
            continue;
        WordPattern this_pat = pat.getPrefix(len);
        for (vector<Dictionary>::const_iterator it1 = dics.begin();
                it1 != dics.end(); it1++) {
            if (len >= MIN_LENGTH_NTW && !non_them_ok && !it1->isThematic())
                continue;
            int nr_words = it1->getNrWordsMatchingPattern(this_pat);
            this->m_nrMatchingWords += nr_words;
            if (it1->isThematic())
                this->m_nrThemWords += nr_words;
        }
    }
}

void WordSlot::setNrMatchWordsToDom() {
    if (m_domain.size() == 0) {
        m_nrMatchingWords = -1;
        return;
    }
    m_nrMatchingWords = 0;
    m_nrThemWords = 0;
    for (int it = 0; it < m_domain.size(); it++) {
        m_nrMatchingWords += m_domain[it].size();
        if (it == 0)
            m_nrThemWords += m_domain[it].size();
    }

    //if (this->m_pattern.isBlank())
    //    cerr << "--- Setting nr matching words to " << this->m_nrMatchingWords << endl;
    // QUICK HACK TO COUNT THE POSSIBILITY OF PUTTING A BLACK POINT ON THE THIRD POSITION:
    //if (this->getLength() > 2 && this->getSimplePattern()[2] == BLANK)
    //    m_nrMatchingWords++;
}

void WordSlot::upwardsPropagation(const vector<Dictionary> & dics) {
    //if (this->m_pattern.isBlank()) {
    //    cerr << "Slot [" << this->m_pattern.getSimplePattern() << "]" << endl;
    //    cerr << "Domain size before: " << this->m_domain[0].size() + this->m_domain[1].size() << endl;
    //}
    if (m_pattern.hasNoConstraints()) {
        //if (this->m_pattern.isBlank())
        //    cerr << "Domain size after: " << this->m_domain[0].size() + this->m_domain[1].size() << endl;
        return;
    }
    assert(dics.size() == m_domain.size());
    vector<int> tmp;
    for (int it = 0; it < dics.size(); it++) {
        tmp.clear();
        for (int idx = 0; idx < m_domain[it].size(); idx++) {
            if (m_pattern.matchesWord(dics[it].getWord((m_domain[it])[idx], (m_word_length[it])[idx]))) {
                tmp.push_back(m_domain[it][idx]);
            }
        }
        if (tmp.size() < m_domain[it].size()) {
            m_nrMatchingWords -= m_domain[it].size();
            m_nrMatchingWords += tmp.size();
            m_domain[it] = tmp;
        }
    }
}

bool WordSlot::isForcedBP(const vector<Dictionary> & dics,
        vector<int> & legal_bps, int idx) const {
    //bool debug = false;
    //if (this->getSimplePattern() == "     GD" && idx == 4) {
    //    cerr << "here" << endl;
    //    debug = true;
    //}
    vector<int> start_positions;
    start_positions.push_back(0);
    for (int i = 0; i < legal_bps.size(); i++)
        start_positions.push_back(legal_bps[i] + 1);
    start_positions.push_back(this->getLength() + 1);
    for (int i = 0; i < start_positions.size() - 1; i++) {
        int start_pos = start_positions[i];
        if (start_pos > idx)
            break;
        for (int j = i + 1; j < start_positions.size(); j++) {
            if (start_positions[j] - 1 <= idx)
                continue;
            int length = start_positions[j] - 1 - start_pos;
            if (length <= 2)
                return false;
            WordSlot sl = this->getSubslot(start_pos, length);
            vector<int> new_legal_bps;
            for (int k = 0; k < legal_bps.size(); k++)
                if (legal_bps[k] > start_pos + 1 &&
                        legal_bps[k] < start_pos + length - 1)
                    new_legal_bps.push_back(legal_bps[k] - start_pos);
            sl.setNrMatchingWordsFromDics(dics, true, new_legal_bps, false);
            if (sl.getNrMatchingWords() > 0) {
                //if (debug)
                //    cerr << sl.getSimplePattern() << endl;
                return false;
            }
        }
    }
    return true;
}

bool WordSlot::hasSolidBlockDeadlock(const vector<Dictionary> & dics,
        bool non_them_ok,
        vector<int> & legal_bps) const {
    if (this->m_pattern.getSimplePattern() == "     AC D CAA") {
        cerr << "here" << endl;
    }
    /*
     * Detect longest solid block
     */
    bool in_solid_block = false;
    int start_idx = -1;
    int length = -1;
    int tmp_start = -1;
    int tmp_length = -1;
    for (int i = 0; i < this->m_pattern.getLength(); i++) {
        if (!in_solid_block &&
                this->getLetter(i) != BLANK) {
            //                (find(legal_bps.begin(), legal_bps.end(), i) == legal_bps.end())) {
            in_solid_block = true;
            tmp_start = i;
        }
        if (in_solid_block &&
                (this->getLetter(i) == BLANK ||
                //                ((find(legal_bps.begin(), legal_bps.end(), i) != legal_bps.end()) || 
                (i == m_pattern.getLength() - 1)
                )
                ) {
            in_solid_block = false;
            tmp_length = i - tmp_start;
            if (i == m_pattern.getLength() - 1 && this->getLetter(i) != BLANK)
                tmp_length++;
            if (tmp_length > length) {
                length = tmp_length;
                start_idx = tmp_start;
            }
        }
    }
    if (length <= 2)
        return false;
    //cerr << "Slot " << m_pattern.getSimplePatternDots();
    //cerr << " ; start: " << start_idx << " ; length: " << length << endl;
    for (int ss = 0; ss <= start_idx; ss++) {
        if (ss > 0 &&
                (find(legal_bps.begin(), legal_bps.end(), ss - 1) == legal_bps.end()))
            continue; // not allowed to place a BP here
        for (int ee = start_idx + length; ee <= this->m_pattern.getLength(); ee++) {
            assert(ee - ss <= m_pattern.getLength());
            if (ee < m_pattern.getLength() &&
                    (find(legal_bps.begin(), legal_bps.end(), ee) == legal_bps.end()))
                continue; // not allowed to place a BP here
            WordPattern this_pat = this->m_pattern.getSubpattern(ss, ee - ss);
            int len = this_pat.getLength();
            for (int it = 0; it < dics.size(); it++) {
                if (len >= MIN_LENGTH_NTW && !non_them_ok &&
                        !dics[it].isThematic())
                    continue;
                if (dics[it].atLeast1Hit(this_pat))
                    return false;
            }
        }
    }
    //cerr << "Pattern with 0 hits: " << this->m_pattern.getSimplePatternDots() << endl;
    return true;
}

vector<int> WordSlot::getLetterBlockStarts() const {
	vector<int> result;
	for (int idx = 0; idx < this->getLength(); idx++) {
		if (this->getLetter(idx) != BLANK) {
			result.push_back(idx);
			while (idx < this->getLength() && this->getLetter(idx) != BLANK)
				idx++;
		}
	}
	return result;
}

vector<WordSlot> WordSlot::getSubslotsAroundPos(int pos) const {
	vector<WordSlot> result;
	for (int start = 0; start <= pos; start++) {
		if (start > 0 && this->getLetter(start - 1) != BLANK)
			continue;
		for (int endpos = pos; endpos < this->getLength(); endpos++) {
			if (endpos < this->getLength() - 1 && this->getLetter(endpos + 1) != BLANK)
				continue;
			WordSlot slot = this->getSubslot(start, endpos - start + 1);
			result.push_back(slot);
		}
	}
	return result;
}

void WordSlot::getMinMatchingWords(int& block_start, int& nr_words,
		const vector<Dictionary> & dics) {
	vector<int> legal_bps;
	vector<int> block_starts = this->getLetterBlockStarts();
	// handle case when the slot is empty:
	if (block_starts.size() == 0)
		block_starts.push_back(0);
	nr_words = 10000000;
	block_start = -1;
	for (int iter = 0; iter < block_starts.size(); iter++) {
		int count = 0;
		vector<WordSlot> subslots = this->getSubslotsAroundPos(block_starts[iter]);
		for(auto it = subslots.begin(); it != subslots.end(); it++) {
			it->setNrMatchingWordsFromDics(dics, true, legal_bps, false);
			count += it->getNrMatchingWords();
		}
		if (count < nr_words) {
			nr_words = count;
			block_start = block_starts[iter];
		}
	}
}

double WordSlot::getStaticScore() const {
    if (m_nrMatchingWords <= 1)
        return m_nrMatchingWords;
    if (g_pm.getSlotScoring() == 0)
        return this->m_nrMatchingWords;
    if (g_pm.getSlotScoring() == 1)
        return 1.01 + (m_nrMatchingWords/(1 + m_nrThemWords*m_nrThemWords));     
    int l2 = getLength() * getLength();
    if (g_pm.getSlotScoring() == 2)
        return 1.01 + (1.01 * m_nrMatchingWords - m_nrThemWords) / l2;
    if (g_pm.getSlotScoring() == 3)
        return 1.01 + (m_nrMatchingWords - m_nrThemWords);
    if (g_pm.getSlotScoring() == 4)
        return 1.0 + 1 / (1 + m_nrThemWords);
    if (g_pm.getSlotScoring() == 5)
        return 1.0 + ((double) m_nrMatchingWords)/l2; // original
    assert (false);
    return 1.0;
}

int WordSlot::getMinDistToBorder() const {
    int result = 0;
    if (this->getDirection() == HORIZONTAL) {
        result = (this->getRow() < MAX_NR_ROWS - this->getRow())?this->getRow():(MAX_NR_ROWS - this->getRow());
    }
    if (this->getDirection() == VERTICAL) {
        result = (this->getCol() < MAX_NR_COLS - this->getCol())?this->getCol():(MAX_NR_COLS - this->getCol());
    }
    return result;
}
