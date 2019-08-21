#include "torrent.h"
brpc::Channel g_user_channel_client;
static sds sql_append_mpd(sds sql, mpd_t *val, bool comma)
{
    char *str = mpd_to_sci(val, 0);
    sql = sdscatprintf(sql, "'%s'", str);
    if (comma) {
        sql = sdscatprintf(sql, ", ");
    }
    free(str);
    return sql;
}
static string itos(int32_t a)
{
	char key[40];
        memset(key,'\0',40);
        sprintf(key, "%d", a);
        string tmp = key;
	return tmp;
}
int init_torrent_level_db() {

	srand((unsigned) time(NULL));

	if (access("./database/torrents", F_OK) == -1) {
		if (mkdir("./database/torrents", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			printf("Create database/torrents fail, path\n");
			return -__LINE__;
		}
	}

	int ret = g_torrent_database.open(fc::path("./database/torrents"));
	if (ret != 0) {
		printf("open database/torrents fail, path\n");
		return -__LINE__;
	}

	if (access("./database/accounts", F_OK) == -1) {
		if (mkdir("./database/accounts", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			printf("Create database/accounts fail, path\n");
			return -__LINE__;
		}
	}

	ret = g_account_database.open(fc::path("./database/accounts"));
	if (ret != 0) {
		printf("open database/accounts fail, path\n");
		return -__LINE__;
	}




	// nw_timer_set(&video_poker_timer, 0.1, true, on_video_poker_timer, NULL);
	// nw_timer_start(&video_poker_timer);


	return 0;
}

int fini_torrent_level_db() {
	if( g_torrent_database.is_open() ) {
		g_torrent_database.close();
	}

	if( g_account_database.is_open() ) {
		g_account_database.close();
	}

	return 0;
}


int init_torrent()
{
	int ret = init_mpd();
	if (ret < 0) {
		LOG(ERROR)<<"init mpd fail: "<<ret;
		return ret;
	}
	
	ret = init_torrent_level_db();
	if (ret < 0) {
		LOG(ERROR)<<"init init_keno_level_db fail: "<<ret;
		return ret;
	}

	set_btt_base_url(FLAGS_http_url);
	ret = init_asset();
	if(ret != 0 ) {
		LOG(ERROR)<<"init asset failed:"<<ret;
		return -__LINE__;
	}
	ret = http_client_init(FLAGS_user_server.c_str(), FLAGS_load_balancer.c_str(),g_user_channel_client);
    if (ret < 0)
    {
         LOG(ERROR)<<"http_client_init fail: "<<ret;
         return -__LINE__;
    }


	/*ret = mysql_conn_pool_init(FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
	  if (ret < 0)
	  {
	  LOG(ERROR)<<"mysql_conn_pool_init: "<<ret;
	  return -__LINE__;
	  }*/
	/*ret = mysql_con_pool_mutex_init();
	  if (ret < 0)
	  {
	  LOG(ERROR)<<"mysql_con_pool_mutex_init: "<<ret;
	  return -__LINE__;
	  }*/
	//sqlpp::mysql::global_library_init();
	return 0;
}

int fini_torrent() {

	fini_torrent_level_db();
	//mysql_conn_pool_fini();

	//int cnt = g_mysql_conn_queue.size();
	/*for (int i=0;i<cnt;++i)
	  {
	  conn=g_mysql_conn_queue.front();
	  mysql_close(conn);
	  conn = NULL;
	  g_mysql_conn_queue.pop();	
	  }
	  mysql_con_pool_mutex_destory();*/
	return 0;
}



int append_torrent_create(const bytetrade_pb::torrent &b)
{

	optional<simple_asset> as = get_asset_by_id(b.asset());
	if(!as)
	{
		LOG(ERROR)<<"do not find asset: "<<b.asset(); 
		return -__LINE__;
	}
	mpd_t *balance = devide_int64_to_mpd((share_type)b.balance(), as->base_precision, 10);

	sds type = sdsempty();
	sds value = sdsempty();
	type = sdscatprintf(type, "REPLACE INTO `torrent` (`id`, `transaction_id`, `time`,`create_block`,`last_modify`, `user_id`, `asset`, `percent`,`pay_times`,`finish_times`, `is_create`, `balance`) VALUES ");
	value = sdscatprintf(value, "('%s', '%s', %f,%lu,%lu, '%s', %d, %d, %d,%d,%d, ", b.id().c_str() ,b.transaction_id().c_str(), current_timestamp(),b.create_block(),b.last_modify(), b.user_id().c_str(), b.asset(), b.percent(),b.pay_times(),b.finish_times(), 1);
	value = sql_append_mpd(value, balance, false);	
	value = sdscatprintf(value, ")");

	bthread_mutex_lock(&g_mutex);

	map<string,string>::iterator iter;
	iter = g_mysql_task.find(type);
	if(iter == g_mysql_task.end())
	{
		g_mysql_task[type]= value;
	}
	else
	{
		g_mysql_task[type] = g_mysql_task[type] + ","+ value;
	}

	bthread_mutex_unlock(&g_mutex);
	LOG(INFO)<<"append_torrent_create "<<type<<value; 
	sdsfree(type);
	sdsfree(value);
	mpd_del(balance);
	return 0;
}


int append_torrent_payment(const bytetrade_pb::torrent_payment &b)
{


	optional<simple_asset> as = get_asset_by_id(b.asset());
	if(!as)
	{
		LOG(ERROR)<<"do not find asset: "<<b.asset(); 
		return -__LINE__;
	}

	mpd_t *author_amount = devide_int64_to_mpd((share_type)b.author_amount(), as->base_precision, 10);
	mpd_t *payment_amount = devide_int64_to_mpd((share_type)b.payment_amount(), as->base_precision, 10);
	mpd_t *transfer_one_piece_amount = devide_int64_to_mpd((share_type)b.transfer_one_piece_amount(), as->base_precision, 10);
	mpd_t *author_one_piece_amount = devide_int64_to_mpd((share_type)b.author_one_piece_amount(), as->base_precision, 10);
	mpd_t *receiver_author_one_piece_amount = devide_int64_to_mpd((share_type)b.receiver_author_one_piece_amount(), as->base_precision, 10);

	sds type = sdsempty();
	sds value = sdsempty();
	string tmp_pay_pieces="";
	int cnt = 0;	
	for(auto it = b.pay_pieces().cbegin(); it != b.pay_pieces().cend(); ++it)
    	{
		if(cnt == 0)
		{
			tmp_pay_pieces ="[";
		}
		else
		{
			tmp_pay_pieces += ",";
		}
		tmp_pay_pieces +="[\""+it->first + "\"," + itos(it->second)+"]";
		cnt = cnt +1;
    	}
	if (cnt != 0)
	{
		tmp_pay_pieces += "]";
	}
	else
	{
		tmp_pay_pieces = "[]";
	}
	
	cnt = 0;
	string tmp_increase_trx="";	
	for(auto it = b.increase_trx().cbegin(); it != b.increase_trx().cend(); ++it)
    	{
		if(cnt == 0)
		{
			tmp_increase_trx = "[";
		}
		else
		{
			tmp_increase_trx += ",";
		}
		tmp_increase_trx +="[\""+it->first + "\"," + it->second+"]";
		cnt = cnt + 1;
    	}
	if (cnt != 0)
	{
		tmp_increase_trx += "]";
	}
	else
	{
		tmp_increase_trx = "[]";
	}
	//string tmp_pay_pieces = fc::json::to_string(*(b.mutable_pay_pieces()));
	//string tmp_increase_trx = fc::json::to_string(*(b.mutable_increase_trx()));
	type = sdscatprintf(type, "REPLACE INTO `torrent_payment` (`payment_id`, `torrent_id`, `time`, `create_time`,`selttement_time`,`refund_time`,  `user_id`, `asset`, `pieces`, `payed_pieces`, `sizes`,`percent`,`is_selttement`, `is_refund`, `sign_address`,`pay_pieces`,`selttement_trx_id`,`refund_trx_id`,`receipt_trx_id`,`increase_trx`,`author_amount`,`payment_amount`, `transfer_one_piece_amount`,`author_one_piece_amount`,`receiver_author_one_piece_amount`) VALUES ");
	value = sdscatprintf(value, "('%s', '%s', %f, %lu, %lu, %lu, '%s', %d, %d, %d , %lu,%d ,%d , %d ,  '%s','%s','%s','%s','%s','%s',", b.payment_id().c_str(), b.torrent_id().c_str() , current_timestamp(), b.create_time(), b.selttement_time(),b.refund_time(),b.user_id().c_str(), b.asset(), b.pieces(), b.payed_pieces(), b.sizes(),b.percent(),b.is_selttement()?1:0,b.is_refund()?1:0,b.sign_address().c_str(), tmp_pay_pieces.c_str(),b.selttement_trx_id().c_str(),b.refund_trx_id().c_str(),b.receipt_trx_id().c_str(),tmp_increase_trx.c_str());
	value = sql_append_mpd(value, author_amount, true); 
	value = sql_append_mpd(value, payment_amount, true); 
	value = sql_append_mpd(value, transfer_one_piece_amount, true); 
	value = sql_append_mpd(value, author_one_piece_amount, true); 
	value= sql_append_mpd(value, receiver_author_one_piece_amount, false);  
	value = sdscatprintf(value, ")");
	bthread_mutex_lock(&g_mutex);

	map<string,string>::iterator iter;
	iter = g_mysql_task.find(type);
	if(iter == g_mysql_task.end())
	{
		g_mysql_task[type]= value;
	}
	else
	{
		g_mysql_task[type] = g_mysql_task[type] + ","+ value;
	}

	bthread_mutex_unlock(&g_mutex);
	LOG(INFO)<<"append_torrent_payment "<<type<<value; 
	sdsfree(type);
	sdsfree(value);
	mpd_del(author_amount);
	mpd_del(payment_amount);
	mpd_del(transfer_one_piece_amount);
	mpd_del(author_one_piece_amount);
	mpd_del(receiver_author_one_piece_amount);
	return 0;
}

