#include "card.hpp"
#include "pokerstrategy.hpp"
#include "pokergames.hpp"
#include <iostream>
#include <string>
#include <cstring>

using namespace rl;
using rl::GameRules;
using rl::StrategyProfile;
using rl::Strategy;
bool near(float val, float expected)
{
	return val >= (expected - 0.0001) && val <= (expected + 0.0001);
}

bool operator==(const map<string, vector<float> >& in1, map<string, vector<float> >& in2)
{
	for(auto it: in1)
	{
		string infoset = it.first;
		vector<float> probs = it.second;
		if(in2.find(infoset) == in2.end())
		  return false;
		for(int j=0; j<probs.size(); j++)
		  if(in2[infoset][j] != probs[j])
		  {
			  std::cout<<"s.policy:"<<in1<<std::endl;
			  std::cout<<"validation_strategy.policy:"<<in2<<std::endl;
			  std::cout<<"infoset:"<<infoset<<" j:"<<j<<" "<<in2[infoset][j]<<" "<<probs[j]<<std::endl;
			  return false;
		  }
	}
	return true;
}
void validate_strategy(Strategy s, std::string filename)
{
    Strategy validation_strategy(s.player);
    validation_strategy.load_from_file(filename);
    assert(s.policy == validation_strategy.policy);
}


void depthTraversal(shared_ptr<Node> root)
{
	if(root != NULL)
	{
		
		if(strcmp(typeid(*root.get()).name() , typeid(rl::TerminalNode).name())==0)
			std::cout<<root->holecards<<" "<<root->board<<" "<<root->bet_history<<" payoffs:"<<std::static_pointer_cast<TerminalNode>(root)->payoffs<<std::endl;
		if(root->children.size()>0)
		{
			for(int i=0; i<root->children.size(); i++)
			{
				depthTraversal(root->children[i]);
			}
		}
	}
}
int main()
{
std::cout<<"Testing Strategy"<<std::endl;


std::cout<<"No-action game with 1 holecard followed by 1 boardcard"<<std::endl;
int players = 2;
vector<Card> deck = {Card(14,1),Card(13,1),Card(12,1),Card(11,1)};
int ante = 1;
vector<int> blinds;
vector<RoundInfo> rounds = {RoundInfo(1,1,1, {0,0})};
GameRules no_action_rules(players, deck, rounds, ante, blinds, rl::leduc_eval, rl::leduc_format);
GameTree no_action_gametree(no_action_rules);
no_action_gametree.build();
Strategy s0(0);
s0.build_default(no_action_gametree);
Strategy s1(1);
s1.build_default(no_action_gametree);
StrategyProfile profile(no_action_rules, {s0,s1});
vector<float> expected_value = profile.expected_value();
//std::cout<<"Expected values: [{0:.9f},{1:.9f}]"<<std::endl;.format(expected_value[0], expected_value[1])
std::cout<<"Expected values: ["<<expected_value[0]<<","<<expected_value[1]<<"]"<<std::endl;
assert(expected_value[0] >= -0.0001 && expected_value[1]>=-0.0001 && expected_value[0] <= 0.0001 && expected_value[1] <=0.0001);
std::pair<StrategyProfile, vector<float> > best_responses = profile.best_response();
//br = best_responses.first;
vector<float> brev = best_responses.second;
//std::cout<<"Best responses: {0}"<<std::endl;.format([r.policy for r in br.strategies])
std::cout<<"Best response EV "<<brev<<std::endl;
assert(brev[0] >= -0.0001 && brev[1]>=-0.0001 && brev[0] <= 0.0001 && brev[1]<=0.0001);
std::cout<<"All passed!"<<std::endl;
std::cout<<""<<std::endl;


GameRules hskuhn_rules = rl::half_street_kuhn_rules();
GameTree hskuhn_gametree = rl::half_street_kuhn_gametree();

s0 = Strategy(0);
s0.policy = { {"A:/:", {0,0,1}}, {"K:/:", {0,1,0}}, {"Q:/:", {0,2.0/3.0,1.0/3.0}} };

s1 = Strategy(1);
s1.policy = { {"A:/r:", {0,1,0}}, {"K:/r:", {2.0/3.0,1.0/3.0,0}}, {"Q:/r:", {1,0,0}}, {"A:/c:", {0,1,0}}, {"K:/c:", {0,1,0}}, {"Q:/c:", {0,1,0}} };

Strategy eq0(0);
eq0.build_default(hskuhn_gametree);
eq0.save_to_file("/home/achao/rl/cppcfr/tests/hskuhn_eq0.strat");

Strategy eq1(1);
eq1.build_default(hskuhn_gametree);
eq1.save_to_file("/home/achao/rl/cppcfr/tests/hskuhn_eq1.strat");

std::cout<<"Half-Street Kuhn Expected Value"<<std::endl;
std::cout<<"Nash0 vs. Nash1: "<<StrategyProfile(hskuhn_rules, {s0,s1}).expected_value()<<std::endl;
std::cout<<"Nash0 vs. Eq1: "<<StrategyProfile(hskuhn_rules, {s0,eq1}).expected_value()<<std::endl;
std::cout<<"Eq0 vs. Nash1: "<<StrategyProfile(hskuhn_rules, {eq0,s1}).expected_value()<<std::endl;
std::cout<<"Eq0 vs. Eq1: "<<StrategyProfile(hskuhn_rules, {eq0,eq1}).expected_value()<<std::endl;
std::cout<<std::endl;

std::cout<<"Half-Street Kuhn Best Response"<<std::endl;
Strategy overbet0(0);
overbet0.policy = { {"Q:/:", {0,0.5,0.5}}, {"K:/:", {0,1,0}}, {"A:/:", {0,0,1}} };
Strategy overbet1(1);
overbet1.policy = { {"Q:/c:", {0,1,0}}, {"K:/c:", {0,1,0}}, {"A:/c:", {0,1,0}}, {"Q:/r:", {1,0,0}}, {"K:/r:", {0.5,0.5,0}}, {"A:/r:", {0,1,0}} };
profile = StrategyProfile(hskuhn_rules, {overbet0,overbet1});
std::pair<StrategyProfile, vector<float> > res = profile.best_response();
Strategy br = res.first.strategies[0];
float ev = res.second[0];
std::cout<<"Overbet0 BR EV: "<<ev<<std::endl;
std::cout<<br.policy<<std::endl;
assert(br.policy.size() == 3);
vector<float> v010 = {0,1,0};
vector<float> v001 = {0,0,1};
vector<float> v100 = {1,0,0};
assert(br.probs("Q:/:") == v010);
assert(br.probs("K:/:") == v010);
assert(br.probs("A:/:") == v001);
assert(near(ev, 1.0/12.0));

br = res.first.strategies[1];
ev = res.second[1];
std::cout<<"Overbet1 BR: "<<ev<<std::endl;
assert(br.policy.size() == 6);
assert(br.probs("Q:/c:") == v010);
assert(br.probs("K:/c:") == v010);
assert(br.probs("A:/c:") == v010);
assert(br.probs("Q:/r:") == v100);
assert(br.probs("K:/r:") == v010);
assert(br.probs("A:/r:") == v010);
assert(near(ev, 0));

std::cout<<"All passed!"<<std::endl;
std::cout<<std::endl;

//sys.exit(0)

GameRules leduc_rules = rl::leduc_rules();
shared_ptr<PublicTree> leduc_gt = std::make_shared<PublicTree>(leduc_rules);
leduc_gt->build();


depthTraversal(leduc_gt->root);




s0 = Strategy(0);
s0.load_from_file("/home/achao/rl/cppcfr/strategies/leduc/0.strat");
// Test a couple of arbitrary values
vector<float> t = {0.000000000, 0.927357111, 0.072642889};
assert(s0.probs("J:/:") == t);
t = {0.546821151, 0.453178849, 0.000000000};
assert(s0.probs("KJ:/rrc/rr:") == t);
// Verify we loaded all of infosets
assert(s0.policy.size() == 144);

s1 = Strategy(1);
s1.load_from_file("/home/achao/rl/cppcfr/strategies/leduc/1.strat");
// Test a couple of arbitrary values
t = {0.819456679, 0.125672407, 0.054870914};
assert(s1.probs("J:/r:") == t);
t = {0.000031258, 0.999968742, 0.000000000};
assert(s1.probs("KJ:/rrc/crr:") == t);
// Verify we loaded all of infosets
assert(s1.policy.size() == 144);

// Generate a default strategy for player 0
eq0 = Strategy(0);
eq0.build_default(*leduc_gt.get());
eq0.save_to_file("/home/achao/rl/cppcfr/tests/leduc_eq0.strat");
assert(eq0.policy.size() == 144);

// Generate a default strategy for player 1
eq1 = Strategy(1);
eq1.build_default(*leduc_gt.get());
eq1.save_to_file("/home/achao/rl/cppcfr/tests/leduc_eq1.strat");
assert(eq1.policy.size() == 144);

Strategy rand0(0);
rand0.load_from_file("/home/achao/rl/cppcfr/strategies/leduc/random.strat");
Strategy rand1(1);
rand1.load_from_file("/home/achao/rl/cppcfr/strategies/leduc/random.strat");

/*
All leduc test values were derived using the open_cfr implementation from UoAlberta
*/
std::cout<<"Leduc Expected Value"<<std::endl;
profile = StrategyProfile(leduc_rules, {s0,s1});
profile.publictree = leduc_gt;
vector<float> result = profile.expected_value();
std::cout<<"Nash0 vs. Nash1 EV: "<<result<<std::endl;
assert(result[0] >= -0.085653 and result[0] <= -0.085651);

profile = StrategyProfile(leduc_rules, {s0,eq1});
profile.publictree = leduc_gt;
result = profile.expected_value();
std::cout<<"Nash0 vs. Eq1 EV: "<<result<<std::endl;
assert(result[0] >= 0.59143 and result[0] <= 0.59145);

profile = StrategyProfile(leduc_rules, {eq0,eq1});
profile.publictree = leduc_gt;
result = profile.expected_value();
std::cout<<"Eq0 vs. Eq1: "<<result<<std::endl;
assert(result[0] >= -0.078126 and result[0] <= -0.078124);

profile = StrategyProfile(leduc_rules, {eq0,s1});
profile.publictree = leduc_gt;
result = profile.expected_value();
std::cout<<"Eq0 vs. Nash1 EV: "<<result<<std::endl;
assert(result[0] >= -0.840442 and result[0] <= -0.840440);

profile = StrategyProfile(leduc_rules, {s0,rand1});
profile.publictree = leduc_gt;
result = profile.expected_value();
std::cout<<"Nash0 vs. Random: "<<result<<std::endl;
assert(result[0] >= 0.591873 and result[0] <= 0.591875);

profile = StrategyProfile(leduc_rules, {rand0,s1});
profile.publictree = leduc_gt;
result = profile.expected_value();
std::cout<<"Random vs. Nash1: "<<result<<std::endl;
assert(result[0] >= -0.84791 and result[0] <= -0.84790);

std::cout<<std::endl;

std::cout<<"Leduc Best Response"<<std::endl;
profile = StrategyProfile(leduc_rules, {s0,s1});
res = profile.best_response();
br = res.first.strategies[0];
ev = res.second[0];
std::cout<<"Nash0 BR EV: "<<ev<<std::endl;
br.save_to_file("/home/achao/rl/cppcfr/tests/leduc_nash0_br.strat");
assert(near(ev, -0.0854363));
validate_strategy(br, "/home/achao/rl/cppcfr/strategies/leduc/nash_br0.strat");

br = res.first.strategies[1];
ev = res.second[1];
std::cout<<"Nash1 BR EV: "<<ev<<std::endl;
br.save_to_file("/home/achao/rl/cppcfr/tests/leduc_nash1_br.strat");
assert(near(ev, 0.0858749));
validate_strategy(br, "/home/achao/rl/cppcfr/strategies/leduc/nash_br1.strat");

profile = StrategyProfile(leduc_rules, {eq0,eq1});
res = profile.best_response();
br = res.first.strategies[0];
ev = res.second[0];
std::cout<<"Eq0 BR EV: "<<ev<<std::endl;
assert(near(ev, 2.0875));
validate_strategy(br, "/home/achao/rl/cppcfr/strategies/leduc/equal_br0.strat");

br = res.first.strategies[1];
ev = res.second[1];
std::cout<<"Eq1 BR EV: "<<ev<<std::endl;
assert(near(ev, 2.65972));
validate_strategy(br, "/home/achao/rl/cppcfr/strategies/leduc/equal_br1.strat");

profile = StrategyProfile(leduc_rules, {rand0, rand1});
res = profile.best_response();
br = res.first.strategies[0];
ev = res.second[0];
std::cout<<"Rand0 BR: "<<ev<<std::endl;
assert(near(ev, 2.14414));
validate_strategy(br, "/home/achao/rl/cppcfr/strategies/leduc/random_br0.strat");

br = res.first.strategies[1];
ev = res.second[1];
std::cout<<"Rand1 BR: "<<ev<<std::endl;
assert(near(ev, 3.21721));
validate_strategy(br, "/home/achao/rl/cppcfr/strategies/leduc/random_br1.strat");

std::cout<<std::endl;

std::cout<<"All passed!"<<std::endl;
}
