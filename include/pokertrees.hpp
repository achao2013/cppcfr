#ifndef POKERTREES_HPP_
#define POKERTREES_HPP_
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "hand_evaluator.hpp"
#include "card.hpp"
#include <assert.h>
#include <memory>
#include <functional>

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::shared_ptr;
using namespace std::placeholders;

namespace rl{

bool overlap(Card t1, vector<Card> t2 );
bool overlap(vector<Card> t1, vector<Card> t2 );
bool all_unique2(vector<vector<Card> >  hc);
bool all_unique(vector<Card> hc);
class RoundInfo
{
public:
  RoundInfo(int holecards, int boardcards, int betsize, vector<int> maxbets)
  {
    this->holecards = holecards;
    this->boardcards = boardcards;
    this->betsize = betsize;
    this->maxbets = maxbets;
  }
  int holecards;
  int boardcards;
  int betsize;
  vector<int> maxbets;
};
class HandEvaluator;
typedef string (*infoset_format_type)(int,  vector<Card>,  vector<Card>, string);
typedef string (*multi_infoset_format_type)(int,  vector<vector<Card> >,  vector<Card>, string);
string default_infoset_format(int player, vector<Card> holecards,vector<Card> board, string bet_history);
class InfosetFormat
{
	public:
		string operator()(int player, vector<Card> holecards, vector<Card> boardcards, string bet_history){
			return infoset_format(player, holecards, boardcards, bet_history);
		}
		vector<string> operator()(int player, vector<vector<Card> > holecards, vector<Card> boardcards, string bet_history){
			vector<string> res;
			for(int i =0; i< holecards.size(); i++)
			  res.push_back(infoset_format(player, holecards[i], boardcards, bet_history));
			return res;
		}
		infoset_format_type infoset_format;
		InfosetFormat(infoset_format_type ift = default_infoset_format){
			infoset_format = ift;
		}
};

class GameRules
{
public:
  typedef float (*handeval_type)(vector<Card>, vector<Card>);
  int players;
  vector<Card> deck;
  vector<RoundInfo> roundinfo;
  int ante;
  vector<int> blinds;
  InfosetFormat infoset_format;
  handeval_type handeval;
  GameRules(){}
  GameRules(int players, vector<Card> deck, vector<RoundInfo> rounds, int ante, vector<int> blinds, handeval_type handeval=&HandEvaluator::evaluate_hand, InfosetFormat infoset_format = InfosetFormat(default_infoset_format))
  {
	  assert(players >= 2);
	  assert(ante >= 0);
	  assert(deck.size()>=2);
	  assert(rounds.size() > 0);
	  for(auto r=rounds.begin(); r!=rounds.end(); r++)
		assert(r->maxbets.size() == players);
	  this->players = players;
	  this->deck = deck;
	  this->roundinfo = rounds;
	  this->ante = ante;
	  this->blinds = blinds;
	  this->handeval = handeval;
	  this->infoset_format = infoset_format;
  }

  GameRules(const GameRules& gr)
  {
	  this->players = gr.players;
	  this->deck = gr.deck;
	  this->roundinfo = gr.roundinfo;
	  this->ante = gr.ante;
	  this->blinds = gr.blinds;
	  this->handeval = gr.handeval;
	  this->infoset_format = gr.infoset_format;

  }
};
class Node;
class TerminalNode;
class HolecardChanceNode;
class BoardcardChanceNode;
class ActionNode;

class GameTree
{
	public:
		GameTree(GameRules rules)
		{
			this->rules=rules;
		}
		GameTree(){}
		GameTree(const GameTree& gt)
		{
			rules = gt.rules;
			information_sets = gt.information_sets;
			root = std::make_shared<Node>(*gt.root.get());
		}
		void build();
		int collect_blinds(vector<int>& committed, vector<int>& bets, int next_player);
		shared_ptr<Node> build_rounds(shared_ptr<Node> root, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, vector<int> bets = vector<int>(0), int next_player = 0);
		vector<vector<vector<Card> > > deal_holecards(vector<Card>& deck, int holecards, int players);
		int get_next_player(int cur_player, vector<bool> players_in);
		shared_ptr<Node> build_holecards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets);
		shared_ptr<Node> build_boardcards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets);
		shared_ptr<Node> build_bets(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		bool all_called_last_raisor_or_folded(vector<bool> players_in, vector<int> bets);
		void add_fold_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		void add_call_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		void add_raise_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		shared_ptr<Node> showdown(shared_ptr<Node> root, vector<bool> players_in, vector<int> committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history);
		vector<pair<vector<Card>,float> > holecard_distributions(int holecards);

		GameRules rules;
		map<string, vector<shared_ptr<Node> > > information_sets;//point to ActionNode
		shared_ptr<Node> root;
		static int count;
};


class PublicTree: public GameTree
{
	public:
		PublicTree(GameRules rules):GameTree(GameRules(rules.players, rules.deck, rules.roundinfo, rules.ante, rules.blinds, rules.handeval, rules.infoset_format))
		{
		}
		PublicTree(const PublicTree& pt)
		{
			rules = pt.rules;
			information_sets = pt.information_sets;
			root = std::make_shared<Node>(*pt.root.get());
		}
		void build();
		shared_ptr<Node> build_holecards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets);
		shared_ptr<Node> build_boardcards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets);
		shared_ptr<Node> build_rounds(shared_ptr<Node> root, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, vector<int> bets = vector<int>(0), int next_player=0);
		shared_ptr<Node> build_bets(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		void add_fold_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		void add_call_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		void add_raise_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round);
		shared_ptr<Node> showdown(shared_ptr<Node> root, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history);
		vector<vector<vector<Card> > > showdown_combinations(vector<vector<vector<Card> > > holes);
		vector<float> calc_payoffs(vector<vector<Card> > hands, map<vector<Card>, float> scores, vector<bool> players_in, vector<int> committed, int pot);

		map<vector<string>, vector<shared_ptr<Node> > > information_sets;//point to ActionNode
};

class Node
{
	public:
		Node(shared_ptr<Node> parent, vector<int> committed, vector<vector<Card> > holecards, vector<Card> board, vector<Card> deck, string bet_history)
		{
			this->committed=committed;
			this->holecards_g=holecards;
			this->board=board;
			this->deck=deck;
			this->bet_history=bet_history;
		}
		Node(shared_ptr<Node> parent, vector<int> committed, vector<vector<vector<Card> > > holecards, vector<Card> board, vector<Card> deck, string bet_history)
		{
			this->committed=committed;
			this->holecards=holecards;
			this->board=board;
			this->deck=deck;
			this->bet_history=bet_history;
		}
		Node(){}
		Node(const Node& other)
		{
			parent = other.parent;
			committed = other.committed;
			holecards_g = other.holecards_g;
			holecards = other.holecards;
			board = other.board;
			deck = other.deck;
			bet_history = other.bet_history;
			children = other.children;
		}
		virtual void add_child(shared_ptr<Node> child)
		{
			this->children.push_back(child);
		}
		shared_ptr<Node> parent;
		vector<int> committed;
		vector<vector<Card> >  holecards_g; //unused
		vector<vector<vector<Card> > >  holecards; //used in cfr
		vector<Card> board;
		vector<Card> deck;
		string bet_history;
		vector<shared_ptr<Node> > children;
};
class TerminalNode: public Node
{
	public:
		TerminalNode(){}
		TerminalNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<Card> > holecards, vector<Card> board, vector<Card> deck, string bet_history, vector<float> payoffs, vector<bool> players_in): Node(parent,committed, holecards, board, deck, bet_history)
		{
			this->payoffs_g=payoffs;
			this->players_in=players_in;
			if(parent)
			{
				this->parent = parent;
				//shared_ptr<Node> sp(this);
				//this->parent->add_child(sp);
			}
		}
		TerminalNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<vector<Card> > > holecards, vector<Card> board, vector<Card> deck, string bet_history, map<vector<vector<Card> >, vector<float> > payoffs, vector<bool> players_in): Node(parent,committed, holecards, board, deck, bet_history)
		{
			this->payoffs=payoffs;
			this->players_in=players_in;
			if(parent)
			{
				this->parent = parent;
				//shared_ptr<Node> sp;
				//sp.reset(this);
				//this->parent->add_child(sp);
			}
		}
		map<vector<vector<Card> >, vector<float> > payoffs;
		vector<float> payoffs_g;
		vector<bool> players_in;
};
class HolecardChanceNode: public Node
{
	public:
		HolecardChanceNode(){}
		HolecardChanceNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<Card> > holecards, vector<Card> board, vector<Card> deck, string bet_history, int todeal ): Node(parent,committed, holecards, board, deck, bet_history)
	{
		if(parent)
		{
			this->parent = parent;
			shared_ptr<Node> sp(this);
			this->parent->add_child(sp);
		}
		this->todeal=todeal;
	}
		HolecardChanceNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<vector<Card> > > holecards, vector<Card> board, vector<Card> deck, string bet_history, int todeal ): Node(parent,committed, holecards, board, deck, bet_history)
	{
		if(parent)
		{
			this->parent = parent;
			shared_ptr<Node> sp;
			sp.reset(this);
			this->parent->add_child(sp);
		}
		this->todeal=todeal;
	}
		int todeal;
};
class BoardcardChanceNode: public Node
{
	public:
		BoardcardChanceNode(){}
		BoardcardChanceNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<Card> > holecards, vector<Card> board, vector<Card> deck, string bet_history, int todeal): Node(parent,committed, holecards, board, deck, bet_history)
	{
		if(parent)
		{
			this->parent = parent;
			shared_ptr<Node> sp(this);
			this->parent->add_child(sp);
		}
		this->todeal=todeal;
	}
		BoardcardChanceNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<vector<Card> > > holecards, vector<Card> board, vector<Card> deck, string bet_history, int todeal): Node(parent,committed, holecards, board, deck, bet_history)
	{
		if(parent)
		{
			this->parent = parent;
			shared_ptr<Node> sp;
			sp.reset(this);
			this->parent->add_child(sp);
		}
		this->todeal=todeal;
	}
		int todeal;
};
class ActionNode: public Node
{
	public:
		enum ACTION_TYPE{
			FOLD,
			CALL,
			RAISE
		};
		ActionNode(){}
		ActionNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<Card> > holecards, vector<Card> board, vector<Card> deck, string bet_history, int player, InfosetFormat infoset_format):Node(parent, committed, holecards, board, deck, bet_history)
	{
		if(parent)
		{
			this->parent = parent;
			shared_ptr<Node> sp(this);
			this->parent->add_child(sp);
		}
		this->player = player;
		this->player_view_g = infoset_format(player, holecards[player], board, bet_history);
	}
		ActionNode(shared_ptr<Node> parent, vector<int> committed, vector<vector<vector<Card> > > holecards, vector<Card> board, vector<Card> deck, string bet_history, int player, InfosetFormat infoset_format):Node(parent, committed, holecards, board, deck, bet_history)
	{
		if(parent)
		{
			this->parent = parent;
			shared_ptr<Node> sp;
			sp.reset(this);
			this->parent->add_child(sp);
		}
		this->player = player;
		this->player_view = infoset_format(player, holecards[player], board, bet_history);
	}
		shared_ptr<Node> valid(int action)
		{
			if(action == FOLD)
			  return this->fold_action;
			if(action == CALL)
			  return this->call_action;
			if(action == RAISE)
			  return this->raise_action;
			return NULL;
		}
		shared_ptr<Node> get_child(int action)
		{
			return valid(action);
		}

		int player;
		shared_ptr<Node> raise_action;
		shared_ptr<Node> call_action;
		shared_ptr<Node> fold_action;
		string player_view_g;
		vector<string> player_view;
};

}

#endif
