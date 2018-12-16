#ifndef HAND_EVALUATOR_HPP
#define HAND_EVALUATOR_HPP

#include "card.hpp"
#include "popcount.hpp"
#include "lookup_tables.hpp"
#include "utils.hpp"
#include <string>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <functional>
#include <numeric>

namespace rl{

class HandEvaluator
{
	public:
		class Two
		{
			public:
				static int evaluate_percentile(std::vector<Card> hand)
				{
					if(hand.size()!=2)
					{
						std::cout<<"Only 2-card hands are supported by the Two evaluator"<<std::endl;
						assert(hand.size()==2);
					}
					if(hand[0].suit == hand[1].suit)
					{
						if(hand[0].rank<hand[1].rank)
						  return LookupTables::Two::suited_ranks_to_percentile[hand[0].rank][hand[1].rank];
						else
						  return LookupTables::Two::suited_ranks_to_percentile[hand[1].rank][hand[0].rank];
					}
					else
					  return LookupTables::Two::unsuited_ranks_to_percentile[hand[0].rank][hand[1].rank];

				}
		};
		class Five
		{
			public:
				static int card_to_binary(Card card)
				{
					int b_mask = 1 << (14+card.rank);
					int cdhs_mask = 1 << (card.suit+11);
					int r_mask = (card.rank - 2) << 8;
					int p_mask = LookupTables::primes[card.rank - 2];
					return b_mask | r_mask | p_mask | cdhs_mask;
				}
				static int card_to_binary_lookup(Card card)
				{
					return LookupTables::Five::card_to_binary[card.rank][card.suit];
				}
				static int evaluate_rank(std::vector<Card> hand)
				{
					if(hand.size()!=5)
					{
						std::cout<<"Only 5-card hands are supported by the Five evaluator"<<std::endl;
						assert(hand.size()==5);
					}
					std::vector<int> bh(hand.size());
					std::transform(hand.begin(),hand.end(),bh.begin(),card_to_binary);
					int has_flush=std::accumulate(bh.begin(),bh.end(),0XF000);
					int q=std::accumulate(bh.begin(),bh.end(),0,[](int a, int b){return a | b;})>>16;
					if(has_flush)
					  return LookupTables::Five::flushes[q];
					else
					{
						int possible_rank = LookupTables::Five::unique5[q];
						if(possible_rank != 0)
						  return possible_rank;
						else
						{
							std::vector<int> tmp(bh.size());
							std::transform(bh.begin(),bh.end(),tmp.begin(),[](int x){return x & 0xFF;});
							q=std::accumulate(tmp.begin(),tmp.end(),1,std::multiplies<int>());
							return LookupTables::Five::pairs[q];
						}
					}
				}

		};
		class Six
		{
			public:
				static int card_to_binary(Card card)
				{
					int b_mask = 1 << (14+card.rank);
					int q_mask = LookupTables::primes[card.suit-1] << 12;
					int r_mask = (card.rank - 2) << 8;
					int p_mask = LookupTables::primes[card.rank - 2];
					return b_mask | r_mask | p_mask | q_mask;

				}
				static int card_to_binary_lookup(Card card)
				{
					return LookupTables::Six::card_to_binary[card.rank][card.suit];
				}
				static int evaluate_rank(std::vector<Card> hand)
				{
					if(hand.size()!=6)
					{
						std::cout<<"Only 6-card hands are supported by the Six evaluator"<<std::endl;
						assert(hand.size()==6);
					}
					std::vector<int> bh(hand.size());
					std::transform(hand.begin(),hand.end(),bh.begin(),card_to_binary);
					std::vector<int> fp_tmp;
					std::transform(bh.begin(),bh.end(),fp_tmp.begin(),[](int c){return (c>>12)&0XF;});
					int flush_prime=std::accumulate(bh.begin(),bh.end(),1,std::multiplies<int>());
					int flush_suit = 0;
					if(LookupTables::Six::prime_products_to_flush.find(flush_prime)!=LookupTables::Six::prime_products_to_flush.end())
					  flush_suit = LookupTables::Six::prime_products_to_flush[flush_prime];

					int odd_xor = std::accumulate(bh.begin(),bh.end(),0,[](int a, int b){return a ^ b;})>>16;
					int even_xor = (std::accumulate(bh.begin(),bh.end(),0,[](int a, int b){return a | b;})>>16) ^ odd_xor;
					if(flush_suit)
					{
						if(even_xor == 0)
						{
							std::vector<int> tmp(bh.size());
							std::copy_if(bh.begin(),bh.end(),tmp.begin(), [&flush_suit](int c){return ((c>>12)&0xF) == flush_suit;});
							std::vector<int> tmpb(bh.size());
							std::transform(tmp.begin(),tmp.end(),tmpb.begin(),[](int c){return c >> 16;});
							int bits = std::accumulate(tmpb.begin(),tmpb.end(),0,[](int a, int b){return a | b;});
							return LookupTables::Six::flush_rank_bits_to_rank[bits];
						}
						else
						  return LookupTables::Six::flush_rank_bits_to_rank[odd_xor | even_xor];
					}

					//Otherwise, get ready for a wild ride:

					//Can determine this by using 2 XORs to reduce the size of the
					//lookup. You have an even number of cards, so any odd_xor with
					//an odd number of bits set is not possible.
					//Possibilities are odd-even:
					//6-0 => High card or straight (1,1,1,1,1,1)
					//  Look up by odd_xor
					//4-1 => Pair (1,1,1,1,2)
					//  Look up by even_xor (which pair) then odd_xor (which set of kickers)
					//4-0 => Trips (1,1,1,3)
					//  Don't know which one is the triple, use prime product of ranks
					//2-2 => Two pair (1,1,2,2)
					//  Look up by odd_xor then even_xor (or vice-versa)
					//2-1 => Four of a kind (1,1,4) or full house (1,3,2)
					//  Look up by prime product
					//2-0 => Full house using 2 trips (3,3)
					//  Look up by odd_xor
					//0-3 => Three pairs (2,2,2)
					//  Look up by even_xor
					//0-2 => Four of a kind with pair (2,4)
					//  Look up by prime product

					//Any time you can't disambiguate 2/4 or 1/3, use primes.
					//We also assume you can count bits or determine a power of two.
					//(see PopCount class.)
					if(even_xor == 0)
					{
						int odd_popcount = PopCount::popcount32_table16(odd_xor);
						if(odd_popcount == 4)
						{
							std::vector<int> tmp(bh.size());
							std::transform(bh.begin(),bh.end(),tmp.begin(),[](int c){return c & 0xFF;});
							int prime_product=std::accumulate(tmp.begin(),tmp.end(),1,std::multiplies<int>());
							return LookupTables::Six::prime_products_to_rank[prime_product];

						}
						else
						  return LookupTables::Six::odd_xors_to_rank[odd_xor];
					}
					else if(odd_xor == 0)
					{
						int even_popcount = PopCount::popcount32_table16(even_xor);
						if(even_popcount == 2)
						{
							std::vector<int> tmp(bh.size());
							std::transform(bh.begin(),bh.end(),tmp.begin(),[](int x){return x & 0xFF;});
							int prime_product=std::accumulate(tmp.begin(),tmp.end(),1,std::multiplies<int>());
							return LookupTables::Six::prime_products_to_rank[prime_product];

						}
						else
						  return LookupTables::Six::even_xors_to_rank[even_xor];

					}
					else
					{
						int odd_popcount = PopCount::popcount32_table16(odd_xor);
						if(odd_popcount == 4)
						  return LookupTables::Six::even_xors_to_odd_xors_to_rank[even_xor][odd_xor];
						else
						{
							int even_popcount = PopCount::popcount32_table16(even_xor);
							if(even_popcount == 2)
							  return LookupTables::Six::even_xors_to_odd_xors_to_rank[even_xor][odd_xor];
							else
							{
								std::vector<int> tmp(bh.size());
								std::transform(bh.begin(),bh.end(),tmp.begin(),[](int x){return x & 0xFF;});
								int prime_product=std::accumulate(tmp.begin(),tmp.end(),1,std::multiplies<int>());
								return LookupTables::Six::prime_products_to_rank[prime_product];

							}

						}

					}
				}

		};

		class Seven
		{
			public:
				static int card_to_binary(Card card)
				{
					int b_mask = 1 << (14+card.rank);
					int q_mask = LookupTables::primes[card.suit-1] << 12;
					int r_mask = (card.rank - 2) << 8;
					int p_mask = LookupTables::primes[card.rank - 2];
					return b_mask | r_mask | p_mask | q_mask;

				}
				static int card_to_binary_lookup(Card card)
				{
					return LookupTables::Seven::card_to_binary[card.rank][card.suit];
				}
				static int evaluate_rank(std::vector<Card> hand)
				{
					if(hand.size()!=7)
					{
						std::cout<<"Only 7-card hands are supported by the Seven evaluator"<<std::endl;
						assert(hand.size()==7);
					}
					std::vector<int> bh(hand.size());
					std::transform(hand.begin(),hand.end(),bh.begin(),card_to_binary);
					std::vector<int> fp_tmp;
					std::transform(bh.begin(),bh.end(),fp_tmp.begin(),[](int c){return (c>>12)&0XF;});
					int flush_prime=std::accumulate(bh.begin(),bh.end(),1,std::multiplies<int>());
					int flush_suit = 0;
					if(LookupTables::Seven::prime_products_to_flush.find(flush_prime)!=LookupTables::Seven::prime_products_to_flush.end())
					  flush_suit = LookupTables::Seven::prime_products_to_flush[flush_prime];

					int odd_xor = std::accumulate(bh.begin(),bh.end(),0,[](int a, int b){return a ^ b;})>>16;
					int even_xor = (std::accumulate(bh.begin(),bh.end(),0,[](int a, int b){return a | b;})>>16) ^ odd_xor;
					if(flush_suit)
					{
						int even_popcount = PopCount::popcount32_table16(even_xor);
						if(even_xor == 0)
						{
							std::vector<int> tmp(bh.size());
							std::copy_if(bh.begin(),bh.end(),tmp.begin(),[&flush_suit](int c){return ((c>>12)&0xF) == flush_suit;});
							std::vector<int> tmpb(bh.size());
							std::transform(tmp.begin(),tmp.end(),tmpb.begin(),[](int c){return c >> 16;});
							int bits = std::accumulate(tmpb.begin(),tmpb.end(),0,[](int a, int b){return a | b;});
							return LookupTables::Seven::flush_rank_bits_to_rank[bits];
						}
						else
						{
							if(even_popcount == 2)
							  return LookupTables::Seven::flush_rank_bits_to_rank[odd_xor | even_xor];
							else
							{
								std::vector<int> tmp(bh.size());
								std::copy_if(bh.begin(),bh.end(),tmp.begin(),[&flush_suit](int c){return ((c>>12)&0xF) == flush_suit;});
								std::vector<int> tmpb(bh.size());
								std::transform(tmp.begin(),tmp.end(),tmpb.begin(),[](int c){return c >> 16;});
								int bits = std::accumulate(tmpb.begin(),tmpb.end(),0,[](int a, int b){return a | b;});
								return LookupTables::Seven::flush_rank_bits_to_rank[bits];
							}
						}
					}


					// Odd-even XOR again, see Six.evaluate_rank for details
					// 7 is odd, so you have to have an odd number of bits in odd_xor
					// 7-0 => (1,1,1,1,1,1,1) - High card
					// 5-1 => (1,1,1,1,1,2) - Pair
					// 5-0 => (1,1,1,1,3) - Trips
					// 3-2 => (1,1,1,2,2) - Two pair
					// 3-1 => (1,1,1,4) or (1,1,3,2) - Quads or full house
					// 3-0 => (1,3,3) - Full house
					// 1-3 => (1,2,2,2) - Two pair
					// 1-2 => (1,2,4) or (3,2,2) - Quads or full house
					// 1-1 => (3,4) - Quads
					if(even_xor == 0)
					{
						int odd_popcount = PopCount::popcount32_table16(odd_xor);
						if(odd_popcount == 7)
						  return LookupTables::Seven::odd_xors_to_rank[odd_xor];
						else
						{
							std::vector<int> tmp(bh.size());
							std::transform(bh.begin(),bh.end(),tmp.begin(),[](int c){return c & 0xFF;});
							int prime_product=std::accumulate(tmp.begin(),tmp.end(),1,std::multiplies<int>());
							return LookupTables::Seven::prime_products_to_rank[prime_product];

						}
					}
					else
					{
						int odd_popcount = PopCount::popcount32_table16(odd_xor);
						if(odd_popcount == 5)
						  return LookupTables::Seven::even_xors_to_odd_xors_to_rank[even_xor][odd_xor];
						else if(odd_popcount ==3)
						{
							int even_popcount = PopCount::popcount32_table16(even_xor);
							if(even_popcount == 2)
							  return LookupTables::Seven::even_xors_to_odd_xors_to_rank[even_xor][odd_xor];
							else
							{
								std::vector<int> tmp(bh.size());
								std::transform(bh.begin(),bh.end(),tmp.begin(),[](int x){return x & 0xFF;});
								int prime_product=std::accumulate(tmp.begin(),tmp.end(),1,std::multiplies<int>());
								return LookupTables::Seven::prime_products_to_rank[prime_product];

							}

						}
						else
						{
							int even_popcount = PopCount::popcount32_table16(even_xor);
							if(even_popcount == 3)
							  return LookupTables::Seven::even_xors_to_odd_xors_to_rank[even_xor][odd_xor];
							if(even_popcount == 2)
							{
								std::vector<int> tmp(bh.size());
								std::transform(bh.begin(),bh.end(),tmp.begin(),[](int x){return x & 0xFF;});
								int prime_product=std::accumulate(tmp.begin(),tmp.end(),1,std::multiplies<int>());
								return LookupTables::Seven::prime_products_to_rank[prime_product];

							}
							else
							  return LookupTables::Seven::even_xors_to_odd_xors_to_rank[even_xor][odd_xor];

						}

					}
				}
		};
		typedef int (*evaluator_type)(std::vector<Card>);
		static float evaluate_hand(std::vector<Card> hand, std::vector<Card> board);
};


}

#endif
