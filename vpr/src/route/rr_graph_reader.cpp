/*This function loads in a routing resource graph written in xml format
 * into vpr when the option --read_rr_graph <file name> is specified.
 * When it is not specified the build_rr_graph function is then called.
 * This is done using the libpugixml library. This is useful
 * when specific routing resources should remain constant or when
 * some information left out in the architecture can be specified
 * in the routing resource graph. The routing resource graph file is
 * contained in a <rr_graph> tag. Its subtags describe the rr graph's
 * various features such as nodes, edges, and segments. Information such
 * as blocks, grids, and segments are verified with the architecture
 * to ensure it matches. An error will through if any feature does not match.
 * Other elements such as edges, nodes, and switches
 * are overwritten by the rr graph file if one is specified. If an optional
 * identifier such as capacitance is not specified, it is set to 0*/

#include <string.h>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iostream>
#include <utility>

#include "vtr_version.h"
#include "vtr_assert.h"
#include "vtr_util.h"
#include "vtr_memory.h"
#include "vtr_math.h"
#include "vtr_log.h"
#include "vtr_time.h"

#include "pugixml.hpp"
#include "pugixml_util.hpp"
#include "read_xml_arch_file.h"
#include "read_xml_util.h"
#include "globals.h"
#include "rr_graph.h"
#include "rr_graph2.h"
#include "rr_metadata.h"
#include "rr_graph_indexed_data.h"
#include "rr_graph_writer.h"
#include "check_rr_graph.h"
#include "echo_files.h"

#include "vpr_types.h"
#include "vpr_utils.h"
#include "vpr_error.h"

#include "rr_graph_reader.h"

#include "rr_graph_uxsdcxx.h"

using namespace std;
using namespace pugiutil;

/*********************** Subroutines local to this module *******************/
void process_rr_node_indices(const DeviceGrid& grid);
void set_cost_indices(pugi::xml_node parent, const pugiutil::loc_data& loc_data, const bool is_global_graph, const int num_seg_types);

/************************ Subroutine definitions ****************************/

static int pin_index_by_num(const t_class& class_inf, int num) {
    for (int index = 0; index < class_inf.num_pins; ++index) {
        if (num == class_inf.pinlist[index]) {
            return index;
        }
    }
    return -1;
}

/*loads the given RR_graph file into the appropriate data structures
 * as specified by read_rr_graph_name. Set up correct routing data
 * structures as well*/
void load_rr_file(const t_graph_type graph_type,
                  const DeviceGrid& grid,
                  t_chan_width nodes_per_chan,
                  const std::vector<t_segment_inf>& segment_inf,
                  const enum e_base_cost_type base_cost_type,
                  int* wire_to_rr_ipin_switch,
                  const char* read_rr_graph_name) {
    auto& device_ctx = g_vpr_ctx.mutable_device();

    vtr::ScopedStartFinishTimer timer("Loading routing resource graph");
    rr_graph::get_root_elements(read_rr_graph_name);

    if(!rr_graph::rr_graph->tool_version.empty() && rr_graph::rr_graph->tool_version != vtr::VERSION){
        VTR_LOG("\n");
        VTR_LOG_WARN("This architecture version is for VPR %s while your current VPR version is %s compatability issues may arise\n",
                     vtr::VERSION, rr_graph::rr_graph->tool_version.c_str());
        VTR_LOG("\n");
    }
    string correct_string = "Generated from arch file ";
    correct_string += get_arch_file_name();
    if(!rr_graph::rr_graph->tool_comment.empty() && rr_graph::rr_graph->tool_comment != correct_string){
            VTR_LOG("\n");
            VTR_LOG_WARN("This RR graph file is based on %s while your input architecture file is %s compatability issues may arise\n",
                         get_arch_file_name(), rr_graph::rr_graph->tool_comment.c_str());
            VTR_LOG("\n");
    }

    //Compare with the architecture file to ensure consistency.
    /* TODO: Fix this by using xs:all instead of xs:choice. */
    for(auto &g: rr_graph::rr_graph->grid_list[0].grid_loc_list){
        t_grid_tile& grid_tile = grid[g.x][g.y];
        if(grid_tile.type->index != g.block_type_id){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "Architecture file does not match RR graph's block_type_id at (%d, %d): arch used ID %d, RR graph used ID %d.", g.x, g.y,
                      (grid_tile.type->index), g.block_type_id);
        }
        if(grid_tile.width_offset != g.width_offset){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "Architecture file does not match RR graph's width_offset at (%d, %d)", g.x, g.y);
        }
        if(grid_tile.height_offset != g.height_offset){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "Architecture file does not match RR graph's height_offset at (%d, %d)", g.x, g.y);
        }
    }

    for(auto &b: rr_graph::rr_graph->block_types_list[0].block_type_list){
        auto block_info = device_ctx.block_types[b.id];
        if(block_info.name != b.name){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "Architecture file does not match RR graph's block name: arch uses name %s, RR graph uses name %s", block_info.name, b.name.c_str());
        }
        if(block_info.width != b.width){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "Architecture file does not match RR graph's block width");
        }
        if(block_info.height != b.height){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "Architecture file does not match RR graph's block height");
        }
        for(int i=0; i<b.pin_class_list.size(); i++){
            auto &pc = b.pin_class_list[i];
            auto &class_inf = block_info.class_inf[i];
            e_pin_type pinmap[] = {OPEN, DRIVER, RECEIVER};
            if(class_inf.type != pinmap[(int)pc.type]){
                vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                          "Architecture file does not match RR graph's block type");
            }
            if (class_inf.num_pins != pc.pin_list.size()) {
                vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                          "Incorrect number of pins in %d pin_class in block %s", i, block_info.name);
            }
            for(auto pin: pc.pin_list){
                auto index = pin_index_by_num(class_inf, pin.ptc);
                if (index < 0) {
                    vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                              "Architecture file does not match RR graph's block pin list: invalid ptc for pin class");
                }
                if (pin.value != block_type_pin_index_to_name(&block_info, class_inf.pinlist[index])) {
                    vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                              "Architecture file does not match RR graph's block pin list");
                }
            }
        }
    }

    /* TODO: timing_list[0]? */
    for(auto &s: rr_graph::rr_graph->segments_list[0].segment_list){
        if(segment_inf[s.id].name != s.name){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "Architecture file does not match RR graph's segment name: arch uses %s, RR graph uses %s", segment_inf[s.id].name.c_str(), s.name.c_str());
        }
        if(s.timing_list.size()){
            if(segment_inf[s.id].Rmetal != s.timing_list[0].R_per_meter){
                vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                          "Architecture file does not match RR graph's segment R_per_meter");
            }
            if(segment_inf[s.id].Cmetal != s.timing_list[0].C_per_meter){
                cout << segment_inf[s.id].Cmetal << "\n";
                cout << s.timing_list[0].C_per_meter << "\n";
                vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                          "Architecture file does not match RR graph's segment C_per_meter");
            }
        }
    }

    VTR_LOG("Starting build routing resource graph...\n");

    /* channels */

    /* TODO: This looks awkward. Fix in the schema. */
    auto &channel = rr_graph::rr_graph->channels_list[0].channel_list[0];

    nodes_per_chan.max = channel.chan_width_max;
    nodes_per_chan.x_min = channel.x_min;
    nodes_per_chan.y_min = channel.y_min;
    nodes_per_chan.x_max = channel.x_max;
    nodes_per_chan.y_max = channel.y_max;

    /* TODO: This too. */
    for(auto &x: rr_graph::rr_graph->channels_list[0].x_list_list){
        if(x.index > nodes_per_chan.x_list.size()){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "index %d on x_list exceeds x_list size %u",
                      x.index, nodes_per_chan.x_list.size());
        }
        nodes_per_chan.x_list[x.index] = x.info;
    }
    for(auto &y: rr_graph::rr_graph->channels_list[0].y_list_list){
        if(y.index > nodes_per_chan.y_list.size()){
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "index %d on y_list exceeds y_list size %u",
                      y.index, nodes_per_chan.y_list.size());
        }
        nodes_per_chan.y_list[y.index] = y.info;
    }

    /* TODO: No connection boxes. Add to the schema. */
    device_ctx.connection_boxes.clear();

    /* Decode the graph_type */
    bool is_global_graph = (GRAPH_GLOBAL == graph_type ? true : false);

    /* Global routing uses a single longwire track */
    int max_chan_width = (is_global_graph ? 1 : nodes_per_chan.max);
    VTR_ASSERT(max_chan_width > 0);

    /* switches */
    auto &switch_list = rr_graph::rr_graph->switches_list[0].switch_list;
    device_ctx.rr_switch_inf.resize(switch_list.size());

    /* TODO: timing_list[0], sizing_list[0]... */
    for(auto &sw: switch_list){
        auto& rr_switch = device_ctx.rr_switch_inf[sw.id];
        bool found_arch_name = false;
        if(!sw.name.empty()){
            for (int i = 0; i < device_ctx.num_arch_switches; ++i) {
                if (sw.name == device_ctx.arch_switch_inf[i].name) {
                    found_arch_name = true;
                    break;
                }
            }
        }
        if (!found_arch_name) {
            VPR_THROW(VPR_ERROR_ROUTE, "Switch name '%s' not found in architecture\n", sw.name.c_str());
        }
        rr_switch.name = strdup(sw.name.c_str());
        SwitchType smap[] = {SwitchType::MUX, SwitchType::TRISTATE, SwitchType::PASS_GATE, SwitchType::SHORT, SwitchType::BUFFER};
        rr_switch.set_type(smap[(int)sw.type]);
        if(sw.timing_list.size()){
            rr_switch.R = sw.timing_list[0].R;
            rr_switch.Cin = sw.timing_list[0].Cin;
            rr_switch.Cout = sw.timing_list[0].Cout;
            rr_switch.Cinternal = sw.timing_list[0].Cinternal;
            rr_switch.Tdel = sw.timing_list[0].Tdel;
        }else{
            rr_switch.R = 0;
            rr_switch.Cin = 0;
            rr_switch.Cout = 0;
            rr_switch.Cinternal = 0;
            rr_switch.Tdel = 0;
        }
        rr_switch.mux_trans_size = sw.sizing_list[0].mux_trans_size;
        rr_switch.buf_size = sw.sizing_list[0].buf_size;
    }

    /* Binary nodes&edges are omitted because they are not relevant here. Also TODO: Fix the schema to include that. */

    /* nodes */
    /* TODO: rr_nodes_list[0] */
    auto &node_list = rr_graph::rr_graph->rr_nodes_list[0].node_list;
    device_ctx.rr_nodes.resize(node_list.size());
    device_ctx.connection_boxes.resize_nodes(node_list.size());

    for(auto &n: node_list){
        auto &node = device_ctx.rr_nodes[n.id];
        t_rr_type node_tmap[] = {CHANX, CHANY, SOURCE, SINK, OPIN, IPIN};
        node.set_type(node_tmap[(int)n.type]);
        /* TODO: add connection_box to node in the schema */
        if (node.type() == CHANX || node.type() == CHANY) {
            e_direction node_dirmap[] = {INC_DIRECTION, DEC_DIRECTION, BI_DIRECTION};
            /* TODO: What's a NO_DIR? Not found in the schema. */
            /* TODO in uxsdcxx: What happens when optional attribute with enum? */
            node.set_direction(node_dirmap[(int)n.direction]);
        }
        node.set_capacity(n.capacity);
        /* TODO: loc_list[0] */
        auto &loc = n.loc_list[0];
        node.set_coordinates(loc.xlow, loc.ylow, loc.xhigh, loc.yhigh);
        node.set_ptc_num(loc.ptc);
        if(node.type() == IPIN || node.type() == OPIN){
            e_side node_sidemap[] = {LEFT, RIGHT, TOP, BOTTOM};
            node.set_side(node_sidemap[(int)loc.side]);
        }
        /* TODO: timing_list[0] */
        auto &timing = n.timing_list[0];
        node.set_rc_index(find_create_rr_rc_data(timing.R, timing.C));
        node.set_num_edges(0);
        /* TODO: metadata_list[0] */
        if(n.metadata_list.size()){
            auto &metadata = n.metadata_list[0];
            for(auto &meta: metadata.meta_list){
                vpr::add_rr_node_metadata(n.id, meta.name, meta.value);
            }
        }
    }

    /* edges */
    /* TODO: rr_edges_list[0] */
    auto &edge_list = rr_graph::rr_graph->rr_edges_list[0].edge_list;

    //count the number of edges and store it in a vector
    vector<int> num_edges_for_node;
    num_edges_for_node.resize(device_ctx.rr_nodes.size(), 0);
    for(auto &e: edge_list){
        if(e.src_node >= device_ctx.rr_nodes.size()) {
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "source_node %d is larger than rr_nodes.size() %d",
                      e.src_node, device_ctx.rr_nodes.size());
        }
        num_edges_for_node[e.src_node]++;
        device_ctx.rr_nodes[e.src_node].set_num_edges(num_edges_for_node[e.src_node]);
    }

    //reset this vector in order to start count for num edges again
    for (size_t inode = 0; inode < device_ctx.rr_nodes.size(); inode++) {
        num_edges_for_node[inode] = 0;
    }

    std::vector<int> count_for_wire_to_ipin_switches;
    count_for_wire_to_ipin_switches.resize(device_ctx.rr_nodes.size(), 0);
    //first is index, second is count
    pair<int, int> most_frequent_switch(-1, 0);

    for(auto &e: edge_list){
        if (e.sink_node >= device_ctx.rr_nodes.size()) {
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "sink_node %d is larger than rr_nodes.size() %d",
                      e.sink_node, device_ctx.rr_nodes.size());
        }
        if (e.switch_id >= device_ctx.rr_nodes.size()) {
            vpr_throw(VPR_ERROR_OTHER, __FILE__, __LINE__,
                      "switch_id %d is larger than num_rr_switches %d",
                      e.switch_id, device_ctx.rr_nodes.size());
        }
        /*Keeps track of the number of the specific type of switch that connects a wire to an ipin
         * use the pair data structure to keep the maximum*/
        if (device_ctx.rr_nodes[e.src_node].type() == CHANX || device_ctx.rr_nodes[e.src_node].type() == CHANY) {
            if (device_ctx.rr_nodes[e.sink_node].type() == IPIN) {
                count_for_wire_to_ipin_switches[e.switch_id]++;
                if (count_for_wire_to_ipin_switches[e.switch_id] > most_frequent_switch.second) {
                    most_frequent_switch.first = e.switch_id;
                    most_frequent_switch.second = count_for_wire_to_ipin_switches[e.switch_id];
                }
            }
        }
        //set edge in correct rr_node data structure
        device_ctx.rr_nodes[e.src_node].set_edge_sink_node(num_edges_for_node[e.src_node], e.sink_node);
        device_ctx.rr_nodes[e.src_node].set_edge_switch(num_edges_for_node[e.src_node], e.switch_id);

        // Read the metadata for the edge
        /* TODO: metadata_list[0] */
        if(e.metadata_list.size()){
            auto &metadata = e.metadata_list[0];
            for(auto &meta: metadata.meta_list){
                vpr::add_rr_edge_metadata(e.src_node, e.sink_node, e.switch_id, meta.name, meta.value);
            }
        }

        num_edges_for_node[e.src_node]++;
    }
    *wire_to_rr_ipin_switch = most_frequent_switch.first;
    num_edges_for_node.clear();
    count_for_wire_to_ipin_switches.clear();

    //Partition the rr graph edges for efficient access to configurable/non-configurable
    //edge subsets. Must be done after RR switches have been allocated
    partition_rr_graph_edges(device_ctx);
    process_rr_node_indices(grid);
    init_fan_in(device_ctx.rr_nodes, device_ctx.rr_nodes.size());

    /* set cost indices */
    for (size_t inode = 0; inode < device_ctx.rr_nodes.size(); inode++) {
        if (device_ctx.rr_nodes[inode].type() == SOURCE) {
            device_ctx.rr_nodes[inode].set_cost_index(SOURCE_COST_INDEX);
        } else if (device_ctx.rr_nodes[inode].type() == SINK) {
            device_ctx.rr_nodes[inode].set_cost_index(SINK_COST_INDEX);
        } else if (device_ctx.rr_nodes[inode].type() == IPIN) {
            device_ctx.rr_nodes[inode].set_cost_index(IPIN_COST_INDEX);
        } else if (device_ctx.rr_nodes[inode].type() == OPIN) {
            device_ctx.rr_nodes[inode].set_cost_index(OPIN_COST_INDEX);
        }
    }
    /*Go through each rr_node and use the segment ids to set CHANX and CHANY cost index*/
    for(auto &n: node_list){
        auto& node = device_ctx.rr_nodes[n.id];
        /*CHANX and CHANY cost index is dependent on the segment id*/
        if(n.segment_list.size()){
            int seg_id = n.segment_list[0].segment_id;
            if (is_global_graph) {
                node.set_cost_index(0);
            } else if (node.type() == CHANX) {
                node.set_cost_index(CHANX_COST_INDEX_START + seg_id);
            } else if (node.type() == CHANY) {
                node.set_cost_index(CHANX_COST_INDEX_START + segment_inf.size() + seg_id);
            }
        }
    }

    alloc_and_load_rr_indexed_data(segment_inf, device_ctx.rr_node_indices,
                                           max_chan_width, *wire_to_rr_ipin_switch, base_cost_type);

    /* process seg id */
    for(auto &n: node_list){
        auto& node = device_ctx.rr_nodes[n.id];

        if(n.segment_list.size()){
            int seg_id = n.segment_list[0].segment_id;
            device_ctx.rr_indexed_data[node.cost_index()].seg_index = seg_id;
        }else{
            //-1 for non chanx or chany nodes
            device_ctx.rr_indexed_data[node.cost_index()].seg_index = -1;
        }
    }

    device_ctx.chan_width = nodes_per_chan;
    check_rr_graph(graph_type, grid, device_ctx.block_types);
    device_ctx.connection_boxes.create_sink_back_ref();
}

/*Allocates and load the rr_node look up table. SINK and SOURCE, IPIN and OPIN
 *share the same look up table. CHANX and CHANY have individual look ups */
void process_rr_node_indices(const DeviceGrid& grid) {
    auto& device_ctx = g_vpr_ctx.mutable_device();

    /* Alloc the lookup table */

    auto& indices = device_ctx.rr_node_indices;

    indices.resize(NUM_RR_TYPES);

    typedef struct max_ptc {
        short chanx_max_ptc = 0;
        short chany_max_ptc = 0;
    } t_max_ptc;

    /*
     * Local multi-dimensional vector to hold max_ptc for every coordinate.
     * It has same height and width as CHANY and CHANX are inverted
     */
    vtr::Matrix<t_max_ptc> coordinates_max_ptc; /* [x][y] */
    size_t max_coord_size = std::max(grid.width(), grid.height());
    coordinates_max_ptc.resize({max_coord_size, max_coord_size}, t_max_ptc());

    /* Alloc the lookup table */
    for (t_rr_type rr_type : RR_TYPES) {
        if (rr_type == CHANX) {
            indices[rr_type].resize(grid.height());
            for (size_t y = 0; y < grid.height(); ++y) {
                indices[rr_type][y].resize(grid.width());
                for (size_t x = 0; x < grid.width(); ++x) {
                    indices[rr_type][y][x].resize(NUM_SIDES);
                }
            }
        } else {
            indices[rr_type].resize(grid.width());
            for (size_t x = 0; x < grid.width(); ++x) {
                indices[rr_type][x].resize(grid.height());
                for (size_t y = 0; y < grid.height(); ++y) {
                    indices[rr_type][x][y].resize(NUM_SIDES);
                }
            }
        }
    }

    /*
     * Add the correct node into the vector
     * For CHANX and CHANY no node is added yet, but the maximum ptc is counted for each
     * x/y location. This is needed later to add the correct node corresponding to CHANX
     * and CHANY.
     *
     * Note that CHANX and CHANY 's x and y are swapped due to the chan and seg convention.
     */
    for (size_t inode = 0; inode < device_ctx.rr_nodes.size(); inode++) {
        auto& node = device_ctx.rr_nodes[inode];
        if (node.type() == SOURCE || node.type() == SINK) {
            for (int ix = node.xlow(); ix <= node.xhigh(); ix++) {
                for (int iy = node.ylow(); iy <= node.yhigh(); iy++) {
                    if (node.type() == SOURCE) {
                        indices[SOURCE][ix][iy][0].push_back(inode);
                        indices[SINK][ix][iy][0].push_back(OPEN);
                    } else {
                        VTR_ASSERT(node.type() == SINK);
                        indices[SINK][ix][iy][0].push_back(inode);
                        indices[SOURCE][ix][iy][0].push_back(OPEN);
                    }
                }
            }
        } else if (node.type() == IPIN || node.type() == OPIN) {
            for (int ix = node.xlow(); ix <= node.xhigh(); ix++) {
                for (int iy = node.ylow(); iy <= node.yhigh(); iy++) {
                    if (node.type() == OPIN) {
                        indices[OPIN][ix][iy][node.side()].push_back(inode);
                        indices[IPIN][ix][iy][node.side()].push_back(OPEN);
                    } else {
                        VTR_ASSERT(node.type() == IPIN);
                        indices[IPIN][ix][iy][node.side()].push_back(inode);
                        indices[OPIN][ix][iy][node.side()].push_back(OPEN);
                    }
                }
            }
        } else if (node.type() == CHANX) {
            for (int ix = node.xlow(); ix <= node.xhigh(); ix++) {
                for (int iy = node.ylow(); iy <= node.yhigh(); iy++) {
                    coordinates_max_ptc[iy][ix].chanx_max_ptc = std::max(coordinates_max_ptc[iy][ix].chanx_max_ptc, node.ptc_num());
                }
            }
        } else if (node.type() == CHANY) {
            for (int ix = node.xlow(); ix <= node.xhigh(); ix++) {
                for (int iy = node.ylow(); iy <= node.yhigh(); iy++) {
                    coordinates_max_ptc[ix][iy].chany_max_ptc = std::max(coordinates_max_ptc[ix][iy].chany_max_ptc, node.ptc_num());
                }
            }
        }
    }

    /* Alloc the lookup table */
    for (t_rr_type rr_type : RR_TYPES) {
        if (rr_type == CHANX) {
            for (size_t y = 0; y < grid.height(); ++y) {
                for (size_t x = 0; x < grid.width(); ++x) {
                    indices[CHANX][y][x][0].resize(coordinates_max_ptc[y][x].chanx_max_ptc + 1, OPEN);
                }
            }
        } else if (rr_type == CHANY) {
            for (size_t x = 0; x < grid.width(); ++x) {
                for (size_t y = 0; y < grid.height(); ++y) {
                    indices[CHANY][x][y][0].resize(coordinates_max_ptc[x][y].chany_max_ptc + 1, OPEN);
                }
            }
        }
    }

    int count;
    /* CHANX and CHANY need to reevaluated with its ptc num as the correct index*/
    for (size_t inode = 0; inode < device_ctx.rr_nodes.size(); inode++) {
        auto& node = device_ctx.rr_nodes[inode];
        if (node.type() == CHANX) {
            for (int iy = node.ylow(); iy <= node.yhigh(); iy++) {
                for (int ix = node.xlow(); ix <= node.xhigh(); ix++) {
                    count = node.ptc_num();
                    if (count >= int(indices[CHANX][iy][ix][0].size())) {
                        VPR_THROW(VPR_ERROR_ROUTE,
                                  "Ptc index %d for CHANX (%d, %d) is out of bounds, size = %zu",
                                  count, ix, iy, indices[CHANX][iy][ix][0].size());
                    }
                    indices[CHANX][iy][ix][0][count] = inode;
                }
            }
        } else if (node.type() == CHANY) {
            for (int ix = node.xlow(); ix <= node.xhigh(); ix++) {
                for (int iy = node.ylow(); iy <= node.yhigh(); iy++) {
                    count = node.ptc_num();
                    if (count >= int(indices[CHANY][ix][iy][0].size())) {
                        VPR_THROW(VPR_ERROR_ROUTE,
                                  "Ptc index %d for CHANY (%d, %d) is out of bounds, size = %zu",
                                  count, ix, iy, indices[CHANY][ix][iy][0].size());
                    }
                    indices[CHANY][ix][iy][0][count] = inode;
                }
            }
        }
    }

    //Copy the SOURCE/SINK nodes to all offset positions for blocks with width > 1 and/or height > 1
    // This ensures that look-ups on non-root locations will still find the correct SOURCE/SINK
    for (size_t x = 0; x < grid.width(); x++) {
        for (size_t y = 0; y < grid.height(); y++) {
            int width_offset = grid[x][y].width_offset;
            int height_offset = grid[x][y].height_offset;
            if (width_offset != 0 || height_offset != 0) {
                int root_x = x - width_offset;
                int root_y = y - height_offset;

                indices[SOURCE][x][y] = indices[SOURCE][root_x][root_y];
                indices[SINK][x][y] = indices[SINK][root_x][root_y];
            }
        }
    }

    device_ctx.rr_node_indices = indices;
}
