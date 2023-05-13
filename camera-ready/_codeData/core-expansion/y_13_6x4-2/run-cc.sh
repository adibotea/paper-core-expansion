#!/bin/bash
#SBATCH --account=def-bulitko
#SBATCH --cpus-per-task=1
#SBATCH --mem=1024M
#SBATCH --mail-user=adi@ualberta.net
#SBATCH --mail-type=ALL
#SBATCH --output=logfile-run-cc.log

#SBATCH --time=12:00:00


./gen-seeds.bash

