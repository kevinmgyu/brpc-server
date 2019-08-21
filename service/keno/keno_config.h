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
#include "ut_cli.h"
#include "ut_misc.h"
#include "ut_list.h"
//#include "ut_mysql.h"
#include "ut_signal.h"
#include "ut_define.h"
//#include "ut_config.h"
#include "ut_decimal.h"
//#include "ut_rpc_clt.h"
//#include "ut_rpc_svr.h"
#include "ut_rpc_cmd.h"
#include "ut_skiplist.h"
#include <set>
#include <fc/io/json.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/exception/exception.hpp>
#include <fc/crypto/hex.hpp>
#include "uint128.hpp"
#include "keno_base.h"

using std::string;

struct area_inner {
	int asset_type;
	vector<share_type> transfer_amounts;
};

struct bonus_adr_inner{
	char * adr;
	int  base;
	int  rate;
};


struct room_inner {
	char *address;
 
	char *contract_id;
	char * channel;

	char *bonus_adr;
	size_t bonus_num;
	struct bonus_adr_inner *bonus;
	
	size_t area_num;
	struct area_inner *areas;
};

struct keno_settings {
	// char *user;
	// char *private_key;

	 bool                debug;
   // process_cfg         process;
    //log_cfg             log;
    //alert_cfg           alert;
   // rpc_svr_cfg         svr;
     //rpc_svr_cfg         svr;
	 

	//mysql_cfg db_history;
	int history_thread;

	char *http_url;
	char *transfer_seed_from;
	//char *seed_id;
	int   seed_change_num;
	char *transfer_seed_to;
	char *sign_ip;
	char *sign_port;
	int  sign_fd_cnt;
	int  sign_timeout;
	

	size_t room_num;
	struct room_inner *rooms;

	//bool  use_message;
	char                *brokers;
};

extern struct  keno_settings keno_settings;


extern std::map<std::string, keno_area> bets;
extern std::map<uint32_t, simple_asset> assets;

int init_keno_rooms_config(const char *path);
 
