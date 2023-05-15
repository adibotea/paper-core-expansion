# Introduction

This folder contains code and data for the paper ``Core Expansion in Optimization Crosswords`` published in ``SoCS 2023``.

# Contents

The contents of the folder is the following:

- Folder `core-expansion`. This contains the following sub-folders:

  - `core-expansion/src` is python code that generates expanded cores and seeds;

  - `core-expansion/y_13_6x4-2` contains sample scripts and data to actually run the code (i.e., the python code, and the `wombat` and `dyn-wombat` binaries) and generate seeds. The naming of the subfolder contains year `13` and pattern `6x4-2`. The code inside uses this name to automatically infer the year and the pattern. To run experiments for other years and/or patterns, create a new copy of the subfolder, and name it accordingly. For the year, choose any of `07`, `08`, `11`, `13`, `14`, `16`, `17`, `18`, `19`, `21`, `23`. Patterns available are `6x4-2`, `5x5-2`, `5x5-1`, `5x4-1`, `5x4-0`, `4x4-0`, `5x3-0`.

- Folder `seed-evolution` contains the code and data to evolve valid seeds using MAP-Elites.

# Running the code

- Go to a folder such as `core-expansion/y_13_6x4-2`. 

- Run `gen-dicts.bash` to generate some dictionaries (lists of substrings of full words) needed to generate cores. The dictionaries created will be saved in subfolder `input-data`.

- Run `gen-seeds.bash` to generate seeds. For some years and patterns, this can take a long time. The seeds will be zipped in a `.tgz` file in the folder at hand (where the script is run).

- Run `mrmeGrids.m` from MATLAB to run seed evolution.

# Disclaimer

All the code is research code that comes AS-IS with no guarantees or explicit or implied liability of any kind.

Everything in the folder `seed-evolution` is licensed under the Apache 2.0 license. See the file `seed-evolution/license.txt` for details.
