#ifndef UTILS_HPP_
#define UTILS_HPP_
#include <vector>
#include <map>
#include "card.hpp"
#include <iostream>
#include <string>

using std::vector;
using std::map;
using std::string;
namespace rl{
	class Card;
	vector<Card> operator+(vector<Card> v1, vector<Card> v2);
	std::ostream& operator<<(std::ostream& out, const vector<Card>& vc);
	std::ostream& operator<<(std::ostream& out, const vector<vector<Card> >& vc);
	std::ostream& operator<<(std::ostream& out, const vector<vector<vector<Card> > >& vc);

	std::ostream& operator<<(std::ostream& out, const vector<string>& in);
	std::ostream& operator<<(std::ostream& out, const vector<float>& in);
	std::ostream& operator<<(std::ostream& out, const map<string, vector<float> >& in);
	std::ostream& operator<<(std::ostream& out, const vector<map<string, vector<float> > >& in);
	std::ostream& operator<<(std::ostream& out, const map<vector<Card>, float>& in);
	std::ostream& operator<<(std::ostream& out, const vector<map<vector<Card>, float> >& in);
	std::ostream& operator<<(std::ostream& out, const map<vector<vector<Card> >, vector<float> >& in);
	std::ostream& operator<<(std::ostream& out, const map<vector<Card>, vector<float> >& in);

	void combinations(int i, int j,vector<Card> &r,int num, std::vector<Card>& all, std::vector<std::vector<Card> > & result);  
	void combinations(int i, int j,vector<vector<Card> > &r,int num, vector<vector<Card> >& all, vector<vector<std::vector<Card> > > & result);  

	void permutations_range(std::vector<Card>& all, int i, int j, std::vector<std::vector<Card> >& res);

	void permutations_range(std::vector<std::vector<Card> >& all, int i, int j, std::vector<std::vector<std::vector<Card> > >& res);
	void product(vector<vector<vector<Card> > > dimvalue, vector<vector<vector<Card> > > &res, int layer, vector<vector<Card> > tmp);  
}

#endif
