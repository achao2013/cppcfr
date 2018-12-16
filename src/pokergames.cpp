#include "pokergames.hpp"

namespace rl{

string leduc_format(int player, vector<Card> holecards, vector<Card> board, string bet_history);
float kuhn_eval(vector<Card> hc, vector<Card> board)
{
    return hc[0].rank;
}
GameRules half_street_kuhn_rules()
{
    int players = 2;
    vector<Card> deck = {Card(14,1),Card(13,1),Card(12,1)};
    int ante = 1;
    vector<int> blinds;

    //RoundInfo(holecards,boardcards,betsize,maxbets)
    vector<RoundInfo> rounds = {RoundInfo(1,0,1,{1,0})};
    return GameRules(players, deck, rounds, ante, blinds, kuhn_eval, leduc_format);
}
GameTree half_street_kuhn_gametree()
{
    GameRules rules = half_street_kuhn_rules();
    GameTree tree(rules);
    tree.build();
    return tree;
}
PublicTree half_street_kuhn_publictree()
{
    GameRules rules = half_street_kuhn_rules();
    PublicTree tree(rules);
    tree.build();
    return tree;
}
GameRules kuhn_rules()
{
    int players = 2;
    vector<Card> deck = {Card(14,1),Card(13,1),Card(12,1)};
    int ante = 1;
    vector<int> blinds;
    vector<RoundInfo> rounds = {RoundInfo(1,0,1,{1,1})};
    return GameRules(players, deck, rounds, ante, blinds, kuhn_eval, leduc_format); 
}
GameTree kuhn_gametree()
{
    GameRules rules = kuhn_rules();
    GameTree tree(rules);
    tree.build();
    return tree;
}
PublicTree kuhn_publictree()
{
    GameRules rules = kuhn_rules();
    PublicTree tree(rules);
    tree.build();
    return tree;
}
string leduc_format(int player, vector<Card> holecards, vector<Card> board, string bet_history)
{
    string cards = holecards[0].RANK_TO_STRING[holecards[0].rank];
    if(board.size() > 0)
        cards += board[0].RANK_TO_STRING[board[0].rank];
    return string(cards + ":" + bet_history+":");
}
float leduc_eval(vector<Card> hc, vector<Card> board)
{
    vector<Card> hand = hc + board;
    if(hand[0].rank == hand[1].rank)
        return 15*14+hand[0].rank;
    return std::max(hand[0].rank, hand[1].rank) * 14 + std::min(hand[0].rank, hand[1].rank);
}
GameRules leduc_rules()
{
    int players = 2;
    vector<Card> deck = {Card(13,1),Card(13,2),Card(12,1),Card(12,2),Card(11,1),Card(11,2)};
    int ante = 1;
    vector<int> blinds;
    vector<RoundInfo> rounds = {RoundInfo(1,0,2,{2,2}),RoundInfo(0,1,4,{2,2})};
    return GameRules(players, deck, rounds, ante, blinds, leduc_eval, leduc_format);
}
GameTree leduc_gametree()
{
    GameRules rules = leduc_rules();
    GameTree tree(rules);
    tree.build();
    return tree;
}
PublicTree leduc_publictree()
{
    GameRules rules = leduc_rules();
    PublicTree tree(rules);
    tree.build();
    return tree;
}
string royal_format(int player, vector<Card> holecards, vector<Card> board, string bet_history)
{
    string cards = holecards[0].RANK_TO_STRING[holecards[0].rank];
    for(int i=0; i<board.size(); i++)
	{
        cards += board[i].RANK_TO_STRING[board[i].rank];
        if(board[i].suit == holecards[0].suit)
            cards += 's';
        else
            cards += 'o';
	}
    return string(cards + ":" + bet_history+":");
}
float royal_eval(vector<Card> hc, vector<Card> board)
{
    vector<Card> hand = hc + board;
    // Flush
    if(hand[0].suit == hand[1].suit && hand[0].suit == hand[2].suit)
        return 10000 + hc[0].rank;
    // Straight
	vector<int> ranks;
	std::transform(hand.begin(),hand.end(),ranks.begin(),[](Card c){return c.rank;});
    if(std::find(ranks.begin(),ranks.end(),Card::RANK_QUEEN)!=ranks.end() && std::find(ranks.begin(),ranks.end(),Card::RANK_KING)!= ranks.end())
	{
        if(std::find(ranks.begin(),ranks.end(),Card::RANK_ACE) != ranks.end())
            return 1000 + Card::RANK_ACE;
        if(std::find(ranks.begin(),ranks.end(),Card::RANK_JACK) != ranks.end())
            return 1000 + Card::RANK_JACK;
	}
    // Holecard used in a pair
    if(hand[0].rank == hand[1].rank || hand[0].rank == hand[2].rank)
        return 100+hand[0].rank;
    return hand[0].rank;
}
GameRules royal_rules()
{
    int players = 2;
    vector<Card> deck = {Card(14,1),Card(14,2),Card(13,1),Card(13,2),Card(12,1),Card(12,2),Card(11,1),Card(11,2)};
    int ante = 1;
    vector<int> blinds;
    RoundInfo preflop = RoundInfo(1,0,2,{2,2});
    RoundInfo flop = RoundInfo(0,1,4,{2,2});
    RoundInfo turn = RoundInfo(0,1,4,{2,2});
    vector<RoundInfo> rounds = {preflop,flop,turn};
    return GameRules(players, deck, rounds, ante, blinds, royal_eval, royal_format);
}
GameTree royal_gametree()
{
    GameRules rules = royal_rules();
    GameTree tree(rules);
    tree.build();
    return tree;
}
PublicTree royal_publictree()
{
    GameRules rules = royal_rules();
    PublicTree tree(rules);
    tree.build();
    return tree;
}

}

