#include <bitset>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "pugixml.hpp"

namespace rr_graph {

struct t_channel;
struct t_x_list;
struct t_y_list;
struct t_timing;
struct t_sizing;
struct t_switch;
struct t_segment_timing;
struct t_segment;
struct t_pin;
struct t_pin_class;
struct t_meta;
struct t_metadata;
struct t_block_type;
struct t_grid_loc;
struct t_node_loc;
struct t_node_timing;
struct t_node_segment;
struct t_node;
struct t_edge;
struct t_channels;
struct t_switches;
struct t_segments;
struct t_block_types;
struct t_grid_locs;
struct t_rr_nodes;
struct t_rr_edges;
struct t_rr_graph;

enum class enum_switch_type {MUX, TRISTATE, PASS_GATE, SHORT, BUFFER};
enum class enum_node_direction {INC_DIR, DEC_DIR, BI_DIR};
enum class enum_node_type {CHANX, CHANY, SOURCE, SINK, OPIN, IPIN};
enum class enum_pin_type {OPEN, OUTPUT, INPUT};
enum class enum_loc_side {LEFT, RIGHT, TOP, BOTTOM};

enum_switch_type lex_switch_type(const char *in){
	if(strcmp(in, "mux") == 0){
		return enum_switch_type::MUX;
	}
	else if(strcmp(in, "tristate") == 0){
		return enum_switch_type::TRISTATE;
	}
	else if(strcmp(in, "pass_gate") == 0){
		return enum_switch_type::PASS_GATE;
	}
	else if(strcmp(in, "short") == 0){
		return enum_switch_type::SHORT;
	}
	else if(strcmp(in, "buffer") == 0){
		return enum_switch_type::BUFFER;
	}
	else throw std::runtime_error("Found unrecognized enum value " + std::string(in) + "of enum_switch_type.");
}

enum_node_direction lex_node_direction(const char *in){
	if(strcmp(in, "INC_DIR") == 0){
		return enum_node_direction::INC_DIR;
	}
	else if(strcmp(in, "DEC_DIR") == 0){
		return enum_node_direction::DEC_DIR;
	}
	else if(strcmp(in, "BI_DIR") == 0){
		return enum_node_direction::BI_DIR;
	}
	else throw std::runtime_error("Found unrecognized enum value " + std::string(in) + "of enum_node_direction.");
}

enum_node_type lex_node_type(const char *in){
	if(strcmp(in, "CHANX") == 0){
		return enum_node_type::CHANX;
	}
	else if(strcmp(in, "CHANY") == 0){
		return enum_node_type::CHANY;
	}
	else if(strcmp(in, "SOURCE") == 0){
		return enum_node_type::SOURCE;
	}
	else if(strcmp(in, "SINK") == 0){
		return enum_node_type::SINK;
	}
	else if(strcmp(in, "OPIN") == 0){
		return enum_node_type::OPIN;
	}
	else if(strcmp(in, "IPIN") == 0){
		return enum_node_type::IPIN;
	}
	else throw std::runtime_error("Found unrecognized enum value " + std::string(in) + "of enum_node_type.");
}

enum_pin_type lex_pin_type(const char *in){
	if(strcmp(in, "OPEN") == 0){
		return enum_pin_type::OPEN;
	}
	else if(strcmp(in, "OUTPUT") == 0){
		return enum_pin_type::OUTPUT;
	}
	else if(strcmp(in, "INPUT") == 0){
		return enum_pin_type::INPUT;
	}
	else throw std::runtime_error("Found unrecognized enum value " + std::string(in) + "of enum_pin_type.");
}

enum_loc_side lex_loc_side(const char *in){
	if(strcmp(in, "LEFT") == 0){
		return enum_loc_side::LEFT;
	}
	else if(strcmp(in, "RIGHT") == 0){
		return enum_loc_side::RIGHT;
	}
	else if(strcmp(in, "TOP") == 0){
		return enum_loc_side::TOP;
	}
	else if(strcmp(in, "BOTTOM") == 0){
		return enum_loc_side::BOTTOM;
	}
	else throw std::runtime_error("Found unrecognized enum value " + std::string(in) + "of enum_loc_side.");
}


struct t_channel {
	int chan_width_max;
	int x_min;
	int y_min;
	int x_max;
	int y_max;
};
struct t_x_list {
	int index;
	int info;
};
struct t_y_list {
	int index;
	int info;
};
struct t_timing {
	float R;
	float Cin;
	float Cinternal;
	float Cout;
	float Tdel;
};
struct t_sizing {
	float mux_trans_size = 0;
	float buf_size = 0;
};
struct t_switch {
	int id;
	std::string name;
	enum_switch_type type;
	std::vector<t_timing> timing_list;
	std::vector<t_sizing> sizing_list;
};
struct t_segment_timing {
	float R_per_meter;
	float C_per_meter;
};
struct t_segment {
	int id;
	std::string name;
	std::vector<t_segment_timing> timing_list;
};
struct t_pin {
	int ptc;
	std::string value;
};
struct t_pin_class {
	enum_pin_type type;
	std::vector<t_pin> pin_list;
};
struct t_meta {
	std::string name;
	int x_offset;
	int y_offset;
	std::string value;
};
struct t_metadata {
	std::vector<t_meta> meta_list;
};
struct t_block_type {
	int id;
	std::string name;
	int width;
	int height;
	std::vector<t_pin_class> pin_class_list;
};
struct t_grid_loc {
	int x;
	int y;
	int block_type_id;
	int width_offset;
	int height_offset;
};
struct t_node_loc {
	int xlow;
	int ylow;
	int xhigh;
	int yhigh;
	enum_loc_side side;
	int ptc;
};
struct t_node_timing {
	float R;
	float C;
};
struct t_node_segment {
	int segment_id;
};
struct t_node {
	int id;
	enum_node_type type;
	enum_node_direction direction;
	int capacity;
	std::vector<t_node_loc> loc_list;
	std::vector<t_node_timing> timing_list;
	std::vector<t_node_segment> segment_list;
	std::vector<t_metadata> metadata_list;
};
struct t_edge {
	int id;
	int src_node;
	int sink_node;
	int switch_id;
	std::vector<t_metadata> metadata_list;
};
struct t_channels {
	std::vector<t_channel> channel_list;
	std::vector<t_x_list> x_list_list;
	std::vector<t_y_list> y_list_list;
};
struct t_switches {
	std::vector<t_switch> switch_list;
};
struct t_segments {
	std::vector<t_segment> segment_list;
};
struct t_block_types {
	std::vector<t_block_type> block_type_list;
};
struct t_grid_locs {
	std::vector<t_grid_loc> grid_loc_list;
};
struct t_rr_nodes {
	std::vector<t_node> node_list;
};
struct t_rr_edges {
	std::vector<t_edge> edge_list;
};
struct t_rr_graph {
	std::string tool_name;
	std::string tool_version;
	std::string tool_comment;
	std::vector<t_channels> channels_list;
	std::vector<t_switches> switches_list;
	std::vector<t_segments> segments_list;
	std::vector<t_block_types> block_types_list;
	std::vector<t_grid_locs> grid_list;
	std::vector<t_rr_nodes> rr_nodes_list;
	std::vector<t_rr_edges> rr_edges_list;
};

std::unique_ptr<t_rr_graph> rr_graph;

void read_t_channel(pugi::xml_node &root, t_channel &out);
void read_t_x_list(pugi::xml_node &root, t_x_list &out);
void read_t_y_list(pugi::xml_node &root, t_y_list &out);
void read_t_timing(pugi::xml_node &root, t_timing &out);
void read_t_sizing(pugi::xml_node &root, t_sizing &out);
void read_t_switch(pugi::xml_node &root, t_switch &out);
void read_t_segment_timing(pugi::xml_node &root, t_segment_timing &out);
void read_t_segment(pugi::xml_node &root, t_segment &out);
void read_t_pin(pugi::xml_node &root, t_pin &out);
void read_t_pin_class(pugi::xml_node &root, t_pin_class &out);
void read_t_meta(pugi::xml_node &root, t_meta &out);
void read_t_metadata(pugi::xml_node &root, t_metadata &out);
void read_t_block_type(pugi::xml_node &root, t_block_type &out);
void read_t_grid_loc(pugi::xml_node &root, t_grid_loc &out);
void read_t_node_loc(pugi::xml_node &root, t_node_loc &out);
void read_t_node_timing(pugi::xml_node &root, t_node_timing &out);
void read_t_node_segment(pugi::xml_node &root, t_node_segment &out);
void read_t_node(pugi::xml_node &root, t_node &out);
void read_t_edge(pugi::xml_node &root, t_edge &out);
void read_t_channels(pugi::xml_node &root, t_channels &out);
void read_t_switches(pugi::xml_node &root, t_switches &out);
void read_t_segments(pugi::xml_node &root, t_segments &out);
void read_t_block_types(pugi::xml_node &root, t_block_types &out);
void read_t_grid_locs(pugi::xml_node &root, t_grid_locs &out);
void read_t_rr_nodes(pugi::xml_node &root, t_rr_nodes &out);
void read_t_rr_edges(pugi::xml_node &root, t_rr_edges &out);
void read_t_rr_graph(pugi::xml_node &root, t_rr_graph &out);

/* runtime error for state machines */
void dfa_error(const char *wrong, int *states, const char **lookup, int len){
	std::vector<std::string> expected;
	for(int i=0; i<len; i++){
		if(states[i] != -1) expected.push_back(lookup[i]);
	}

	std::string expected_or = expected[0];
	for(int i=1; i<expected.size(); i++)
		expected_or += std::string(" or ") + expected[i];

	throw std::runtime_error("Expected " + expected_or + ", found " + std::string(wrong));
}


/* runtime error for attributes */
template<std::size_t N>
void attr_error(std::bitset<N> astate, const char **lookup){
	std::vector<std::string> missing;
	for(int i=0; i<astate.size(); i++){
		if(astate[i] == 0) missing.push_back(lookup[i]);
	}

	std::string missing_and = missing[0];
	for(int i=1; i<missing.size(); i++)
		missing_and += std::string(" and ") + missing[i];

	throw std::runtime_error("Didn't find required attributes " + missing_and + ".");
}

enum class atok_t_channel {CHAN_WIDTH_MAX, X_MIN, Y_MIN, X_MAX, Y_MAX};
const char *atok_lookup_t_channel[] = {"chan_width_max", "x_min", "y_min", "x_max", "y_max"};
atok_t_channel alex_t_channel(const char *in){
	if(strcmp(in, "chan_width_max") == 0){
		return atok_t_channel::CHAN_WIDTH_MAX;
	}
	else if(strcmp(in, "x_min") == 0){
		return atok_t_channel::X_MIN;
	}
	else if(strcmp(in, "y_min") == 0){
		return atok_t_channel::Y_MIN;
	}
	else if(strcmp(in, "x_max") == 0){
		return atok_t_channel::X_MAX;
	}
	else if(strcmp(in, "y_max") == 0){
		return atok_t_channel::Y_MAX;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <channel>.");
}
void read_t_channel(pugi::xml_node &root, t_channel &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <channel>.");
	std::bitset<5> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_channel in = alex_t_channel(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <channel>.");
		switch(in){
		case atok_t_channel::CHAN_WIDTH_MAX:
			out.chan_width_max = std::stoi(attr.value());
			break;
		case atok_t_channel::X_MIN:
			out.x_min = std::stoi(attr.value());
			break;
		case atok_t_channel::Y_MIN:
			out.y_min = std::stoi(attr.value());
			break;
		case atok_t_channel::X_MAX:
			out.x_max = std::stoi(attr.value());
			break;
		case atok_t_channel::Y_MAX:
			out.y_max = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<5> test_state = astate | std::bitset<5>(0b00000);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_channel);
}

enum class atok_t_x_list {INDEX, INFO};
const char *atok_lookup_t_x_list[] = {"index", "info"};
atok_t_x_list alex_t_x_list(const char *in){
	if(strcmp(in, "index") == 0){
		return atok_t_x_list::INDEX;
	}
	else if(strcmp(in, "info") == 0){
		return atok_t_x_list::INFO;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <x_list>.");
}
void read_t_x_list(pugi::xml_node &root, t_x_list &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <x_list>.");
	std::bitset<2> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_x_list in = alex_t_x_list(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <x_list>.");
		switch(in){
		case atok_t_x_list::INDEX:
			out.index = std::stoi(attr.value());
			break;
		case atok_t_x_list::INFO:
			out.info = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<2> test_state = astate | std::bitset<2>(0b00);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_x_list);
}

enum class atok_t_y_list {INDEX, INFO};
const char *atok_lookup_t_y_list[] = {"index", "info"};
atok_t_y_list alex_t_y_list(const char *in){
	if(strcmp(in, "index") == 0){
		return atok_t_y_list::INDEX;
	}
	else if(strcmp(in, "info") == 0){
		return atok_t_y_list::INFO;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <y_list>.");
}
void read_t_y_list(pugi::xml_node &root, t_y_list &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <y_list>.");
	std::bitset<2> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_y_list in = alex_t_y_list(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <y_list>.");
		switch(in){
		case atok_t_y_list::INDEX:
			out.index = std::stoi(attr.value());
			break;
		case atok_t_y_list::INFO:
			out.info = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<2> test_state = astate | std::bitset<2>(0b00);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_y_list);
}

enum class atok_t_timing {R, CIN, CINTERNAL, COUT, TDEL};
const char *atok_lookup_t_timing[] = {"R", "Cin", "Cinternal", "Cout", "Tdel"};
atok_t_timing alex_t_timing(const char *in){
	if(strcmp(in, "R") == 0){
		return atok_t_timing::R;
	}
	else if(strcmp(in, "Cin") == 0){
		return atok_t_timing::CIN;
	}
	else if(strcmp(in, "Cinternal") == 0){
		return atok_t_timing::CINTERNAL;
	}
	else if(strcmp(in, "Cout") == 0){
		return atok_t_timing::COUT;
	}
	else if(strcmp(in, "Tdel") == 0){
		return atok_t_timing::TDEL;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <timing>.");
}
void read_t_timing(pugi::xml_node &root, t_timing &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <timing>.");
	std::bitset<5> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_timing in = alex_t_timing(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <timing>.");
		switch(in){
		case atok_t_timing::R:
			out.R = std::stof(attr.value());
			break;
		case atok_t_timing::CIN:
			out.Cin = std::stof(attr.value());
			break;
		case atok_t_timing::CINTERNAL:
			out.Cinternal = std::stof(attr.value());
			break;
		case atok_t_timing::COUT:
			out.Cout = std::stof(attr.value());
			break;
		case atok_t_timing::TDEL:
			out.Tdel = std::stof(attr.value());
			break;
		}
	}
	std::bitset<5> test_state = astate | std::bitset<5>(0b11111);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_timing);
}

enum class atok_t_sizing {MUX_TRANS_SIZE, BUF_SIZE};
const char *atok_lookup_t_sizing[] = {"mux_trans_size", "buf_size"};
atok_t_sizing alex_t_sizing(const char *in){
	if(strcmp(in, "mux_trans_size") == 0){
		return atok_t_sizing::MUX_TRANS_SIZE;
	}
	else if(strcmp(in, "buf_size") == 0){
		return atok_t_sizing::BUF_SIZE;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <sizing>.");
}
void read_t_sizing(pugi::xml_node &root, t_sizing &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <sizing>.");
	std::bitset<2> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_sizing in = alex_t_sizing(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <sizing>.");
		switch(in){
		case atok_t_sizing::MUX_TRANS_SIZE:
			out.mux_trans_size = std::stof(attr.value());
			break;
		case atok_t_sizing::BUF_SIZE:
			out.buf_size = std::stof(attr.value());
			break;
		}
	}
	std::bitset<2> test_state = astate | std::bitset<2>(0b00);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_sizing);
}

enum class gtok_t_switch {SIZING, TIMING};
const char *gtok_lookup_t_switch[] = {"timing", "sizing"};
enum class atok_t_switch {ID, NAME, TYPE};
const char *atok_lookup_t_switch[] = {"id", "name", "type"};
gtok_t_switch glex_t_switch(const char *in){
	if(strcmp(in, "sizing") == 0){
		return gtok_t_switch::SIZING;
	}
	else if(strcmp(in, "timing") == 0){
		return gtok_t_switch::TIMING;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <switch>.");
}
atok_t_switch alex_t_switch(const char *in){
	if(strcmp(in, "id") == 0){
		return atok_t_switch::ID;
	}
	else if(strcmp(in, "name") == 0){
		return atok_t_switch::NAME;
	}
	else if(strcmp(in, "type") == 0){
		return atok_t_switch::TYPE;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <switch>.");
}
int gstates0_t_switch[2][2] = {
	{0, 0},
	{0, 0},
};
void read_t_switch(pugi::xml_node &root, t_switch &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_switch in = glex_t_switch(node.name());
		next = gstates0_t_switch[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_switch[(int)in], gstates0_t_switch[state0], gtok_lookup_t_switch, 2);
		state0 = next;
		switch(in){
		case gtok_t_switch::TIMING:
			out.timing_list.push_back(t_timing());
			read_t_timing(node, out.timing_list.back());
			break;
		case gtok_t_switch::SIZING:
			out.sizing_list.push_back(t_sizing());
			read_t_sizing(node, out.sizing_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_switch[state0], gtok_lookup_t_switch, 2);
	std::bitset<3> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_switch in = alex_t_switch(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <switch>.");
		switch(in){
		case atok_t_switch::ID:
			out.id = std::stoi(attr.value());
			break;
		case atok_t_switch::NAME:
			out.name = attr.value();
			break;
		case atok_t_switch::TYPE:
			out.type = lex_switch_type(attr.value());
			break;
		}
	}
	std::bitset<3> test_state = astate | std::bitset<3>(0b100);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_switch);
}

enum class atok_t_segment_timing {R_PER_METER, C_PER_METER};
const char *atok_lookup_t_segment_timing[] = {"R_per_meter", "C_per_meter"};
atok_t_segment_timing alex_t_segment_timing(const char *in){
	if(strcmp(in, "R_per_meter") == 0){
		return atok_t_segment_timing::R_PER_METER;
	}
	else if(strcmp(in, "C_per_meter") == 0){
		return atok_t_segment_timing::C_PER_METER;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <segment_timing>.");
}
void read_t_segment_timing(pugi::xml_node &root, t_segment_timing &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <segment_timing>.");
	std::bitset<2> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_segment_timing in = alex_t_segment_timing(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <segment_timing>.");
		switch(in){
		case atok_t_segment_timing::R_PER_METER:
			out.R_per_meter = std::stof(attr.value());
			break;
		case atok_t_segment_timing::C_PER_METER:
			out.C_per_meter = std::stof(attr.value());
			break;
		}
	}
	std::bitset<2> test_state = astate | std::bitset<2>(0b11);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_segment_timing);
}

enum class gtok_t_segment {TIMING};
const char *gtok_lookup_t_segment[] = {"timing"};
enum class atok_t_segment {ID, NAME};
const char *atok_lookup_t_segment[] = {"id", "name"};
gtok_t_segment glex_t_segment(const char *in){
	if(strcmp(in, "timing") == 0){
		return gtok_t_segment::TIMING;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <segment>.");
}
atok_t_segment alex_t_segment(const char *in){
	if(strcmp(in, "id") == 0){
		return atok_t_segment::ID;
	}
	else if(strcmp(in, "name") == 0){
		return atok_t_segment::NAME;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <segment>.");
}
int gstates0_t_segment[2][1] = {
	{0},
	{0},
};
void read_t_segment(pugi::xml_node &root, t_segment &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_segment in = glex_t_segment(node.name());
		next = gstates0_t_segment[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_segment[(int)in], gstates0_t_segment[state0], gtok_lookup_t_segment, 1);
		state0 = next;
		switch(in){
		case gtok_t_segment::TIMING:
			out.timing_list.push_back(t_segment_timing());
			read_t_segment_timing(node, out.timing_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_segment[state0], gtok_lookup_t_segment, 1);
	std::bitset<2> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_segment in = alex_t_segment(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <segment>.");
		switch(in){
		case atok_t_segment::ID:
			out.id = std::stoi(attr.value());
			break;
		case atok_t_segment::NAME:
			out.name = attr.value();
			break;
		}
	}
	std::bitset<2> test_state = astate | std::bitset<2>(0b00);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_segment);
}

enum class atok_t_pin {PTC};
const char *atok_lookup_t_pin[] = {"ptc"};
atok_t_pin alex_t_pin(const char *in){
	if(strcmp(in, "ptc") == 0){
		return atok_t_pin::PTC;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <pin>.");
}
void read_t_pin(pugi::xml_node &root, t_pin &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <pin>.");
	out.value = root.child_value();
	std::bitset<1> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_pin in = alex_t_pin(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <pin>.");
		switch(in){
		case atok_t_pin::PTC:
			out.ptc = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<1> test_state = astate | std::bitset<1>(0b0);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_pin);
}

enum class gtok_t_pin_class {PIN};
const char *gtok_lookup_t_pin_class[] = {"pin"};
enum class atok_t_pin_class {TYPE};
const char *atok_lookup_t_pin_class[] = {"type"};
gtok_t_pin_class glex_t_pin_class(const char *in){
	if(strcmp(in, "pin") == 0){
		return gtok_t_pin_class::PIN;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <pin_class>.");
}
atok_t_pin_class alex_t_pin_class(const char *in){
	if(strcmp(in, "type") == 0){
		return atok_t_pin_class::TYPE;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <pin_class>.");
}
int gstates0_t_pin_class[2][1] = {
	{0},
	{0},
};
void read_t_pin_class(pugi::xml_node &root, t_pin_class &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_pin_class in = glex_t_pin_class(node.name());
		next = gstates0_t_pin_class[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_pin_class[(int)in], gstates0_t_pin_class[state0], gtok_lookup_t_pin_class, 1);
		state0 = next;
		switch(in){
		case gtok_t_pin_class::PIN:
			out.pin_list.push_back(t_pin());
			read_t_pin(node, out.pin_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_pin_class[state0], gtok_lookup_t_pin_class, 1);
	std::bitset<1> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_pin_class in = alex_t_pin_class(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <pin_class>.");
		switch(in){
		case atok_t_pin_class::TYPE:
			out.type = lex_pin_type(attr.value());
			break;
		}
	}
	std::bitset<1> test_state = astate | std::bitset<1>(0b0);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_pin_class);
}

enum class atok_t_meta {NAME, X_OFFSET, Y_OFFSET};
const char *atok_lookup_t_meta[] = {"name", "x_offset", "y_offset"};
atok_t_meta alex_t_meta(const char *in){
	if(strcmp(in, "name") == 0){
		return atok_t_meta::NAME;
	}
	else if(strcmp(in, "x_offset") == 0){
		return atok_t_meta::X_OFFSET;
	}
	else if(strcmp(in, "y_offset") == 0){
		return atok_t_meta::Y_OFFSET;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <meta>.");
}
void read_t_meta(pugi::xml_node &root, t_meta &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <meta>.");
	out.value = root.child_value();
	std::bitset<3> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_meta in = alex_t_meta(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <meta>.");
		switch(in){
		case atok_t_meta::NAME:
			out.name = attr.value();
			break;
		case atok_t_meta::X_OFFSET:
			out.x_offset = std::stoi(attr.value());
			break;
		case atok_t_meta::Y_OFFSET:
			out.y_offset = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<3> test_state = astate | std::bitset<3>(0b110);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_meta);
}

enum class gtok_t_metadata {META};
const char *gtok_lookup_t_metadata[] = {"meta"};
gtok_t_metadata glex_t_metadata(const char *in){
	if(strcmp(in, "meta") == 0){
		return gtok_t_metadata::META;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <metadata>.");
}
int gstates0_t_metadata[2][1] = {
	{0},
	{0},
};
void read_t_metadata(pugi::xml_node &root, t_metadata &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_metadata in = glex_t_metadata(node.name());
		next = gstates0_t_metadata[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_metadata[(int)in], gstates0_t_metadata[state0], gtok_lookup_t_metadata, 1);
		state0 = next;
		switch(in){
		case gtok_t_metadata::META:
			out.meta_list.push_back(t_meta());
			read_t_meta(node, out.meta_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_metadata[state0], gtok_lookup_t_metadata, 1);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <metadata>.");
}

enum class gtok_t_block_type {PIN_CLASS};
const char *gtok_lookup_t_block_type[] = {"pin_class"};
enum class atok_t_block_type {ID, NAME, WIDTH, HEIGHT};
const char *atok_lookup_t_block_type[] = {"id", "name", "width", "height"};
gtok_t_block_type glex_t_block_type(const char *in){
	if(strcmp(in, "pin_class") == 0){
		return gtok_t_block_type::PIN_CLASS;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <block_type>.");
}
atok_t_block_type alex_t_block_type(const char *in){
	if(strcmp(in, "id") == 0){
		return atok_t_block_type::ID;
	}
	else if(strcmp(in, "name") == 0){
		return atok_t_block_type::NAME;
	}
	else if(strcmp(in, "width") == 0){
		return atok_t_block_type::WIDTH;
	}
	else if(strcmp(in, "height") == 0){
		return atok_t_block_type::HEIGHT;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <block_type>.");
}
int gstates0_t_block_type[1][1] = {
	{0},
};
void read_t_block_type(pugi::xml_node &root, t_block_type &out){
	int next, state0 = 0;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_block_type in = glex_t_block_type(node.name());
		next = gstates0_t_block_type[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_block_type[(int)in], gstates0_t_block_type[state0], gtok_lookup_t_block_type, 1);
		state0 = next;
		switch(in){
		case gtok_t_block_type::PIN_CLASS:
			out.pin_class_list.push_back(t_pin_class());
			read_t_pin_class(node, out.pin_class_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_block_type[state0], gtok_lookup_t_block_type, 1);
	std::bitset<4> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_block_type in = alex_t_block_type(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <block_type>.");
		switch(in){
		case atok_t_block_type::ID:
			out.id = std::stoi(attr.value());
			break;
		case atok_t_block_type::NAME:
			out.name = attr.value();
			break;
		case atok_t_block_type::WIDTH:
			out.width = std::stoi(attr.value());
			break;
		case atok_t_block_type::HEIGHT:
			out.height = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<4> test_state = astate | std::bitset<4>(0b0000);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_block_type);
}

enum class atok_t_grid_loc {X, Y, BLOCK_TYPE_ID, WIDTH_OFFSET, HEIGHT_OFFSET};
const char *atok_lookup_t_grid_loc[] = {"x", "y", "block_type_id", "width_offset", "height_offset"};
atok_t_grid_loc alex_t_grid_loc(const char *in){
	if(strcmp(in, "x") == 0){
		return atok_t_grid_loc::X;
	}
	else if(strcmp(in, "y") == 0){
		return atok_t_grid_loc::Y;
	}
	else if(strcmp(in, "block_type_id") == 0){
		return atok_t_grid_loc::BLOCK_TYPE_ID;
	}
	else if(strcmp(in, "width_offset") == 0){
		return atok_t_grid_loc::WIDTH_OFFSET;
	}
	else if(strcmp(in, "height_offset") == 0){
		return atok_t_grid_loc::HEIGHT_OFFSET;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <grid_loc>.");
}
void read_t_grid_loc(pugi::xml_node &root, t_grid_loc &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <grid_loc>.");
	std::bitset<5> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_grid_loc in = alex_t_grid_loc(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <grid_loc>.");
		switch(in){
		case atok_t_grid_loc::X:
			out.x = std::stoi(attr.value());
			break;
		case atok_t_grid_loc::Y:
			out.y = std::stoi(attr.value());
			break;
		case atok_t_grid_loc::BLOCK_TYPE_ID:
			out.block_type_id = std::stoi(attr.value());
			break;
		case atok_t_grid_loc::WIDTH_OFFSET:
			out.width_offset = std::stoi(attr.value());
			break;
		case atok_t_grid_loc::HEIGHT_OFFSET:
			out.height_offset = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<5> test_state = astate | std::bitset<5>(0b00000);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_grid_loc);
}

enum class atok_t_node_loc {XLOW, YLOW, XHIGH, YHIGH, SIDE, PTC};
const char *atok_lookup_t_node_loc[] = {"xlow", "ylow", "xhigh", "yhigh", "side", "ptc"};
atok_t_node_loc alex_t_node_loc(const char *in){
	if(strcmp(in, "xlow") == 0){
		return atok_t_node_loc::XLOW;
	}
	else if(strcmp(in, "ylow") == 0){
		return atok_t_node_loc::YLOW;
	}
	else if(strcmp(in, "xhigh") == 0){
		return atok_t_node_loc::XHIGH;
	}
	else if(strcmp(in, "yhigh") == 0){
		return atok_t_node_loc::YHIGH;
	}
	else if(strcmp(in, "side") == 0){
		return atok_t_node_loc::SIDE;
	}
	else if(strcmp(in, "ptc") == 0){
		return atok_t_node_loc::PTC;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <node_loc>.");
}
void read_t_node_loc(pugi::xml_node &root, t_node_loc &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <node_loc>.");
	std::bitset<6> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_node_loc in = alex_t_node_loc(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <node_loc>.");
		switch(in){
		case atok_t_node_loc::XLOW:
			out.xlow = std::stoi(attr.value());
			break;
		case atok_t_node_loc::YLOW:
			out.ylow = std::stoi(attr.value());
			break;
		case atok_t_node_loc::XHIGH:
			out.xhigh = std::stoi(attr.value());
			break;
		case atok_t_node_loc::YHIGH:
			out.yhigh = std::stoi(attr.value());
			break;
		case atok_t_node_loc::SIDE:
			out.side = lex_loc_side(attr.value());
			break;
		case atok_t_node_loc::PTC:
			out.ptc = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<6> test_state = astate | std::bitset<6>(0b010000);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_node_loc);
}

enum class atok_t_node_timing {R, C};
const char *atok_lookup_t_node_timing[] = {"R", "C"};
atok_t_node_timing alex_t_node_timing(const char *in){
	if(strcmp(in, "R") == 0){
		return atok_t_node_timing::R;
	}
	else if(strcmp(in, "C") == 0){
		return atok_t_node_timing::C;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <node_timing>.");
}
void read_t_node_timing(pugi::xml_node &root, t_node_timing &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <node_timing>.");
	std::bitset<2> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_node_timing in = alex_t_node_timing(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <node_timing>.");
		switch(in){
		case atok_t_node_timing::R:
			out.R = std::stof(attr.value());
			break;
		case atok_t_node_timing::C:
			out.C = std::stof(attr.value());
			break;
		}
	}
	std::bitset<2> test_state = astate | std::bitset<2>(0b00);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_node_timing);
}

enum class atok_t_node_segment {SEGMENT_ID};
const char *atok_lookup_t_node_segment[] = {"segment_id"};
atok_t_node_segment alex_t_node_segment(const char *in){
	if(strcmp(in, "segment_id") == 0){
		return atok_t_node_segment::SEGMENT_ID;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <node_segment>.");
}
void read_t_node_segment(pugi::xml_node &root, t_node_segment &out){
	if(root.first_child().type() == pugi::node_element) throw std::runtime_error("Unexpected child element in <node_segment>.");
	std::bitset<1> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_node_segment in = alex_t_node_segment(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <node_segment>.");
		switch(in){
		case atok_t_node_segment::SEGMENT_ID:
			out.segment_id = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<1> test_state = astate | std::bitset<1>(0b0);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_node_segment);
}

enum class gtok_t_node {SEGMENT, LOC, TIMING, METADATA};
const char *gtok_lookup_t_node[] = {"metadata", "loc", "segment", "timing"};
enum class atok_t_node {ID, TYPE, DIRECTION, CAPACITY};
const char *atok_lookup_t_node[] = {"id", "type", "direction", "capacity"};
gtok_t_node glex_t_node(const char *in){
	if(strcmp(in, "metadata") == 0){
		return gtok_t_node::METADATA;
	}
	else if(strcmp(in, "loc") == 0){
		return gtok_t_node::LOC;
	}
	else if(strcmp(in, "timing") == 0){
		return gtok_t_node::TIMING;
	}
	else if(strcmp(in, "segment") == 0){
		return gtok_t_node::SEGMENT;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <node>.");
}
atok_t_node alex_t_node(const char *in){
	if(strcmp(in, "id") == 0){
		return atok_t_node::ID;
	}
	else if(strcmp(in, "type") == 0){
		return atok_t_node::TYPE;
	}
	else if(strcmp(in, "direction") == 0){
		return atok_t_node::DIRECTION;
	}
	else if(strcmp(in, "capacity") == 0){
		return atok_t_node::CAPACITY;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <node>.");
}
int gstates0_t_node[2][4] = {
	{0, 0, 0, 0},
	{0, 0, 0, 0},
};
void read_t_node(pugi::xml_node &root, t_node &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_node in = glex_t_node(node.name());
		next = gstates0_t_node[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_node[(int)in], gstates0_t_node[state0], gtok_lookup_t_node, 4);
		state0 = next;
		switch(in){
		case gtok_t_node::LOC:
			out.loc_list.push_back(t_node_loc());
			read_t_node_loc(node, out.loc_list.back());
			break;
		case gtok_t_node::TIMING:
			out.timing_list.push_back(t_node_timing());
			read_t_node_timing(node, out.timing_list.back());
			break;
		case gtok_t_node::SEGMENT:
			out.segment_list.push_back(t_node_segment());
			read_t_node_segment(node, out.segment_list.back());
			break;
		case gtok_t_node::METADATA:
			out.metadata_list.push_back(t_metadata());
			read_t_metadata(node, out.metadata_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_node[state0], gtok_lookup_t_node, 4);
	std::bitset<4> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_node in = alex_t_node(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <node>.");
		switch(in){
		case atok_t_node::ID:
			out.id = std::stoi(attr.value());
			break;
		case atok_t_node::TYPE:
			out.type = lex_node_type(attr.value());
			break;
		case atok_t_node::DIRECTION:
			out.direction = lex_node_direction(attr.value());
			break;
		case atok_t_node::CAPACITY:
			out.capacity = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<4> test_state = astate | std::bitset<4>(0b0100);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_node);
}

enum class gtok_t_edge {METADATA};
const char *gtok_lookup_t_edge[] = {"metadata"};
enum class atok_t_edge {ID, SRC_NODE, SINK_NODE, SWITCH_ID};
const char *atok_lookup_t_edge[] = {"id", "src_node", "sink_node", "switch_id"};
gtok_t_edge glex_t_edge(const char *in){
	if(strcmp(in, "metadata") == 0){
		return gtok_t_edge::METADATA;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <edge>.");
}
atok_t_edge alex_t_edge(const char *in){
	if(strcmp(in, "id") == 0){
		return atok_t_edge::ID;
	}
	else if(strcmp(in, "src_node") == 0){
		return atok_t_edge::SRC_NODE;
	}
	else if(strcmp(in, "sink_node") == 0){
		return atok_t_edge::SINK_NODE;
	}
	else if(strcmp(in, "switch_id") == 0){
		return atok_t_edge::SWITCH_ID;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <edge>.");
}
int gstates0_t_edge[1][1] = {
	{0},
};
void read_t_edge(pugi::xml_node &root, t_edge &out){
	int next, state0 = 0;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_edge in = glex_t_edge(node.name());
		next = gstates0_t_edge[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_edge[(int)in], gstates0_t_edge[state0], gtok_lookup_t_edge, 1);
		state0 = next;
		switch(in){
		case gtok_t_edge::METADATA:
			out.metadata_list.push_back(t_metadata());
			read_t_metadata(node, out.metadata_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_edge[state0], gtok_lookup_t_edge, 1);
	std::bitset<4> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_edge in = alex_t_edge(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <edge>.");
		switch(in){
		case atok_t_edge::ID:
			out.id = std::stoi(attr.value());
			break;
		case atok_t_edge::SRC_NODE:
			out.src_node = std::stoi(attr.value());
			break;
		case atok_t_edge::SINK_NODE:
			out.sink_node = std::stoi(attr.value());
			break;
		case atok_t_edge::SWITCH_ID:
			out.switch_id = std::stoi(attr.value());
			break;
		}
	}
	std::bitset<4> test_state = astate | std::bitset<4>(0b0001);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_edge);
}

enum class gtok_t_channels {CHANNEL, Y_LIST, X_LIST};
const char *gtok_lookup_t_channels[] = {"channel", "y_list", "x_list"};
gtok_t_channels glex_t_channels(const char *in){
	if(strcmp(in, "x_list") == 0){
		return gtok_t_channels::X_LIST;
	}
	else if(strcmp(in, "y_list") == 0){
		return gtok_t_channels::Y_LIST;
	}
	else if(strcmp(in, "channel") == 0){
		return gtok_t_channels::CHANNEL;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <channels>.");
}
int gstates0_t_channels[2][3] = {
	{0, 0, 0},
	{0, 0, 0},
};
void read_t_channels(pugi::xml_node &root, t_channels &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_channels in = glex_t_channels(node.name());
		next = gstates0_t_channels[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_channels[(int)in], gstates0_t_channels[state0], gtok_lookup_t_channels, 3);
		state0 = next;
		switch(in){
		case gtok_t_channels::CHANNEL:
			out.channel_list.push_back(t_channel());
			read_t_channel(node, out.channel_list.back());
			break;
		case gtok_t_channels::X_LIST:
			out.x_list_list.push_back(t_x_list());
			read_t_x_list(node, out.x_list_list.back());
			break;
		case gtok_t_channels::Y_LIST:
			out.y_list_list.push_back(t_y_list());
			read_t_y_list(node, out.y_list_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_channels[state0], gtok_lookup_t_channels, 3);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <channels>.");
}

enum class gtok_t_switches {SWITCH};
const char *gtok_lookup_t_switches[] = {"switch"};
gtok_t_switches glex_t_switches(const char *in){
	if(strcmp(in, "switch") == 0){
		return gtok_t_switches::SWITCH;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <switches>.");
}
int gstates0_t_switches[2][1] = {
	{0},
	{0},
};
void read_t_switches(pugi::xml_node &root, t_switches &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_switches in = glex_t_switches(node.name());
		next = gstates0_t_switches[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_switches[(int)in], gstates0_t_switches[state0], gtok_lookup_t_switches, 1);
		state0 = next;
		switch(in){
		case gtok_t_switches::SWITCH:
			out.switch_list.push_back(t_switch());
			read_t_switch(node, out.switch_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_switches[state0], gtok_lookup_t_switches, 1);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <switches>.");
}

enum class gtok_t_segments {SEGMENT};
const char *gtok_lookup_t_segments[] = {"segment"};
gtok_t_segments glex_t_segments(const char *in){
	if(strcmp(in, "segment") == 0){
		return gtok_t_segments::SEGMENT;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <segments>.");
}
int gstates0_t_segments[2][1] = {
	{0},
	{0},
};
void read_t_segments(pugi::xml_node &root, t_segments &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_segments in = glex_t_segments(node.name());
		next = gstates0_t_segments[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_segments[(int)in], gstates0_t_segments[state0], gtok_lookup_t_segments, 1);
		state0 = next;
		switch(in){
		case gtok_t_segments::SEGMENT:
			out.segment_list.push_back(t_segment());
			read_t_segment(node, out.segment_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_segments[state0], gtok_lookup_t_segments, 1);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <segments>.");
}

enum class gtok_t_block_types {BLOCK_TYPE};
const char *gtok_lookup_t_block_types[] = {"block_type"};
gtok_t_block_types glex_t_block_types(const char *in){
	if(strcmp(in, "block_type") == 0){
		return gtok_t_block_types::BLOCK_TYPE;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <block_types>.");
}
int gstates0_t_block_types[2][1] = {
	{0},
	{0},
};
void read_t_block_types(pugi::xml_node &root, t_block_types &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_block_types in = glex_t_block_types(node.name());
		next = gstates0_t_block_types[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_block_types[(int)in], gstates0_t_block_types[state0], gtok_lookup_t_block_types, 1);
		state0 = next;
		switch(in){
		case gtok_t_block_types::BLOCK_TYPE:
			out.block_type_list.push_back(t_block_type());
			read_t_block_type(node, out.block_type_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_block_types[state0], gtok_lookup_t_block_types, 1);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <block_types>.");
}

enum class gtok_t_grid_locs {GRID_LOC};
const char *gtok_lookup_t_grid_locs[] = {"grid_loc"};
gtok_t_grid_locs glex_t_grid_locs(const char *in){
	if(strcmp(in, "grid_loc") == 0){
		return gtok_t_grid_locs::GRID_LOC;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <grid_locs>.");
}
int gstates0_t_grid_locs[2][1] = {
	{0},
	{0},
};
void read_t_grid_locs(pugi::xml_node &root, t_grid_locs &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_grid_locs in = glex_t_grid_locs(node.name());
		next = gstates0_t_grid_locs[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_grid_locs[(int)in], gstates0_t_grid_locs[state0], gtok_lookup_t_grid_locs, 1);
		state0 = next;
		switch(in){
		case gtok_t_grid_locs::GRID_LOC:
			out.grid_loc_list.push_back(t_grid_loc());
			read_t_grid_loc(node, out.grid_loc_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_grid_locs[state0], gtok_lookup_t_grid_locs, 1);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <grid_locs>.");
}

enum class gtok_t_rr_nodes {NODE};
const char *gtok_lookup_t_rr_nodes[] = {"node"};
gtok_t_rr_nodes glex_t_rr_nodes(const char *in){
	if(strcmp(in, "node") == 0){
		return gtok_t_rr_nodes::NODE;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <rr_nodes>.");
}
int gstates0_t_rr_nodes[2][1] = {
	{0},
	{0},
};
void read_t_rr_nodes(pugi::xml_node &root, t_rr_nodes &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_rr_nodes in = glex_t_rr_nodes(node.name());
		next = gstates0_t_rr_nodes[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_rr_nodes[(int)in], gstates0_t_rr_nodes[state0], gtok_lookup_t_rr_nodes, 1);
		state0 = next;
		switch(in){
		case gtok_t_rr_nodes::NODE:
			out.node_list.push_back(t_node());
			read_t_node(node, out.node_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_rr_nodes[state0], gtok_lookup_t_rr_nodes, 1);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <rr_nodes>.");
}

enum class gtok_t_rr_edges {EDGE};
const char *gtok_lookup_t_rr_edges[] = {"edge"};
gtok_t_rr_edges glex_t_rr_edges(const char *in){
	if(strcmp(in, "edge") == 0){
		return gtok_t_rr_edges::EDGE;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <rr_edges>.");
}
int gstates0_t_rr_edges[2][1] = {
	{0},
	{0},
};
void read_t_rr_edges(pugi::xml_node &root, t_rr_edges &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_rr_edges in = glex_t_rr_edges(node.name());
		next = gstates0_t_rr_edges[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_rr_edges[(int)in], gstates0_t_rr_edges[state0], gtok_lookup_t_rr_edges, 1);
		state0 = next;
		switch(in){
		case gtok_t_rr_edges::EDGE:
			out.edge_list.push_back(t_edge());
			read_t_edge(node, out.edge_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_rr_edges[state0], gtok_lookup_t_rr_edges, 1);
	if(root.first_attribute()) std::runtime_error("Unexpected attribute in <rr_edges>.");
}

enum class gtok_t_rr_graph {GRID, CHANNELS, SEGMENTS, BLOCK_TYPES, RR_NODES, RR_EDGES, SWITCHES};
const char *gtok_lookup_t_rr_graph[] = {"grid", "segments", "channels", "switches", "rr_edges", "block_types", "rr_nodes"};
enum class atok_t_rr_graph {TOOL_NAME, TOOL_VERSION, TOOL_COMMENT};
const char *atok_lookup_t_rr_graph[] = {"tool_name", "tool_version", "tool_comment"};
gtok_t_rr_graph glex_t_rr_graph(const char *in){
	if(strcmp(in, "grid") == 0){
		return gtok_t_rr_graph::GRID;
	}
	else if(strcmp(in, "channels") == 0){
		return gtok_t_rr_graph::CHANNELS;
	}
	else if(strcmp(in, "switches") == 0){
		return gtok_t_rr_graph::SWITCHES;
	}
	else if(strcmp(in, "rr_nodes") == 0){
		return gtok_t_rr_graph::RR_NODES;
	}
	else if(strcmp(in, "segments") == 0){
		return gtok_t_rr_graph::SEGMENTS;
	}
	else if(strcmp(in, "block_types") == 0){
		return gtok_t_rr_graph::BLOCK_TYPES;
	}
	else if(strcmp(in, "rr_edges") == 0){
		return gtok_t_rr_graph::RR_EDGES;
	}
	else throw std::runtime_error("Found unrecognized child " + std::string(in) + " of <rr_graph>.");
}
atok_t_rr_graph alex_t_rr_graph(const char *in){
	if(strcmp(in, "tool_name") == 0){
		return atok_t_rr_graph::TOOL_NAME;
	}
	else if(strcmp(in, "tool_version") == 0){
		return atok_t_rr_graph::TOOL_VERSION;
	}
	else if(strcmp(in, "tool_comment") == 0){
		return atok_t_rr_graph::TOOL_COMMENT;
	}
	else throw std::runtime_error("Found unrecognized attribute " + std::string(in) + " of <rr_graph>.");
}
int gstates0_t_rr_graph[2][7] = {
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
};
void read_t_rr_graph(pugi::xml_node &root, t_rr_graph &out){
	int next, state0 = 1;
	for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling()){
		gtok_t_rr_graph in = glex_t_rr_graph(node.name());
		next = gstates0_t_rr_graph[state0][(int)in];
		if(next == -1) dfa_error(gtok_lookup_t_rr_graph[(int)in], gstates0_t_rr_graph[state0], gtok_lookup_t_rr_graph, 7);
		state0 = next;
		switch(in){
		case gtok_t_rr_graph::CHANNELS:
			out.channels_list.push_back(t_channels());
			read_t_channels(node, out.channels_list.back());
			break;
		case gtok_t_rr_graph::SWITCHES:
			out.switches_list.push_back(t_switches());
			read_t_switches(node, out.switches_list.back());
			break;
		case gtok_t_rr_graph::SEGMENTS:
			out.segments_list.push_back(t_segments());
			read_t_segments(node, out.segments_list.back());
			break;
		case gtok_t_rr_graph::BLOCK_TYPES:
			out.block_types_list.push_back(t_block_types());
			read_t_block_types(node, out.block_types_list.back());
			break;
		case gtok_t_rr_graph::GRID:
			out.grid_list.push_back(t_grid_locs());
			read_t_grid_locs(node, out.grid_list.back());
			break;
		case gtok_t_rr_graph::RR_NODES:
			out.rr_nodes_list.push_back(t_rr_nodes());
			read_t_rr_nodes(node, out.rr_nodes_list.back());
			break;
		case gtok_t_rr_graph::RR_EDGES:
			out.rr_edges_list.push_back(t_rr_edges());
			read_t_rr_edges(node, out.rr_edges_list.back());
			break;
		}
	}
	if((state0 != 0)) dfa_error("end of input", gstates0_t_rr_graph[state0], gtok_lookup_t_rr_graph, 7);
	std::bitset<3> astate = 0;
	for(pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()){
		atok_t_rr_graph in = alex_t_rr_graph(attr.name());
		if(astate[(int)in] == 0) astate[(int)in] = 1;
		else throw std::runtime_error("Duplicate attribute " + std::string(attr.name()) + " in <rr_graph>.");
		switch(in){
		case atok_t_rr_graph::TOOL_NAME:
			out.tool_name = attr.value();
			break;
		case atok_t_rr_graph::TOOL_VERSION:
			out.tool_version = attr.value();
			break;
		case atok_t_rr_graph::TOOL_COMMENT:
			out.tool_comment = attr.value();
			break;
		}
	}
	std::bitset<3> test_state = astate | std::bitset<3>(0b111);
	if(!test_state.all()) attr_error(test_state, atok_lookup_t_rr_graph);
}


void get_root_elements(const char *filename){
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename);
	if(!result){
		throw std::runtime_error("Could not load XML file " + std::string(filename) + ".");
	}
	pugi::xml_node node = doc.first_child();
	for(; node; node = node.next_sibling()){
		if(std::strcmp(node.name(), "rr_graph") == 0){
			rr_graph = std::unique_ptr<t_rr_graph>(new t_rr_graph);
			read_t_rr_graph(node, *rr_graph);
		}
		else throw std::runtime_error("Invalid root-level element " + std::string(node.name()));
	}
}

}
