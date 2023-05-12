#include <iostream>
#include "util.h"
#include "parammanager.h"
#include <stdlib.h>
#include "globals.h"
#include "Experiment.h"

using namespace std;

int main(int argc, char * const argv[]) {
    std::cout << "\033[1;31mWombat DBC version 3.1\033[0m\n";
    if (!g_pm.readParams(argc, argv)) {
        g_pm.displayUsage();
        exit(0);
    }
    Experiment exp;
    if (g_pm.getSearch() == 15)
        exp.staticRootAnal();
    else
        exp.runDFS();
    cout.flush();
    return 0;
}
