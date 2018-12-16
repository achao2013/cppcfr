#ifndef POKERSTRATEGY_HPP_
#define POKERSTRATEGY_HPP_

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "pokertrees.hpp"
#include "utils.hpp"
using std::vector;
using std::map;
using std::string;
using std::pair;
namespace rl{

	int choose(int n, int k);

class Strategy
{
	public:
		Strategy(){}
		Strategy(int player, string filename="")
		{
			this->player = player;
			if(!filename.empty())
			  this->load_from_file(filename);
		}
		Strategy(const Strategy& s)
		{
			policy = s.policy;
			player = s.player;
		}
		void build_default(PublicTree ptree);
		void build_default(GameTree gtree);
		void build_random(GameTree gametree);
		vector<float> probs(string infoset);
		int sample_action(string infoset);
		void load_from_file(string filename);
		void save_to_file(string filename);

		int player;
		map<string, vector<float> > policy;
};


class StrategyProfile
{
	public:
		enum ACTION_TYPE{
			FOLD,
			CALL,
			RAISE
		};
		StrategyProfile(){}
		StrategyProfile(const StrategyProfile& sp)
		{
			//rules = std::make_shared<GameRules>(*sp.rules.get());
			//gametree = std::make_shared<GameTree>(*sp.gametree.get());
			//publictree = std::make_shared<PublicTree>(*sp.publictree.get());
			rules = sp.rules;
			gametree = sp.gametree;
			publictree = sp.publictree;
			strategies = sp.strategies;
			node2typename = sp.node2typename;
		}
		StrategyProfile(GameRules rules, vector<Strategy> strategies)
		{
			assert(rules.players == strategies.size());
			this->rules=std::make_shared<GameRules>(rules);
			this->strategies=strategies;
			node2typename.push_back(typeid(TerminalNode).name());
			node2typename.push_back(typeid(HolecardChanceNode).name());
			node2typename.push_back(typeid(BoardcardChanceNode).name());
			node2typename.push_back(typeid(ActionNode).name());
		}
		vector<float> expected_value();
		vector<float> old_ev_helper(shared_ptr<Node> root, vector<float> pathprobs);
		vector<map<vector<Card>,float> > ev_helper(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		vector<map<vector<Card>,float> > ev_terminal_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		vector<map<vector<Card>,float> > ev_holecard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		vector<map<vector<Card>,float> > ev_boardcard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		vector<map<vector<Card>,float> > ev_action_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs);
		pair<StrategyProfile, vector<float> > best_response();
		vector<map<vector<Card>,float> > br_helper(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses);
		vector<map<vector<Card>,float> > br_holecard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses);
		vector<map<vector<Card>,float> > br_boardcard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses);
		vector<map<vector<Card>,float> > br_action_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses);
		map<vector<Card>,float> br_response_action( shared_ptr<Node> root, vector<Strategy>& responses, vector<vector<map<vector<Card>,float> > > action_payoffs);

		shared_ptr<GameRules> rules;
		shared_ptr<GameTree> gametree;
		shared_ptr<PublicTree> publictree;
		vector<Strategy> strategies;
		vector<string> node2typename;
		static int count;
};

}



#endif
