#ifndef POKERCFR_HPP_
#define POKERCFR_HPP_

#include "pokertrees.hpp"
#include "pokerstrategy.hpp"
#include <random>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <algorithm>


using std::vector;
using std::map;
using std::string;
using std::pair;

namespace rl{

class CounterfactualRegretMinimizer
{
	public:
		enum ACTION_TYPE
		{
			FOLD,
			CALL,
			RAISE
		};
		CounterfactualRegretMinimizer(GameRules rules)
		{
			this->rules = rules;
			vector<Strategy> tmp(rules.players);
			for(int i=0; i<rules.players; i++)
			  tmp[i]=Strategy(i);
			StrategyProfile sp(rules, tmp);
			this->profile =sp; 
			this->current_profile = StrategyProfile(rules, tmp);
			this->iterations = 0;
			this->tree = std::shared_ptr<PublicTree>(new PublicTree(rules));
			this->tree->build();
			std::cout<<"Information sets size: "<<tree->information_sets.size()<<std::endl;
			for(auto& s : profile.strategies)
			{
				std::cout<<"================"<<std::endl;
				s.build_default(*tree.get());
				map<string, vector<float> > tm, tm1;
				for(auto p : s.policy)
				{
					string infoset = p.first;
					tm[infoset] = vector<float>(3,0);
				}
				counterfactual_regret.push_back(tm);
				tm1=tm;
				action_reachprobs.push_back(tm1);
			}

			node2typename.push_back(typeid(TerminalNode).name());
			node2typename.push_back(typeid(HolecardChanceNode).name());
			node2typename.push_back(typeid(BoardcardChanceNode).name());
			node2typename.push_back(typeid(ActionNode).name());
		}
		void run(int num_iterations);
		void cfr();
		vector<map<vector<Card>,float> > cfr_helper(shared_ptr<Node> root, vector<map<vector<Card>,float> > reachprobs);
		vector<map<vector<Card>,float> > cfr_terminal_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		vector<map<vector<Card>,float> > cfr_holecard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		vector<map<vector<Card>,float> > cfr_boardcard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		vector<map<vector<Card>,float> > cfr_action_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		Strategy cfr_strategy_update(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		void cfr_regret_update(shared_ptr<Node> root, vector<vector<map<vector<Card>, float> > > action_payoffs, map<vector<Card>, float> ev);
		vector<float> equal_probs(shared_ptr<Node> root);

		GameRules rules;
		StrategyProfile profile;
		StrategyProfile current_profile;
		int iterations;
		vector<map<string, vector<float> > > counterfactual_regret;
		vector<map<string, vector<float> > > action_reachprobs;
		shared_ptr<PublicTree> tree;
		vector<string> node2typename;
		static int count;
};



}

#endif
