#include <vector>
#include "utils.hpp"

using std::vector;
namespace rl{
    vector<Card> operator+(vector<Card> v1, vector<Card> v2)
	{
		vector<Card> res = v1;
		res.insert(res.end(),v2.begin(),v2.end());
		return res;
	}
	std::ostream& operator<<(std::ostream& out, const vector<Card>& vc)
	{
		out<<"(";
			for(auto it2 = vc.begin(); it2 != vc.end(); it2++)
			  out<<it2->repr()<<",";
		out<<")";
		return out;
	}
	std::ostream& operator<<(std::ostream& out, const vector<vector<Card> >& vc)
	{
		out<<"[";
		for(auto it = vc.begin(); it != vc.end(); it++)
		{
			out<<*it<<",";
		}
		out<<"]";
		return out;
	}
	std::ostream& operator<<(std::ostream& out, const vector<vector<vector<Card> > >& vc)
	{
		out<<"{";
		for(auto it = vc.begin(); it != vc.end(); it++)
		{
			  out<<*it<<",";
		}
		out<<"}";
		return out;
	}

	//std::ostream& operator<<(std::ostream& out, const vector<string>& in)
	//{

	//	out<<"[";
	//	for(auto it = in.begin(); it!=in.end(); it++)
	//	{
	//		out<<*it<<",";
	//	}
	//	out<<"]";
	//}
	std::ostream& operator<<(std::ostream& out, const vector<float>& in)
	{

		out<<"[";
		for(auto it = in.begin(); it!=in.end(); it++)
		{
			out<<*it<<",";
		}
		out<<"]";
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const map<string, vector<float> >& in)
	{
		out<<"{";
		for(auto it = in.begin(); it!=in.end(); it++)
		{
			out<<"\""<<it->first<<"\": "<<it->second<<",";
		}
		out<<"}";
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const vector<map<string, vector<float> > >& in)
	{
		out<<"[";
		for(auto it = in.begin(); it!=in.end(); it++)
		{
			out<<*it<<",";
		}
		out<<"]";
		return out;
	}

	std::ostream& operator<<(std::ostream& out, const map<vector<Card>, float>& in)
	{
		out<<"{";
		for(auto it2 = in.begin(); it2 != in.end(); it2++)
		{
			out<<it2->first<<":"<<it2->second<<",";
		}
		out<<"}";

		return out;
	}
	std::ostream& operator<<(std::ostream& out, const vector<map<vector<Card>, float> >& in)
	{
		out<<"[";
		for(auto it = in.begin(); it!=in.end(); it++)
		{
			out<<*it<<",";
		}
		out<<"]";
		return out;
	}
	std::ostream& operator<<(std::ostream& out, const map<vector<vector<Card> >, vector<float> >& in)
	{
		out<<"{";
		for(auto it2 = in.begin(); it2 != in.end(); it2++)
		{
			out<<it2->first<<":"<<it2->second<<",";
		}
		out<<"}";
		return out;
	}
	std::ostream& operator<<(std::ostream& out, const map<vector<Card>, vector<float> >& in)
	{
		out<<"{";
		for(auto it2 = in.begin(); it2 != in.end(); it2++)
		{
			out<<it2->first<<":"<<it2->second<<",";
		}
		out<<"}";
		return out;
	}

	void combinations(int i, int j,vector<Card> &r,int num, std::vector<Card>& all, std::vector<std::vector<Card> > & result)  
	{  
		if (j == 1)  
		{  
			for (int k = 0; k < i; k++)  
			{  
				std::vector<Card> temp(num);  
				r[num - 1] = all[k];  
				for (int i = 0; i < num;i++)  
				{  
					temp[i]=r[i];  
				}  
				result.push_back(temp);  
			}  
		}  
		else if (j == 0)  
		{  
		}  
		else  
		{  
			for (int k = i; k >= j; k--)  
			{  
				r[j-2] = all[k-1];  
				combinations(k - 1, j - 1,r,num,all,result);  
			}  
		}  
	}  
	void combinations(int i, int j,vector<vector<Card> > &r,int num, vector<vector<Card> >& all, vector<vector<std::vector<Card> > > & result)  
	{  
		if (j == 1)  
		{  
			for (int k = 0; k < i; k++)  
			{  
				vector<vector<Card> > temp;
				temp.resize(num);
				r[num - 1] = all[k];  
				for (int i = 0; i < num;i++)  
				{  
					temp[i]=r[i];  
				}  
				result.push_back(temp);  
			}  
		}  
		else if (j == 0)  
		{  
		}  
		else  
		{  
			for (int k = i; k >= j; k--)  
			{  
				r[j-2] = all[k-1];  
				combinations(k - 1, j - 1,r,num,all,result);  
			}  
		}  
	} 

	void permutations_range(std::vector<Card>& all, int i, int j, std::vector<std::vector<Card> >& res)
	{
		if(i==j)
		  res.push_back(all);
		else
		{
			for(int k=i; k<j; k++)
			{
				std::swap(all[k], all[i]);
				permutations_range(all,i+1,j,res);
				std::swap(all[k], all[i]);
			}
		}
	}

	void permutations_range(std::vector<std::vector<Card> >& all, int i, int j, std::vector<std::vector<std::vector<Card> > >& res)
	{
		if(i==j)
		  res.push_back(all);
		else
		{
			for(int k=i; k<j; k++)
			{
				std::swap(all[k], all[i]);
				permutations_range(all,i+1,j, res);
				std::swap(all[k], all[i]);
			}
		}
	}
	void product(vector<vector<vector<Card> > > dimvalue, vector<vector<vector<Card> > > &res, int layer, vector<vector<Card> > tmp){  
		if (layer < dimvalue.size() - 1){  
			for (int i = 0; i < dimvalue[layer].size(); i++){  
				vector<vector<Card> > sb;  
				sb.clear();  

				for (int i = 0; i < tmp.size(); i++){  
					sb.push_back(tmp[i]);  
				}  
				sb.push_back(dimvalue[layer][i]);  
				product(dimvalue, res, layer+1,sb);  
			}  
		}  
		else if (layer == dimvalue.size()-1){  
			for (int j = 0; j < dimvalue[layer].size();j++){  
				tmp.push_back(dimvalue[layer][j]);  
				res.push_back(tmp);  
				tmp.pop_back();  
			}  
		}  
	} 

}

