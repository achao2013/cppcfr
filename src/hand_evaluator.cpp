#include "hand_evaluator.hpp"

namespace rl{

	float HandEvaluator::evaluate_hand(std::vector<Card> hand, std::vector<Card> board)
	{
		if(hand.size()!=2)
		{
			std::cout<<"Only 2 hole cards are supported";
			assert(hand.size() == 2);
		}
		std::vector<Card> cards;
		cards.insert(cards.end(),hand.begin(),hand.end());
		cards.insert(cards.end(),board.begin(),board.end());
		int rank = 7463;
		evaluator_type evaluator;
		switch(cards.size())
		{
			case 2:
				return HandEvaluator::Two::evaluate_percentile(hand);
			case 5:
				evaluator = HandEvaluator::Five::evaluate_rank;
				rank=HandEvaluator::Five::evaluate_rank(cards);
				break;
			case 6:
				evaluator = HandEvaluator::Six::evaluate_rank;
				rank=HandEvaluator::Six::evaluate_rank(cards);
				break;
			case 7:
				evaluator = HandEvaluator::Seven::evaluate_rank;
				rank=HandEvaluator::Seven::evaluate_rank(cards);
				break;
			default:
				std::cout<<"Only 2, 5, 6, 7 cards total are supported by evaluate_hand";
				assert(1==0);
		}

		float percentile = 0.0;
		std::vector<Card> left_cards;
		std::remove_copy_if(LookupTables::deck.begin(),LookupTables::deck.end(),std::back_inserter(left_cards),[&cards](Card card){return std::find(cards.begin(),cards.end(),card) != cards.end();});
		std::vector<std::vector<Card> > possible_opponent_hands;
		std::vector<Card> r(hand.size());
		combinations(left_cards.size(),hand.size(),r,hand.size(),left_cards,possible_opponent_hands);
		float hands_beaten=0;
		for(auto it=possible_opponent_hands.begin(); it!=possible_opponent_hands.end(); it++)
		{
			std::vector<Card> opnt_cards;
			opnt_cards.insert(opnt_cards.end(),it->begin(),it->end());
			opnt_cards.insert(opnt_cards.end(),board.begin(),board.end());
			int possible_opponent_rank = evaluator(opnt_cards);
			if( rank < possible_opponent_rank)
			  // you beat this hand
			  hands_beaten += 1;
			else if (rank == possible_opponent_rank)
			  hands_beaten += 0.5;

		}
		return hands_beaten/possible_opponent_hands.size();
	}
}
