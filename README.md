# Flow Free Solver Algorithm with Analysis
This project was part of an algorithms subject with the aim being to implement Dijkstra's algorithm to solve the game Flow Free. Information on this game can be found [here](https://en.wikipedia.org/wiki/Flow_Free). The code was base was adapted (by the subject) from the open-source code made available by [mzucker](https://github.com/mzucker/flow_solver). The code written by me includes the **game_dijkstra_search** function in **search.c**, part of the **game_check_deadends** function in **extensions.c** and the code to output data to csv in **flow_solver.c** (commented out). I also include a Jupyter notebook for dealing with the csv file output to produce complexity graphs.
## Running this Program
To run the flow executable (can be created using the Makefile) use:
```
./flow [options]  <puzzleName1> ... <puzzleNameN>
```
for example:
```
 ./flow puzzles/regular_5x5_01.txt
 ```
 Running `./flow -h` will bring up the various options that can change how to sovler is executed. The `-d` flag is used to activate the deadend pruning, used in the analysis to compare with non-deadend pruning.

 To create an output csv file, uncomment the required sections in **flow_solver.c** and run any puzzles (will buildup file each run). To generate plots, run the notebook with this output file. This will average the data for each puzzle to ensure accuracy on the run time.
