import copy

class Graph:
    
    def __init__(self, lines):
        self.lines = lines
        self.height = len(lines)
        for line in lines:
            if '@' in line:
                self.width = len(line)

    def flood(self, row, col):
        open = [(row, col)]
        closed = set()
        while (len(open) > 0):
            top = open.pop(0)
            #print ("Expanding " + str(top))
            succs = self.gen_successors(top)
            for succ in succs:
                if succ in open or succ in closed:
                    continue
                open.append(succ)
            closed.add(top)
            #print ("Open " + str(open))
            #print ("Closed " + str(closed))
        return len(closed)


    def gen_successors(self, position):
        result = []
        row = position[0]
        col = position[1]
        assert (row >= 0)
        assert (col >= 0)
        for r in [-1, 1]:
            if row == 0 and r == -1:
                continue
            if row == self.height - 1 and r == 1:
                continue
            if (self.lines[row + r])[col] == '@':
                continue
            assert (row + r >= 0)
            result.append((row + r, col))
            #print("Generated successor " + str((row + r, col)))
        for c in [-1, 1]:
            if col == 0 and c == -1:
                continue
            if col == self.width - 1 and c == 1:
                continue
            if (self.lines[row])[col + c] == '@':
                continue
            assert (col + c >= 0)
            result.append((row, col + c))
            #print("Generated successor " + str((row, col + c)))
        return result

    def get_area(self):
        return self.height*self.width

    def get_nr_black_cells(self):
        result = 0
        for line in self.lines:
            result += line.count('@')
        return result

    def has_closure(self):
        row = 0
        col = 0
        while (self.lines[0][col] == "@"):
            col += 1
        assert (col >= 0)
        assert (col < len(self.lines[0]))
        flood_area = self.flood(row, col)
        total_area = self.get_area()
        nrbc = self.get_nr_black_cells()
        result = (int(flood_area) + int(nrbc) < int(total_area))
        #if result:
        #    print ("Flooded area: " + str(flood_area))
        #    print ("Total area: " + str(total_area))
        #    print ("Black cells: " + str(nrbc))
        #    print (". "*(len(self.lines[0]) + 2))
        #    for line in self.lines:
        #        print (". " + " ".join(line).upper() + " .")
        #    print (". "*(len(self.lines[0]) + 2))
        return result

    def has_closure2(self):
        result = False
        row = 0
        col = 0
        while (self.lines[0][col] == "@"):
            col += 1
        assert (col >= 0)
        assert (col < len(self.lines[0]))
        #print ("line: .%s. %d" %(self.lines[0], col))
        assert (col >= 0)
        flood_area = self.flood(row, col)
        total_area = self.get_area()
        nrbc = self.get_nr_black_cells()
        result = (int(flood_area) + int(nrbc) < int(total_area) - 1) and flood_area > 1
        #if result:
            #print ("Flooded area: " + str(flood_area))
            #print ("Total area: " + str(total_area))
            #print ("Black cells: " + str(nrbc))
            #print (". "*(len(self.lines[0]) + 2))
            #for line in self.lines:
            #    print (". " + " ".join(line).upper() + " .")
            #print (". "*(len(self.lines[0]) + 2))
        return result

    def has_semiclosure(self):
        for r in range(self.height):
            for c in range(self.width):
                lines = copy.deepcopy(self.lines)
                ll = list(lines[r])
                ll[c] = '@'
                lines[r] = "".join(ll)
                g = Graph(lines)
                if g.has_closure2():
                    return True
        return False