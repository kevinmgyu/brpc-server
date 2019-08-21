#pragma once

#include <math.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <map>
#include <inttypes.h>

#include "ut_sds.h"
#include "ut_misc.h"
#include "ut_list.h"
#include "ut_define.h"
#include "ut_decimal.h"
#include "ut_rpc_cmd.h"
#include "ut_skiplist.h"
#include <set>
#include <fc/io/json.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/exception/exception.hpp>
#include <fc/crypto/hex.hpp>
#include "uint128.hpp"
# include "chain_api.h"
# include "bmath.hpp"
 

using std::string;

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
   



extern std::map<int, simple_asset> assets;
extern optional<simple_asset> get_asset_by_id(int id);
extern int init_asset();

 


 
