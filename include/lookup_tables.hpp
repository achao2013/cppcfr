#ifndef LOOKUP_TABLES_HPP
#define LOOKUP_TABLES_HPP

#include "card.hpp"
#include <vector>
#include <set>
#include <map>

namespace rl
{

	class LookupTables
	{
		public:
			static std::vector<int> primes;
			static std::set<Card> deck;
			class Two{
				public:
					static std::vector<std::vector<int> >  preflop_order_matrix;
					static std::vector<int> preflop_count_matrix;
					static float preflop_count_sum;
					// Table for suited 2-card hands
					// The row corresponds to the lower of the two cards
					// TODO: I think this should be a symmetric matrix
					// to remove the sorting requirement
					static std::vector<std::vector<float> > suited_ranks_to_percentile;
					// Table for unsuited 2-card hands
					// For some reason I actually bothered to make this one
					// symmetric and not the other one
					static std::vector<std::vector<float> > unsuited_ranks_to_percentile;
			};

			class Five{
				public:
					static std::vector<std::vector<int> > card_to_binary;   
					// Map flush AND value to rank of flushes in 5-card hands
					static std::vector<int> flushes;
					static std::vector<int> unique5;
					static std::map<int,int> pairs;
					static std::vector<double> rank_to_percentile_5;
					static std::vector<double> rank_to_percentile_6;
					static std::vector<double> rank_to_percentile_7;
			};

			class Six{
				public:
					static std::vector<std::vector<int> > card_to_binary;
					static std::map<int,int> prime_products_to_flush;
					static std::map<int,int> flush_rank_bits_to_rank;
					static std::map<int,int> prime_products_to_rank;
					static std::map<int,int> odd_xors_to_rank;
					static std::map<int,int> even_xors_to_rank;
					static std::map<int,std::map<int,int> > even_xors_to_odd_xors_to_rank;
			};

			class Seven{
				public:
					static std::vector<std::vector<int> > card_to_binary;
					static std::map<int,int> prime_products_to_flush;
					static std::map<int,int> flush_rank_bits_to_rank;
					static std::map<int,int> prime_products_to_rank;
					static std::map<int,int> odd_xors_to_rank;
					static std::map<int,std::map<int,int> > even_xors_to_odd_xors_to_rank;
			};


	};
}
#endif
