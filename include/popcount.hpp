#ifndef POPCOUNT_HPP
#define POPCOUNT_HPP
#include <vector>
namespace{

	class PopCount{
		public:
			static int popcount32_table16(int v)
			{
				return POPCOUNT_TABLE16[v&0xffff]+POPCOUNT_TABLE16[(v >> 16) & 0xffff];

			}
			static std::vector< int> POPCOUNT_TABLE16;
			static void pre_compute(int N)
			{
				PopCount::pre_compute(N-1);
				POPCOUNT_TABLE16[N]=(N&1)+POPCOUNT_TABLE16[N>>1];
			}



	};

	std::vector<int> PopCount::POPCOUNT_TABLE16(65536,0);


}
#endif
