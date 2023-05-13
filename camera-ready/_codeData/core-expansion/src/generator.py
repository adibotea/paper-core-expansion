import graph

class Generator:

    def __init__(self):
        self.counter = 0
        self.local_counter = 0
        self.nodes = 0

    def reset_local_counter(self):
        self.local_counter = 0
        self.nodes = 0

    def expand_grids(self, ds, state, max_counter):
        """Recursive, depth-first search that expands seeds into full words

        Args:
            ds (DataStore): DataStore object
            state (State): parent state
        """
        if (self.local_counter >= 1000):
            return
        if state.has_duplicate_words():
            return
        if (state.slot_to_expand > 0):
            state.extract_lines()
            if state.has_illegal_sequence(state.lines, ds):
                #print ("illegal state: ")
                #state.print_state_2()
                return
        if state.is_goal():
            state.extract_lines()
            #print ("goal state: ")
            #state.print_state_2()
            # state.print_state()
            self.local_counter += state.write_shifted_pzls(self.counter, ds)
            self.counter += 1
        else:
            if (state.slot_to_expand > 0):
                state.extract_lines()
                #print ("Expanding state: ")
                #state.print_state()
                #state.print_state_2()
            self.nodes += 1
            successors = state.expand(ds.them_dict)
            for succ in successors:
                self.expand_grids(ds, succ, max_counter)
        return (self.counter, self.local_counter)
