#!/bin/bash
#SBATCH --cpus-per-task=32
#SBATCH --mem=64G
#SBATCH --time=2-04:00      # time (DD-HH:MM)
#SBATCH --output=cc/mrmeGrids-%N-%j.out  # %N for node name, %j for jobID
#SBATCH --account=def-bulitko
module load matlab/2022b
matlab -nodesktop -r "mrmeGrids('$SLURM_JOB_ID'); quit"
