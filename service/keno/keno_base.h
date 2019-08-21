#pragma once

#include "service.pb.h"
#include "operations.hpp"
#include "service_config.h"

#define DICE_BONUS_BASE 1000
#define DICE_BONUS_RATE 15

enum {
    GAME_RECEIVED_FAILED,
    GAME_RECEIVED,
    GAME_PAYOUT_FAILED,
    GAME_PAYOUT_AMOUNT_FAILED,
    GAME_FINISHED    
};

struct transfer_op {

    bool isvalid;
    share_type fee;
    account_id_type from;
    account_id_type to;
    asset_id_type asset_type;
    share_type amount;
    
    optional<string> message;
    string transaction_id;
    optional<string> proposal_transaction_id;
    string block_id;
    uint64_t block_num;
    int reason;
    int transaction_in_block;
    string block_hash;
    string dapp;

    transfer_op() {
        isvalid = false;
    }

    transfer_op(transfer_operation o) {
        isvalid = true;
        fee = o.fee;
        from = o.from;
        to = o.to;
        asset_type = o.asset_type;
        amount = o.amount;
        
       
    }

    transfer_op(transfer2_operation o) {
        isvalid = true;
        fee = o.fee;
        from = o.from;
        to = o.to;
        asset_type = o.asset_type;
        amount = o.amount;
        message = o.message;

    }

    transfer_op(const transfer_op& o) {
        isvalid = o.isvalid;
        fee = o.fee;
        from = o.from;
        to = o.to;
        asset_type = o.asset_type;
        amount = o.amount;
        message = o.message;
        transaction_id = o.transaction_id;
        block_id = o.block_id;
        block_num = o.block_num;
        reason = o.reason;
        transaction_in_block = o.transaction_in_block;
        proposal_transaction_id = o.proposal_transaction_id;
        block_hash = o.block_hash;
        dapp = o.dapp;
    }

};
 
struct bonus_adr {
    
    string adr;
    int base;
	int rate;
    bonus_adr();

    bonus_adr(const bonus_adr &g);
};


struct keno_area {
    string address;
  //  string id;
    
    //uint32_t asset_type;
  //  string   asset_name;
    // int      chain_type;
    // string   chain_contract_address;
  //  share_type precision;
    //share_type transfer_amount;
   
    string     channel;
    string     contract_id;
    map<int, bonus_adr> bonus_adrs;
    std::map<asset_id_type,std::unordered_map<std::string,std::string>> area;
    std::vector<asset_id_type> area_list;
    //map<int, room> rooms;
    //vector<int> rooms_bet;

    keno_area();

    keno_area(const keno_area &g);

};


 
/*struct keno_game {
  
    string received_transaction_id;
    double received_time;
    block_num_type received_block_num;


    
    string payout_transaction_id;
    double payout_time;
    block_num_type payout_block_num;    
     
    string block_hash;
    string server_seed_hash;
   
    string shoe;
    string pick;
    string hit;

    string room_address;
    string user_id;

    uint32_t asset_type;
    string   asset_name;
    share_type amount;
    int status;
    bool win_or_lose;

    string payout_user_id;
    int hand_type;
    int pay_ratio;
    bytetrade::uint128 pay_out;

    string external;
    string channel;
    uint32_t bet_multiple;
    share_type basic_amount;

    keno_game() {
        received_transaction_id = "";
        received_time = 0;
        received_block_num = 0;

 
        payout_transaction_id = "";
        payout_time = 0;
        payout_block_num = 0;
     
        block_hash = "";
        server_seed_hash = "";

        shoe = "";
        pick = "";
        hit = "";

        room_address = "";
        user_id = "";

        asset_type = 0;
        asset_name = "";
        amount = 0;
        //basic_amount = 0;
	//bet_multiple = 0;
        status = 0;
        win_or_lose = false;

        payout_user_id = "";
        hand_type = 100;
        pay_ratio  = 0;
        pay_out = 0;

        external  = "";
        channel = "";
    }

    keno_game(const keno_game &g) {
        received_transaction_id = g.received_transaction_id;
        received_time = g.received_time;
        received_block_num = g.received_block_num;


    

        payout_transaction_id = g.payout_transaction_id;
        payout_time = g.payout_time;
        payout_block_num = g.payout_block_num;
     
        block_hash = g.block_hash;
        server_seed_hash = g.server_seed_hash;
        
        shoe =  g.shoe;
        pick = g.pick;
        hit = g.hit;

        room_address = g.room_address;
        user_id = g.user_id;

        asset_type = g.asset_type;
        asset_name = g.asset_name;
        amount = g.amount;
        //basic_amount = g.basic_amount;
	//bet_multiple = g.bet_multiple;
        status = g.status;
        win_or_lose= g.win_or_lose;

        payout_user_id = g.payout_user_id;
        hand_type = g.hand_type;
        pay_ratio  = g.pay_ratio;
        pay_out = g.pay_out;

        external = g.external;
        channel = g.channel;
    }
};*/


struct simple_asset {
    int id;
    string symbol;
   // int prec;
    //double price;
    //string market;
    string chain_contract_address;
    int chain_type;
    uint64_t base_precision;
    uint64_t min_transfer_amount;

    simple_asset() {

    }

    simple_asset(const simple_asset& a) {
        id = a.id;
        symbol = a.symbol;
       // prec = a.prec;
        //price = a.price;
        //market = a.market;
        base_precision = a.base_precision;
        min_transfer_amount = a.min_transfer_amount;
        chain_contract_address = a.chain_contract_address;
        chain_type = a.chain_type;
    }
};

bool is_digit(string message);
bytetrade_pb::keno_game get_game_by_transfer_op_and_room(  const transfer_op& op, const keno_area& area);
//optional<keno_game> game_hold(const string& block_hash, const keno_game& g, const transfer_op& op);
optional<bytetrade_pb::keno_game> game_merge(const bytetrade_pb::keno_game& g, const transfer_op& op);
 
//FC_REFLECT(keno_game, (received_transaction_id)(received_time)(received_block_num)(payout_transaction_id)(payout_time)(payout_block_num)(block_hash)(server_seed_hash)(shoe)(pick)(hit)(room_address)(user_id)(asset_type)(asset_name)(amount)(status)(win_or_lose)(payout_user_id)(hand_type)(pay_ratio)(pay_out)(external)(channel))

 
