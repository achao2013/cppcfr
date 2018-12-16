#include "pokertrees.hpp"
#include "pokergames.hpp"
#include "card.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <typeinfo>
using namespace std;
using rl::Card;
using rl::RoundInfo;
using rl::HolecardChanceNode;
using rl::TerminalNode;
using rl::ActionNode;
using rl::BoardcardChanceNode;
using rl::GameTree;
using rl::PublicTree;
int main()
{
	std::cout<< "Testing GameTree"<<std::endl;
	rl::GameRules rules(2, {Card(14,1),Card(13,2),Card(13,1),Card(12,1)}, {RoundInfo(1, 0, 1, {2,2}),RoundInfo(0, 1, 2, {2,2})}, 1, {1,2}, rl::leduc_eval);
	rl::GameTree tree(rules);
	tree.build();
	assert(typeid(*tree.root.get()) == typeid(HolecardChanceNode));
	assert(tree.root->children.size() == 12);
	assert(typeid(*tree.root->children[11].get()) == typeid(ActionNode));
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11])->player == 0);
	assert(tree.root->children[11]->children.size() == 2);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11])->player_view_g == string("As:/:"));
	// /f;
	//for(int i=0; i<tree.root->children[11]->children.size(); i++)
	//{
	//	std::cout<<typeid(*tree.root->children[11]->children[i].get()).name()<<std::endl;
	//	for(int j=0; j<tree.root->children[11]->children[i]->children.size(); j++)
	//	{
	//		std::cout<<typeid(*tree.root->children[11]->children[i]->children[j].get()).name()<<std::endl;
	//		for(int k=0; k<tree.root->children[11]->children[i]->children[j]->children.size(); k++)
	//		{
	//			std::cout<<typeid(*tree.root->children[11]->children[i]->children[j]->children[k].get()).name()<<std::endl;

	//		}
	//		
	//		std::cout<<"----------------------------------"<<std::endl;
	//	}
	//	std::cout<<"===================================================="<<std::endl;

	//}
	assert(typeid(*tree.root->children[11]->children[0].get()) == typeid(TerminalNode));
	vector<float> tmp0 = {-2,2};
	assert(std::static_pointer_cast<TerminalNode>(tree.root->children[0]->children[0])->payoffs_g == tmp0);
	assert(tree.root->children[11]->children[0]->bet_history == "/f");
	// /c;
	assert(typeid(*tree.root->children[11]->children[1].get()) == typeid(ActionNode));
	assert(tree.root->children[11]->children[1]->bet_history == "/c");
	assert((tree.root->children[11]->children[1]->children).size() == 1);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1])->player == 1);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1])->fold_action == nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1])->raise_action == nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1])->player_view_g == string("Kh:/c:"));
	// /cc/ [boardcard];
	assert(typeid(*tree.root->children[11]->children[1]->children[0].get()) == typeid(BoardcardChanceNode));
	assert(tree.root->children[11]->children[1]->children[0]->bet_history == string("/cc/"));
	assert((tree.root->children[11]->children[1]->children[0]->children).size() == 2);
	// /cc/ [action];
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0].get()) == typeid(ActionNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->bet_history == "/cc/");
	assert((tree.root->children[11]->children[1]->children[0]->children[0]->children).size() == 2);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0])->player == 0);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0])->fold_action == nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0])->raise_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0])->player_view_g == string("AsKs:/cc/:"));
	// /cc/r;
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0]->children[1].get()) == typeid(ActionNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->children[1]->bet_history == string("/cc/r"));
	assert((tree.root->children[11]->children[1]->children[0]->children[0]->children[1]->children).size() == 3);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[1])->player == 1);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[1])->fold_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[1])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[1])->raise_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[1])->player_view_g == string("KhKs:/cc/r:"));
	// /cc/c;
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0]->children[0].get()) == typeid(ActionNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->bet_history == string("/cc/c"));
	assert((tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children).size() == 2);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0])->player == 1);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0])->fold_action == nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0])->raise_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0])->player_view_g == string("KhKs:/cc/c:"));
	// /cc/cc;
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[0].get()) == typeid(TerminalNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[0]->bet_history == string("/cc/cc"));
	vector<float> tmp1 = {-3,3};
	assert(std::static_pointer_cast<TerminalNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[0])->payoffs_g == tmp1);
	// /cc/cr;
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1].get()) == typeid(ActionNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->bet_history == string("/cc/cr"));
	assert((tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children).size() == 3);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1])->player == 0);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1])->fold_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1])->raise_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1])->player_view_g == string("AsKs:/cc/cr:"));
	// /cc/crr;
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2].get()) == typeid(ActionNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->bet_history == string("/cc/crr"));
	assert((tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->children).size() == 2);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2])->player == 1);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2])->fold_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2])->raise_action == nullptr);
	assert(std::static_pointer_cast<ActionNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2])->player_view_g == string("KhKs:/cc/crr:"));
	// /cc/crrf;
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->children[0].get()) == typeid(TerminalNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->children[0]->bet_history == string("/cc/crrf"));
	vector<float> tmp2 = {5, -5};
	assert(std::static_pointer_cast<TerminalNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->children[0])->payoffs_g == tmp2);
	// /cc/crrc;
	assert(typeid(*tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->children[1].get()) == typeid(TerminalNode));
	assert(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->children[1]->bet_history == string("/cc/crrc"));
	vector<float> tmp3 = {-7,7};
	assert(std::static_pointer_cast<TerminalNode>(tree.root->children[11]->children[1]->children[0]->children[0]->children[0]->children[1]->children[2]->children[1])->payoffs_g == tmp3);
	std::cout<< "All passed!"<<std::endl;

	std::cout<< "Testing PublicTree"<<std::endl;
	rl::PublicTree ptree = rl::PublicTree(rules);
	ptree.build();

	assert(typeid(*ptree.root.get()) == typeid(HolecardChanceNode));
	assert((ptree.root->children).size() == 1);
	// /;
	assert(typeid(*ptree.root->children[0].get()) == typeid(ActionNode));
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0])->player == 0);
	vector<string> ptpv1 = {string("As:/:"), string("Kh:/:"), string("Ks:/:"), string("Qs:/:")};
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0])->player_view == ptpv1);
	assert((ptree.root->children[0]->children).size() == 2);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0])->fold_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0])->raise_action == nullptr);
	// /f;
	assert(typeid(*ptree.root->children[0]->children[0].get()) == typeid(TerminalNode));
	map<vector<vector<Card> >, vector<float> > tmp5 = { {{{Card(14,1),},{Card(13,1),}}, {-2,2}}, {{{Card(14,1),},{Card(13,2),}}, {-2,2}}, {{{Card(14,1),},{Card(12,1),}}, {-2,2}}, {{{Card(13,1),},{Card(14,1),}}, {-2,2}}, {{{Card(13,1),},{Card(13,2),}}, {-2,2}}, {{{Card(13,1),},{Card(12,1),}}, {-2,2}}, {{{Card(13,2),},{Card(14,1),}}, {-2,2}}, {{{Card(13,2),},{Card(13,1),}}, {-2,2}}, {{{Card(13,2),},{Card(12,1),}}, {-2,2}}, {{{Card(12,1),},{Card(14,1),}}, {-2,2}}, {{{Card(12,1),},{Card(13,2),}}, {-2,2}}, {{{Card(12,1),},{Card(13,1),}}, {-2,2}} };
	assert(std::static_pointer_cast<TerminalNode>(ptree.root->children[0]->children[0])->payoffs == tmp5);
	// /c;
	assert(typeid(*ptree.root->children[0]->children[1].get()) == typeid(ActionNode));
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1])->player == 1);
	vector<string> ptpv2 = {string("As:/c:"), string("Kh:/c:"), string("Ks:/c:"), string("Qs:/c:")};
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1])->player_view == ptpv2);
	assert((ptree.root->children[0]->children[1]->children).size() == 1);
	// /cc/ [boardcard];
	assert(typeid(*ptree.root->children[0]->children[1]->children[0].get()) == typeid(BoardcardChanceNode));
	assert(ptree.root->children[0]->children[1]->children[0]->bet_history == string("/cc/"));
	assert((ptree.root->children[0]->children[1]->children[0]->children).size() == 4);
	// xAs:/cc/ [action];
	assert(typeid(*ptree.root->children[0]->children[1]->children[0]->children[0].get()) == typeid(ActionNode));
	assert(ptree.root->children[0]->children[1]->children[0]->children[0]->bet_history == string("/cc/"));
	assert((ptree.root->children[0]->children[1]->children[0]->children[0]->children).size() == 2);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0])->player == 0);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0])->fold_action == nullptr);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0])->raise_action != nullptr);
	vector<string> ptpv3 = {string("KhAs:/cc/:"), string("KsAs:/cc/:"), string("QsAs:/cc/:")};
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0])->player_view == ptpv3);
	// xAs:/cc/r;
	assert(typeid(*ptree.root->children[0]->children[1]->children[0]->children[0]->children[1].get()) == typeid(ActionNode));
	assert(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1]->bet_history == string("/cc/r"));
	assert((ptree.root->children[0]->children[1]->children[0]->children[0]->children[1]->children).size() == 3);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1])->player == 1);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1])->fold_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1])->call_action != nullptr);
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1])->raise_action != nullptr);
	vector<string> ptpv4 = {string("KhAs:/cc/r:"), string("KsAs:/cc/r:"), string("QsAs:/cc/r:")};
	assert(std::static_pointer_cast<ActionNode>(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1])->player_view == ptpv4);
	// xAs:/cc/rc;
	assert(typeid(*ptree.root->children[0]->children[1]->children[0]->children[0]->children[1]->children[1].get()) == typeid(TerminalNode));
	assert(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1]->children[1]->bet_history == string("/cc/rc"));
	map<vector<vector<Card> >, vector<float> > tmp6 ={ {{{Card(13,1)},{Card(13,2)}}, {0,0}}, {{{Card(13,1),},{Card(12,1),}}, {5,-5}}, {{{Card(13,2),},{Card(13,1),}}, {0,0}}, {{{Card(13,2),},{Card(12,1),}}, {5,-5}}, {{{Card(12,1),},{Card(13,2),}}, {-5,5}}, {{{Card(12,1),},{Card(13,1),}}, {-5,5}} }; 
	assert(std::static_pointer_cast<TerminalNode>(ptree.root->children[0]->children[1]->children[0]->children[0]->children[1]->children[1])->payoffs == tmp6);
	// xKh:/cc/rc;
	assert(typeid(*ptree.root->children[0]->children[1]->children[0]->children[1]->children[1]->children[1].get()) == typeid(TerminalNode));
	assert(ptree.root->children[0]->children[1]->children[0]->children[1]->children[1]->children[1]->bet_history == string("/cc/rc"));
	map<vector<vector<Card> >, vector<float> > tmp7 ={ {{{Card(13,1),},{Card(14,1),}}, {5,-5}}, {{{Card(13,1),},{Card(12,1),}}, {5,-5}}, {{{Card(14,1),},{Card(13,1),}}, {-5,5}}, {{{Card(14,1),},{Card(12,1),}}, {5,-5}}, {{{Card(12,1),},{Card(14,1),}}, {-5,5}}, {{{Card(12,1),},{Card(13,1),}}, {-5,5}} };
	assert(std::static_pointer_cast<TerminalNode>(ptree.root->children[0]->children[1]->children[0]->children[1]->children[1]->children[1])->payoffs == tmp7 );
	std::cout<< "All passed!"<<std::endl;

	return 0;
}




