#pragma once

#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/redis.h>
#include <brpc/channel.h>
#include <brpc/server.h>
#include <librdkafka/rdkafka.h>
#include "glog/logging.h"
#include "glog/raw_logging.h"
#include "block.hpp"
#include <iostream>


#include "service_config.h"
//#include "person-odb.hxx"
#include "keno.h"
#include "keno_block.h"
#include "utils.h"
#include "get_sign_manager.h"
using namespace std;
extern keno_database g_keno_database;
extern get_sign_manager g_get_sign_manager;
extern string g_seed_hash_transaction_id;
extern string g_previous_seed_hash_transaction_id;

int handle_block(bool isreal, const signed_block& block);
void handler_receivd_transactions(const transfer_op& op);
void handler_payout_transactions(const transfer_op& op);
