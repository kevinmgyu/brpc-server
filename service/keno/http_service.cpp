#include "http_service.h"
#include <brpc/periodic_task.h>
/*#include <odb/database.hxx>
#include <odb/transaction.hxx>


#include <odb/mysql/database.hxx>

#include "person-odb.hxx"
using namespace odb::core;*/
using namespace std;
using namespace brpc_sqlpp;
using namespace bytetrade_pb;
double current_timestamp(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double) tv.tv_sec + tv.tv_usec / 1000000.0;
}
uint64_t BytetradeServiceImpl::g_a = 0;
brpc::Channel g_channel_client;

void BytetradeServiceImpl::get_block_seed_use_sqlpp11(::google::protobuf::RpcController* controller,
		const ::bytetrade_pb::keno_query_block_seed_request* request,
		::bytetrade_pb::keno_query_block_seed_response* response,
		::google::protobuf::Closure* done){
	brpc::ClosureGuard done_guard(done);

	/*brpc::Controller* cntl =
	  static_cast<brpc::Controller*>(controller);*/
	if (request->params_size() != 1)
	{
		LOG(ERROR)<< "request params size is not 1";
		response->set_code(1);
		return ;
	}
	uint64_t block_num  = request->params(0);
	/*sds sql = sdsempty();

	  sql = sdscatprintf(sql, "SELECT server_seed_hash,seed, `seed_hash_transaction`,`seed_transaction` from keno_seed_publish WHERE `start_block_num` <= '%lu' and `end_block_num` >= '%lu'",block_num,block_num);
	 */


//	sqlpp::mysql::global_library_init();


	auto config = std::make_shared<sqlpp::mysql::connection_config>();
	config->user = FLAGS_mysql_user;
	config->host = FLAGS_mysql_host;
	config->database = FLAGS_mysql_name;
	config->password =FLAGS_mysql_password;
	config->debug = true;
	int cnt = 0;
	try
	{
		sqlpp::mysql::connection db(config);
		const auto keno_seed = KenoSeedPublish{};
		for (const auto& row :db(select(keno_seed.serverSeedHash,keno_seed.seed,keno_seed.seedHashTransaction,keno_seed.seedTransaction).from(keno_seed).where(keno_seed.startBlockNum <= block_num and keno_seed.endBlockNum >=block_num)))
		{
			bytetrade_pb::keno_query_block_seed_response_result* tmp_result = response->mutable_result();
			tmp_result->set_server_seed_hash(row.serverSeedHash);
			tmp_result->set_seed(row.seed);
			tmp_result->set_seed_hash_transaction(row.seedHashTransaction);
			tmp_result->set_seed_transaction(row.seedTransaction);
			response->set_code(0);

			LOG(INFO)  << "serverSeedHash: " << row.serverSeedHash << ", seed: " << row.seed
				<< ", seedHashTransaction: " <<row.seedHashTransaction ;
			cnt = cnt + 1;
		};
		if (cnt == 0)
		{
			response->set_code(2);
		}
	}
	catch (const std::exception& e)
	{
		LOG(ERROR) << "Exception: " << e.what() << std::endl;
		response->set_code(3);
		return ;
	}


}
void BytetradeServiceImpl::process_keno_list(::google::protobuf::RpcController* controller,
		const ::bytetrade_pb::keno_list_request* request,
		::bytetrade_pb::keno_list_response* response,
		::google::protobuf::Closure* done){
	// This object helps you to call done->Run() in RAII style. If you need
	// to process the request asynchronously, pass done_guard.release().
	brpc::ClosureGuard done_guard(done);

	//brpc::Controller* cntl =
	//static_cast<brpc::Controller*>(controller);
	bytetrade_pb::keno_list_response_result* tmp_result = response->mutable_result();
	for (size_t i = 0; i < keno_settings.room_num; ++i)
	{
		bytetrade_pb::room* pbroom = tmp_result->add_rooms();

		keno_area ar = bets[keno_settings.rooms[i].address];

		pbroom->set_address(ar.address);
		pbroom->set_channel(ar.channel);
		pbroom->set_contract_id(ar.contract_id);


		for( size_t i = 0; i < ar.area_list.size(); ++i ) {
			//map<asset_id_type,std::unordered_map<std::string,std::string>>::iterator iter;
			//for(iter = ar.area.begin(); iter != ar.area.end(); iter++) {
			map<asset_id_type,std::unordered_map<std::string,std::string>>::iterator iter = ar.area.find(ar.area_list[i]);

			bytetrade_pb::area* pbarea = pbroom->add_areas();
			pbarea->set_asset_type(iter->first);

			auto asset_info = assets.find(iter->first);
			if(asset_info != assets.end())
			{
				pbarea->set_asset_name(assets[iter->first].symbol);
				pbarea->set_precision(to_string(share_type(assets[iter->first].base_precision)));
			}
			std::unordered_map<std::string,std::string>::iterator iter1;
			for(iter1=iter->second.begin();iter1!=iter->second.end();iter1++)
			{
				pbarea->add_transfer_amount(iter1->second);
			}

			//LOG(ERROR)<<"transfer_amount_size:"<<pbarea->transfer_amount_size();
		}
		//response = &response_obj;	

		}
		response->set_code(0);
		/*if(!request->has_user_id())
		  {
		  response->set_result(1);
		  response->set_message("invalid input paramter");
		  return ;
		  }*/
		/*string body;
		  response->SerializeToString(&body);
		//LOG(ERROR)<<"rooms_size:"<<response->rooms_size();
		//LOG(ERROR)<<"transfer_amount_size:"<<response->transfer_amount_size();
		LOG(INFO) << body.c_str();*/
	}

	void BytetradeServiceImpl::process_keno_query(::google::protobuf::RpcController* controller,
			const ::bytetrade_pb::keno_query_request* request,
			::bytetrade_pb::keno_query_response* response,
			::google::protobuf::Closure* done){
		// This object helps you to call done->Run() in RAII style. If you need
		// to process the request asynchronously, pass done_guard.release().
		brpc::ClosureGuard done_guard(done);

		/*brpc::Controller* cntl =
		  static_cast<brpc::Controller*>(controller);
		  LOG(INFO)<<"enter"; */
		string received_id = request->params(0);
		if (received_id.size() == 0) {
			LOG(ERROR)<<"received_id == NULL";
			response->set_code(1);
			return;
		}
		optional<bytetrade_pb::keno_game> g1;
		g1 = g_keno_database.fetch_by_id(received_id);
		if( !g1 ) {
			LOG(ERROR)<<"received_id do not find in g_keno_database";
			response->set_code(2);
			return;
		} else {
			bytetrade_pb::keno_query_response_result* tmp_result = response->mutable_result();
			response->set_code(0);
			tmp_result->set_received_transaction_id(g1->received_transaction_id());
			tmp_result->set_received_time(g1->received_time());
			tmp_result->set_shoe(g1->shoe());
			tmp_result->set_pick(g1->pick());
			tmp_result->set_hit(g1->hit());
			tmp_result->set_room_address(g1->room_address());
			tmp_result->set_user_id(g1->user_id());
			tmp_result->set_asset_type(g1->asset_type());
			tmp_result->set_amount(g1->amount());
			tmp_result->set_status(g1->status());
			tmp_result->set_win_or_lose(g1->win_or_lose());
			tmp_result->set_pay_ratio(g1->pay_ratio());
			tmp_result->set_pay_out(g1->pay_out());

		}
		/*string body;
		  response->SerializeToString(&body);
		//LOG(ERROR)<<"transfer_amount_size:"<<response->transfer_amount_size();
		LOG(INFO) << body.c_str();*/
	}

	void BytetradeServiceImpl::get_new_transactions(::google::protobuf::RpcController* controller,
			const ::bytetrade_pb::keno_newest_trasaction_request* request,
			::bytetrade_pb::keno_newest_trasaction_response* response,
			::google::protobuf::Closure* done){
		// This object helps you to call done->Run() in RAII style. If you need
		// to process the request asynchronously, pass done_guard.release().
		brpc::ClosureGuard done_guard(done);

		/*brpc::Controller* cntl =
		  static_cast<brpc::Controller*>(controller);*/
		if (request->params_size() !=  2 && request->params_size() != 3)
		{
			LOG(ERROR)<< "request params size is not 2 or 3";
			response->set_code(1);
			return ;
		}	
		size_t offset  = request->params(0);
		size_t limit = request->params(1);
		if (limit<=0)
		{
			LOG(ERROR)<<"limt<=0";
			response->set_code(3);
			return;
		}
		size_t asset_type=0;
		if(request->params_size()==3)
		{
			asset_type = request->params(2);
			if(asset_type<=0)
			{
				LOG(ERROR)<<"asset_type<=0";
				response->set_code(4);
				return;
			}	
		}

		sds sql = sdsempty();
		sql = sdscatprintf(sql, "SELECT LOWER(HEX(`received_transaction_id`)),received_time,received_block_num,LOWER(HEX(`payout_transaction_id`)),payout_time,payout_block_num,LOWER(HEX(`block_hash`)),server_seed_hash,shoe,pick,hit,room_address,user_id,asset_type,asset_name,amount,status,win_or_lose,payout_user_id,pay_ratio,pay_out,external,channel from received where status > 0");

		if (asset_type > 0) {
			//  char _asset[2 * asset_len + 1];
			//        mysql_real_escape_string(conn, _asset, asset, strlen(asset));
			sql = sdscatprintf(sql, " AND `asset_type` = %lu", asset_type);
		}

		//    sql = sdscatprintf(sql, " ORDER BY `id` DESC");
		sql = sdscatprintf(sql, " ORDER BY `received_time` DESC");
		if (offset) {
			sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
		} else {
			sql = sdscatprintf(sql, " LIMIT %zu", limit);
		}

		LOG(INFO)<<"exec sql: "<<sql;
		MYSQL *conn = NULL;
		int flag = 0;
		conn = get_mysql_conn(flag,FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
		if (NULL == conn)
		{
			sdsfree(sql);
			response->set_code(6);
			return;
		}


		int ret = mysql_real_query(conn, sql, sdslen(sql));
		if (ret != 0) {
			LOG(ERROR)<<"exec sql: "<<sql<<" fail: "<<mysql_errno(conn)<<" "<<mysql_error(conn);
			sdsfree(sql);
			response->set_code(5);
			return;
		}
		sdsfree(sql);

		MYSQL_RES *result = mysql_store_result(conn);
		size_t num_rows = mysql_num_rows(result);
		for (size_t i = 0; i < num_rows; ++i) {
			bytetrade_pb::keno_newest_trasaction_response_result* pbresult = response->add_result();
			MYSQL_ROW row = mysql_fetch_row(result);

			pbresult->set_received_transaction_id(row[0]);
			double timestamp = strtod(row[1], NULL);
			pbresult->set_received_time(timestamp);
			uint32_t received_block_num = atoi(row[2]);
			pbresult->set_received_block_num(received_block_num);
			pbresult->set_block_hash(row[6]);
			pbresult->set_server_seed_hash(row[7]);
			pbresult->set_shoe(row[8]);
			pbresult->set_pick(row[9]);
			pbresult->set_hit(row[10]);



			pbresult->set_room_address(row[11]);
			pbresult->set_user_id(row[12]);

			uint32_t asset_type = atoi(row[13]);
			pbresult->set_asset_type(asset_type);
			pbresult->set_amount(row[15]);

			uint32_t status = atoi(row[16]);
			pbresult->set_status(status);
			uint32_t win_or_lose = atoi(row[17]);
			pbresult->set_win_or_lose(win_or_lose);


			if(win_or_lose == 1)
			{
				pbresult->set_payout_transaction_id(row[3]);
				double payout_time = strtod(row[4], NULL);
				pbresult->set_payout_time(payout_time);
				uint32_t payout_block_num = atoi(row[5]);
				pbresult->set_payout_block_num(payout_block_num);
				pbresult->set_payout_user_id(row[18]);
				uint32_t pay_ratio = atoi(row[19]);
				pbresult->set_pay_ratio(pay_ratio);
				pbresult->set_pay_out(row[20]);
			}

		}
		mysql_free_result(result);
		response->set_code(0);
		/*string body;
		  response->SerializeToString(&body);
		//LOG(ERROR)<<"transfer_amount_size:"<<response->transfer_amount_size();
		LOG(INFO) << body.c_str();*/

		release_mysql_conn(conn,flag);

	}
	void BytetradeServiceImpl::Echo2(google::protobuf::RpcController* cntl_base,
			const EchoRequest* request,
			EchoResponse* response,
			google::protobuf::Closure* done) {
		// This object helps you to call done->Run() in RAII style. If you need
		// to process the request asynchronously, pass done_guard.release().
		brpc::ClosureGuard done_guard(done);

		/*brpc::Controller* cntl =
		  static_cast<brpc::Controller*>(cntl_base);*/
		LOG(INFO) << "echo2 finish";
		/*try
		  {
		  auto_ptr<database> db ;

		  db.reset(new odb::mysql::database ("root", "abcd1234", "odb_test"));
		  unsigned long john_id, joe_id;

		  {
		  person john ("John", "Doe", 33);
		  person jane ("Jane", "Doe", 32);
		  person joe ("Joe", "Dirt", 30);

		  odb::transaction t (db->begin ());

		  john_id = db->persist (john);
		  db->persist (jane);
		  joe_id = db->persist (joe);

		  t.commit ();
		  }

		  typedef odb::query<person> query;
		  typedef odb::result<person> result;

		  {
		  odb::transaction t (db->begin ());

		  result r (db->query<person> (query::age > 30));*/

		/*for (result::iterator i (r.begin ()); i != r.end (); ++i)
		  {
		  cout << "Hello, " << i->first () << " " << i->last () << "!" << endl;
		  }*/

		/*t.commit ();
		  }

		  {
		  odb::transaction t (db->begin ());

		  auto_ptr<person> joe (db->load<person> (joe_id));
		  joe->age (joe->age () + 1);
		  db->update (*joe);

		  t.commit ();
		  }


		  {
		  odb::transaction t (db->begin ());

		  person_stat ps (db->query_value<person_stat> ());

		  << "count  : " << ps.count << endl
		  << "min age: " << ps.min_age << endl
		  << "max age: " << ps.max_age << endl;*/

		/*		t.commit ();
				}

				{
				odb::transaction t (db->begin ());
				db->erase<person> (john_id);
				t.commit ();
				}
				}
				catch (const odb::exception& e)
				{
				cerr << e.what () << endl;
				return ;
				}*/
		response->set_message(request->message());

	}

	void BytetradeServiceImpl::get_my_transactions_use_sqlpp(::google::protobuf::RpcController* controller,const ::bytetrade_pb::keno_my_trasaction_request* request,::bytetrade_pb::keno_my_trasaction_response* response,::google::protobuf::Closure* done)
	{
		// This object helps you to call done->Run() in RAII style. If you need
		// to process the request asynchronously, pass done_guard.release().
		brpc::ClosureGuard done_guard(done);

		/*brpc::Controller* cntl =
		  static_cast<brpc::Controller*>(controller);*/
		if (request->params_size() != 4)
		{
			LOG(ERROR)<< "request params size is not 4";
			response->set_code(1);
			return ;
		}	
		string user_id  = request->params(0).user_id();
		if(user_id.size()==0)
		{
			LOG(ERROR)<<"user_id.size()==0";
			response->set_code(2);
			return;
		}
		int asset_type  = request->params(1).asset_type();
		if (asset_type <=0)
		{
			LOG(ERROR)<<"asset_type<=0";
			response->set_code(3);
			return;
		}	
		size_t offset  = request->params(2).offset();
		size_t limit  = request->params(3).limit();
		if (limit<=0)
		{
			LOG(ERROR)<<"limt<=0";
			response->set_code(4);
			return;
		}

		string method  = request->method();
		if(method != "keno.my_transactions")
		{
			LOG(ERROR)<<method.c_str();
			response->set_code(11);
			return;
		}

		auto config = std::make_shared<sqlpp::mysql::connection_config>();
		config->user = FLAGS_mysql_user;
		config->host = FLAGS_mysql_host;
		config->database = FLAGS_mysql_name;
		config->password =FLAGS_mysql_password;
		config->debug = true;
		//int cnt = 0;
		
		try
		{
			sqlpp::mysql::connection db(config);
			const auto keno_Received = Received{};
			for (const auto& row :db(select(keno_Received.receivedTransactionId,keno_Received.receivedTime,keno_Received.receivedBlockNum,
			keno_Received.payoutTransactionId,keno_Received.payoutTime,keno_Received.payoutBlockNum).from(keno_Received).where(keno_Received.userId == user_id and keno_Received.status > 0 and keno_Received.assetType == asset_type ).order_by(keno_Received.receivedTime.desc()).limit(limit).offset(offset)))
			{
				
				bytetrade_pb::keno_my_trasaction_response_result* pbresult = response->add_result();

				pbresult->set_received_transaction_id(row.receivedTransactionId);
				pbresult->set_received_time(row.receivedTime);
				pbresult->set_received_block_num(row.receivedBlockNum);
				pbresult->set_payout_transaction_id(row.payoutTransactionId);
				pbresult->set_payout_time(row.payoutTime);
				pbresult->set_payout_block_num(row.payoutBlockNum);
				response->set_code(0);
			};
		}
		catch (const std::exception& e)
		{
			LOG(ERROR) << "Exception: " << e.what() << std::endl;
			response->set_code(2);
			return ;
		}
		
	}
	//}
	void BytetradeServiceImpl::get_my_transactions(::google::protobuf::RpcController* controller,const ::bytetrade_pb::keno_my_trasaction_request* request,::bytetrade_pb::keno_my_trasaction_response* response,::google::protobuf::Closure* done)
	{
		// This object helps you to call done->Run() in RAII style. If you need
		// to process the request asynchronously, pass done_guard.release().
		brpc::ClosureGuard done_guard(done);

		/*brpc::Controller* cntl =
		  static_cast<brpc::Controller*>(controller);*/
		if (request->params_size() != 4)
		{
			LOG(ERROR)<< "request params size is not 4";
			response->set_code(1);
			return ;
		}	
		string user_id  = request->params(0).user_id();
		if(user_id.size()==0)
		{
			LOG(ERROR)<<"user_id.size()==0";
			response->set_code(2);
			return;
		}
		int asset_type  = request->params(1).asset_type();
		if (asset_type <=0)
		{
			LOG(ERROR)<<"asset_type<=0";
			response->set_code(3);
			return;
		}	
		size_t offset  = request->params(2).offset();
		size_t limit  = request->params(3).limit();
		if (limit<=0)
		{
			LOG(ERROR)<<"limt<=0";
			response->set_code(4);
			return;
		}

		sds sql = sdsempty();

		sql = sdscatprintf(sql, "SELECT LOWER(HEX(`received_transaction_id`)),received_time,received_block_num,LOWER(HEX(`payout_transaction_id`)),payout_time,payout_block_num,LOWER(HEX(`block_hash`)),server_seed_hash,shoe,pick,hit,room_address,user_id,asset_type,asset_name,amount,status,win_or_lose,payout_user_id,pay_ratio,pay_out,external,channel from received WHERE  `user_id` = '%s' and status > 0",user_id.c_str());

		//size_t asset_len = strlen(asset);
		if (asset_type > 0) {
			//  char _asset[2 * asset_len + 1];
			//        mysql_real_escape_string(conn, _asset, asset, strlen(asset));
			sql = sdscatprintf(sql, " AND `asset_type` = %u", asset_type);
		}
		//    sql = sdscatprintf(sql, " ORDER BY `id` DESC");
		sql = sdscatprintf(sql, " ORDER BY `received_time` DESC");
		if (offset) {
			sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
		} else {
			sql = sdscatprintf(sql, " LIMIT %zu", limit);
		}

		LOG(INFO)<<"exec sql: "<<sql;
		MYSQL *conn = NULL;
		int flag = 0;
		conn = get_mysql_conn(flag,FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
		if (NULL == conn)
		{
			sdsfree(sql);
			response->set_code(5);
			return;
		}

		int ret = mysql_real_query(conn, sql, sdslen(sql));
		if (ret != 0) {
			LOG(ERROR)<<"exec sql: "<<sql<<" fail: "<<mysql_errno(conn)<<" "<<mysql_error(conn);
			sdsfree(sql);
			response->set_code(6);
			return;
		}
		sdsfree(sql);

		MYSQL_RES *result = mysql_store_result(conn);
		size_t num_rows = mysql_num_rows(result);
		for (size_t i = 0; i < num_rows; ++i) {
			bytetrade_pb::keno_my_trasaction_response_result* pbresult = response->add_result();
			MYSQL_ROW row = mysql_fetch_row(result);

			pbresult->set_received_transaction_id(row[0]);
			double timestamp = strtod(row[1], NULL);
			pbresult->set_received_time(timestamp);
			uint32_t received_block_num = atoi(row[2]);
			pbresult->set_received_block_num(received_block_num);
			pbresult->set_block_hash(row[6]);
			pbresult->set_server_seed_hash(row[7]);
			pbresult->set_shoe(row[8]);
			pbresult->set_pick(row[9]);
			pbresult->set_hit(row[10]);



			pbresult->set_room_address(row[11]);
			pbresult->set_user_id(row[12]);

			uint32_t asset_type = atoi(row[13]);
			pbresult->set_asset_type(asset_type);
			pbresult->set_amount(row[15]);

			uint32_t status = atoi(row[16]);
			pbresult->set_status(status);
			uint32_t win_or_lose = atoi(row[17]);
			pbresult->set_win_or_lose(win_or_lose);


			if(win_or_lose == 1)
			{
				pbresult->set_payout_transaction_id(row[3]);
				double payout_time = strtod(row[4], NULL);
				pbresult->set_payout_time(payout_time);
				uint32_t payout_block_num = atoi(row[5]);
				pbresult->set_payout_block_num(payout_block_num);
				pbresult->set_payout_user_id(row[18]);
				uint32_t pay_ratio = atoi(row[19]);
				pbresult->set_pay_ratio(pay_ratio);
				pbresult->set_pay_out(row[20]);
			}

		}
		mysql_free_result(result);
		response->set_code(0);
		/*string body;
		  response->SerializeToString(&body);
		//LOG(ERROR)<<"transfer_amount_size:"<<response->transfer_amount_size();
		LOG(INFO) << body.c_str();*/
		release_mysql_conn(conn, flag);
	}
	void BytetradeServiceImpl::get_block_seed(::google::protobuf::RpcController* controller,
			const ::bytetrade_pb::keno_query_block_seed_request* request,
			::bytetrade_pb::keno_query_block_seed_response* response,
			::google::protobuf::Closure* done){
		brpc::ClosureGuard done_guard(done);

		/*brpc::Controller* cntl =
		  static_cast<brpc::Controller*>(controller);*/
		if (request->params_size() != 1)
		{
			LOG(ERROR)<< "request params size is not 1";
			response->set_code(1);
			return ;
		}
		uint64_t block_num  = request->params(0);
		sds sql = sdsempty();

		sql = sdscatprintf(sql, "SELECT server_seed_hash,seed, `seed_hash_transaction`,`seed_transaction` from keno_seed_publish WHERE `start_block_num` <= '%lu' and `end_block_num` >= '%lu'",block_num,block_num);

		LOG(INFO)<<"exec sql: "<<sql;

		MYSQL *conn = NULL;
		int flag = 0;
		conn = get_mysql_conn(flag,FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
		if (NULL == conn)
		{
			sdsfree(sql);
			response->set_code(1);
			return;
		}

		int ret = mysql_real_query(conn, sql, sdslen(sql));
		if (ret != 0) {
			LOG(ERROR)<<"exec sql: "<<sql<<" fail: "<<mysql_errno(conn)<<" "<<mysql_error(conn);
			sdsfree(sql);
			response->set_code(2);
			return;
		}
		sdsfree(sql);

		MYSQL_RES *result = mysql_store_result(conn);
		size_t num_rows = mysql_num_rows(result);
		if (num_rows == 0) {
			mysql_free_result(result);
			response->set_code(3);
			return ;
		}
		MYSQL_ROW row = mysql_fetch_row(result);
		if(NULL == row[0])
		{
			mysql_free_result(result);
			response->set_code(4);
			return ;
		}
		bytetrade_pb::keno_query_block_seed_response_result* tmp_result = response->mutable_result();
		tmp_result->set_server_seed_hash(row[0]);
		tmp_result->set_seed(row[1]);
		tmp_result->set_seed_hash_transaction(row[2]);
		tmp_result->set_seed_transaction(row[3]);
		mysql_free_result(result);
		response->set_code(0);
		/*string body;
		  response->SerializeToString(&body);
		//LOG(ERROR)<<"transfer_amount_size:"<<response->transfer_amount_size();
		LOG(INFO) << body.c_str();*/
		release_mysql_conn(conn, flag);
	}
	void BytetradeServiceImpl::get_block_seed_list(::google::protobuf::RpcController* controller,
			const ::bytetrade_pb::keno_query_block_seed_list_request* request,
			::bytetrade_pb::keno_query_block_seed_list_response* response,
			::google::protobuf::Closure* done){
		brpc::ClosureGuard done_guard(done);

		/*brpc::Controller* cntl =
		  static_cast<brpc::Controller*>(controller);*/
		if (request->params_size() != 2)
		{
			LOG(ERROR)<< "request params size is not 1";
			response->set_code(1);
			return ;
		}
		size_t offset  = request->params(0);
		size_t limit  = request->params(1);
		sds sql = sdsempty();

		sql = sdscatprintf(sql, "SELECT  server_seed_hash,seed,start_block_num,end_block_num,seed_hash_transaction,seed_transaction from keno_seed_publish order by time desc ");

		if (offset) {
			sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
		} else {
			sql = sdscatprintf(sql, " LIMIT %zu", limit);
		}
		LOG(INFO)<<"exec sql: "<<sql;

		MYSQL *conn = NULL;
		int flag = 0;
		conn = get_mysql_conn(flag,FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
		if (NULL == conn)
		{
			sdsfree(sql);
			response->set_code(1);
			return;
		}

		int ret = mysql_real_query(conn, sql, sdslen(sql));
		if (ret != 0) {
			LOG(ERROR)<<"exec sql: "<<sql<<" fail: "<<mysql_errno(conn)<<" "<<mysql_error(conn);
			sdsfree(sql);
			response->set_code(2);
			return;
		}
		sdsfree(sql);
		MYSQL_RES *result = mysql_store_result(conn);
		size_t num_rows = mysql_num_rows(result);
		for (size_t i = 0; i < num_rows; ++i) {
			bytetrade_pb::keno_query_block_seed_list_response_result* pbresult = response->add_result();
			MYSQL_ROW row = mysql_fetch_row(result);

			// int id = atoi(row[0]);
			// json_object_set_new(record, "id", json_integer(id));
			pbresult->set_server_seed_hash(row[0]);
			pbresult->set_seed(row[1]);

			int start_block_num = atoi(row[2]);
			pbresult->set_start(start_block_num);
			int end_block_num = atoi(row[3]);
			pbresult->set_end(end_block_num);
			pbresult->set_seed_hash_transaction(row[4]);
			pbresult->set_seed_transaction(row[5]);

		}
		mysql_free_result(result);
		response->set_code(0);
		/*string body;
		  response->SerializeToString(&body);
		//LOG(ERROR)<<"transfer_amount_size:"<<response->transfer_amount_size();
		LOG(INFO) << body.c_str();*/
		release_mysql_conn(conn, flag);
	}

