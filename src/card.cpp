#include "card.hpp"
namespace rl{

    int Card::RANK_JACK = 11;
    int Card::RANK_QUEEN = 12;
    int Card::RANK_KING = 13;
    int Card::RANK_ACE = 14;
	std::map<int, std::string> Card::SUIT_TO_STRING = {
		{1, "s"},
		{2, "h"},
		{3, "d"},
		{4, "c"} 
	};

	std::map<std::string, int> Card::STRING_TO_SUIT = {
		{"s", 1},
		{"h", 2},
		{"d", 3},
		{"c", 4} 
	};

	std::map<int, std::string> Card::RANK_TO_STRING = {
		{2, "2"},
		{3, "3"},
		{4, "4"},
		{5, "5"},
		{6, "6"},
		{7, "7"},
		{8, "8"},
		{9, "9"},
		{10, "T"},
		{11, "J"},
		{12, "Q"},
		{13, "K"},
		{14, "A"}
	};

	std::map<std::string, int> Card::STRING_TO_RANK = {
		{"2", 2},
		{"3", 3},
		{"4", 4},
		{"5", 5},
		{"6", 6},
		{"7", 7},
		{"8", 8},
		{"9", 9},
		{"T", 10},
		{"J", 11},
		{"Q", 12},
		{"K", 13},
		{"A", 14}
	};

	std::string Card::REPR_RE=std::string("\\((.*?)\\)");


	
}
