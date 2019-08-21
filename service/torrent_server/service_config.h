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
#include "service.pb.h"
#include <librdkafka/rdkafka.h>
#include "glog/logging.h"
#include "glog/raw_logging.h"
/*DECLARE_string(protocol);
DECLARE_string(connection_type);*/
//DECLARE_string(server);
DECLARE_string(fullnode_server);
DECLARE_string(user_server);
DECLARE_string(load_balancer);
DECLARE_string(kafka_brokers);
DECLARE_string(kafka_topic);
DECLARE_string(redis_address);
DECLARE_string(mysql_host);
DECLARE_string(mysql_user);
DECLARE_string(mysql_password);
DECLARE_string(mysql_name);
DECLARE_string(mysql_charset);
DECLARE_string(full_node_path);
DECLARE_string(user_node_url);
DECLARE_string(full_node_url);
DECLARE_string(http_url);
DECLARE_int32(mysql_port);
DECLARE_int32(mysql_exe_time);
DECLARE_int32(get_block_time);
//DECLARE_int32(timeout_ms);
DECLARE_int32(port);
DECLARE_uint64(start_num);
DECLARE_uint64(chain_type);

/*DECLARE_int32(idle_timeout_s);
DECLARE_int32(logoff_ms);
DECLARE_int32(max_retry);
DECLARE_bool(echo_attachment);*/
