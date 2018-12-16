#ifndef POKERGAMES_HPP_
#define POKERGAMES_HPP_
#include "card.hpp"
#include "pokertrees.hpp"

namespace rl{

float kuhn_eval(vector<Card> hc, vector<Card> board);
GameRules half_street_kuhn_rules();
GameTree half_street_kuhn_gametree();
PublicTree half_street_kuhn_publictree();
GameRules kuhn_rules();
GameTree kuhn_gametree();
PublicTree kuhn_publictree();
string leduc_format(int player, vector<Card> holecards, vector<Card> board, string bet_history);
float leduc_eval(vector<Card> hc, vector<Card> board);
GameRules leduc_rules();
GameTree leduc_gametree();
PublicTree leduc_publictree();
string royal_format(int player, vector<Card> holecards, vector<Card> board, string bet_history);
float royal_eval(vector<Card> hc, vector<Card> board);
GameRules royal_rules();
GameTree royal_gametree();
PublicTree royal_publictree();

}

#endif
