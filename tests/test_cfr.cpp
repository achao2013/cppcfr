#include "pokerstrategy.hpp"
#include "pokergames.hpp"
#include "pokercfr.hpp"
#include <iostream>

using namespace rl;
using rl::GameRules;
using rl::CounterfactualRegretMinimizer;
using rl::StrategyProfile;
bool near(float val, float expected, float distance=0.0001)
{
	return val >= (expected - distance) && val <= (expected + distance);
}
int main()
{
	enum ACTION_TYPE{
		FOLD,
		CALL,
		RAISE
	};
	std::cout<<"Testing CFR"<<std::endl;

	std::cout<<"Computing NE for Half-Street Kuhn poker"<<std::endl;
	GameRules hskuhn = rl::half_street_kuhn_rules();
	//hskuhn = test_rules()
	CounterfactualRegretMinimizer cfr(hskuhn);
    std::cout<<cfr.node2typename[0]<<" "<<cfr.node2typename[1]<<" "<<cfr.node2typename[2]<<" "<<cfr.node2typename[3]<<" "<<std::endl;
	int iterations_per_block = 1000;
	int blocks = 10;
	std::cout<<"begin to iteration"<<std::endl;
	for(int block=0; block<blocks; block++)
	{
		std::cout<<"Iterations:"<<block * iterations_per_block<<std::endl;
		cfr.run(iterations_per_block);
		std::pair<StrategyProfile, vector<float> > result = cfr.profile.best_response();
		std::cout<<"Best response EV: "<<result.second<<std::endl;
		std::cout<<"Total exploitability:"<<std::accumulate(result.second.begin(), result.second.end(),0.0)<<std::endl;
	}
	std::cout<< cfr.profile.strategies[0].policy<<std::endl;
	std::cout<< cfr.profile.strategies[1].policy<<std::endl;
	std::cout<< cfr.counterfactual_regret<<std::endl;
	std::cout<< "Verifying P1 policy"<<std::endl;
	assert(near(cfr.profile.strategies[0].policy["Q:/:"][CALL], 2.0 / 3.0, 0.01));
	assert(near(cfr.profile.strategies[0].policy["Q:/:"][RAISE], 1.0 / 3.0, 0.01));
	assert(near(cfr.profile.strategies[0].policy["K:/:"][CALL], 1, 0.01));
	assert(near(cfr.profile.strategies[0].policy["K:/:"][RAISE], 0, 0.01));
	assert(near(cfr.profile.strategies[0].policy["A:/:"][CALL], 0, 0.01));
	assert(near(cfr.profile.strategies[0].policy["A:/:"][RAISE], 1.0, 0.01));
	std::cout<< "Verifying P2 policy"<<std::endl;
	assert(near(cfr.profile.strategies[1].policy["Q:/r:"][FOLD], 1.0, 0.01));
	assert(near(cfr.profile.strategies[1].policy["Q:/r:"][CALL], 0, 0.01));
	assert(near(cfr.profile.strategies[1].policy["K:/r:"][FOLD], 2.0 / 3.0, 0.01));
	assert(near(cfr.profile.strategies[1].policy["K:/r:"][CALL], 1.0 / 3.0, 0.01));
	assert(near(cfr.profile.strategies[1].policy["A:/r:"][FOLD], 0, 0.01));
	assert(near(cfr.profile.strategies[1].policy["A:/r:"][CALL], 1.0, 0.01));

	std::cout<< "Done!"<<std::endl;

	std::cout<< "Computing NE for Leduc poker"<<std::endl;
	GameRules leduc = rl::leduc_rules();

	cfr = CounterfactualRegretMinimizer(leduc);

	iterations_per_block = 10;
	blocks = 1000;
	for(int block=0; block <blocks; block++)
	{
		std::cout<< "Iterations: "<<block * iterations_per_block<<std::endl;
		cfr.run(iterations_per_block);
		std::pair<StrategyProfile, vector<float> > result = cfr.profile.best_response();
		std::cout<<"Best response EV: "<<result.second<<std::endl;
		std::cout<<"Total exploitability:"<<std::accumulate(result.second.begin(), result.second.end(),0.0)<<std::endl;
	}
	std::cout<< "Done!"<<std::endl;
	return 0;
}
