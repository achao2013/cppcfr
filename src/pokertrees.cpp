#include "pokertrees.hpp"

#include <typeinfo>
namespace rl{

bool overlap(vector<Card> t1, vector<Card> t2 )
{
	for(auto it=t1.begin(); it!=t1.end(); it++)
	{
		if(std::find(t2.begin(), t2.end(), *it)!=t2.end())
		  return true;
	}
	return false;
}
bool overlap(Card t1, vector<Card> t2 )
{
	return std::find(t2.begin(), t2.end(), t1) != t2.end();
}
bool all_unique2(vector<vector<Card> >  hc)
{
	for(auto it1=hc.begin(); it1!=hc.end()-1; it1++)
	  for(auto it2=it1+1; it2!=hc.end(); it2++)
		if(overlap(*it1,*it2))
		  return false;
	return true;
}
bool all_unique(vector<Card> hc)
{
	for(auto it1=hc.begin(); it1!=hc.end(); it1++)
	  for(auto it2=it1+1; it2!=hc.end(); it2++)
		if(*it1 == *it2)
		  return false;
	return true;
}

string default_infoset_format(int player, vector<Card> holecards,vector<Card> boardcards, string bet_history)
{
	string s1="";
	for(auto it=holecards.begin(); it!=holecards.end(); it++)
	{
		s1+=it->repr();
	}
	for(auto it=boardcards.begin(); it!=boardcards.end(); it++)
	{
		s1+=it->repr();
	}
	s1+=":";
	s1+=bet_history+":";
	return s1;
}
int GameTree::count = 1;
void GameTree::build()
{
	vector<bool> players_in(this->rules.players, true);
	vector<int> committed(this->rules.players,this->rules.ante);
	vector<int> bets(this->rules.players,0);
	int next_player = collect_blinds(committed,bets,0); 
	vector<vector<Card> > holes(this->rules.players);
	vector<Card> board;
	string bet_history="";
	this->root=build_rounds(NULL,players_in,committed,holes,board,this->rules.deck,bet_history,0,bets,next_player);
}
int GameTree::collect_blinds(vector<int>& committed, vector<int>& bets, int next_player)
{
	if(!this->rules.blinds.empty())
	{
		for(auto it=this->rules.blinds.begin(); it!=this->rules.blinds.end(); it++)
		{
			committed[next_player]+=(*it);
			bets[next_player]=int((committed[next_player]-this->rules.ante)/this->rules.roundinfo[0].betsize);
			next_player=(next_player+1)%this->rules.players;
		}
	}
	return next_player;
}

 vector<vector<vector<Card> > > GameTree::deal_holecards(vector<Card>& deck, int holecards, int players)
{
	vector<vector<Card> > res1;
	vector<Card> r(holecards);
	combinations(deck.size(), holecards, r, holecards, deck, res1);
	vector<vector<vector<Card> > > res2;
	vector<vector<Card> > rr(players);
	rr.resize(players);
	combinations(res1.size(),players, rr, players, res1, res2);
	vector<vector<vector<Card> > > res;
	for(int i=0; i<res2.size(); i++)
	{
		vector<vector<vector<Card> > > tmp;
		permutations_range(res2[i], 0, players, tmp);
		std::copy(tmp.begin(),tmp.end(),std::back_inserter(res));
	}
	vector<vector<vector<Card> > > final_res;
	std::copy_if(res.begin(),res.end(), std::back_inserter(final_res), all_unique2);
	return final_res;

}

shared_ptr<Node> GameTree::build_rounds(shared_ptr<Node> root, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, vector<int> bets, int next_player)
{
	if(round_idx == this->rules.roundinfo.size())
	  return showdown(root,  players_in, committed, holes, board, deck, bet_history);
	bet_history += "/";
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	while(!players_in[next_player])
	  next_player = (next_player+1)%this->rules.players;
	if(bets.empty())
	  bets=vector<int>(this->rules.players, 0);
	int min_actions_this_round=std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;});
	int actions_this_round=0;
	if(cur_round.holecards)
	  return build_holecards(root, next_player, players_in, committed, holes, board, deck ,bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	if(cur_round.boardcards)
	  return build_boardcards(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	return build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);

}

int GameTree::get_next_player(int cur_player, vector<bool> players_in)
{
	int next_player = (cur_player+1) % this->rules.players;
	while(!players_in[next_player])
	  next_player = (next_player + 1) % this->rules.players;
	return next_player;
}

shared_ptr<Node> GameTree::build_holecards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets)
{
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	shared_ptr<Node> hnode(new HolecardChanceNode(root,committed,holes,board,rules.deck,"",cur_round.holecards));
	vector<vector<vector<Card> > > all_hc = this->deal_holecards(deck, cur_round.holecards, std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;}));
	for(auto cur_holes : all_hc)
	{
		vector<Card> dealt_cards;
		int cur_idx = 0;
		int i = 0;
		for(auto it=holes.begin(); it!=holes.end(); it++,i++)
		  if(players_in[i])
		  {
			  cur_holes[cur_idx] = *it+cur_holes[cur_idx];
			  cur_idx+=1;
		  }
		for(auto hc : cur_holes)
		  dealt_cards.insert(dealt_cards.end(),hc.begin(),hc.end());
		vector<Card> cur_deck;
		std::remove_copy_if(deck.begin(),deck.end(),std::back_inserter(cur_deck),[&dealt_cards](Card card){return std::find(dealt_cards.begin(), dealt_cards.end(), card) != dealt_cards.end();});

		if(cur_round.boardcards)
		  build_boardcards(hnode, next_player, players_in, committed, cur_holes, board, cur_deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
		else
		  build_bets(hnode, next_player, players_in, committed, cur_holes, board, cur_deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	}
	return hnode;
}

	shared_ptr<Node> GameTree::build_boardcards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets)
{

	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	shared_ptr<Node> bnode(new BoardcardChanceNode(root,committed,holes,board,deck,bet_history,cur_round.boardcards));
	vector<vector<Card> > all_bc;
	vector<Card> r(cur_round.boardcards);
	combinations(deck.size(), cur_round.boardcards, r, cur_round.boardcards, deck, all_bc);
	for(auto bc : all_bc)
	{
		vector<Card> cur_board = board;
		cur_board.insert(cur_board.end(),bc.begin(),bc.end());
		vector<Card> cur_deck;
		std::remove_copy_if(deck.begin(),deck.end(),std::back_inserter(cur_deck),[&bc](Card card){return std::find(bc.begin(), bc.end(), card) != bc.end();});
		build_bets(bnode, next_player, players_in, committed, holes, cur_board, cur_deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	}
	return bnode;
}
	shared_ptr<Node> GameTree::build_bets(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{
    // if everyone else folded, end the hand
	if(std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;})==1)
	{
		shared_ptr<Node> res = showdown(root, players_in, committed, holes, board, deck, bet_history);
		//root->add_child(res);
		return NULL;
	}

	// if everyone checked or the last raisor has been called, end the round
	if(actions_this_round >= min_actions_this_round && all_called_last_raisor_or_folded(players_in, bets_this_round))
	{    
		build_rounds(root, players_in, committed, holes, board, deck, bet_history, round_idx + 1);
		return NULL;
	}
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	shared_ptr<Node> anode(new ActionNode(root, committed, holes, board, deck, bet_history, next_player, this->rules.infoset_format));
	// add the node to the information set
	this->information_sets[std::static_pointer_cast<ActionNode>(anode)->player_view_g].push_back(anode);
	// get the next player to act
	next_player = get_next_player(next_player, players_in);
	// add a folding option if someone has bet more than this player
	if(committed[std::static_pointer_cast<ActionNode>(anode)->player] < *std::max_element(committed.begin(),committed.end()))
	  add_fold_child(anode, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets_this_round);
	// add a calling/checking option
	add_call_child(anode, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets_this_round);
	// add a raising option if this player has not reached their max bet level
	if(cur_round.maxbets[std::static_pointer_cast<ActionNode>(anode)->player] >*std:: max_element(bets_this_round.begin(), bets_this_round.end()))
	  add_raise_child(anode, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets_this_round);
	return anode;
}

bool GameTree::all_called_last_raisor_or_folded(vector<bool> players_in, vector<int> bets)
{
	int betlevel = *std::max_element(bets.begin(), bets.end());
	int i=0;
	for(auto it=bets.begin();it!=bets.end(),i<bets.size(); it++,i++)
	  if(players_in[i] && bets[i] < betlevel)
		return false;
	return true;
}

void GameTree::add_fold_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{
	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	players_in[root_player] = false;
	bet_history += 'f';
	build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round + 1, bets_this_round);
	std::static_pointer_cast<ActionNode>(root)->fold_action = root->children[root->children.size()-1];
	players_in[root_player] = true;
}

void GameTree::add_call_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{

	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	int player_commit = committed[root_player];
	int player_bets = bets_this_round[root_player];
	committed[root_player] = *(std::max_element(committed.begin(),committed.end()));
	bets_this_round[root_player] = *(std::max_element(bets_this_round.begin(),bets_this_round.end()));
	bet_history += 'c';
	build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round + 1, bets_this_round);
	std::static_pointer_cast<ActionNode>(root)->call_action = root->children[root->children.size()-1];
	committed[root_player] = player_commit;
	bets_this_round[root_player] = player_bets;
}

void GameTree::add_raise_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{

	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	int prev_betlevel = bets_this_round[root_player];
	int prev_commit = committed[root_player];
	bets_this_round[root_player] = *(std::max_element(bets_this_round.begin(),bets_this_round.end())) + 1;
	committed[root_player] += (bets_this_round[root_player] - prev_betlevel) * cur_round.betsize;
	bet_history += 'r';
	build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round + 1, bets_this_round);
	std::static_pointer_cast<ActionNode>(root)->raise_action = root->children[root->children.size()-1];
	bets_this_round[root_player] = prev_betlevel;
	committed[root_player] = prev_commit;
}

shared_ptr<Node> GameTree::showdown(shared_ptr<Node> root, vector<bool> players_in, vector<int> committed, vector<vector<Card> >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history)
{
	vector<int> winners;
	if(std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;})==1)
	{
		for(int i=0; i<players_in.size(); i++)
		  if(players_in[i])
			winners.push_back(i);
		//winners.push_back(std::distance(players_in.begin(),std::find(players_in.begin(),players_in.end(),true)));
	}
	else
	{
		vector<float> scores(holes.size());
		std::transform(holes.begin(),holes.end(),scores.begin(),[&board,this](vector<Card> hc){return this->rules.handeval(hc,board);});
		float max_score = -1;
		for(int i = 0; i<scores.size(); i++)
		{
			if(players_in[i])
			{
			  if(winners.size() == 0 || scores[i] > max_score)
			  {
				  max_score = scores[i];
				  vector<int> tmp(1,i);
				  winners=tmp;
			  }
			  else if(fabs(scores[i] - max_score)<1e-15)
				winners.push_back(i);
			}

		}
	}
	int pot = std::accumulate(committed.begin(),committed.end(),0);
	float payoff = pot / float(winners.size());
	vector<float> payoffs;
	std::transform(committed.begin(),committed.end(),std::back_inserter(payoffs),[](int c){return -c;});
	for(auto w : winners)
	  payoffs[w]+=payoff;
	//shared_ptr<Node> res_node = std::make_shared<TerminalNode>(root, committed, holes, board, deck, bet_history, payoffs, players_in);
	shared_ptr<Node> res_node(new TerminalNode(root, committed, holes, board, deck, bet_history, payoffs, players_in));
	root->add_child(res_node);
	return res_node;
}

vector<pair<vector<Card>,float> > GameTree::holecard_distributions(int holecards)
{
	vector<vector<Card> > res1;
	vector<Card> r(holecards);
	combinations(this->rules.deck.size(), holecards, r, holecards, this->rules.deck, res1);
	map<vector<Card>, int> x;
	for (auto hc : res1)
	{
		if(x.find(hc)!=x.end())
		  x[hc]+=1;
		else
		  x[hc]=1;
	}
	vector<int> values;
	std::transform(x.begin(),x.end(),values.begin(),[](pair<vector<Card>,int> p){return p.second;});
	float d = float(std::accumulate(values.begin(),values.end(),0));
	vector<pair<vector<Card>,float> > res;
	int i=0;
	for(auto it = x.begin(); it!=x.end(),i<values.size(); it++,i++)
	  res.push_back(std::make_pair(it->first,values[i]/d));
	return res;
}


void PublicTree::build()
{
	vector<bool> players_in(this->rules.players, true);
	vector<int> committed(this->rules.players,this->rules.ante);
	vector<int> bets(this->rules.players,0);
	int next_player = collect_blinds(committed,bets,0); 
	vector<vector<vector<Card> > > holes(this->rules.players);
	vector<Card> board;
	string bet_history="";
	shared_ptr<Node> nul;
	this->root=build_rounds(nul,players_in,committed,holes,board,this->rules.deck,bet_history,0,bets,next_player);
}

shared_ptr<Node> PublicTree::build_rounds(shared_ptr<Node> root, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, vector<int> bets, int next_player)
{
	//std::cout<<".build round "<<round_idx<<std::endl;
	if(round_idx == this->rules.roundinfo.size())
	  return showdown(root,  players_in, committed, holes, board, deck, bet_history);
	bet_history += "/";
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	while(!players_in[next_player])
	  next_player = (next_player+1)%this->rules.players;
	if(bets.empty())
	  bets=vector<int>(this->rules.players);
	int min_actions_this_round=std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;});
	int actions_this_round=0;
	//std::cout<<"holecards:"<<cur_round.holecards<<std::endl;
	if(cur_round.holecards)
	  return build_holecards(root, next_player, players_in, committed, holes, board, deck ,bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	if(cur_round.boardcards)
	  return build_boardcards(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	return build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);

}

shared_ptr<Node> PublicTree::build_holecards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets)
{
	//std::cout<<"start build holecards"<<std::endl;
	RoundInfo cur_round = rules.roundinfo[round_idx];
	shared_ptr<Node> hnode(new HolecardChanceNode(root, committed, holes, board, rules.deck, "", cur_round.holecards));
	// Deal holecards
	vector<vector<Card> > all_hc;
	vector<Card> r(cur_round.holecards);
	//std::cout<<"deck:"<<deck<<" "<<cur_round.holecards<<std::endl;
	combinations(deck.size(), cur_round.holecards, r, cur_round.holecards, deck, all_hc);
	//std::cout<<"all_hc"<<all_hc<<std::endl;
	vector<vector<vector<Card> > > updated_holes;
	for(int player=0; player < this->rules.players; player++)
	{
		if(!players_in[player])
		{
			// Only deal to players who are still : the hand
			vector<vector<Card> > tmp;
			for(auto old_hc : holes[player])
			  tmp.push_back(old_hc);
			updated_holes.push_back(tmp);
		}
		else if(holes[player].size() == 0)
		{
			// If this player has no cards, just set their holecards to be the newly dealt ones
			vector<vector<Card> > tmp;
			for(auto new_hc : all_hc)
			  tmp.push_back(new_hc);
			updated_holes.push_back(tmp);
		}
		else
		{
			// Filter holecards to valid combinations
			// TODO: Speed this up by removing duplicate holecard combinations
			for(auto new_hc : all_hc)
			  for(auto old_hc : holes[player])
				if(!overlap(old_hc, new_hc))
				{
					updated_holes[player].push_back(old_hc+new_hc);
				}
		}
	}
	if(cur_round.boardcards)
	  build_boardcards(hnode, next_player, players_in, committed, updated_holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	else
	{
	  //std::cout<<".build bets in building holecards"<<", cur_round.boardcards:"<<cur_round.boardcards<<std::endl;
	  build_bets(hnode, next_player, players_in, committed, updated_holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	}
	return hnode;
}

template<class T>
vector<T> operator+(vector<T> left, vector<T> right)
{
	vector<T> res;
	std::copy(left.begin(),left.end(),std::back_inserter<T>(res));
	std::copy(right.begin(),right.end(),std::back_inserter<T>(res));
	return res;
}
shared_ptr<Node> PublicTree::build_boardcards(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets)
{
	//std::cout<<".build boardcards"<<std::endl;
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	shared_ptr<Node> bnode(new BoardcardChanceNode(root, committed, holes, board, deck, bet_history, cur_round.boardcards));
	vector<vector<Card> > all_bc;
	vector<Card> r(cur_round.boardcards);
	//std::cout<<"deck:"<<deck<< " boardcards:"<<cur_round.boardcards<<std::endl;
	combinations(deck.size(),cur_round.boardcards, r, cur_round.boardcards, deck, all_bc);
	//std::cout<<"all_bc:"<<all_bc<<std::endl;
	for(auto bc : all_bc){
		vector<Card> cur_board = board + bc;
		vector<Card> cur_deck;
		std::remove_copy_if(deck.begin(),deck.end(),std::back_inserter(cur_deck),[&bc](Card card){return std::find(bc.begin(), bc.end(), card) != bc.end();});
		vector<vector<vector<Card> > > updated_holes;
		// Filter any holecards that are now impossible
		for(int player=0; player<rules.players; player++)
		{
			updated_holes.push_back(vector<vector<Card> >());
			for(auto hc : holes[player])
			  if(!overlap(hc, bc))
				updated_holes[player].push_back(hc);
		}
		//std::cout<<".build bets in building boardcards with "<<bet_history<<std::endl;
		build_bets(bnode, next_player, players_in, committed, updated_holes, cur_board, cur_deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets);
	}
	return bnode;
}

std::ostream& operator<<(std::ostream& out, const vector<string>& vs)
{
	out<<"(";
	for(auto it2 = vs.begin(); it2 != vs.end(); it2++)
	  out<<*it2<<",";
	out<<")";
	return out;
}
shared_ptr<Node> PublicTree::build_bets(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string& bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{
	//std::cout<<"players_in:"<<std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;})<<std::endl;
	// if everyone else folded, end the hand
	if(std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;})==1)
	{
		//std::cout<<"shown_down in building bets"<<std::endl;
		showdown(root, players_in, committed, holes, board, deck, bet_history);
		return NULL;
	}

	// if everyone checked or the last raisor has been called, end the round
	if(actions_this_round >= min_actions_this_round && all_called_last_raisor_or_folded(players_in, bets_this_round))
	{    
		//std::cout<<".build round "<<round_idx+1<<" in building bets with "<<bet_history<<" commit:"<<committed[0]<<" "<<committed[1]<<std::endl;
		build_rounds(root, players_in, committed, holes, board, deck, bet_history, round_idx + 1);
		return NULL;
	}
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	shared_ptr<Node> anode(new ActionNode(root, committed, holes, board, deck, bet_history, next_player, this->rules.infoset_format));
	// add the node to the information set
	this->information_sets[std::static_pointer_cast<ActionNode>(anode)->player_view].push_back(anode);
	// get the next player to act
	next_player = get_next_player(next_player, players_in);
	//std::cout<<"player:"<<std::static_pointer_cast<ActionNode>(anode)->player<<" play_view:"<<std::static_pointer_cast<ActionNode>(anode)->player_view<<" commit:"<<committed[std::static_pointer_cast<ActionNode>(anode)->player]<<" max commit:"<<*std::max_element(committed.begin(),committed.end())<<std::endl;
	// add a folding option if someone has bet more than this player
	if(committed[std::static_pointer_cast<ActionNode>(anode)->player] < *std::max_element(committed.begin(),committed.end()))
	  add_fold_child(anode, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets_this_round);
	// add a calling/checking option
	add_call_child(anode, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets_this_round);
	// add a raising option if this player has not reached their max bet level
	if(cur_round.maxbets[std::static_pointer_cast<ActionNode>(anode)->player] >*std:: max_element(bets_this_round.begin(), bets_this_round.end()))
	  add_raise_child(anode, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round, bets_this_round);
	return anode;
}
void PublicTree::add_fold_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{
	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	players_in[root_player] = false;
	bet_history += 'f';
	//std::cout<<".build bets in add_fold_child"<<std::endl;
	build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round + 1, bets_this_round);
	std::static_pointer_cast<ActionNode>(root)->fold_action = root->children[root->children.size()-1];
	players_in[root_player] = true;
}

void PublicTree::add_call_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{

	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	int player_commit = committed[root_player];
	int player_bets = bets_this_round[root_player];
	committed[root_player] = *(std::max_element(committed.begin(),committed.end()));
	bets_this_round[root_player] = *(std::max_element(bets_this_round.begin(),bets_this_round.end()));
	bet_history += 'c';
	//std::cout<<".build bets in add_call_child"<<std::endl;
	build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round + 1, bets_this_round);
	std::static_pointer_cast<ActionNode>(root)->call_action = root->children[root->children.size()-1];
	committed[root_player] = player_commit;
	bets_this_round[root_player] = player_bets;
}

void PublicTree::add_raise_child(shared_ptr<Node> root, int next_player, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history, int round_idx, int min_actions_this_round, int actions_this_round, vector<int>& bets_this_round)
{

	int root_player = std::static_pointer_cast<ActionNode>(root)->player;
	RoundInfo cur_round = this->rules.roundinfo[round_idx];
	int prev_betlevel = bets_this_round[root_player];
	int prev_commit = committed[root_player];
	bets_this_round[root_player] = *(std::max_element(bets_this_round.begin(),bets_this_round.end())) + 1;
	committed[root_player] += (bets_this_round[root_player] - prev_betlevel) * cur_round.betsize;
	bet_history += 'r';
	//std::cout<<".build bets in add_raise_child"<<std::endl;
	build_bets(root, next_player, players_in, committed, holes, board, deck, bet_history, round_idx, min_actions_this_round, actions_this_round + 1, bets_this_round);
	std::static_pointer_cast<ActionNode>(root)->raise_action = root->children[root->children.size()-1];
	bets_this_round[root_player] = prev_betlevel;
	committed[root_player] = prev_commit;
}


shared_ptr<Node> PublicTree::showdown(shared_ptr<Node> root, vector<bool>& players_in, vector<int>& committed, vector<vector<vector<Card> > >& holes, vector<Card>& board, vector<Card>& deck, string bet_history)
{
	// TODO: Speedup
	// - Pre-order list of hands
	int pot = std::accumulate(committed.begin(),committed.end(),0);
	vector<vector<vector<Card> > > showdowns_possible = showdown_combinations(holes);
	map<vector<vector<Card> >, vector<float> > payoffs;
	if(std::count_if(players_in.begin(),players_in.end(),[](bool b){return b;})==1)
	{
		vector<float> fold_payoffs(committed.size());
		std::transform(committed.begin(),committed.end(),fold_payoffs.begin(),[](int x){return -x;});
		for(int i=0; i< fold_payoffs.size(); i++)
		  if(players_in[i])
			fold_payoffs[i]+=pot;
		for(auto hands : showdowns_possible)
		  payoffs[hands] = fold_payoffs;
	}
	else
	{
		map<vector<Card>, float> scores;
		for(int i=0; i < this->rules.players; i++)
		  if( players_in[i])
			for(auto hc : holes[i])
			  if(scores.find(hc)==scores.end())
				scores[hc] = this->rules.handeval(hc, board);
		for(auto hands : showdowns_possible)
		  payoffs[hands]=calc_payoffs(hands, scores, players_in, committed, pot);
	}
	shared_ptr<Node> res_node(new TerminalNode(root, committed, holes, board, deck, bet_history, payoffs, players_in));
	//std::cout<<"payoffs first:"<<payoffs.begin()->first<<std::endl;
	root->add_child(res_node);
	return res_node;
}
vector<vector<vector<Card> > > PublicTree::showdown_combinations(vector<vector<vector<Card> > > holes)
{
	// Get all the possible holecard matchups for a given showdown.
	// Every card must be unique because two players cannot have the same holecard.
	vector<vector<vector<Card> > > res;
	vector<vector<Card> > tmp;
	product(holes, res, 0, tmp);
	vector<vector<vector<Card> > > res_final;
	std::copy_if(res.begin(),res.end(),std::back_inserter(res_final), all_unique2);
	return res_final;
}

vector<float> PublicTree::calc_payoffs(vector<vector<Card> > hands, map<vector<Card>, float> scores, vector<bool> players_in, vector<int> committed, int pot)
{
	vector<int> winners;
	float maxscore = -1;
	for(int i=0; i<hands.size(); i++)
	  if( players_in[i])
	  {
		  float s = scores[hands[i]];
		  if(winners.size() == 0 || s > maxscore)
		  {
			  maxscore = s;
			  vector<int> tmp(1,i);
			  winners=tmp;
		  }
		  else if( fabs(s - maxscore)<1e-15 )
			winners.push_back(i);
	  }
	float payoff = pot / float(winners.size());
	vector<float> payoffs(committed.size());
	std::transform(committed.begin(),committed.end(),payoffs.begin(),[](int x){return -x;});
	for(auto w : winners)
	  payoffs[w] += payoff;
	return payoffs;    

}

}
