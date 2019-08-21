#pragma once
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/redis.h>
#include <brpc/channel.h>
#include <brpc/server.h>
#include <librdkafka/rdkafka.h>
#include "glog/logging.h"
#include "glog/raw_logging.h"

using namespace std;
int http_client_init(const string server, const string load_balancer,brpc::Channel& g_channel_client);
//extern bytetrade::BytetradeService_Stub g_stub;
