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
#include <stdlib.h>
#include <fstream>
#include "block.hpp"
#include "ts_config.h"
#include "ts_database.hpp"
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
#include <cassert>
#include <iostream>
#include <vector>
#include <sys/stat.h>
//using namespace brpc_sqlpp;
using namespace std;


using namespace bytetrade_pb;
int init_torrent();
int fini_torrent();
int append_torrent_create(const bytetrade_pb::torrent &b);
int append_torrent_payment(const bytetrade_pb::torrent_payment &b);
extern brpc::Channel g_user_channel_client;




