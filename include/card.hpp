#ifndef CARD_HPP
#define CARD_HPP

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include "utils.hpp"
using std::vector;
namespace rl{

    class Card{
    public:
      static std::map<int,std::string> SUIT_TO_STRING;
      static std::map<int,std::string> RANK_TO_STRING;
      static int RANK_JACK;
      static int RANK_QUEEN;
      static int RANK_KING;
      static int RANK_ACE;
    
      static std::map<std::string,int> STRING_TO_SUIT;
      static std::map<std::string,int> STRING_TO_RANK;

      static std::string REPR_RE;
	  Card(){}
      Card(int rank, int suit)
      {
        this->rank = rank;
        this->suit = suit;
      }
	  Card(const Card& other)
	  {
		  rank = other.rank;
		  suit = other.suit;
	  }
	  bool operator ==(const Card& other)const
	  {
		return this->rank == other.rank && this->suit == other.suit;
	  }
	  Card& operator=(const Card& other)
	  {
		  rank = other.rank;
		  suit = other.suit;
		  return *this;
	  }
      std::string repr()const
      {
        std::stringstream ss;
		//std::cout<<"rank:"<<this->rank<<" suit:"<<this->suit<<std::endl;
        ss << RANK_TO_STRING.at(this->rank) << SUIT_TO_STRING.at(this->suit);
        return ss.str();
      }
      bool eq(const Card& other)
      {return this->rank == other.rank && this->suit == other.suit;}
      size_t hash()
      { 
        std::hash<std::string> h;
        return h(repr());
      }
      bool operator< (const	Card& c) const
	  {
		  if(rank+13*suit<c.rank+13*c.suit)
			return true;
		  else
			return false;
	  }

      int rank;
      int suit;
    };

}
#endif
