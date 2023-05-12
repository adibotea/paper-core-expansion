
import state
import generator
import datastore
import sys
import time
import logging


def read_cores(filename):
    """Read a list of cores from a file.
       A core is a K x L pattern (e.g., 6x4) with K horizontal slots
       and L vertical slots, each slot being a substring of a thematic word.

    Args:
        filename ([string]): [input filename]

    Returns:
        [list]: [a list of lists]
    """
    f = open(filename, "r")
    result = []
    horiz_slots = []
    vert_slots = []
    for x in f:
        substring = x.strip()
        if len(substring) > 0:
            horiz_slots.append(x.strip().lower())
        else:
            for colidx in range(len(horiz_slots[0])):
                word = ""
                for rowidx in range(len(horiz_slots)):
                    word = word + horiz_slots[rowidx][colidx]
                vert_slots.append(word)
            result.append((horiz_slots, vert_slots))
            horiz_slots = []
            vert_slots = []
    return result


def main():
    """Main function
    """
    logging.basicConfig(filename='corex.log', encoding='utf-8',
                        level=logging.INFO, format='%(asctime)s %(message)s')
    GENERATOR = generator.Generator()
    CORES = read_cores(sys.argv[1])
    DS = datastore.DataStore(sys.argv[2], sys.argv[3], sys.argv[4],
                             sys.argv[5], sys.argv[6], sys.argv[7], sys.argv[8], sys.argv[9])

    tt = len(CORES)
    logging.info(
        'Python Corex 1.0 -- Start generating expanded cores and seeds')
    total_seeds = 0
    total_exp_cores = 0
    coreidx = -1
    if len(sys.argv) == 11:
        coreidx = int(sys.argv[10])
    if coreidx == -1:
        for cc in range(tt):
            CORE = CORES[cc]
            start = time.time()
            STATE = state.State(CORE[0], CORE[1], cc)
            GENERATOR.reset_local_counter()
            (nr_exp_cores, nr_seeds) = GENERATOR.expand_grids(DS, STATE, 10)
            end = time.time()
            total_seeds += nr_seeds
            if cc % 1000 == 0:
                logging.info("Processed core %d out of %d in %.02f seconds. Generated %d expanded cores and %d seeds." % (
                    cc, tt, end - start, nr_exp_cores - total_exp_cores, nr_seeds))
            total_exp_cores = nr_exp_cores
    else:
        CORE = CORES[coreidx]
        start = time.time()
        STATE = state.State(CORE[0], CORE[1], coreidx)
        GENERATOR.reset_local_counter()
        (nr_exp_cores, nr_seeds) = GENERATOR.expand_grids(DS, STATE, 10)
        end = time.time()
        total_seeds += nr_seeds
        logging.info("Processed core %d out of %d in %.02f seconds. Generated %d expanded cores and %d seeds." % (
            coreidx, tt, end - start, nr_exp_cores - total_exp_cores, nr_seeds))
        total_exp_cores = nr_seeds

    logging.info('Python Corex 1.0 -- Generated %d expanded cores and %d seeds from %d cores' %
                 (total_exp_cores, total_seeds, tt))
    print('Python Corex 1.0 -- Generated %d expanded cores and %d seeds from %d cores' %
          (total_exp_cores, total_seeds, tt))


if __name__ == '__main__':
    main()
