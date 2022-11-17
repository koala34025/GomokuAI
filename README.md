# GomokuAI
An AI based on Minimax and Alpha-Beta pruning algorithms plays the board game gomoku.
## Features
* State Value Function
    * Check the existence of some specific chess patterns and give score with them
* Minimax algorithm
    * Optimized with Alpha-Beta Pruning algorithm
* Ordered Move
    * Heuristic Function
    * Sort valid movements according to its heuristic value
* Deepening search
    * Gradually search with deeper depth if it still has time
## Results
* 100 wins out of 100 games as first hand against baseline1
    * 23.24 steps to win a round in average 
    * In face, there are only 66 non-duplicated rounds
* 100 wins out of 100 games as second hand against baseline1
    * 21.32 steps to win a round in average
    * In fact, there are only 23 non-duplicated rounds 
* 100% win rates no matter first or second hand against baseline2
    * Every round is identical
* Eventually, my AI beated all 5 baselines and ranked top 7 in the class
## Screenshot
<img src="https://camo.githubusercontent.com/078d4831ace55500e9698ce209e312b9e94a86e1d0cc4193143e52db9f7055d7/68747470733a2f2f692e696d6775722e636f6d2f375553787734702e706e67" alt="" data-canonical-src="https://i.imgur.com/7USxw4p.png" width="50%" height="50%">
