#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include "pokertrees.hpp"
#include "pokergames.hpp"
#include <sstream>
namespace py = pybind11;

using rl::InfosetFormat;
using rl::GameRules;
using rl::Node;
using rl::TerminalNode;
using rl::GameTree;
using rl::PublicTree;
using rl::Card;
using rl::RoundInfo;
using rl::HandEvaluator;
using rl::default_infoset_format;
using rl::leduc_rules;
using rl::operator<<;

PYBIND11_MAKE_OPAQUE(vector<int>);
PYBIND11_MAKE_OPAQUE(vector<float>);
PYBIND11_MAKE_OPAQUE(vector<RoundInfo>);
PYBIND11_MAKE_OPAQUE(vector<Card>);
PYBIND11_MAKE_OPAQUE(vector<vector<Card> >);
PYBIND11_MAKE_OPAQUE(vector<vector<vector<Card> > >);
using map_payoffs = map<vector<vector<Card> >,vector<float> >;
PYBIND11_MAKE_OPAQUE(map_payoffs);


//std::ostream& rl::operator<<(std::ostream& out, const vector<Card>& vc);
//std::ostream& rl::operator<<(std::ostream& out, const vector<float>& vc);

PYBIND11_MODULE(pycppcfr,pcc) {


	py::class_<InfosetFormat>(pcc,"InfosetFormat");
	py::class_<Card>(pcc,"Card")
		.def(py::init<Card>())
		.def(py::init<int, int>())
		.def_readonly("rank",&Card::rank)
		.def_readonly("suit",&Card::suit)
		.def("__eq__",[](const Card& c1, const Card& c2){return c1.rank==c2.rank && c1.suit==c2.suit;})
		.def("__hash__",[](const Card& c1){return c1.rank+c1.suit*13;})
		.def("__lt__",[](const Card& c1, const Card& c2){return (c1.rank+c1.suit*13) < (c2.rank+c2.suit*13);})
		.def("__gt__",[](const Card& c1, const Card& c2){return (c1.rank+c1.suit*13) > (c2.rank+c2.suit*13);})
		.def("__le__",[](const Card& c1, const Card& c2){return (c1.rank+c1.suit*13) <= (c2.rank+c2.suit*13);})
		.def("__ge__",[](const Card& c1, const Card& c2){return (c1.rank+c1.suit*13) >= (c2.rank+c2.suit*13);})
		.def("__repr__", &Card::repr);
	//py::bind_vector<vector<float> >(pcc,"pyvec_float");
	//py::bind_vector<vector<Card> >(pcc,"pyvec_board");
	//py::bind_vector<vector<vector<vector<Card> > > >(pcc,"pyvec_holecards");
	//py::bind_map<std::map<vector<vector<Card> >,vector<float> > >(pcc,"pymap_payoffs");
	py::class_<vector<float> >(pcc, "pyvec_float")
		    .def("__repr__",[](const vector<float> &v){std::ostringstream ss; ss<<v;return ss.str();})
			.def("__iter__", [](const vector<float> &s) { return py::make_iterator(s.begin(), s.end()); },
				        py::keep_alive<0, 1>());

	py::class_<vector<Card> >(pcc, "pyvec_board")
		    .def("__repr__",[](const vector<Card> &v){std::ostringstream ss; ss<<v;return ss.str();})
			.def("__getitem__", [](const vector<Card>& vc, int key){
						try{return vc[key];}
						catch(const std::out_of_range&){
							std::ostringstream ss; ss<<key;
							throw py::key_error(string("key '")+ss.str()+string("' does not exist"));
						}
				})
			.def("__iter__", [](const vector<Card> &s) { return py::make_iterator(s.begin(), s.end()); },
				        py::keep_alive<0, 1>())
			.def("__len__", &vector<Card>::size);
	
	py::class_<vector<vector<Card> > >(pcc, "pytwovec_card")
		    .def("__repr__",[](const vector<vector<Card> > &v){std::ostringstream ss; ss<<v;return ss.str();})
			.def("__getitem__", [](const vector<vector<Card> >& vvc, int key){
						try{return vvc[key];}
						catch(const std::out_of_range&){
							std::ostringstream ss; ss<<key;
							throw py::key_error(string("key '")+ss.str()+string("' does not exist"));
						}
				})
			.def("__iter__", [](const vector<vector<Card> > &s) { return py::make_iterator(s.begin(), s.end()); },
				        py::keep_alive<0, 1>());

	py::class_<vector<vector<vector<Card> > > >(pcc, "pyvec_holecards")
		    .def("__repr__",[](const vector<vector<vector<Card> > > &v){std::ostringstream ss; ss<<v;return ss.str();})
			.def("__getitem__", [](const vector<vector<vector<Card> > >& vvvc, int key){
						try{return vvvc[key];}
						catch(const std::out_of_range&){
							std::ostringstream ss; ss<<key;
							throw py::key_error(string("key '")+ss.str()+string("' does not exist"));
						}
				})
			.def("__iter__", [](const vector<vector<vector<Card> > > &s) { return py::make_iterator(s.begin(), s.end()); },
				        py::keep_alive<0, 1>());

	py::class_<std::map<vector<vector<Card> >,vector<float> > >(pcc, "pymap_payoffs")
		    .def("__repr__",[](const std::map<vector<vector<Card> >,vector<float> >  &v){std::ostringstream ss; ss<<v;return ss.str();})
			.def("items",[](const map<vector<vector<Card> >,vector<float> >& m){return py::make_iterator(m.begin(),m.end());},py::keep_alive<0,1>())
			.def("__getitem__", [](const map<vector<vector<Card> >,vector<float> >& m, vector<vector<Card> > key){
						try{return m.at(key);}
						catch(const std::out_of_range&){
							std::ostringstream ss; ss<<key;
							throw py::key_error(string("key '")+ss.str()+string("' does not exist"));
						}
				})
			.def("__iter__", [](const map<vector<vector<Card> >, vector<float> > &s) { return py::make_iterator(s.begin(), s.end()); },
				        py::keep_alive<0, 1>())
			.def("__len__", &map<vector<vector<Card> >,vector<float> >::size);

	py::class_<GameRules> gr(pcc,"GameRules");
	gr.def(py::init<int, vector<Card>, vector<RoundInfo>, int, 
						vector<int>, GameRules::handeval_type, InfosetFormat>()); 
//						py::arg("handeval")=&HandEvaluator::evaluate_hand,
//						py::arg("infoset_format")=&default_infoset_format);
	gr.def(py::init<GameRules>());

	pcc.def("leduc_rules", &leduc_rules);

	py::class_<Node, std::shared_ptr<Node> > node(pcc,"Node");
	node.def(py::init<std::shared_ptr<Node>, vector<int>, 
					vector<vector<Card> >, vector<Card>,
					vector<Card>, string>())
		.def(py::init<std::shared_ptr<Node>, vector<int>, 
					vector<vector<vector<Card> > >, vector<Card>,
					vector<Card>, string>())
		.def_readonly("holecards", &Node::holecards)
		.def_readonly("board", &Node::board)
		.def_readonly("bet_history", &Node::bet_history)
		.def_readonly("children", &Node::children);

	py::class_<TerminalNode, std::shared_ptr<TerminalNode> >(pcc,"TerminalNode", node)
		.def(py::init<std::shared_ptr<Node>, vector<int>, 
					vector<vector<Card> >, vector<Card>,
					vector<Card>, string, vector<float>, vector<bool> >())
		.def(py::init<std::shared_ptr<Node>, vector<int>, 
					vector<vector<vector<Card> > >, vector<Card>,
					vector<Card>, string, map<vector<vector<Card> >, vector<float> >, vector<bool> >())
		.def_readonly("holecards", &TerminalNode::holecards)
		.def_readonly("board", &TerminalNode::board)
		.def_readonly("bet_history", &TerminalNode::bet_history)
		.def_readonly("payoffs", &TerminalNode::payoffs)
		.def_readonly("children", &TerminalNode::children);

	py::class_<GameTree> gt(pcc, "GameTree");
	gt.def(py::init<GameRules>())
	   .def("build", &GameTree::build)
	   .def_readonly("root",&GameTree::root);

	py::class_<PublicTree>(pcc, "PublicTree", gt)
		.def(py::init<GameRules>())
		.def("build", &PublicTree::build)
	    .def_readonly("root",&PublicTree::root);

}

