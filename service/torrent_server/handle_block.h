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

#include "chain_api.h"
#include "torrent.h"
#include "ts_config.h"
#include "bmath.hpp"
#include "torrent.h"
#include "service_config.h"
#include "ts_database.hpp"
//#include "person-odb.hxx"

#include "utils.h"
using namespace std;

extern btp::torrent_database g_torrent_database;
extern account_database g_account_database;
int handle_block(bool isreal, const signed_block& block);
