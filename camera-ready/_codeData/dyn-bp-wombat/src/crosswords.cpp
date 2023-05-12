#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <limits.h>

/* for setpriority */
#include <sys/time.h>
#include <sys/resource.h>

#include "Experiment.h"

using namespace std;

int main(int argc, char *argv[])
{
      std::cout << "\033[1;31mWombat NRPA, version 0.4\033[0m\n";
      Options options = Options::parse(argc, argv);
      Experiment exp;

      if (argc > 1)
      {
            string arg = string(argv[1]);

            if (arg == "nrpa")
                  exp.runNRPA(options);
            else if (arg == "dsa")
                  exp.evalDelayedStaticAnal(options);
            else if (arg == "ddb")
                  exp.evalDeadlockDB(options);
            else if (arg == "pwbp")
                  exp.evalPruneWithBPs(options);
            else if (arg == "corner1")
                  exp.runDFSCorner(options, 1);
            else if (arg == "corner2")
                  exp.runDFSCorner(options, 2);
            else if (arg == "corner3")
                  exp.runDFSCorner(options, 3);
            else if (arg == "corner4")
                  exp.runDFSCorner(options, 4);
            else if (arg == "ild")
                  exp.evalILD(options);
            else if (arg == "dfs100")
                  exp.runDFS100Times(options);
      }
      else
      {
            cerr << "Running DFS" << endl;
            exp.runDFS(options);
            //exp.evalPayloadPruning(options);
            //exp.runWithDetailedLog(options);
            //exp.evalDicSearch(options);
            //exp.evalDelayedStaticAnal(options);
            //exp.evalDeadlockDB(options);
            //exp.evalPruneWithBPs(options);
            //exp.stanardEval(options); // this runs the original code in the main function
      }
}
