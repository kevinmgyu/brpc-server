#pragma once

#include "utils.h"

#include "block_database.hpp"

struct video_poker_seed {
    block_num_type start_block_num;
    block_num_type end_block_num;
    string g_seed;
    string g_seed_hash;
};

extern string g_seed;
extern string g_seed_hash;
extern uint64_t g_seed_change_num;
extern string g_previous_seed_hash;
//extern string g_previous_previous_seed_hash;
extern string g_previous_seed;
//extern string g_previous_previous_seed;


int init_server_seed();

extern vector<video_poker_seed> seeds;
string get_seed(block_num_type block_num);
string get_seed_hash();
string generate_seed();
//void transfer_seed_hash(block_num_type block_num);

 
