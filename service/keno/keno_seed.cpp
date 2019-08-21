# include "keno_seed.h"
# include "chain_api.h"
# include "keno_config.h"
# include "ut_rand.h"

string g_seed;
string g_previous_seed;
//string g_previous_previous_seed;
string g_previous_seed_hash;
//string g_previous_previous_seed_hash;
string g_seed_hash;
uint64_t g_seed_change_num;
// map<block_num_type, string> g_seed;
// map<block_num_type, string> g_seed;

vector<video_poker_seed> seeds;
//string current_seed_hash;


int init_server_seed() {
	g_seed = "";
	g_previous_seed = ""; 
	//g_previous_previous_seed = "";
	g_previous_seed_hash = ""; 
	//g_previous_previous_seed_hash = "";
	g_seed_hash = "";
	g_seed_change_num = 0;
	return 0;
}

//
string get_seed() {
	return g_seed;
}

//
string get_seed_hash() {
	return bytetrade::get_sha256(g_seed);
	// for( size_t i = 0; i < seeds.size(); ++i ) {
	//     if( block_num >= seeds[i].start_block_num && block_num < seeds[i].end_block_num ) {
	//         return seeds[i].g_seed_hash;
	//     }
	// }
}

string generate_seed()
{
   
	char seed[256];
	if( rand_bytes(seed,256) == false) {
		assert(0);
	}

	// string  result ="";
    //     for(int i = 0; i < 40; i++) {
    //             /* rand() generate random number */
    //           result += charList [rand() % charList.size()];
    //     }
	string result = fc::to_hex(seed,256);

	//log_trace("generate_seed seed %s", result.c_str());

	return result;

}
 
