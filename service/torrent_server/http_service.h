#pragma once
#include "service_config.h"
#include "redis_client.h"
#include "http_client.h"
#include "kafka_exe_queue.h"
#include "mysql_exe_queue.h"
#include "mysql_task.h"
//#include "leveldb_client.h"
#include "ts_database.hpp"
#include "ut_sds.h"
#include "ts_config.h"
#include "handle_block.h"
#include <brpc/periodic_task.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <cassert>
#include <iostream>
#include <vector>
using namespace std;
using namespace brpc;
using namespace bytetrade_pb;
class BytetradeServiceImpl : public BytetradeService {
	public:
		BytetradeServiceImpl() {
		};
		virtual ~BytetradeServiceImpl() {};
		virtual	void get_create_history(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::get_create_torrent_request* request,
				::bytetrade_pb::get_create_torrent_response* response,
				::google::protobuf::Closure* done);
		virtual void get_payment_history(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::get_torrent_payment_request* request,
				::bytetrade_pb::get_torrent_payment_response* response,
				::google::protobuf::Closure* done);
		virtual void get_torrent(google::protobuf::RpcController*, const bytetrade_pb::get_torrent_realtime_request*, bytetrade_pb::get_torrent_realtime_response*, google::protobuf::Closure*);
		virtual  void get_torrent_payment(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::get_torrent_payment_realtime_request* request,
				::bytetrade_pb::get_torrent_payment_realtime_response* response,
				::google::protobuf::Closure* done);
	//	void HandleUserResponse(brpc::Controller* cntl,::bytetrade_pb::get_torrent_payment_realtime_response* response,bytetrade_pb::torrent_payment o);
	void get_account(::google::protobuf::RpcController* controller,
                const ::bytetrade_pb::get_account_request* request,
                ::bytetrade_pb::get_account_response* response,
                ::google::protobuf::Closure* done);
	private:
		static uint64_t  g_a;
};
extern brpc::Channel g_channel_client;

