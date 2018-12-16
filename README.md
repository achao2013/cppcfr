cppcfr
=====

A c++ implementation of Counterfactual Regret Minimization (CFR) [1,4] for flop-style poker games like Texas Hold'em, Leduc, and Kuhn poker. The library currently implements vanilla CFR [1], 


c++ using demo
--------------
```
make all -j8
source bashrc_cfr
./build/tests/test_gametree
./build/tests/test_strategy
./build/tests/test_cfr
```

python call demo
----------------
python interface with pybind11 which support c++11 well
```
python python/check_pokertrees.py
```


**\[Todo\]**
- [ ] paralleling walking through publictree
- [ ] Chance Sampling (CS) CFR [1,2], Outcome Sampling (CS) CFR [2], and Public Chance Sampling (PCS) CFR [3].
- [ ] reach-maxmargin refinement[5]



Contributors
------------
Achao


Acknowledgement
---------------
thanks for the python implemention [pycfr](https://github.com/tansey/pycfr)

References
----------
[1] Zinkevich, M., Johanson, M., Bowling, M., & Piccione, C. (2008). Regret minimization in games with incomplete information. Advances in neural information processing systems, 20, 1729-1736.

[2] Lanctot, M., Waugh, K., Zinkevich, M., & Bowling, M. (2009). Monte Carlo sampling for regret minimization in extensive games. Advances in Neural Information Processing Systems, 22, 1078-1086.

[3] Johanson, M., Bard, N., Lanctot, M., Gibson, R., & Bowling, M. (2012). Efficient Nash equilibrium approximation through Monte Carlo counterfactual regret minimization. In Proceedings of the 11th International Conference on Autonomous Agents and Multiagent Systems-Volume 2 (pp. 837-846). International Foundation for Autonomous Agents and Multiagent Systems.

[4] Johanson, M., Waugh, K., Bowling, M., & Zinkevich, M. (2011). Accelerating best response calculation in large extensive games. In Proceedings of the Twenty-Second international joint conference on Artificial Intelligence-Volume Volume One (pp. 258-265). AAAI Press.

[5] Noam Brown, Tuomas Sandholm (2017). safe and nested endgame solving for Imperfect-Information Games. In Proceedings of the Twenty-Second international joint conference on neural information processing systems.
