#include "http_service.h"
#include <brpc/periodic_task.h>
/*#include <odb/database.hxx>
#include <odb/transaction.hxx>


#include <odb/mysql/database.hxx>

#include "person-odb.hxx"
using namespace odb::core;*/
using namespace std;
//using namespace brpc_sqlpp;
using namespace bytetrade_pb;
double current_timestamp(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double) tv.tv_sec + tv.tv_usec / 1000000.0;
}
uint64_t BytetradeServiceImpl::g_a = 0;
brpc::Channel g_channel_client;



static vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}


void BytetradeServiceImpl::get_create_history(::google::protobuf::RpcController* controller,
		const ::bytetrade_pb::get_create_torrent_request* request,
		::bytetrade_pb::get_create_torrent_response* response,
		::google::protobuf::Closure* done){
	brpc::ClosureGuard done_guard(done);

	brpc::Controller* cntl =
		static_cast<brpc::Controller*>(controller);
	size_t offset = 0;
	size_t limit = 20;
	const std::string* user_id = cntl->http_request().uri().GetQuery("user_id");

	/*if (request->params_size() !=0 && request->params_size() != 2)
	  {
	  LOG(ERROR)<< "request params size is not 2";
	  response->set_code(1);
	  return ;
	  }
	  offset  = request->params(0);
	  limit  = request->params(1);*/
	const std::string* s_offset = cntl->http_request().uri().GetQuery("offset");
	const std::string* s_limit = cntl->http_request().uri().GetQuery("limit");
	if(s_offset!=NULL)
	{
		offset = atoi((*s_offset).c_str());
	}	
	if(s_limit!=NULL)
	{
		limit = atoi((*s_limit).c_str());
	}

	if (limit > 100)
		limit =100;

	sds sql = sdsempty();

	sql = sdscatprintf(sql, "SELECT `id`, `transaction_id`, `time`, `create_block`, `last_modify`, `user_id`, `asset`,`balance`, `percent`, `pay_times`, `finish_times`, `is_create` from torrent ");

	bool flag = false;
	if( user_id != NULL ) {
		sql = sdscatprintf(sql, " where `user_id` = '%s' ",(*user_id).c_str());
		flag = true;
	}  
	sql = sdscatprintf(sql, " order by create_block desc");
	if (offset) {
		sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
	} else {
		sql = sdscatprintf(sql, " LIMIT %zu", limit);
	}
	LOG(INFO)<<"exec sql: "<<sql;

	MYSQL *conn = NULL;
	int mysql_flag = 0;
	conn = get_mysql_conn(mysql_flag,FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
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
		bytetrade_pb::torrent* pbresult = response->mutable_result();
		MYSQL_ROW row = mysql_fetch_row(result);		

		pbresult->set_id(row[0]);
		pbresult->set_transaction_id(row[1]);

		double time = strtod(row[2], NULL);
		pbresult->set_time(time);

		uint64_t create_block = atoi(row[3]);
		pbresult->set_create_block(create_block);

		uint64_t last_modify = atoi(row[4]);
		pbresult->set_last_modify(last_modify);

		pbresult->set_user_id(row[5]);

		uint32_t asset = atoi(row[6]);
		pbresult->set_asset(asset);

		pbresult->set_balance(rstripzero(row[7]));


		uint32_t percent = atoi(row[8]);
		pbresult->set_percent(percent);

		uint32_t pay_times = atoi(row[9]);
		pbresult->set_pay_times(pay_times);

		uint32_t finish_times = atoi(row[10]);
		pbresult->set_finish_times(finish_times);

		uint32_t is_create = atoi(row[11]);
		pbresult->set_is_create(is_create);


	}
	mysql_free_result(result);
	response->set_code(0);
	/*string body;
	  response->SerializeToString(&body);
	//LOG(ERROR)<<"transfer_amount_size:"<<response->transfer_amount_size();
	LOG(INFO) << body.c_str();*/
	release_mysql_conn(conn, mysql_flag);
}


void BytetradeServiceImpl::get_torrent(::google::protobuf::RpcController* controller,
		const ::bytetrade_pb::get_torrent_realtime_request* request,
		::bytetrade_pb::get_torrent_realtime_response* response,
		::google::protobuf::Closure* done){
	brpc::ClosureGuard done_guard(done);

	brpc::Controller* cntl =
		static_cast<brpc::Controller*>(controller);
	const std::string* torrent_id = cntl->http_request().uri().GetQuery("id");

	optional<bytetrade_pb::torrent> o = g_torrent_database.get_torrent((*torrent_id).c_str());
	if(!o)
	{
		response->set_code(1);
		response->set_message("torrent_id do not exist");
		return ;
	}
	bytetrade_pb::torrent* pbresult = response->mutable_result();
	response->set_code(0);	

	pbresult->set_id(o->id());
	pbresult->set_time(o->time());

	pbresult->set_create_block(o->create_block());
	pbresult->set_last_modify(o->last_modify());
	pbresult->set_user_id(o->user_id());

	pbresult->set_asset(o->asset());

	pbresult->set_balance(to_string(o->balance()));
	pbresult->set_percent(o->percent());
	pbresult->set_pay_times(o->pay_times());

	pbresult->set_finish_times(o->finish_times());
	pbresult->set_is_create(o->is_create());

	return;
}

int get_address(account_object o, std::string& out) {
	if( o.active.address_auths.size() == 1 ) {
		out =  o.active.address_auths.begin()->first.str();
		return 0;
	}

	return 1;
}

void HandleAccountResponse(
		brpc::Controller* cntl,
		::bytetrade_pb::get_account_response* response)
{
	std::unique_ptr<brpc::Controller> cntl_guard(cntl);
	if (cntl->Failed()) {
		LOG(WARNING) << "Fail to send userRequest, " << cntl->ErrorText();
		response->set_warning("failt to send userRequest");
		return;
	}
	jsonxx::Object o1;
	o1.parse(cntl->response_attachment().to_string().c_str());
	LOG(INFO)<<cntl->response_attachment().to_string().c_str();
	if( o1.has<jsonxx::Object>("error") )
	{
		response->set_warning("id not exists");
		return;
	}
	jsonxx::Object result = o1.get<jsonxx::Object>("result") ;
	if( o1.has<jsonxx::Object>("result") )
	{
		if( !result.has<jsonxx::Number>("total") )
		{
			response->set_warning("server error");
			return;
		}
		else
		{
			int total = result.get<jsonxx::Number>("total");
			if( total != 1 || !result.has<jsonxx::Array>("res")  )
			{
				response->set_warning("id not exists");
				return;
			}
			else
			{
				jsonxx::Array res = result.get<jsonxx::Array>("res");
				if( res.size() != 1 ) {
					response->set_warning("id not exists");
					return;
				}
				else
				{
					std::string r = res.get<std::string>(0);
					account_object ao = fc::json::from_string(r).as<account_object>();
					std::string aa;
					int rr = get_address(ao,aa);
					if( rr == 0 ) {
						g_account_database.save_or_update_account(ao);

						response->set_account(aa);


						return ;

					}						}
			}}}else
	{
		response->set_warning("server error");
		return;
	}
}

void HandleUserResponse(
		brpc::Controller* cntl,
		::bytetrade_pb::get_torrent_payment_realtime_response* response,bytetrade_pb::torrent_payment o) {
	// std::unique_ptr makes sure cntl/response will be deleted before returning.
	std::unique_ptr<brpc::Controller> cntl_guard(cntl);
	//std::unique_ptr<::bytetrade_pb::get_torrent_payment_realtime_response> response_guard(response);

	if (cntl->Failed()) {
		LOG(WARNING) << "Fail to send userRequest, " << cntl->ErrorText();
		response->set_code(3);
		response->set_message("failt to send userRequest");
		return;
	}

	jsonxx::Object o1;
	o1.parse(cntl->response_attachment().to_string().c_str());
	LOG(INFO)<<cntl->response_attachment().to_string().c_str();
	if( o1.has<jsonxx::Object>("error") )
	{
		response->set_code(4);
		response->set_message("id not exists");
		return;
	}
	jsonxx::Object result = o1.get<jsonxx::Object>("result") ;
	if( o1.has<jsonxx::Object>("result") )
	{
		if( !result.has<jsonxx::Number>("total") )
		{
			response->set_code(5);
			response->set_message("server error");
			return;
		}
		else
		{
			int total = result.get<jsonxx::Number>("total");
			if( total != 1 || !result.has<jsonxx::Array>("res")  )
			{
				response->set_code(6);
				response->set_message("id not exists");
				return;
			}
			else
			{
				jsonxx::Array res = result.get<jsonxx::Array>("res");
				if( res.size() != 1 ) {
					response->set_code(7);
					response->set_message("id not exists");
					return;
				}
				else
				{
					std::string r = res.get<std::string>(0);
					account_object ao = fc::json::from_string(r).as<account_object>();
					std::string aa;
					int rr = get_address(ao,aa);
					if( rr == 0 ) {
						/*optional<bytetrade_pb::torrent_payment> o = g_torrent_database.get_torrent_payment(torrent_payment_id_type(payment_id));
						  if(!o) {
						  response->set_code(9);
						  response->set_message("id not exists");
						  return;
						  } else*/ {
							  g_account_database.save_or_update_account(ao);
							  /*optional<account_object> user = g_account_database.get_account_by_id(o.user_id());

							    if( !user ) {
							    response->set_code(20);
							    response->set_message("user not exists");
							    return;
							    }*/
							  optional<bytetrade_pb::torrent> torrent = g_torrent_database.get_torrent(o.torrent_id());
							  if( !torrent ) {
								  response->set_code(21);
								  response->set_message("torrent id not exists");
								  return;
							  }
							  // to do format message
							  response->set_code(0);
							  bytetrade_pb::torrent_payment_realtime_body* pbresult = response->mutable_result();
							  bytetrade_pb::torrent_payment* ptp = pbresult->mutable_torrent_payment();

							  pbresult->set_account(aa);

							  bytetrade_pb::torrent* pt = pbresult->mutable_torrent();
							  ptp->set_payment_id(o.payment_id());
							  ptp->set_torrent_id(o.torrent_id());
							  ptp->set_time(o.time());
							  ptp->set_create_time(o.create_time());
							  ptp->set_selttement_time(o.selttement_time());
							  ptp->set_refund_time(o.refund_time());
							  ptp->set_user_id(o.user_id());
							  ptp->set_asset(o.asset());
							  ptp->set_pieces(o.pieces());
							  ptp->set_payed_pieces(o.payed_pieces());
							  ptp->set_sizes(o.sizes());
							  ptp->set_percent(o.percent());
							  ptp->set_is_selttement(o.is_selttement());
							  ptp->set_is_refund(o.is_refund());
							  ptp->set_sign_address(o.sign_address());
							  for(auto it = o.pay_pieces().cbegin(); it != o.pay_pieces().cend(); ++it)
							  {
								  (*ptp->mutable_pay_pieces())[it->first]= it->second;			
							  }	
							  ptp->set_author_amount(o.author_amount());
							  ptp->set_payment_amount(o.payment_amount());
							  ptp->set_transfer_one_piece_amount(o.transfer_one_piece_amount());
							  ptp->set_author_one_piece_amount(o.author_one_piece_amount());
							  ptp->set_receiver_author_one_piece_amount(o.receiver_author_one_piece_amount());
							  ptp->set_selttement_trx_id(o.selttement_trx_id());
							  ptp->set_refund_trx_id(o.refund_trx_id());	
							  ptp->set_receipt_trx_id(o.receipt_trx_id());


							  for(auto it = o.increase_trx().cbegin(); it != o.increase_trx().cend(); ++it)
							  {
								  (*ptp->mutable_increase_trx())[it->first]= it->second;			
							  }	

							  pt->set_id(torrent->id());
							  pt->set_time(torrent->time());
							  pt->set_create_block(torrent->create_block());
							  pt->set_last_modify(torrent->last_modify());
							  pt->set_user_id(torrent->user_id());
							  pt->set_asset(torrent->asset());
							  pt->set_balance(to_string(torrent->balance()));
							  pt->set_percent(torrent->percent());
							  pt->set_pay_times(torrent->pay_times());
							  pt->set_finish_times(torrent->finish_times());
							  pt->set_is_create(torrent->is_create());
							  /*if (response !=NULL)
							    {
							    delete response;
							    }
							    if(cntl != NULL)
							    {
							    delete cntl;
							    }*/
							  return ;

						  }
					}						}
			}}}else
	{
		response->set_code(30);
		response->set_message("server error");
		return;
	}
}


void BytetradeServiceImpl::get_account(::google::protobuf::RpcController* controller,
		const ::bytetrade_pb::get_account_request* request,
		::bytetrade_pb::get_account_response* response,
		::google::protobuf::Closure* done){
	brpc::ClosureGuard done_guard(done);

	brpc::Controller* cntl =
		static_cast<brpc::Controller*>(controller);
	const std::string* user_id = cntl->http_request().uri().GetQuery("user_id");
	if( user_id == NULL ||  is_valid_id((*user_id).c_str()) == false ) {
		response->set_warning("user_id error");
		return ;
	};
	optional<account_object> o = g_account_database.get_account_by_id((*user_id).c_str());
	if (!o)
	{
		brpc::Controller* client_cntl =new brpc::Controller();
		const brpc::CallId cid1 = client_cntl->call_id();
		client_cntl->http_request().uri() = FLAGS_user_node_url;  // 设置为待访问的URL
		client_cntl->http_request().set_method(brpc::HTTP_METHOD_GET);
		client_cntl->http_request().uri().SetQuery("user_id", (*user_id));//to do
		//LOG(INFO)<<"send full cmd request:"<<body.c_str();
		//last_send_time = current_timestamp();
		google::protobuf::Closure* done = brpc::NewCallback(&HandleAccountResponse, client_cntl,response);
		g_user_channel_client.CallMethod(NULL, client_cntl, NULL, NULL, done);
		brpc::Join(cid1);	


	}
	else
	{
		std::string aa;
		int rr = get_address(*o,aa);
		if( rr == 0 ) {
			response->set_account(aa);
			return;
		} else {
			response->set_warning("user id do not exist");
			return;
		}
	}
}
void BytetradeServiceImpl::get_torrent_payment(::google::protobuf::RpcController* controller,
		const ::bytetrade_pb::get_torrent_payment_realtime_request* request,
		::bytetrade_pb::get_torrent_payment_realtime_response* response,
		::google::protobuf::Closure* done){
	brpc::ClosureGuard done_guard(done);

	brpc::Controller* cntl =
		static_cast<brpc::Controller*>(controller);
	const std::string* payment_hash = cntl->http_request().uri().GetQuery("payment_hash");
	if(payment_hash  == NULL || strlen((*payment_hash).c_str()) != 40 )
	{
		response->set_code(1);
		response->set_message("payment_hash is not valid");
		return ;
	}
	optional<bytetrade_pb::torrent_payment> o = g_torrent_database.get_torrent_payment(torrent_payment_id_type(*payment_hash));
	if(!o)
	{
		response->set_code(2);
		response->set_message("payment_hash do not exist");
		return ;
	}
	if(g_account_database.has_account_by_id(o->user_id()) == false)
	{

		brpc::Controller* client_cntl =new brpc::Controller();
		const brpc::CallId cid1 = client_cntl->call_id();
		client_cntl->http_request().uri() = FLAGS_user_node_url;  // 设置为待访问的URL
		client_cntl->http_request().set_method(brpc::HTTP_METHOD_GET);
		client_cntl->http_request().uri().SetQuery("user_id", o->user_id());//to do
		//LOG(INFO)<<"send full cmd request:"<<body.c_str();
		//last_send_time = current_timestamp();
		google::protobuf::Closure* done = brpc::NewCallback(&HandleUserResponse, client_cntl,response,*o);
		g_user_channel_client.CallMethod(NULL, client_cntl, NULL, NULL, done);
		brpc::Join(cid1);	
	}
	else
	{
		optional<account_object> user = g_account_database.get_account_by_id(o->user_id());
		if (!user)
		{
			response->set_code(10);
			response->set_message("user do not exist");
			return ;
		}
		optional<bytetrade_pb::torrent> torrent = g_torrent_database.get_torrent(o->torrent_id());
		if (!torrent)
		{
			response->set_code(11);
			response->set_message("user do not exist");
			return ;
		}
		// format reply message
		response->set_code(0);
		bytetrade_pb::torrent_payment_realtime_body* pbresult = response->mutable_result();
		bytetrade_pb::torrent_payment* ptp = pbresult->mutable_torrent_payment();
		std::string aa;
		int rr = get_address(*user ,aa);
		if( rr != 0 ) {
			response->set_code(11);
			response->set_message("user_id error");
			return ;
		}
		pbresult->set_account(aa);

		bytetrade_pb::torrent* pt = pbresult->mutable_torrent();
		ptp->set_payment_id(o->payment_id());
		ptp->set_torrent_id(o->torrent_id());
		ptp->set_time(o->time());
		ptp->set_create_time(o->create_time());
		ptp->set_selttement_time(o->selttement_time());
		ptp->set_refund_time(o->refund_time());
		ptp->set_user_id(o->user_id());
		ptp->set_asset(o->asset());
		ptp->set_pieces(o->pieces());
		ptp->set_payed_pieces(o->payed_pieces());
		ptp->set_sizes(o->sizes());
		ptp->set_percent(o->percent());
		ptp->set_is_selttement(o->is_selttement());
		ptp->set_is_refund(o->is_refund());
		ptp->set_sign_address(o->sign_address());
		for(auto it = o->pay_pieces().cbegin(); it != o->pay_pieces().cend(); ++it)
		{
			(*ptp->mutable_pay_pieces())[it->first]= it->second;			
		}	
		ptp->set_author_amount(o->author_amount());
		ptp->set_payment_amount(o->payment_amount());
		ptp->set_transfer_one_piece_amount(o->transfer_one_piece_amount());
		ptp->set_author_one_piece_amount(o->author_one_piece_amount());
		ptp->set_receiver_author_one_piece_amount(o->receiver_author_one_piece_amount());
		ptp->set_selttement_trx_id(o->selttement_trx_id());
		ptp->set_refund_trx_id(o->refund_trx_id());	
		ptp->set_receipt_trx_id(o->receipt_trx_id());

		for(auto it = o->increase_trx().cbegin(); it != o->increase_trx().cend(); ++it)
		{
			(*ptp->mutable_increase_trx())[it->first]= it->second;			
		}	

		pt->set_id(torrent->id());
		pt->set_time(torrent->time());
		pt->set_create_block(torrent->create_block());
		pt->set_last_modify(torrent->last_modify());
		pt->set_user_id(torrent->user_id());
		pt->set_asset(torrent->asset());
		pt->set_balance(to_string(torrent->balance()));
		pt->set_percent(torrent->percent());
		pt->set_pay_times(torrent->pay_times());
		pt->set_finish_times(torrent->finish_times());
		pt->set_is_create(torrent->is_create());

		return ;

	}}


void BytetradeServiceImpl::get_payment_history(::google::protobuf::RpcController* controller,
		const ::bytetrade_pb::get_torrent_payment_request* request,
		::bytetrade_pb::get_torrent_payment_response* response,
		::google::protobuf::Closure* done){
	brpc::ClosureGuard done_guard(done);

	brpc::Controller* cntl =
		static_cast<brpc::Controller*>(controller);
	size_t offset = 0;
	size_t limit = 20;
	const std::string* user_id = cntl->http_request().uri().GetQuery("user_id");
	const std::string* torrent_id = cntl->http_request().uri().GetQuery("torrent_id");
	const std::string* s_offset = cntl->http_request().uri().GetQuery("offset");
	const std::string* s_limit = cntl->http_request().uri().GetQuery("limit");
	if(s_offset!=NULL)
	{
		offset = atoi((*s_offset).c_str());
	}	
	if(s_limit!=NULL)
	{
		limit = atoi((*s_limit).c_str());
	}
	/*if (request->params_size() !=0 && request->params_size() != 2)
	  {
	  LOG(ERROR)<< "request params size is not 2";
	  response->set_code(1);
	  return ;
	  }
	  offset	= request->params(0);
	  limit  = request->params(1);*/
	if (limit > 100)
		limit =100;

	sds sql = sdsempty();

	sql = sdscatprintf(sql, "SELECT `payment_id`, `torrent_id`, `time`, `create_time`, `selttement_time`, `refund_time`, `user_id`,`asset`, `pieces`, `payed_pieces`, `sizes`, `percent`,`is_selttement`, `is_refund`, `sign_address`, `pay_pieces`, `author_amount`, `payment_amount`,`transfer_one_piece_amount`,`author_one_piece_amount`,`receiver_author_one_piece_amount`,`selttement_trx_id`,`refund_trx_id`,`receipt_trx_id`,`increase_trx` from torrent_payment ");

	bool flag = false;
	if( user_id != NULL ) {
		sql = sdscatprintf(sql, " where `user_id` = '%s' ",(*user_id).c_str());
		flag = true;
	}  

	if( torrent_id != NULL ) {
		if( flag ) {
			sql = sdscatprintf(sql, " AND ");
		} else {
			sql = sdscatprintf(sql, " where ");
		}
		sql = sdscatprintf(sql, " `torrent_id`  = '%s'", (*torrent_id).c_str());   
	}
	sql = sdscatprintf(sql, " order by create_time desc");
	if (offset) {
		sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
	} else {
		sql = sdscatprintf(sql, " LIMIT %zu", limit);
	}
	LOG(INFO)<<"exec sql: "<<sql;

	MYSQL *conn = NULL;
	int mysql_flag = 0;
	conn = get_mysql_conn(mysql_flag,FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
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
		bytetrade_pb::torrent_payment* pbresult = response->mutable_result();
		MYSQL_ROW row = mysql_fetch_row(result);



		// int id = atoi(row[0]);
		// json_object_set_new(record, "id", json_integer(id));
		pbresult->set_payment_id(row[0]);
		pbresult->set_torrent_id(row[1]);

		double time = strtod(row[2], NULL);
		pbresult->set_time(time);


		uint64_t create_time = atoi(row[3]);
		pbresult->set_create_time(create_time);

		uint64_t selttement_time = atoi(row[4]);
		pbresult->set_selttement_time(selttement_time);

		uint64_t refund_time = atoi(row[5]);
		pbresult->set_refund_time(refund_time);

		pbresult->set_user_id(row[6]);

		uint32_t asset = atoi(row[7]);
		pbresult->set_asset(asset);

		uint32_t pieces = atoi(row[8]);
		pbresult->set_pieces(pieces);

		uint32_t payed_pieces = atoi(row[9]);
		pbresult->set_payed_pieces(payed_pieces);

		uint64_t sizes = atoi(row[10]);
		pbresult->set_sizes(sizes);

		uint32_t percent = atoi(row[11]);
		pbresult->set_percent(percent);

		uint32_t is_selttement = atoi(row[12]);
		pbresult->set_is_selttement(is_selttement);


		uint32_t is_refund = atoi(row[13]);
		pbresult->set_is_refund(is_refund);
		pbresult->set_sign_address(row[14]);
	
		map<string, uint32_t>  pay_pieces = fc::json::from_string(row[15]).as<map<string,uint32_t>>();
		for(auto& a: pay_pieces)
		{
			(*pbresult->mutable_pay_pieces())[a.first]= a.second;			
		}
		//pbresult->set_pay_pieces_str(row[15]);


		pbresult->set_author_amount(row[16]);

		pbresult->set_payment_amount(row[17]);
		pbresult->set_transfer_one_piece_amount(row[18]);

		pbresult->set_author_one_piece_amount(row[19]);
		pbresult->set_receiver_author_one_piece_amount(row[20]);

		pbresult->set_selttement_trx_id(row[21]);
		pbresult->set_refund_trx_id(row[22]);	
		pbresult->set_receipt_trx_id(row[23]);
		//pbresult->set_increase_trx_str(row[24]);
		map<string, share_type>  increase_trxs = fc::json::from_string(row[24]).as<map<string,share_type>>();
		for(auto &a: increase_trxs)
		{
			(*pbresult->mutable_increase_trx())[a.first] = a.second;			
		}


	}
	mysql_free_result(result);
	response->set_code(0);

	release_mysql_conn(conn, mysql_flag);
}


