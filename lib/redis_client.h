#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/redis.h>
#include <brpc/channel.h>
#include <brpc/server.h>
#include <librdkafka/rdkafka.h>
#include "glog/logging.h"
#include "glog/raw_logging.h"
using namespace std;
using namespace brpc;
int redis_client_init(std::string address);
extern brpc::Channel g_redis_channel;
