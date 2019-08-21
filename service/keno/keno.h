#pragma once
#include <bthread/execution_queue.h>
#include <bthread/sys_futex.h>
#include <bthread/countdown_event.h>
#include <butil/time.h>
#include <butil/fast_rand.h>
#include <iostream>
#include <string>
#include <fstream>
#include <bthread/execution_queue.h>
#include <bthread/sys_futex.h>
#include <bthread/countdown_event.h>
#include <butil/time.h>
#include <butil/fast_rand.h>
#include <iostream>
#include <string>
#include <fstream>
#include "block.hpp"
#include "keno_seed.h"
#include "keno_config.h"
#include "keno_base.h"
#include "keno_database.hpp"
#include "keno_block.h"
#include "service.pb.h"
#include "service_config.h"
#include "redis_client.h"
#include "http_client.h"
#include "kafka_exe_queue.h"
#include "mysql_task.h"
//#include "leveldb_client.h"
#include "jsonxx.h"
#include "handle_block.h"
#include "service_config.h"
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include "sqlpp_keno_seed.h"
#include "sqlpp_keno.h"
#include <cassert>
#include <iostream>
#include <vector>
using namespace brpc_sqlpp;
using namespace std;
#define KENO_REC_BLOCK 0
#define KENO_PAYOUT_BLOCK 1


int init_keno(std::function< void (const transfer_op& op) > recfunc,std::function< void (const transfer_op& op) > payoutfunc);
int append_game_received(const bytetrade_pb::keno_game& game);
int append_game_payout(const bytetrade_pb::keno_game& game);
int append_game_seed(double t, uint64_t block_num,string id );
int append_game_received_use_sqlpp(const bytetrade_pb::keno_game& game);
int append_game_payout_use_sqlpp(const bytetrade_pb::keno_game& game);
int append_game_seed_use_sqlpp(double t, uint64_t block_num,string id );

int append_game_seed_publish(double t, uint64_t block_num,string id,string id2);
int append_game_seed_publish_use_sqlpp11(double t, uint64_t block_num,string id,string id2);
int push_game_message (bool isreal, const bytetrade_pb::keno_game& game);
int fini_keno();


