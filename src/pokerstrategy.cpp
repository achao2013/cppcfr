#include "pokertrees.hpp"
#include "pokerstrategy.hpp"
#include <random>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
namespace rl{

int StrategyProfile::count=1;
	int choose(int n, int k)
	{
		//A fast way to calculate binomial coefficients by Andrew Dalke (contrib).
		if(0 <= k && k <= n)
		{
			int ntok = 1;
			int ktok = 1;
			for(int t=1; t< std::min(k, n - k) + 1; t++)
			{
				ntok *= n;
				ktok *= t;
				n -= 1;
			}
			return ntok / ktok;
		}
		else
		  return 0;
	}

	void Strategy::build_default(GameTree gtree)
	{
		for(auto p : gtree.information_sets)
		{
			string key=p.first;
			vector<shared_ptr<Node> >& infoset = gtree.information_sets[key];
			shared_ptr<ActionNode> test_node = std::static_pointer_cast<ActionNode>(infoset[0]);
			//std::cout<<"test_node->player , this->player:"<<test_node->player<<","<<this->player<<std::endl;
			//std::cout<<"infoset size:"<<infoset.size()<<std::endl;
			if(test_node->player == this->player)
			  for(auto node : infoset)
			  {
				  float prob = 1.0 / float(node->children.size());
				  vector<float> probs(3,0);
				  for(int action=0; action<3; action++)
					if(std::static_pointer_cast<ActionNode>(node)->valid(action))
					  probs[action] = prob;
				  if(typeid(std::static_pointer_cast<ActionNode>(node)->player_view_g) == typeid(string))
					this->policy[std::static_pointer_cast<ActionNode>(node)->player_view_g] = probs;
			  }
		}
		//std::cout<<"in build_default policy:"<<policy<<std::endl;
	}

	void Strategy::build_default(PublicTree ptree)
	{
		for(auto p : ptree.information_sets)
		{
			vector<string> key=p.first;
			vector<shared_ptr<Node> >& infoset = ptree.information_sets[key];
			shared_ptr<ActionNode> test_node = std::static_pointer_cast<ActionNode>(infoset[0]);
			//std::cout<<"test_node->player , this->player:"<<test_node->player<<","<<this->player<<std::endl;
			if(test_node->player == this->player)
			  for(auto node : infoset)
			  {
				  float prob = 1.0 / float(node->children.size());
				  vector<float> probs(3,0);
				  for(int action=0; action<3; action++)
					if(std::static_pointer_cast<ActionNode>(node)->valid(action))
					  probs[action] = prob;
				  if(typeid(std::static_pointer_cast<ActionNode>(node)->player_view) != typeid(string))
					for(auto pview : std::static_pointer_cast<ActionNode>(node)->player_view)
					  this->policy[pview] = probs;
				  else
					this->policy[std::static_pointer_cast<ActionNode>(node)->player_view_g] = probs;
			  }
		}
		//std::cout<<"in build_default policy:"<<policy<<std::endl;
	}
	void Strategy::build_random(GameTree gametree)
	{
		for(auto p : gametree.information_sets)
		{
			string key=p.first;
			vector<shared_ptr<Node> >& infoset = gametree.information_sets[key];
			shared_ptr<ActionNode> test_node = std::static_pointer_cast<ActionNode>(infoset[0]);
			if(test_node->player == this->player)
			  for(auto node : infoset)
			  {
				  vector<float> probs(3,0);
				  float total = 0;
				  for(int action=0; action<3; action++)
					if(std::static_pointer_cast<ActionNode>(node)->valid(action))
					{
						std::default_random_engine random(time(NULL));
						//generate range [0,1)
						std::uniform_real_distribution<float> dis(0.0, 1.0);
						probs[action] = dis(random);
						total += probs[action];
					}
				  std::for_each(probs.begin(),probs.end(),[&total](float& x){x=x/total;});
				  if(typeid(std::static_pointer_cast<ActionNode>(node)->player_view) != typeid(string))
					for(auto pview : std::static_pointer_cast<ActionNode>(node)->player_view)
					  policy[pview] = probs;
				  else
					policy[std::static_pointer_cast<ActionNode>(node)->player_view_g] = probs;
			  }
		}
	}
	vector<float> Strategy::probs(string infoset)
	{
		if(policy.find(infoset)==policy.end())
		  std::cout<<"in probs policy:"<<policy<<std::endl;
		assert(policy.find(infoset)!=policy.end());
		return policy[infoset];
	}
	int Strategy::sample_action(string infoset)
	{
		assert(policy.find(infoset)!=policy.end());
		vector<float> probs = policy[infoset];
		std::default_random_engine random(time(NULL));
		//generate range [0,1)
		std::uniform_real_distribution<float> dis(0.0, 1.0);
		float val = dis(random);
		float total = 0;
		for(int i=0; i<probs.size(); i++)
		{
			float p =probs[i];
			total += p;
			if(p > 0 && val <= total)
			  return i;
		}
		std::cout<<"Invalid probability distribution. Infoset: "<<infoset<<" Probs:"<<std::endl;
		for_each(probs.begin(),probs.end(),[&](float x){std::cout<<x<<" ";});
		std::cout<<std::endl;
		exit(-1);
	}
	vector<string>  split(const string& str,const string& delim) { //将分割后的子字符串存储在vector中
		vector<string> res;
		if("" == str) return  res;
					
		string strs = str + delim; //*****扩展字符串以方便检索最后一个分隔出的字符串
		size_t pos;
		size_t size = strs.size();
							 
		for (int i = 0; i < size; ++i) {
			pos = strs.find(delim, i); //pos为分隔符第一次出现的位置，从i到pos之前的字符串是分隔出来的字符串
			if( pos < size) { //如果查找到，如果没有查找到分隔符，pos为string::npos
			string s = strs.substr(i, pos - i);//*****从i开始长度为pos-i的子字符串
			res.push_back(s);//两个连续空格之间切割出的字符串为空字符串，这里没有判断s是否为空，所以最后的结果中有空字符的输出，
			i = pos + delim.size() - 1;
		}
															
														}
		return res;	
	}
	template <class Type>
	Type str2num(const string& str)
	{
		std::istringstream iss(str);
		Type num;
		iss >> num;
		return num;    
	}

	void Strategy::load_from_file(string filename)
	{
		std::ifstream f(filename);
		if(!f) std::cout<<"load "<<filename<<" failed"<<std::endl;
		string pv;
		float a1,a2,a3;
		string line;
		while(getline(f, line))
		{
			if(line[0] == '\0' || line[0]=='#')
			  continue;
			vector<string> list = split(line, " ");
			assert(list.size() == 4);
			pv = list[0];
			a1 = str2num<float>(list[1]);
			a2 = str2num<float>(list[2]);
			a3 = str2num<float>(list[3]);
			//tokens = line.split(' ')
			vector<float> probs;
			probs.push_back(a3);
			probs.push_back(a2);
			probs.push_back(a1);
			policy[pv] = probs;
		}
		//std::cout<<"in load_from_file policy:"<<policy<<std::endl;
		f.close();
	}
	void Strategy::save_to_file(string filename)
	{
		std::ofstream f(filename);
		for(auto p : policy)
		{
			vector<float> val = policy[p.first];
			f<<p.first<<" "<<val[2]<<" "<<val[1]<<" "<<val[0]<<std::endl;
		}
		f.close();
	}

	vector<float> StrategyProfile::expected_value()
	{
		/*
		   Calculates the expected value of each strategy : the profile.
		   Returns an array of scalars corresponding to the expected payoffs.
		   */
		if(publictree.get() == NULL)
		  publictree = std::make_shared<PublicTree>(*rules.get());
		if(publictree->root.get() == NULL)
		  publictree->build();
		map<vector<Card>, float> tmpv;
		tmpv.insert(pair<vector<Card>, float>(vector<Card>(),1));
		vector<map<vector<Card>, float> > tmp(rules->players, tmpv);
		vector<map<vector<Card>, float> > expected_values = ev_helper(publictree->root, tmp);
		for(auto ev : expected_values)
		  assert(ev.size() == 1);
		vector<float> res;
		for(auto ev : expected_values)
		{
			for(auto item: ev)
			  res.push_back(item.second);
		}
		return res; // pull the EV from the dict returned
	}

	//many problems, like data type
	vector<float> StrategyProfile::old_ev_helper(shared_ptr<Node> root, vector<float> pathprobs)
	{
	//	if(strcmp(typeid(*root.get()).name() , node2typename[0])==0)
	//	  return ev_terminal_node(root, pathprobs);
	//	if(strcmp(typeid(*root.get()).name() , node2typename[1])==0 || strcmp(typeid(*root.get()).name() , node2typename[2])==0)
	//	{
	//		vector<float> payoffs(rules->players,0);
	//		prob = pathprob / float(root->children.size());
	//		for(auto child : root->children)
	//		{
	//			vector<map<vector<Card>,float> > subpayoffs = ev_helper(child, prob);
	//			for(i,p : enumerate(subpayoffs))
	//			  payoffs[i] += p;
	//		}
	//		return payoffs;
	//	}
	//	// Otherwise, it's an ActionNode
	//	vector<float> probs = strategies[root->player].probs(root->player_view);
		vector<float> payoffs(rules->players,0);
	//	if(root->fold_action && probs[FOLD] > 0.0000000001)
	//	{
	//		vector<float> subpayoffs = ev_helper(root->fold_action, pathprob * probs[FOLD]);
	//		for(int i=0; i<subpayoffs.size(); i++)
	//		  payoffs[i] += subpayoffs[i];
	//	}
	//	if(root->call_action && probs[CALL] > 0.0000000001)
	//	{
	//		vector<float> subpayoffs = ev_helper(root->call_action, pathprob * probs[CALL]);
	//		for(int i=0; i<subpayoffs.size(); i++)
	//		  payoffs[i] += subpayoffs[i];
	//	}
	//	if(root->raise_action && probs[RAISE] > 0.0000000001)
	//	{
	//		vector<float> subpayoffs = ev_helper(root->raise_action, pathprob * probs[RAISE]);
	//		for(int i=0; i<subpayoffs.size(); i++)
	//		  payoffs[i] += subpayoffs[i];
	//	}
		return payoffs;
	}
	vector<map<vector<Card>,float> > StrategyProfile::ev_helper(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
	{
		if(strcmp(typeid(*root.get()).name() , node2typename[0].c_str())==0)
		  return ev_terminal_node(root, reachprobs);
		if(strcmp(typeid(*root.get()).name() , node2typename[1].c_str())==0)
		  return ev_holecard_node(root, reachprobs);
		if(strcmp(typeid(*root.get()).name() , node2typename[2].c_str())==0)
		  return ev_boardcard_node(root, reachprobs);
		return ev_action_node(root, reachprobs);
	}
	vector<map<vector<Card>,float> > StrategyProfile::ev_terminal_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
	{
		//std::cout<<"in ev_terminal_node reachprobs:"<<reachprobs<<std::endl;
		vector<map<vector<Card>,float> > payoffs(rules->players);
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>,float> player_payoffs;
			for(auto hc : root->holecards[player])
			  player_payoffs[hc]=0;
			map<vector<Card>,int> counts;
			for(auto hc : root->holecards[player])
			  counts[hc]=0;
			//jzc:root.payoffs[profile idx][cards profile]=utilities for each player
			//std::cout<<"root payoffs"<<std::static_pointer_cast<TerminalNode>(root)->payoffs<<std::endl;
			for(auto p : std::static_pointer_cast<TerminalNode>(root)->payoffs)
			{
				vector<vector<Card> > hands=p.first;
				vector<float> winnings=p.second;
				float prob = 1.0;
				vector<Card> player_hc;
				for(int opp=0;opp<hands.size();opp++)
				  if(opp == player)
					player_hc = hands[opp];
				  else
					prob *= reachprobs[opp][hands[opp]];
				player_payoffs[player_hc] += prob * winnings[player];
				counts[player_hc] += 1;
			}
			//std::cout<<"1.player_payoffs : "<<player_payoffs<<std::endl;
			for(auto p : counts)
			{
				vector<Card> hc=p.first;
				int count=p.second;
				if(count > 0)
				  player_payoffs[hc] /= float(count);
			}
			payoffs[player] = player_payoffs;
			//std::cout<<"2.player_payoffs : "<<player_payoffs<<std::endl;
		}
		//std::cout<<"in ev_terminal_node payoffs:"<<payoffs<<std::endl;
		return payoffs;
	}
	vector<map<vector<Card>,float> > StrategyProfile::ev_holecard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
	{
		assert(root->children.size() == 1);
		int prevlen = reachprobs[0].begin()->first.size();
		//std::cout<<"reachprobs:"<<reachprobs<<"prevlen:"<<prevlen<<std::endl;
		int possible_deals = float(choose(root->deck.size() - prevlen,std::static_pointer_cast<HolecardChanceNode>(root)->todeal));
		//std::cout<<"deck:"<<root->deck<<"todeal:"<<static_pointer_cast<HolecardChanceNode>(root)->todeal<<"possible_deals:"<<possible_deals;
		vector<map<vector<Card>, float> > next_reachprobs;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> m;
			for(auto hc : root->children[0]->holecards[player])
			{
				vector<Card> tmp;
				std::copy(hc.begin(),hc.begin()+prevlen,tmp.begin());
				m[hc]=reachprobs[player][tmp]/possible_deals;
			}
			next_reachprobs.push_back(m);
		}
		//std::cout<<"next_reachprobs:"<<next_reachprobs;
		vector<map<vector<Card>,float> > subpayoffs = ev_helper(root->children[0], next_reachprobs);
		//std::cout<<"subpayoffs"<<subpayoffs;
		vector<map<vector<Card>,float> > payoffs;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> m;
			for(auto hc : root->holecards[player])
			  m[hc]=0;
			payoffs.push_back(m);
		}
		for(int player=0; player<subpayoffs.size(); player++)
		{
			map<vector<Card>,float> subpayoff=subpayoffs[player];
			for(auto sp : subpayoff)
			{
				vector<Card> hand=sp.first;
				float winnings=sp.second;
				vector<Card> hc(prevlen);
				std::copy( hand.begin(),hand.begin()+prevlen,hc.begin());
				payoffs[player][hc] += winnings;
			}
		}
		return payoffs;
	}
	vector<map<vector<Card>,float> > StrategyProfile::ev_boardcard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
	{
		int prevlen = reachprobs[0].begin()->first.size();
		int possible_deals = float(choose(root->deck.size() - prevlen,std::static_pointer_cast<BoardcardChanceNode>(root)->todeal));
		vector<map<vector<Card>,float> > payoffs;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> m;
			for(auto hc : root->holecards[player])
			  m[hc]=0;
			payoffs.push_back(m);
		}
		for(auto bc : root->children)
		{
			vector<map<vector<Card>, float> > next_reachprobs;
			for(int player=0; player<rules->players; player++)
			{
				map<vector<Card>, float> m;
				for(auto hc : bc->holecards[player])
				{
					m[hc]=reachprobs[player][hc]/possible_deals;
				}
				next_reachprobs.push_back(m);
			}
			vector<map<vector<Card>,float> > subpayoffs = ev_helper(bc, next_reachprobs);
			for(int player=0; player<subpayoffs.size(); player++)
			{
				map<vector<Card>,float> subpayoff=subpayoffs[player];
				for(auto sp : subpayoff)
				{
					vector<Card> hand=sp.first;
					float winnings=sp.second;
					payoffs[player][hand] += winnings;
				}
			}
		}
		return payoffs;
	}
	vector<map<vector<Card>,float> > StrategyProfile::ev_action_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
	{
		int root_player= std::static_pointer_cast<ActionNode>(root)->player;
		Strategy strategy = strategies[root_player];
		vector<map<vector<Card>, float> > next_reachprobs = reachprobs;
		map<vector<Card>, vector<float> > action_probs;
		for(auto hc : root->holecards[root_player])
		{
			action_probs[hc]=strategy.probs(rules->infoset_format(root_player, hc, root->board, root->bet_history));
		}
		vector<vector<map<vector<Card>,float> > > action_payoffs(3);
		if(std::static_pointer_cast<ActionNode>(root)->fold_action)
		{
			map<vector<Card>, float> tmp;
			for(auto hc : root->holecards[root_player])
			{
				tmp[hc]=action_probs[hc][FOLD] * reachprobs[root_player][hc];
			}
			next_reachprobs[root_player] = tmp;
			action_payoffs[FOLD] = ev_helper(std::static_pointer_cast<ActionNode>(root)->fold_action, next_reachprobs);
		}
		if(std::static_pointer_cast<ActionNode>(root)->call_action)
		{
			map<vector<Card>, float> tmp;
			for(auto hc : root->holecards[root_player])
			{
				tmp[hc]=action_probs[hc][CALL] * reachprobs[root_player][hc];
			}
			next_reachprobs[root_player] = tmp;
			action_payoffs[CALL] = ev_helper(std::static_pointer_cast<ActionNode>(root)->call_action, next_reachprobs);
		}
		if(std::static_pointer_cast<ActionNode>(root)->raise_action)
		{
			map<vector<Card>, float> tmp;
			for(auto hc : root->holecards[root_player])
			{
				tmp[hc]=action_probs[hc][RAISE] * reachprobs[root_player][hc];
			}
			next_reachprobs[root_player] = tmp;
			action_payoffs[RAISE] = ev_helper(std::static_pointer_cast<ActionNode>(root)->raise_action, next_reachprobs);
		}
		vector<map<vector<Card>,float> > payoffs;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> player_payoffs;
			for(auto hc : root->holecards[player])
			  player_payoffs[hc]=0;
			for(int action=0; action<action_payoffs.size(); action++)
			{
				vector<map<vector<Card>, float > > subpayoff=action_payoffs[action];
				if(subpayoff.empty())
				  continue;
				if(root_player == player)
				  for(auto sp  : subpayoff[player])
				  {
					  vector<Card> hc = sp.first;
					  float winnings = sp.second;
					  player_payoffs[hc] += winnings * action_probs[hc][action];
				  }
				else
				  for(auto sp  : subpayoff[player])
				  {
					  vector<Card> hc = sp.first;
					  float winnings = sp.second;
					  player_payoffs[hc] += winnings;
				  }
			}
			payoffs.push_back(player_payoffs);
		}
		return payoffs;
	}
	pair<StrategyProfile, vector<float> > StrategyProfile::best_response()
	{
		/*
		   Calculates the best response for each player : the strategy profile.
		   Returns a list of tuples of the best response strategy and its expected value for each player.
		   */
		if(publictree.get() == NULL)
		  publictree = std::make_shared<PublicTree>(*rules.get());
		if(publictree->root.get() ==NULL)
		  publictree->build();
		vector<Strategy> responses;
		for(int player=0; player<rules->players; player++)
		{
			Strategy s(player);
		  responses.push_back(s);
		}
		vector<map<vector<Card>, float> > tmp;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> t;
			pair<vector<Card>, float> p= std::make_pair<vector<Card>, float>(vector<Card>(),1);
			t.insert(p);
		    tmp.push_back(t);
		}
		vector<map<vector<Card>,float> > expected_values = br_helper(publictree->root, tmp, responses);
		for(auto ev : expected_values)
		  assert(ev.size() == 1);
		// pull the EV from the dict returned
		vector<float> expected_value_vec; 
		for(auto ev : expected_values)
		  expected_value_vec.push_back(ev.begin()->second);
		StrategyProfile sp(*rules.get(), responses);
		pair<StrategyProfile, vector<float> > pse(sp, expected_value_vec);
		return pse;
	}
	vector<map<vector<Card>,float> > StrategyProfile::br_helper(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses)
	{
	//std::cout<<count++<<".br_helper root type:"<<typeid(*root.get()).name()<<std::endl;
		if(strcmp(typeid(*root.get()).name() , node2typename[0].c_str())==0)
		  return ev_terminal_node(root, reachprobs);
		if(strcmp(typeid(*root.get()).name() , node2typename[1].c_str())==0)
		  return br_holecard_node(root, reachprobs, responses);
		if(strcmp(typeid(*root.get()).name() , node2typename[2].c_str())==0)
		  return br_boardcard_node(root, reachprobs, responses);
		return br_action_node(root, reachprobs, responses);
	}
	vector<map<vector<Card>,float> > StrategyProfile::br_holecard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses)
	{
		assert(root->children.size() == 1);
		int prevlen = reachprobs[0].begin()->first.size();
		int possible_deals = float(choose(root->deck.size() - prevlen,std::static_pointer_cast<HolecardChanceNode>(root)->todeal));
		vector<map<vector<Card>, float> > next_reachprobs;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> m;
			for(auto hc : root->children[0]->holecards[player])
			{
				vector<Card> tmp(prevlen);
				std::copy(hc.begin(),hc.begin()+prevlen,tmp.begin());
				m[hc]=reachprobs[player][tmp]/possible_deals;
			}
			next_reachprobs.push_back(m);
		}
		vector<map<vector<Card>,float> > subpayoffs = br_helper(root->children[0], next_reachprobs, responses);
		vector<map<vector<Card>,float> > payoffs;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> m;
			for(auto hc : root->holecards[player])
			  m[hc]=0;
			payoffs.push_back(m);
		}
		for(int player=0;player<subpayoffs.size(); player++)
		{
			for(auto sp : subpayoffs[player])
			{
				vector<Card> hand = sp.first;
				float winnings = sp.second;
				vector<Card> hc(prevlen);
				std::copy(hand.begin(), hand.begin()+prevlen, hc.begin());
				payoffs[player][hc] += winnings;
			}
		}
		//std::cout<<"in br_holecard_node payoffs:"<<payoffs<<std::endl;
		return payoffs;
	}
	vector<map<vector<Card>,float> > StrategyProfile::br_boardcard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses)
	{
		int prevlen = reachprobs[0].begin()->first.size();
		int possible_deals = float(choose(root->deck.size() - prevlen, std::static_pointer_cast<BoardcardChanceNode>(root)->todeal));
		vector<map<vector<Card>,float> > payoffs;
		for(int player=0; player<rules->players; player++)
		{
			map<vector<Card>, float> m;
			for(auto hc : root->holecards[player])
			  m[hc]=0;
			payoffs.push_back(m);
		}
		for(auto bc : root->children)
		{
			vector<map<vector<Card>, float> > next_reachprobs;
			for(int player=0; player<rules->players; player++)
			{
				map<vector<Card>, float> m;
				for(auto hc : bc->holecards[player])
				{
					m[hc]=reachprobs[player][hc]/possible_deals;
				}
				next_reachprobs.push_back(m);
			}
			vector<map<vector<Card>,float> > subpayoffs = br_helper(bc, next_reachprobs, responses);
			for(int player=0; player<subpayoffs.size(); player++)
			{
				map<vector<Card>,float> subpayoff=subpayoffs[player];
				for(auto sp : subpayoff)
				{
					vector<Card> hand=sp.first;
					float winnings=sp.second;
					payoffs[player][hand] += winnings;
				}
			}
		}
		return payoffs;
	}

	vector<map<vector<Card>,float> > StrategyProfile::br_action_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs, vector<Strategy>& responses)
	{
		//std::cout<<"in br_action_node reachprobs:"<<reachprobs<<std::endl;
		int root_player= std::static_pointer_cast<ActionNode>(root)->player;
		Strategy strategy = strategies[root_player];
		vector<map<vector<Card>, float> > next_reachprobs = reachprobs;
		map<vector<Card>, vector<float> > action_probs;
		for(auto hc : root->holecards[root_player])
		{
			action_probs[hc]=strategy.probs(rules->infoset_format(root_player, hc, root->board, root->bet_history));
		}
		//std::cout<<"in br_action_node action_probs:"<<action_probs<<std::endl;
		vector<vector<map<vector<Card>,float> > > action_payoffs(3);
		if(std::static_pointer_cast<ActionNode>(root)->fold_action.get())
		{
			map<vector<Card>, float> tmp;
			for(auto hc : root->holecards[root_player])
			{
				tmp[hc]=action_probs[hc][FOLD] * reachprobs[root_player][hc];
			}
			next_reachprobs[root_player] = tmp;
			action_payoffs[FOLD] = br_helper(std::static_pointer_cast<ActionNode>(root)->fold_action, next_reachprobs, responses);
		}
		if(std::static_pointer_cast<ActionNode>(root)->call_action.get())
		{
			map<vector<Card>, float> tmp;
			for(auto hc : root->holecards[root_player])
			{
				tmp[hc]=action_probs[hc][CALL] * reachprobs[root_player][hc];
			}
			next_reachprobs[root_player] = tmp;
			action_payoffs[CALL] = br_helper(std::static_pointer_cast<ActionNode>(root)->call_action, next_reachprobs, responses);
		}
		if(std::static_pointer_cast<ActionNode>(root)->raise_action.get())
		{
			map<vector<Card>, float> tmp;
			for(auto hc : root->holecards[root_player])
			{
				tmp[hc]=action_probs[hc][RAISE] * reachprobs[root_player][hc];
			}
			next_reachprobs[root_player] = tmp;
			action_payoffs[RAISE] = br_helper(std::static_pointer_cast<ActionNode>(root)->raise_action, next_reachprobs, responses);
		}
		vector<map<vector<Card>,float> > payoffs;
		for(int player=0; player<rules->players; player++)
		{
			if(player == root_player)
			  payoffs.push_back(br_response_action(root, responses, action_payoffs));
			else
			{
				map<vector<Card>, float> player_payoffs;
				for(auto hc : root->holecards[player])
				  player_payoffs[hc]=0;
				for(auto subpayoff : action_payoffs)
				{
					if(subpayoff.empty())
					  continue;
					for(auto sp : subpayoff[player])
					{
						vector<Card> hc = sp.first;
						float winnings = sp.second;
						player_payoffs[hc] += winnings;
					}
				}
				payoffs.push_back(player_payoffs);
			}
		}
		//std::cout<<"in br_action_node payoffs:"<<payoffs<<std::endl;
		return payoffs;
	}
	map<vector<Card>,float> StrategyProfile::br_response_action( shared_ptr<Node> root, vector<Strategy>& responses, vector<vector<map<vector<Card>,float> > > action_payoffs)
	{
		int root_player = std::static_pointer_cast<ActionNode>(root)->player;
		map<vector<Card>, float> player_payoffs;
		Strategy& max_strategy = responses[root_player];
		float max_value=-1,value=-1;
		for(auto hc : root->holecards[root_player])
		{
			string infoset = rules->infoset_format(root_player, hc, root->board, root->bet_history);
			vector<int> max_action;
			if(!action_payoffs[FOLD].empty())
			{
				max_action = vector<int>(1,FOLD);
				max_value = action_payoffs[FOLD][root_player][hc];
				if(infoset == string("QQ:/cc/cr:"))
				{
				  std::cout<<"in fold infoset:"<<infoset<<" player:"<<root_player<<" hc:"<<hc<<std::setprecision(16)<<" value:"<<value<<" max_value:"<<max_value<<" max_action.size:"<<max_action.size()<<std::endl;
				}
			}
			if(!action_payoffs[CALL].empty())
			{
				value = action_payoffs[CALL][root_player][hc];
				if(infoset == string("QQ:/cc/cr:"))
				  std::cout<<"in call infoset:"<<infoset<<" player:"<<root_player<<" hc:"<<hc<<std::setprecision(16)<<" value:"<<value<<" max_value:"<<max_value<<" max_action.size:"<<max_action.size()<<std::endl;
				if(max_action.empty() || value > max_value)
				{
					max_action = vector<int>(1,CALL);
					max_value = value;
				}
				else if(fabs(max_value - value)<1e-15)
				  max_action.push_back(CALL);
			}
			if(!action_payoffs[RAISE].empty())
			{
				value = action_payoffs[RAISE][root_player][hc];
				if(infoset == string("QQ:/cc/cr:"))
				  std::cout<<"in raise infoset:"<<infoset<<" player:"<<root_player<<" hc:"<<hc<<std::setprecision(16)<<" value:"<<value<<" max_value:"<<max_value<<" max_action.size:"<<max_action.size()<<std::endl;
				if(max_action.empty() || value > max_value)
				{
					max_action =vector<int>(1,RAISE);
					max_value = value;
				}
				else if(fabs(max_value - value)<1e-15)
				  max_action.push_back(RAISE);
			}
			vector<float> probs(3,0);
			for(auto action : max_action)
			  probs[action] = 1.0 / float(max_action.size());
			max_strategy.policy[infoset] = probs;
			player_payoffs[hc] = max_value;
		}
		return player_payoffs;
	}

}

