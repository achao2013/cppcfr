#include "pokercfr.hpp"
#include <cstring>
#include <algorithm>
using std::strcmp;
namespace rl{
int	CounterfactualRegretMinimizer::count = 1;
void CounterfactualRegretMinimizer::run(int num_iterations)
{
    for(int i=0; i<num_iterations; i++)
	{
		//std::cout<<"block iteration "<<i<<std::endl;
        cfr();
		this->iterations++;
	}
}
void CounterfactualRegretMinimizer::cfr()
{
	vector<map<vector<Card>,float> > tmp(rules.players);
	for(int i=0; i<rules.players; i++)
	  tmp[i].insert(std::make_pair<vector<Card>, float>(vector<Card>(), 1));
	cfr_helper(tree->root, tmp);
}
vector<map<vector<Card>,float> > CounterfactualRegretMinimizer::cfr_helper(shared_ptr<Node> root, vector<map<vector<Card>,float> > reachprobs)
{

	//std::cout<<count++<<".cfr_helper root type:"<<typeid(*root.get()).name()<<std::endl;
	if(strcmp(typeid(*root.get()).name() , node2typename[0].c_str())==0)
	  return cfr_terminal_node(root, reachprobs);
	if(strcmp(typeid(*root.get()).name() , node2typename[1].c_str())==0)
	  return cfr_holecard_node(root, reachprobs);
	if(strcmp(typeid(*root.get()).name() , node2typename[2].c_str())==0)
	  return cfr_boardcard_node(root, reachprobs);
	return cfr_action_node(root, reachprobs);
}
vector<map<vector<Card>,float>> CounterfactualRegretMinimizer::cfr_terminal_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
{
	//std::cout<<"in cfr_terminal_node reachprobs:"<<reachprobs<<std::endl;
	vector<map<vector<Card>,float> > payoffs(rules.players);
	for(int player=0; player<rules.players; player++)
	{
		map<vector<Card>,float> player_payoffs;
		for(auto hc : root->holecards[player])
		  player_payoffs[hc]=0;
		map<vector<Card>,int> counts;
		for(auto hc : root->holecards[player])
		  counts[hc]=0;
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
		for(auto p : counts)
		{
			vector<Card> hc=p.first;
			int count=p.second;
			if(count > 0)
			  player_payoffs[hc] /= float(count);
		}
		payoffs[player] = player_payoffs;
	}
	return payoffs;
}
vector<map<vector<Card>,float> > CounterfactualRegretMinimizer::cfr_holecard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
{
	//std::cout<<"in cfr_holecard_node reachprobs:"<<reachprobs<<std::endl;
	assert(root->children.size() == 1);
	int prevlen = reachprobs[0].begin()->first.size();
	int possible_deals = float(choose(root->deck.size() - prevlen,std::static_pointer_cast<HolecardChanceNode>(root)->todeal));
	vector<map<vector<Card>, float> > next_reachprobs;
	for(int player=0; player<rules.players; player++)
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
	vector<map<vector<Card>,float> > subpayoffs = cfr_helper(root->children[0], next_reachprobs);
	vector<map<vector<Card>,float> > payoffs;
	for(int player=0; player<rules.players; player++)
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
			vector<Card>hc;
			std::copy( hand.begin(),hand.begin()+prevlen,hc.begin());
			payoffs[player][hc] += winnings;
		}
	}
	return payoffs;
}

vector<map<vector<Card>,float> > CounterfactualRegretMinimizer::cfr_boardcard_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
{
	int prevlen = reachprobs[0].begin()->first.size();
	int possible_deals = float(choose(root->deck.size() - prevlen,std::static_pointer_cast<BoardcardChanceNode>(root)->todeal));
	vector<map<vector<Card>,float> > payoffs;
	for(int player=0; player<rules.players; player++)
	{
		map<vector<Card>, float> m;
		for(auto hc : root->holecards[player])
		  m[hc]=0;
		payoffs.push_back(m);
	}
	for(auto bc : root->children)
	{
		vector<map<vector<Card>, float> > next_reachprobs;
		for(int player=0; player<rules.players; player++)
		{
			map<vector<Card>, float> m;
			for(auto hc : bc->holecards[player])
			{
				m[hc]=reachprobs[player][hc]/possible_deals;
			}
			next_reachprobs.push_back(m);
		}
		vector<map<vector<Card>,float> > subpayoffs = cfr_helper(bc, next_reachprobs);
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

vector<map<vector<Card>,float> > CounterfactualRegretMinimizer::cfr_action_node(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
{
	//std::cout<<"in cfr_action_node reachprobs:"<<reachprobs<<std::endl;
	//std::cout<<"in cfr_action_node"<<std::endl;
	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	// Calculate strategy from counterfactual regret
	Strategy strategy;
	strategy = cfr_strategy_update(root, reachprobs);
	//std::cout<<"in cfr_action_node policy:"<<strategy.policy<<std::endl;
	vector<map<vector<Card>, float> > next_reachprobs = reachprobs;
	map<vector<Card>, vector<float> > action_probs;
	for(auto it : reachprobs[root_player])
	{
		vector<Card> hc=it.first;
		action_probs[hc]=strategy.probs(rules.infoset_format(root_player, hc, root->board, root->bet_history));
	}
	//std::cout<<"in cfr_action_node action_probs:"<<action_probs<<std::endl;
	vector<vector<map<vector<Card>,float> > > action_payoffs(3);
	if(std::static_pointer_cast<ActionNode>(root)->fold_action)
	{
		map<vector<Card>, float> tmp;
		for(auto it : reachprobs[root_player])
		{
			vector<Card> hc=it.first;
			tmp[hc]=action_probs[hc][FOLD] * reachprobs[root_player][hc];
		}
		next_reachprobs[root_player] = tmp;
		//std::cout<<"fold_action type:"<<typeid(*std::static_pointer_cast<ActionNode>(root)->fold_action.get()).name()<<std::endl;
		action_payoffs[FOLD] = cfr_helper(std::static_pointer_cast<ActionNode>(root)->fold_action, next_reachprobs);
	}
	if(std::static_pointer_cast<ActionNode>(root)->call_action)
	{
		map<vector<Card>, float> tmp;
		for(auto it : reachprobs[root_player])
		{
			vector<Card> hc=it.first;
			tmp[hc]=action_probs[hc][CALL] * reachprobs[root_player][hc];
		}
		next_reachprobs[root_player] = tmp;
		action_payoffs[CALL] = cfr_helper(std::static_pointer_cast<ActionNode>(root)->call_action, next_reachprobs);
	}
	if(std::static_pointer_cast<ActionNode>(root)->raise_action)
	{
		map<vector<Card>, float> tmp;
		for(auto it : reachprobs[root_player])
		{
			vector<Card> hc=it.first;
			tmp[hc]=action_probs[hc][RAISE] * reachprobs[root_player][hc];
		}
		next_reachprobs[root_player] = tmp;
		action_payoffs[RAISE] = cfr_helper(std::static_pointer_cast<ActionNode>(root)->raise_action, next_reachprobs);
	}
	vector<map<vector<Card>,float> > payoffs;
	for(int player=0; player<rules.players; player++)
	{
		map<vector<Card>, float> player_payoffs;
		for(auto it : reachprobs[player])
		{
			vector<Card> hc=it.first;
			player_payoffs[hc]=0;
		}
		for(int action=0; action<action_payoffs.size(); action++)
		{
			vector<map<vector<Card>, float > > subpayoff=action_payoffs[action];
			if(subpayoff.empty())
			  continue;
			for(auto sp  : subpayoff[player])
			{
				vector<Card> hc = sp.first;
				float winnings = sp.second;
				if(root_player == player)
				  player_payoffs[hc] += winnings * action_probs[hc][action];
				else
				  player_payoffs[hc] += winnings;
			}
		}
		payoffs.push_back(player_payoffs);
	}
	// Update regret calculations
	cfr_regret_update(root, action_payoffs, payoffs[root_player]);
	return payoffs;
}

Strategy CounterfactualRegretMinimizer::cfr_strategy_update(shared_ptr<Node> root, vector<map<vector<Card>, float> > reachprobs)
{
	//std::cout<<"in cfr_strategy_update iter:"<<this->iterations<<std::endl;
	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	if(this->iterations == 0)
	{
		Strategy default_strat = profile.strategies[root_player];
		//std::cout<<default_strat.policy<<std::endl;
		for(auto hc : root->holecards[root_player])
		{
			string infoset = rules.infoset_format(root_player, hc, root->board, root->bet_history);
			//std::cout<<"CounterfactualRegretMinimizer infoset:"<<infoset<<std::endl;
			vector<float> probs = default_strat.probs(infoset);
			for(int i=0; i<3; i++)
			  action_reachprobs[root_player][infoset][i] += reachprobs[root_player][hc] * probs[i];
		}
		return default_strat;
	}
	for(auto hc : root->holecards[root_player])
	{
		string infoset = rules.infoset_format(root_player, hc, root->board, root->bet_history);
		vector<float> prev_cfr = counterfactual_regret[root_player][infoset];
		vector<float> max0_prev_cfr(prev_cfr.size());
		std::transform(prev_cfr.begin(), prev_cfr.end(), max0_prev_cfr.begin(), [](float x){return std::max(float(0),x);});
		float sumpos_cfr = std::accumulate(max0_prev_cfr.begin(), max0_prev_cfr.end(), 0.0);
		vector<float> probs(prev_cfr.size());
		if(sumpos_cfr == 0)
		  probs = equal_probs(root);
		else
		  std::transform(max0_prev_cfr.begin(), max0_prev_cfr.end(), probs.begin(), [&sumpos_cfr](float x){return x/sumpos_cfr;});
		current_profile.strategies[root_player].policy[infoset] = probs;
		for(int i=0; i<3; i++)
		  action_reachprobs[root_player][infoset][i] += reachprobs[root_player][hc] * probs[i];
		float sum_ar=std::accumulate(action_reachprobs[root_player][infoset].begin(), action_reachprobs[root_player][infoset].end(), 0.0);
		vector<float> tmp;
		for(int i=0; i<3; i++)
		  tmp.push_back(action_reachprobs[root_player][infoset][i] / sum_ar);
		profile.strategies[root_player].policy[infoset]=tmp;
	}
	//std::cout<<"in cfr_update_strategy " << profile.strategies[0].policy<<std::endl;
	//std::cout<<"in cfr_update_strategy " << profile.strategies[1].policy<<std::endl;
	return current_profile.strategies[root_player];
}
void CounterfactualRegretMinimizer::cfr_regret_update(shared_ptr<Node> root, vector<vector<map<vector<Card>, float> > > action_payoffs, map<vector<Card>, float> ev)
{
	int root_player=std::static_pointer_cast<ActionNode>(root)->player;
	for(int i=0; i<action_payoffs.size(); i++)
	{
		vector<map<vector<Card>, float> >subpayoff = action_payoffs[i];
		//std::cout<<"in cfr_regret_update subpayoffs:"<<subpayoff<<std::endl;
		if(subpayoff.empty())
		  continue;
		for(auto sp : subpayoff[root_player])
		{
			vector<Card> hc = sp.first;
			float winnings = sp.second;
			float immediate_cfr = winnings - ev[hc];
			string infoset = rules.infoset_format(root_player, hc, root->board, root->bet_history);
			counterfactual_regret[root_player][infoset][i] += immediate_cfr;
			//std::cout<<"hc: "<<hc<<" winnings:"<<winnings<<" counterfactual_regret:"<<counterfactual_regret[root_player][infoset][i]<<std::endl;
		}
	}
}
vector<float> CounterfactualRegretMinimizer::equal_probs(shared_ptr<Node> root)
{
	int total_actions = root->children.size();
	vector<float> probs(3,0);
	if(std::static_pointer_cast<ActionNode>(root)->fold_action)
	  probs[FOLD] = 1.0 / total_actions;
	if(std::static_pointer_cast<ActionNode>(root)->call_action)
	  probs[CALL] = 1.0 / total_actions;
	if(std::static_pointer_cast<ActionNode>(root)->raise_action)
	  probs[RAISE] = 1.0 / total_actions;
	return probs;
}

/*
   class PublicChanceSamplingCFR(CounterfactualRegretMinimizer):
   __init__(self, rules)
   CounterfactualRegretMinimizer.__init__(self, rules)

   cfr(self)
# Sample all board cards to be used
self.board = random.sample(self.rules.deck, sum([x.boardcards for x : self.rules.roundinfo]))
# Call the standard CFR algorithm
self.cfr_helper(self.tree.root, [{(): 1} for _ : range(self.rules.players)])

cfr_terminal_node(self, root, reachprobs)
payoffs = [None for _ : range(self.rules.players)]
for(player : range(self.rules.players))
player_payoffs = {hc: 0 for hc : reachprobs[player]}
counts = {hc: 0 for hc : reachprobs[player]}
for(hands,winnings : root.payoffs.items())
if(not self.terminal_match(hands))
continue
prob = 1.0
player_hc = None
for(opp,hc : enumerate(hands))
if(opp == player)
player_hc = hc
else
prob *= reachprobs[opp][hc]
player_payoffs[player_hc] += prob * winnings[player]
counts[player_hc] += 1
for(hc,count : counts.items())
if(count > 0)
player_payoffs[hc] /= float(count)
payoffs[player] = player_payoffs
return payoffs

terminal_match(self, hands)
for(hc : hands)
if(self.has_boardcard(hc))
return False
return True

cfr_holecard_node(self, root, reachprobs)
assert(len(root.children) == 1)
prevlen = len(reachprobs[0].keys()[0])
possible_deals = float(choose(len(root.deck) - len(self.board) - prevlen,root.todeal))
next_reachprobs = [{ hc: reachprobs[player][hc[0:prevlen]] / possible_deals for hc : root.children[0].holecards[player] if not self.has_boardcard(hc) } for player : range(self.rules.players)]
subpayoffs = self.cfr_helper(root.children[0], next_reachprobs)
payoffs = [{ hc: 0 for hc : reachprobs[player] } for player : range(self.rules.players)]
for(player, subpayoff : enumerate(subpayoffs))
for(hand,winnings : subpayoff.items())
hc = hand[0:prevlen]
payoffs[player][hc] += winnings
return payoffs

has_boardcard(self, hc)
for(c : hc)
if(c : self.board)
return True
return False

cfr_boardcard_node(self, root, reachprobs)
# Number of community cards dealt this round
num_dealt = len(root.children[0].board) - len(root.board)
# Find the child that matches the sampled board card(s)
for(bc : root.children)
if(self.boardmatch(num_dealt, bc))
# Update the probabilities for each HC. Assume chance prob = 1 and renormalize reach probs by new holecard range
#next_reachprobs = [{ hc: reachprobs[player][hc] for hc : reachprobs[player] } for player : range(self.rules.players)]
#sumprobs = [sum(next_reachprobs[player].values()) for player : range(self.rules.players)]
#if(min(sumprobs) == 0)
#    return [{ hc: 0 for hc : reachprobs[player] } for player : range(self.rules.players)]
#next_reachprobs = [{ hc: reachprobs[player][hc] / sumprobs[player] for hc : bc.holecards[player] } for player : range(self.rules.players)]
# Perform normal CFR
results = self.cfr_helper(bc, reachprobs)
# Return the payoffs
		return results
		raise Exception('Sampling from impossible board card')

boardmatch(self, num_dealt, node)
# Checks if this node is a match for the sampled board card(s)
for(next_card : range(0, len(node.board)))
if(self.board[next_card] not : node.board)
		return False
		return True

cfr_strategy_update(self, root, reachprobs)
# Update the strategies and regrets for each infoset
for(hc : reachprobs[root.player])
infoset = self.rules.infoset_format(root.player, hc, root.board, root.bet_history)
# Get the current CFR
		prev_cfr = self.counterfactual_regret[root.player][infoset]
# Get the total positive CFR
sumpos_cfr = float(sum([max(0,x) for x : prev_cfr]))
if(sumpos_cfr == 0)
# Default strategy is equal probability
probs = self.equal_probs(root)
		else
# Use the strategy that's proportional to accumulated positive CFR
		probs = [max(0,x) / sumpos_cfr for x : prev_cfr]
# Use the updated strategy as our current strategy
		self.current_profile.strategies[root.player].policy[infoset] = probs
# Update the weighted policy probabilities (used to recover the average strategy)
for(i : range(3))
self.action_reachprobs[root.player][infoset][i] += reachprobs[root.player][hc] * probs[i]
if(sum(self.action_reachprobs[root.player][infoset]) == 0)
# Default strategy is equal weight
self.profile.strategies[root.player].policy[infoset] = self.equal_probs(root)
		else
# Recover the weighted average strategy
		self.profile.strategies[root.player].policy[infoset] = [self.action_reachprobs[root.player][infoset][i] / sum(self.action_reachprobs[root.player][infoset]) for i : range(3)]
# Return and use the current CFR strategy
		return self.current_profile.strategies[root.player]


		class ChanceSamplingCFR(CounterfactualRegretMinimizer):
__init__(self, rules)
	CounterfactualRegretMinimizer.__init__(self, rules)

	cfr(self)
# Sample all cards to be used
holecards_per_player = sum([x.holecards for x : self.rules.roundinfo])
boardcards_per_hand = sum([x.boardcards for x : self.rules.roundinfo])
todeal = random.sample(self.rules.deck, boardcards_per_hand + holecards_per_player * self.rules.players)
# Deal holecards
self.holecards = [tuple(todeal[p*holecards_per_player:(p+1)*holecards_per_player]) for p : range(self.rules.players)]
self.board = tuple(todeal[-boardcards_per_hand:])
# Set the top card of the deck
		self.top_card = len(todeal) - boardcards_per_hand
# Call the standard CFR algorithm
self.cfr_helper(self.tree.root, [1 for _ : range(self.rules.players)])

cfr_terminal_node(self, root, reachprobs)
payoffs = [0 for _ : range(self.rules.players)]
for(hands,winnings : root.payoffs.items())
if(not self.terminal_match(hands))
continue
for(player : range(self.rules.players))
prob = 1.0
for(opp,hc : enumerate(hands))
if(opp != player)
		prob *= reachprobs[opp]
		payoffs[player] = prob * winnings[player]
		return payoffs

terminal_match(self, hands)
for(p : range(self.rules.players))
if(not self.hcmatch(hands[p], p))
		return False
		return True

hcmatch(self, hc, player)
# Checks if this hand is isomorphic to the sampled hand
sampled = self.holecards[player][:len(hc)]
for(c : hc)
if(c not : sampled)
		return False
		return True

cfr_holecard_node(self, root, reachprobs)
assert(len(root.children) == 1)
return self.cfr_helper(root.children[0], reachprobs)

cfr_boardcard_node(self, root, reachprobs)
# Number of community cards dealt this round
num_dealt = len(root.children[0].board) - len(root.board)
# Find the child that matches the sampled board card(s)
for(bc : root.children)
if(self.boardmatch(num_dealt, bc))
# Perform normal CFR
results = self.cfr_helper(bc, reachprobs)
# Return the payoffs
		return results
		raise Exception('Sampling from impossible board card')

boardmatch(self, num_dealt, node)
# Checks if this node is a match for the sampled board card(s)
for(next_card : range(0, len(node.board)))
if(self.board[next_card] not : node.board)
		return False
		return True

cfr_action_node(self, root, reachprobs)
# Calculate strategy from counterfactual regret
strategy = self.cfr_strategy_update(root, reachprobs)
next_reachprobs = deepcopy(reachprobs)
hc = self.holecards[root.player][0:len(root.holecards[root.player])]
action_probs = strategy.probs(self.rules.infoset_format(root.player, hc, root.board, root.bet_history))
action_payoffs = [None, None, None]
if(root.fold_action)
next_reachprobs[root.player] = action_probs[FOLD] * reachprobs[root.player]
action_payoffs[FOLD] = self.cfr_helper(root.fold_action, next_reachprobs)
if(root.call_action)
next_reachprobs[root.player] = action_probs[CALL] * reachprobs[root.player]
action_payoffs[CALL] = self.cfr_helper(root.call_action, next_reachprobs)
if(root.raise_action)
next_reachprobs[root.player] = action_probs[RAISE] * reachprobs[root.player]
action_payoffs[RAISE] = self.cfr_helper(root.raise_action, next_reachprobs)
payoffs = [0 for player : range(self.rules.players)]
for(i,subpayoff : enumerate(action_payoffs))
if(subpayoff is None)
continue
for(player,winnings : enumerate(subpayoff))
# action_probs is baked into reachprobs for everyone except the acting player
if(player == root.player)
		payoffs[player] += winnings * action_probs[i]
		else
		payoffs[player] += winnings
# Update regret calculations
self.cfr_regret_update(root, action_payoffs, payoffs[root.player])
		return payoffs

cfr_strategy_update(self, root, reachprobs)
# Update the strategies and regrets for each infoset
hc = self.holecards[root.player][0:len(root.holecards[root.player])]
infoset = self.rules.infoset_format(root.player, hc, root.board, root.bet_history)
# Get the current CFR
		prev_cfr = self.counterfactual_regret[root.player][infoset]
# Get the total positive CFR
sumpos_cfr = float(sum([max(0,x) for x : prev_cfr]))
if(sumpos_cfr == 0)
# Default strategy is equal probability
probs = self.equal_probs(root)
		else
# Use the strategy that's proportional to accumulated positive CFR
		probs = [max(0,x) / sumpos_cfr for x : prev_cfr]
# Use the updated strategy as our current strategy
		self.current_profile.strategies[root.player].policy[infoset] = probs
# Update the weighted policy probabilities (used to recover the average strategy)
for(i : range(3))
self.action_reachprobs[root.player][infoset][i] += reachprobs[root.player] * probs[i]
if(sum(self.action_reachprobs[root.player][infoset]) == 0)
# Default strategy is equal weight
self.profile.strategies[root.player].policy[infoset] = self.equal_probs(root)
		else
# Recover the weighted average strategy
		self.profile.strategies[root.player].policy[infoset] = [self.action_reachprobs[root.player][infoset][i] / sum(self.action_reachprobs[root.player][infoset]) for i : range(3)]
# Return and use the current CFR strategy
		return self.current_profile.strategies[root.player]

cfr_regret_update(self, root, action_payoffs, ev)
hc = self.holecards[root.player][0:len(root.holecards[root.player])]
for(i,subpayoff : enumerate(action_payoffs))
if(subpayoff is None)
		continue
immediate_cfr = subpayoff[root.player] - ev
infoset = self.rules.infoset_format(root.player, hc, root.board, root.bet_history)
		self.counterfactual_regret[root.player][infoset][i] += immediate_cfr

		class OutcomeSamplingCFR(ChanceSamplingCFR):
__init__(self, rules, exploration=0.4)
	ChanceSamplingCFR.__init__(self, rules)
	self.exploration = exploration

	cfr(self)
# Sample all cards to be used
holecards_per_player = sum([x.holecards for x : self.rules.roundinfo])
boardcards_per_hand = sum([x.boardcards for x : self.rules.roundinfo])
todeal = random.sample(self.rules.deck, boardcards_per_hand + holecards_per_player * self.rules.players)
# Deal holecards
self.holecards = [tuple(todeal[p*holecards_per_player:(p+1)*holecards_per_player]) for p : range(self.rules.players)]
self.board = tuple(todeal[-boardcards_per_hand:])
# Set the top card of the deck
		self.top_card = len(todeal) - boardcards_per_hand
# Call the standard CFR algorithm
self.cfr_helper(self.tree.root, [1 for _ : range(self.rules.players)], 1.0)

cfr_helper(self, root, reachprobs, sampleprobs)
if(type(root) is TerminalNode)
return self.cfr_terminal_node(root, reachprobs, sampleprobs)
if(type(root) is HolecardChanceNode)
return self.cfr_holecard_node(root, reachprobs, sampleprobs)
if(type(root) is BoardcardChanceNode)
return self.cfr_boardcard_node(root, reachprobs, sampleprobs)
return self.cfr_action_node(root, reachprobs, sampleprobs)

cfr_terminal_node(self, root, reachprobs, sampleprobs)
payoffs = [0 for _ : range(self.rules.players)]
for(hands,winnings : root.payoffs.items())
if(not self.terminal_match(hands))
continue
for(player : range(self.rules.players))
prob = 1.0
for(opp,hc : enumerate(hands))
if(opp != player)
		prob *= reachprobs[opp]
		payoffs[player] = prob * winnings[player] / sampleprobs
		return payoffs

cfr_holecard_node(self, root, reachprobs, sampleprobs)
assert(len(root.children) == 1)
return self.cfr_helper(root.children[0], reachprobs, sampleprobs)

cfr_boardcard_node(self, root, reachprobs, sampleprobs)
# Number of community cards dealt this round
num_dealt = len(root.children[0].board) - len(root.board)
# Find the child that matches the sampled board card(s)
for(bc : root.children)
if(self.boardmatch(num_dealt, bc))
# Perform normal CFR
results = self.cfr_helper(bc, reachprobs, sampleprobs)
# Return the payoffs
		return results
		raise Exception('Sampling from impossible board card')

cfr_action_node(self, root, reachprobs, sampleprobs)
# Calculate strategy from counterfactual regret
strategy = self.cfr_strategy_update(root, reachprobs, sampleprobs)
hc = self.holecards[root.player][0:len(root.holecards[root.player])]
infoset = self.rules.infoset_format(root.player, hc, root.board, root.bet_history)
action_probs = strategy.probs(infoset)
if(random.random() < self.exploration)
action = self.random_action(root)
else
action = strategy.sample_action(infoset)
		reachprobs[root.player] *= action_probs[action]
csp = self.exploration * (1.0 / len(root.children)) + (1.0 - self.exploration) * action_probs[action]
payoffs = self.cfr_helper(root.get_child(action), reachprobs, sampleprobs * csp)
# Update regret calculations
self.cfr_regret_update(root, payoffs[root.player], action, action_probs[action])
		payoffs[root.player] *= action_probs[action]
		return payoffs

random_action(self, root)
options = []
if(root.fold_action)
options.append(FOLD)
if(root.call_action)
options.append(CALL)
if(root.raise_action)
options.append(RAISE)
return random.choice(options)

cfr_strategy_update(self, root, reachprobs, sampleprobs)
# Update the strategies and regrets for each infoset
hc = self.holecards[root.player][0:len(root.holecards[root.player])]
infoset = self.rules.infoset_format(root.player, hc, root.board, root.bet_history)
# Get the current CFR
		prev_cfr = self.counterfactual_regret[root.player][infoset]
# Get the total positive CFR
sumpos_cfr = float(sum([max(0,x) for x : prev_cfr]))
if(sumpos_cfr == 0)
# Default strategy is equal probability
probs = self.equal_probs(root)
		else
# Use the strategy that's proportional to accumulated positive CFR
		probs = [max(0,x) / sumpos_cfr for x : prev_cfr]
# Use the updated strategy as our current strategy
		self.current_profile.strategies[root.player].policy[infoset] = probs
# Update the weighted policy probabilities (used to recover the average strategy)
for(i : range(3))
self.action_reachprobs[root.player][infoset][i] += reachprobs[root.player] * probs[i] / sampleprobs
if(sum(self.action_reachprobs[root.player][infoset]) == 0)
# Default strategy is equal weight
self.profile.strategies[root.player].policy[infoset] = self.equal_probs(root)
		else
# Recover the weighted average strategy
		self.profile.strategies[root.player].policy[infoset] = [self.action_reachprobs[root.player][infoset][i] / sum(self.action_reachprobs[root.player][infoset]) for i : range(3)]
# Return and use the current CFR strategy
		return self.current_profile.strategies[root.player]

cfr_regret_update(self, root, ev, action, actionprob)
hc = self.holecards[root.player][0:len(root.holecards[root.player])]
infoset = self.rules.infoset_format(root.player, hc, root.board, root.bet_history)
for(i : range(3))
if(not root.valid(i))
		continue
immediate_cfr = -ev * actionprob
if(action == i)
		immediate_cfr += ev
		self.counterfactual_regret[root.player][infoset][i] += immediate_cfr
		*/
		}
