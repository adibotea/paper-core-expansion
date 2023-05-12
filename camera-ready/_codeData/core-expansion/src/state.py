
import copy
import re
import sys
import graph

class State:
    """ A class to implement a state
    """
    def __init__(self, horiz_seeds, vert_seeds, coreidx):
        self.horiz_seeds = horiz_seeds
        self.vert_seeds = vert_seeds
        self.nr_cols = len(self.vert_seeds)
        self.nr_rows = len(self.horiz_seeds)
        self.horiz_words = []
        self.vert_words = []
        self.horiz_offset = []
        self.vert_offset = []
        for idx in range(self.nr_rows):
            self.horiz_words.append("")
            self.horiz_offset.append(0)
        for idx in range(self.nr_cols):
            self.vert_words.append("")
            self.vert_offset.append(0)
        self.slot_to_expand = 0
        self.lines = []
        self.score = 0
        self.width = 0
        self.heigth = 0
        self.coreidx = coreidx

    def get_substrings(self, lines):
        """Compute all substrings in a list of input strings

        Args:
            lines (list): a list of strings

        Returns:
            list: All substrings computed with separators AT and BLANK
        """
        result = []
        for line in lines:
            substrings = re.split('@| ', line)
            for substring in substrings:
                result.append(substring.strip())
        return result

    def get_prefixes(self, lines):
        """Compute all prefixes in a list of input strings.
           A prefix is either a substring at the beginning of a line/column,
           or a substring started with AT

        Args:
            lines (list): a list of strings

        Returns:
            list: All prefixes
        """
        result = []
        for line in lines:
            substrings = re.split('@| ', line)
            for substring in substrings:
                if ("@" + substring in line):
                    result.append(substring.strip())
                if (line.startswith(substring)):
                    result.append(substring.strip())
        return result

    def get_suffixes(self, lines):
        """Compute all suffixes in a list of input strings.
           A suffix is either a substring at the end of a line/column,
           or a substring ending in AT

        Args:
            lines (list): a list of strings

        Returns:
            list: All suffixes
        """
        result = []
        for line in lines:
            substrings = re.split('@| ', line)
            for substring in substrings:
                if (substring + "@" in line):
                    result.append(substring.strip())
                if (line.endswith(substring)):
                    result.append(substring.strip())
        return result

    def get_column(self, lines, idx):
        """Extracts a column from a list of lines

        Args:
            lines (list): list of lines
            idx (int): index of the column to extract

        Returns:
            string: column computed
        """
        result = "          "
        for line in lines:
            line = line + "             "
            result = result + line[idx]
        return result + "          "

    def get_columns(self, lines):
        """Extracts columns from a list of lines

        Args:
            lines (list): list of lines

        Returns:
            list: columns computed
        """
        result = []
        max_col = max(len(line) for line in lines)
        for idx in range(13):
            result.append(self.get_column(lines, idx))
        return result        

    def add_horiz_word(self, word, idx):
        """Adds a horizontal word

        Args:
            word ([string]): [a word from the dictionary]
            idx ([int]): [the index where to add the word]
        """
        assert(self.horiz_seeds[idx] in word)
        self.horiz_words[idx] = word
        self.horiz_offset[idx] = word.index(self.horiz_seeds[idx])
        self.score += len(word.replace("@", "").strip())

    def add_vert_word(self, word, idx):
        """Adds a vertical word

        Args:
            word ([string]): [a word from the dictionary]
            idx ([int]): [the index where to add the word]
        """
        assert(self.vert_seeds[idx] in word)
        self.vert_words[idx] = word
        self.vert_offset[idx] = word.index(self.vert_seeds[idx])
        self.score += len(word.replace("@", "").strip())

    def word_matches_horiz_seed(self, word, idx):
        """Tells whether a word matches a given horizontal seed.
            That is, whether the seed is a substring of the word.

        Args:
            word ([string]): [a word from the dictionary]
            idx ([int]): [the index of the seed]

        Returns:
            [bool]: [true iff the word matches the seed]
        """
        return self.horiz_seeds[idx] in word

    def word_matches_vert_seed(self, word, idx):
        """Tells whether a word matches a given vertical seed.
            That is, whether the seed is a substring of the word.

        Args:
            word ([string]): [a word from the dictionary]
            idx ([int]): [the index of the seed]

        Returns:
            [bool]: [true iff the word matches the seed]
        """
        return self.vert_seeds[idx] in word

    def is_goal(self):
        """Checks if the state at hand has instantiated all substrings of the core

        Returns:
            Boolean: True iff all substrings of the core are instantiated
        """
        return self.slot_to_expand >= self.nr_cols + self.nr_rows

    def has_duplicate_words(self):
        """Checks whether a state has duplicate words

        Returns:
            Boolean: True iff duplicate words exist
        """
        hw = [w for w in self.horiz_words if w != ""]
        vw = [w for w in self.vert_words if w != ""]
        setw = set(hw + vw)
        return (len(setw) < len(hw) + len(vw))

    def expand(self, dict):
        """Expands the state with words that match the seeds

        Args:
            dict ([list]): [list of words]

        Returns:
            [list]: [list of successor states]
        """
        result = []
        if self.slot_to_expand < self.nr_rows:
            idx = self.slot_to_expand
            for word in dict:
                if self.horiz_seeds[idx] in word:
                    state = copy.deepcopy(self)
                    state.add_horiz_word(word, idx)
                    state.slot_to_expand = state.slot_to_expand + 1
                    result.append(state)
        else:
            idx = self.slot_to_expand - self.nr_rows
            #print ("idx = %d, nr rows = %d, nr cols = %d" %(idx, self.nr_rows, self.nr_cols))
            #print (self.vert_seeds)
            for word in dict:
                if self.vert_seeds[idx] in word:
                    state = copy.deepcopy(self)
                    state.add_vert_word(word, idx)
                    state.slot_to_expand = state.slot_to_expand + 1
                    result.append(state)
        return result

    def print_state(self):
        print ("Horizontal words:")
        print (self.horiz_words)
        print ("Vertical words:")
        print (self.vert_words)

    def print_state_2(self):
        print (". "*(len(self.lines[0]) + 2))
        for line in self.lines:
            print (". " + " ".join(line).upper() + " .")
        print (". "*(len(self.lines[0]) + 2))

    def is_oversize(self, lines):
        """Tells whether a state is too wide or too tall

        Args:
            lines (list): the lines of the state

        Returns:
            Bool: True iff the state is too wide or too tall
        """
        minidx = min(line.find("@") for line in lines if line.find("@") >= 0)
        maxidx = max(line.rfind("@") for line in lines if line.rfind("@") >= 0)
        #print ("maxidx = %d" %maxidx)
        #print ("minidx = %d" %minidx)
        return maxidx - minidx > 14

    def extract_lines(self):
        """Extracts the lines of the grid.
           All lines have the same length in the result of a given call to this method.
           The length is not necessarily the same from one state to another.
           The length of the lines can vary from one call to another (one state to another).
           The number of lines is not fixed.
        """
        self.lines = []
        shift = 16
        for row in range(shift):
            line = list("".ljust(2*shift))
            for col in range(len(self.vert_words)):
                if (self.vert_offset[col] >= shift - row):
                    line[shift + col] = self.vert_words[col][self.vert_offset[col] - shift + row]
            if "".join(line).strip() == "":
                continue
            self.lines.append("".join(line).upper())
        for idx in range(len(self.horiz_words)):
            word = self.horiz_words[idx]
            for yy in range(shift - self.horiz_offset[idx]):
                word = " " + word
            self.lines.append(word)
        for row in range(1,shift/2):
            line = list("".ljust(2*shift))
            for col in range(self.nr_cols):
                if (row <= len(self.vert_words[col]) - self.vert_offset[col] - self.nr_rows):
                    line[shift + col] = self.vert_words[col][self.vert_offset[col] + row + self.nr_rows - 1]
            if "".join(line).strip() == "":
                continue
            self.lines.append("".join(line).upper())
        lefttrim = min(line.index("@") for line in self.lines if "@" in line)
        for idx in range(len(self.lines)):
            self.lines[idx] = self.lines[idx][lefttrim:]
        leftkeep = max(line.rindex("@") for line in self.lines if "@" in line) + 1
        for idx in range(len(self.lines)):
            self.lines[idx] = (self.lines[idx] + "                             ")[:leftkeep]
        #self.print_state_2()


    def is_legal(self, lines, ds):
        """Checks if a partial grid is legal

        Args:
            lines (list): lines of partial grid
            ds (DataStore): DataStore object

        Returns:
            Boolean: Whether the state is legal or not
        """
        columns = self.get_columns(lines)
        if self.has_adjacent_bps(lines, columns):
            return False
        if self.has_illegal_substring(lines, columns, ds.substr3, ds.substr4, ds.substr5):
            return False
        if self.has_illegal_prefix(lines, columns, ds.prefix3, ds.prefix4):
            return False
        if self.has_illegal_suffix(lines, columns, ds.suffix3, ds.suffix4):
            return False
        g = graph.Graph(lines)
        if g.has_closure():
            #print("Illegal grid with closure")
            #self.print_state_2()
            return False
        if g.has_semiclosure():
            #print("Illegal grid with semi-closure")
            #self.print_state_2()
            return False
        return True

    def has_illegal_sequence(self, lines, ds):
        """Tells whether the state has an illegal sequence
           or it is too wide or too tall

        Args:
            lines (list): a list of lines
            ds (list): list of dictionary words 

        Returns:
            Bool: True if the state has an illegal sequence, or it is too wide or too tall
        """
        columns = self.get_columns(lines)
        #print ("lines: " + str(lines))
        if self.has_illegal_substring(lines, columns, ds.substr3, ds.substr4, ds.substr5):
            return True
        if self.has_illegal_prefix(lines, columns, ds.prefix3, ds.prefix4):
            return True
        if self.has_illegal_suffix(lines, columns, ds.suffix3, ds.suffix4):
            return True
        if self.is_oversize(lines):
            #print ("oversize width")
            return True
        if self.is_oversize(columns):
            #print ("columns: " + str(columns))
            #print ("oversize height")
            return True
        return False

    def has_adjacent_bps(self, lines, columns):
        """Checks whether adjacent black points are present

        Args:
            lines (list): lines of partial grid
            columns (list): columns of partial grid

        Returns:
            Boolean: Whether adjacent black points are present
        """
        for line in lines:
            if "@@" in line:
                return True
        for column in columns:
            if "@@" in column:
                return True
        return False


    def has_illegal_substring(self, lines, columns, list3, list4, list5):
        """Checks if a partial grid has an illegal substring

        Args:
            lines (list): list of (horizontal) lines of the partial grid
            columns (list): list of (vertical) columns of the partial grid
            list3 (list): length-3 substrings of all dictionary words
            list4 (list): length-4 substrings of all dictionary words

        Returns:
            Boolean: Whether an illegal substring is detected
        """
        h_s = (self.get_substrings(lines) + self.get_substrings(columns))
        for substring in h_s:
            if len(substring) == 3:
                if substring.strip().lower() not in list3:
                    #print ("substring %s is illegal" %substring)
                    return True
            if len(substring) == 4:
                if substring.strip().lower() not in list4:
                    #print ("substring %s is illegal" %substring)
                    return True
            if len(substring) == 5:
                if substring.strip().lower() not in list5:
                    #print ("substring %s is illegal" %substring)
                    return True
        return False

    def has_illegal_prefix(self, lines, columns, list3, list4):
        """Checks if a partial grid has an illegal prefix

        Args:
            lines (list): list of (horizontal) lines of the partial grid
            columns (list): list of (vertical) columns of the partial grid
            list3 (list): length-3 substrings of all dictionary words
            list4 (list): length-4 substrings of all dictionary words

        Returns:
            Boolean: Whether an illegal substring is detected
        """
        h_s = (self.get_prefixes(lines) + self.get_prefixes(columns))
        for substring in h_s:
            if len(substring) == 3:
                if substring.strip().lower() not in list3:
                    #print ("prefix %s is illegal" %substring)
                    return True
            if len(substring) == 4:
                if substring.strip().lower() not in list4:
                    #print ("prefix %s is illegal" %substring)
                    return True
        return False


    def has_illegal_suffix(self, lines, columns, list3, list4):
        """Checks if a partial grid has an illegal prefix

        Args:
            lines (list): list of (horizontal) lines of the partial grid
            columns (list): list of (vertical) columns of the partial grid
            list3 (list): length-3 substrings of all dictionary words
            list4 (list): length-4 substrings of all dictionary words

        Returns:
            Boolean: Whether an illegal substring is detected
        """
        h_s = (self.get_suffixes(lines) + self.get_suffixes(columns))
        for substring in h_s:
            if len(substring) == 3:
                if substring.strip().lower() not in list3:
                    #print ("suffix %s is illegal" %substring)
                    return True
            if len(substring) == 4:
                if substring.strip().lower() not in list4:
                    #print ("suffix %s is illegal" %substring)
                    return True
        return False

    def can_cut_left(self, lines):
        """Check if cutting the left logical column is possible

        Args:
            lines (list): partial grid as a list of lines

        Returns:
            Boolean: Whether cutting the left logical column is possible
        """
        for line in lines:
            if line[0] not in [" ", "@"]:
                return False
        return True


    def can_cut_right(self, lines):
        """Check if cutting the right logical column is possible

        Args:
            lines (list): partial grid as a list of lines

        Returns:
            Boolean: Whether cutting the right logical column is possible
        """
        for line in lines:
            if line[-1] not in [" ", "@"]:
                return False
        return True


    def can_cut_top(self, lines):
        """Check if cutting the top line is possible

        Args:
            lines (list): partial grid as a list of lines

        Returns:
            Boolean: Whether cutting the top line is possible
        """
        line = lines[0].replace("@", " ").strip()
        if line != "":
            return False
        return True


    def can_cut_bottom(self, lines):
        """Check if cutting the bottom line is possible

        Args:
            lines (list): partial grid as a list of lines

        Returns:
            Boolean: Whether cutting the bottom line is possible
        """
        line = lines[-1].replace("@", " ").strip()
        if line != "":
            return False
        return True


    def cut_left(self, lines):
        """Gives a copy of the input with the first logical column

        Args:
            lines (list): input list of lines

        Returns:
            list: a copy of the input list, with the first logical column removed
        """
        result = []
        for line in lines:
            result.append(line[1:] + " ")
        assert (len(result) == len(lines))
        for line in result:
            assert (len(line) == self.width)
        return result


    def cut_right(self, lines):
        """Gives a copy of the input with the last logical column

        Args:
            lines (list): input list of lines

        Returns:
            list: a copy of the input list, with the last logical column removed
        """
        result = []
        for line in lines:
            result.append(line[:-1])
        assert (len(result) == len(lines))
        return result


    def cut_top(self, lines):
        """Gives a copy of the input with the first line removed

        Args:
            lines (list): input list of lines

        Returns:
            list: a copy of the input list, with the first element removed
        """
        result = []
        for line in lines[1:]:
            result.append(line)
        assert (len(result) == len(lines) - 1)
        return result


    def cut_bottom(self, lines):
        """Gives a copy of the input with the last line removed

        Args:
            lines (list): input list of lines

        Returns:
            list: a copy of the input list, with the last element removed
        """
        result = []
        for line in lines[:-1]:
            result.append(line)
        assert (len(result) == len(lines) - 1)
        return result


    def shift_horizontally(self, lines, offset):
        """Shifts horizontally a partial grid.
           Cut trailing spaces so that the length of lines is preserved in the result.

        Args:
            lines (list): lines of the partial grid
            offset (int): shifting offset

        Returns:
            list: the result as a list of lines
        """
        if offset == -1 and self.can_cut_left(lines):
            return self.cut_left(lines)
        else:
            result = []
            for line in lines:
                nl = (" "*offset + line)
                max_accept_pos = max(13, self.width)
                #print ("." + nl + ". " + str(self.last_letter_pos(nl)) + " " + str(len(nl)))
                assert (self.last_letter_pos(nl) < max_accept_pos)
                result.append(nl)
            return result

    def last_letter_pos(self, line):
        return len(line.replace("@", " ").rstrip()) - 1


    def shift_vertically(self, lines, offset):
        """Shifts a partial grid vertically

        Args:
            lines (list): lines of the partial grid
            offset (int): shifting offset

        Returns:
            list: the result as a list of lines
        """
        result = []
        width = len(lines[0])
        if offset == -1 and self.can_cut_top(lines):
            result = self.cut_top(lines)
        else:
            for idx in range(offset):
                result.append(" "*width)
            for line in lines:
                result.append(line)
        if (len(result) >= 14) and self.can_cut_bottom:
                result = self.cut_bottom(result)
        while len(result) < 13:
            result.append(" "*width)
        assert (len(result) == 13)
        return result

    def write_shifted_pzls(self, counter, ds):
        """Shifts a partial grid horizontally and vertically,
           checks the correctness of the resulting grid,
           and writes each grid to a .pzl file,
           unless it's proven incorrect.

        Args:
            counter (int): an integer to be used as part of the id captured in the filename
            ds (DataStore): DataStore object

        Returns:
            int: the number of puzzles written to files
        """
        #print ("Shifting around state ")
        #self.print_state_2()
        for line in self.lines:
            assert (len(line) == len(self.lines[0]))
        self.width = len(self.lines[0]) 
        assert (self.width <= 15)
        result = 0
        for i in range(-1, 10):
            if i + self.width > 14:
                continue
            if (i + self.width == 14) and not self.can_cut_right(self.lines):
                continue
            for j in range(-1, 10):
                if j + len(self.lines) > 14:
                    continue
                lines = self.shift_horizontally(self.lines, i)
                lines = self.shift_vertically(lines, j)
                lines2 = []
                for line in lines:
                    assert (self.last_letter_pos(line) < 13)
                    l2 = line[:13].ljust(13, ' ')
                    assert (len(l2) == 13)
                    lines2.append(l2)
                if self.is_legal(lines2, ds):
                    filename = "puzzle-%d-%d-%d-%d-%d.pzl" %(self.score, self.coreidx, counter, i, j)
                    self.write_pzl(filename, lines2)
                    result += 1
        return result


    def write_pzl(self, filename, lines):
        """Writes a .pzl file

        Args:
            filename (string): path of the output file
            lines (list): the lines of the grid to write
        """
        extended_height = len(lines)
        extended_width = max(len(lines[i]) for i in range(len(lines)))
        assert (extended_height <= 13)
        f = open(filename, "w")
        f.write("13\n13\n1\n0\n0\n")
        for i in range(extended_height):
            this_line = " ".join(lines[i]).upper()
            assert (len(this_line) <= 26)
            this_line = (this_line + "                          ")[:26] + "\n"
            f.write(this_line)
        for i in range(13 - extended_height):
            f.write("                          \n")
        f.write("2\n1\nthem-dic-21.txt\n1\n1\n0\ndictionary.txt\n1\n1\n")
        f.close()