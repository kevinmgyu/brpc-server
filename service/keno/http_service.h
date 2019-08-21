#pragma once
#include "service_config.h"
#include "redis_client.h"
#include "http_client.h"
#include "kafka_exe_queue.h"
#include "mysql_exe_queue.h"
#include "mysql_task.h"
//#include "leveldb_client.h"
#include "keno_database.hpp"
#include "ut_sds.h"
#include "keno_config.h"
#include "handle_block.h"
#include <brpc/periodic_task.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include "sqlpp_keno_seed.h"
#include <cassert>
#include <iostream>
#include <vector>
using namespace brpc;
using namespace bytetrade_pb;
class BytetradeServiceImpl : public BytetradeService {
	public:
		BytetradeServiceImpl() {
		};
		virtual ~BytetradeServiceImpl() {};
		virtual void process_keno_list(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::keno_list_request* request,
				::bytetrade_pb::keno_list_response* response,
				::google::protobuf::Closure* done);
		virtual void process_keno_query(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::keno_query_request* request,
				::bytetrade_pb::keno_query_response* response,
				::google::protobuf::Closure* done);
		virtual void get_new_transactions(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::keno_newest_trasaction_request* request,
				::bytetrade_pb::keno_newest_trasaction_response* response,
				::google::protobuf::Closure* done);
		virtual void get_my_transactions(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::keno_my_trasaction_request* request,
				::bytetrade_pb::keno_my_trasaction_response* response,
				::google::protobuf::Closure* done);

		virtual void get_my_transactions_use_sqlpp(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::keno_my_trasaction_request* request,
				::bytetrade_pb::keno_my_trasaction_response* response,
				::google::protobuf::Closure* done);
		virtual void get_block_seed(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::keno_query_block_seed_request* request,
				::bytetrade_pb::keno_query_block_seed_response* response,
				::google::protobuf::Closure* done);

		virtual void get_block_seed_list(::google::protobuf::RpcController* controller,
				const ::bytetrade_pb::keno_query_block_seed_list_request* request,
				::bytetrade_pb::keno_query_block_seed_list_response* response,
				::google::protobuf::Closure* done);


		virtual void Echo2(google::protobuf::RpcController* cntl_base,
				const EchoRequest* request,
				EchoResponse* response,
				google::protobuf::Closure* done) ;
		virtual void get_block_seed_use_sqlpp11(::google::protobuf::RpcController* controller,
                        const ::bytetrade_pb::keno_query_block_seed_request* request,
                        ::bytetrade_pb::keno_query_block_seed_response* response,
                        ::google::protobuf::Closure* done);
	private:
		static uint64_t  g_a;
};
extern brpc::Channel g_channel_client;

