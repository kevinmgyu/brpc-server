#include "keno.h"
int init_from_db(void);
void add_rec_block_callback(std::function< void (const transfer_op& op) > t) {
	g_rec_block_solved_callback = t;
}

void add_payout_block_callback(std::function< void (const transfer_op& op) > t) {
	g_payout_block_solved_callback = t;
}
std::string trim(std::string const& str)
{
	if(str.empty())
		return str;

	std::size_t firstScan = str.find_first_not_of(' ');
	std::size_t first     = firstScan == std::string::npos ? str.length() : firstScan;
	std::size_t last      = str.find_last_not_of(' ');
	return str.substr(first, last-first+1);
}

string HexStrToByte(const char* source, int sourceLen)
{
    char dest[sourceLen];
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);

        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;

        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;

        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return dest;
}

string Hex2Str( const char *sSrc,  int nSrcLen )
{
    int  i;
    char szTmp[3]={'\0'};
    char sDest[nSrcLen*2];
    for( i = 0; i < nSrcLen; i++ )
    {
        sprintf( szTmp, "%02X", sSrc[i] );
        //printf("%c\n",sSrc[i]);
        //printf("%d",i);
        memcpy( &sDest[i * 2], szTmp, 3 );
        szTmp[0]='\0';
        szTmp[1]='\0';
        //printf("%s\n",sDest); 
    }
    return sDest;
}



int init_keno_level_db(const string& path)
{
	//srand((unsigned) time(NULL));
	if (access(path.c_str(), F_OK) == -1) {
		if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			LOG(ERROR)<<"Create "<<path.c_str()<<" fail, path";
			return -__LINE__;
		}
	}

	int ret = g_keno_database.open(path);
	if (ret != 0) {
		LOG(ERROR)<<"open "<<path.c_str()<<" fail, path";
		return -__LINE__;
	}
	return 0;
}
int fini_keno_level_db()
{
	if( g_keno_database.is_open() ) {
		g_keno_database.close();
	}
	return 0;
}

int init_keno(std::function< void (const transfer_op& op) > recfunc,std::function< void (const transfer_op& op) > payoutfunc)
{
	keno_block_exe_queue_start();
	add_rec_block_callback(recfunc);
	add_payout_block_callback(payoutfunc);
	int ret = init_mpd();
	if (ret < 0) {
		LOG(ERROR)<<"init mpd fail: "<<ret;
		return ret;
	}
	ret = init_keno_rooms_config(fc::trim(FLAGS_rooms_info).c_str());
	if (ret < 0) {
		LOG(ERROR)<<"init init_keno_rooms_config fail: "<<ret<<","<<fc::trim(FLAGS_rooms_info).c_str()<<".";
		return ret;
	}
	ret = init_mpd();
	if (ret < 0) {
		LOG(ERROR)<<"init mpd fail: "<<ret;
		return ret;
	}
	ret = g_get_sign_manager.init(FLAGS_sign_fd_cnt, FLAGS_sign_timeout, FLAGS_sign_ip, FLAGS_sign_port);
	if (ret < 0) {
		LOG(ERROR)<<"init g_get_sign_manager.init fail: "<<ret;
		return ret;
	}
	ret = init_server_seed();
	if (ret < 0) {
		LOG(ERROR)<<"init init_server_seed fail: "<<ret;
		return ret;
	}
	ret = init_from_db();
	if (ret < 0) {
		LOG(ERROR)<<"init init_from_db fail: "<<ret;
		return ret;
	}


	ret = init_keno_level_db(FLAGS_keno_path);
	if (ret < 0) {
		LOG(ERROR)<<"init init_keno_level_db fail: "<<ret;
		return ret;
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

int fini_keno() {
	keno_block_exe_queue_stop();
	fini_keno_level_db();
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


int append_game_received(const bytetrade_pb::keno_game& game) {


	// `received_transaction_id`         VARCHAR(20)  CHARACTER SET latin1 NOT NULL PRIMARY KEY,
	// `received_time`          DOUBLE NOT NULL,
	// `received_block_num`         BIGINT NOT NULL,


	// `hold_transaction_id`    VARCHAR(20) CHARACTER SET latin1,
	// `hold_time`          DOUBLE NOT NULL,
	// `hold_block_num`         BIGINT NOT NULL,

	// `payout_transaction_id`    VARCHAR(20) CHARACTER SET latin1,
	// `payout_time`          DOUBLE NOT NULL,
	// `payout_block_num`         BIGINT NOT NULL,

	// `block_hash`         VARCHAR(80) NOT NULL default "",
	// `server_seed_hash`   VARCHAR(65) NOT NULL,
	// `hold_seed_hash`   VARCHAR(65) NOT NULL,
	// `shoe`              VARCHAR(256) NOT NULL,
	// `shoe2`              VARCHAR(256) NOT NULL,
	// `cards`             BIGINT NOT NULL,
	// `card2s`             BIGINT NOT NULL,
	// `holds`             BIGINT NOT NULL,
	// `recommend`             BIGINT NOT NULL,

	// `room_address`         VARCHAR(65) NOT NULL,
	// `user_id`       VARCHAR(42) NOT NULL,


	// `asset_type`         INT NOT NULL,
	// `asset_name`       VARCHAR(42) NOT NULL,
	// `amount`        VARCHAR(65) NOT NULL,
	// `status`        SMALLINT NOT NULL,
	// `win_or_lose`       TINYINT(1) NOT NULL,

	// `payout_user_id`     VARCHAR(42) NOT NULL,
	// `hand_type`        INT NOT NULL,
	// `pay_ratio`     INT NOT NULL,
	// `pay_out`       VARCHAR(65) NOT NULL,


	// `external`       VARCHAR(1024) NOT NULL,
	// `channel`       VARCHAR(65) NOT NULL

	sds type = sdsempty();
	sds value = sdsempty();
	type = sdscatprintf(type, "REPLACE INTO `received` (`received_transaction_id`, `received_time`, `received_block_num`,`payout_transaction_id`, `payout_time`, `payout_block_num`, `block_hash`, `server_seed_hash`, `shoe`, `pick`,`hit`, `room_address`, `user_id`,`asset_type`, `asset_name`, `amount`, `status`, `win_or_lose`, `payout_user_id`, `pay_ratio`, `pay_out`, `external`, `channel`) VALUES ");
	value = sdscatprintf(value, "(unhex('%s'),%f, %lu,unhex('%s'),%f, %lu,  unhex('%s'), '%s', '%s','%s', '%s','%s' , '%s', %d, '%s', '%s', %d, %d,'%s', %d,'%s','%s','%s') ", game.received_transaction_id().c_str(), game.received_time(), game.received_block_num(), "0",game.payout_time(),game.payout_block_num(),game.block_hash().c_str(),game.server_seed_hash().c_str(),game.shoe().c_str(),game.pick().c_str(),game.hit().c_str(),game.room_address().c_str(), game.user_id().c_str(),game.asset_type(),game.asset_name().c_str(),game.amount().c_str(),game.status(), game.win_or_lose() ? 1 : 0, game.payout_user_id().c_str(),game.pay_ratio(),game.pay_out().c_str(), game.external().c_str(),game.channel().c_str());

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
	LOG(INFO)<<"append_game_received "<<type<<value; 
	sdsfree(type);
	sdsfree(value);


	return 0;

}

int append_game_received_use_sqlpp(const bytetrade_pb::keno_game& game) {


	// `received_transaction_id`         VARCHAR(20)  CHARACTER SET latin1 NOT NULL PRIMARY KEY,
	// `received_time`          DOUBLE NOT NULL,
	// `received_block_num`         BIGINT NOT NULL,


	// `hold_transaction_id`    VARCHAR(20) CHARACTER SET latin1,
	// `hold_time`          DOUBLE NOT NULL,
	// `hold_block_num`         BIGINT NOT NULL,

	// `payout_transaction_id`    VARCHAR(20) CHARACTER SET latin1,
	// `payout_time`          DOUBLE NOT NULL,
	// `payout_block_num`         BIGINT NOT NULL,

	// `block_hash`         VARCHAR(80) NOT NULL default "",
	// `server_seed_hash`   VARCHAR(65) NOT NULL,
	// `hold_seed_hash`   VARCHAR(65) NOT NULL,
	// `shoe`              VARCHAR(256) NOT NULL,
	// `shoe2`              VARCHAR(256) NOT NULL,
	// `cards`             BIGINT NOT NULL,
	// `card2s`             BIGINT NOT NULL,
	// `holds`             BIGINT NOT NULL,
	// `recommend`             BIGINT NOT NULL,

	// `room_address`         VARCHAR(65) NOT NULL,
	// `user_id`       VARCHAR(42) NOT NULL,


	// `asset_type`         INT NOT NULL,
	// `asset_name`       VARCHAR(42) NOT NULL,
	// `amount`        VARCHAR(65) NOT NULL,
	// `status`        SMALLINT NOT NULL,
	// `win_or_lose`       TINYINT(1) NOT NULL,

	// `payout_user_id`     VARCHAR(42) NOT NULL,
	// `hand_type`        INT NOT NULL,
	// `pay_ratio`     INT NOT NULL,
	// `pay_out`       VARCHAR(65) NOT NULL,


	// `external`       VARCHAR(1024) NOT NULL,
	// `channel`       VARCHAR(65) NOT NULL

	
	auto config = std::make_shared<sqlpp::mysql::connection_config>();
	config->user = FLAGS_mysql_user;
	config->host = FLAGS_mysql_host;
	config->database = FLAGS_mysql_name;
	config->password =FLAGS_mysql_password;
	config->debug = true;
	try
	{
		int flag = game.win_or_lose() ? 1 : 0;
		sqlpp::mysql::connection db(config);
		const auto keno_Received = Received{};
		/*string received_transaction_id = HexStrToByte(game.received_transaction_id().c_str(),strlen(game.received_transaction_id().c_str()));
		string block_hash = HexStrToByte(game.block_hash().c_str(),strlen(game.block_hash().c_str()));
		string payoutTransactionId = HexStrToByte("00",2);*/
	
		db(insert_into(keno_Received).set(keno_Received.receivedTransactionId=game.received_transaction_id().c_str(),keno_Received.receivedTime=game.received_time(),keno_Received.receivedBlockNum= game.received_block_num(),
			keno_Received.payoutTransactionId ="0",keno_Received.payoutTime=game.payout_time(),keno_Received.payoutBlockNum=game.payout_block_num(),
			keno_Received.blockHash=game.block_hash().c_str(),keno_Received.serverSeedHash=game.server_seed_hash().c_str(),
			keno_Received.shoe=game.shoe().c_str(),keno_Received.pick=game.pick().c_str(),keno_Received.hit=game.hit().c_str(),
		keno_Received.roomAddress=game.room_address().c_str(),keno_Received.userId=game.user_id().c_str(),keno_Received.assetType=game.asset_type(),
		keno_Received.assetName=game.asset_name().c_str(),keno_Received.amount=game.amount().c_str(),keno_Received.status=flag,
		keno_Received.payoutUserId=game.payout_user_id().c_str(),keno_Received.payRatio=game.pay_ratio(),
		keno_Received.payOut=game.pay_out().c_str(),keno_Received.external= game.external().c_str(),keno_Received.channel=game.channel().c_str()));
 		
	}
	catch (const std::exception& e)
	{
		LOG(ERROR) << "Exception: " << e.what() << std::endl;
		return 1;
	}
	
	
	return 0;


}


int append_game_payout(const bytetrade_pb::keno_game& game) {


	// `received_transaction_id`         VARCHAR(65) NOT NULL PRIMARY KEY,
	// 	`payout_user_id`     VARCHAR(42) NOT NULL,
	// 	`payout_transaction_id`    VARCHAR(65) NOT NULL,
	// 	`payout_time`          DOUBLE NOT NULL,
	// 	`payout_block_num`         BIGINT NOT NULL



	sds type = sdsempty();
	sds value = sdsempty();
	type = sdscatprintf(type, "REPLACE INTO `received` (`received_transaction_id`, `received_time`, `received_block_num`,`payout_transaction_id`, `payout_time`, `payout_block_num`, `block_hash`, `server_seed_hash`, `shoe`, `pick`,`hit`, `room_address`, `user_id`,`asset_type`, `asset_name`, `amount`, `status`, `win_or_lose`, `payout_user_id`, `pay_ratio`, `pay_out`, `external`, `channel`) VALUES ");
	value = sdscatprintf(value, "(unhex('%s'),%f, %lu,unhex('%s'),%f, %lu,	unhex('%s'), '%s', '%s','%s', '%s','%s' , '%s', %d, '%s', '%s', %d, %d,'%s', %d,'%s','%s','%s') ", game.received_transaction_id().c_str(), game.received_time(), game.received_block_num(), game.payout_transaction_id().c_str(),game.payout_time(),game.payout_block_num(),game.block_hash().c_str(),game.server_seed_hash().c_str(),game.shoe().c_str(),game.pick().c_str(),game.hit().c_str(),game.room_address().c_str(), game.user_id().c_str(),game.asset_type(),game.asset_name().c_str(),game.amount().c_str(),game.status(), game.win_or_lose() ? 1 : 0, game.payout_user_id().c_str(),game.pay_ratio(),game.pay_out().c_str(), game.external().c_str(),game.channel().c_str());

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
	LOG(INFO)<<"append_game_payout "<<type<<value; 
	sdsfree(type);
	sdsfree(value);



	return 0;


} 

int append_game_payout_use_sqlpp(const bytetrade_pb::keno_game& game) {


	// `received_transaction_id`         VARCHAR(65) NOT NULL PRIMARY KEY,
	// 	`payout_user_id`     VARCHAR(42) NOT NULL,
	// 	`payout_transaction_id`    VARCHAR(65) NOT NULL,
	// 	`payout_time`          DOUBLE NOT NULL,
	// 	`payout_block_num`         BIGINT NOT NULL


		auto config = std::make_shared<sqlpp::mysql::connection_config>();
		config->user = FLAGS_mysql_user;
		config->host = FLAGS_mysql_host;
		config->database = FLAGS_mysql_name;
		config->password =FLAGS_mysql_password;
		config->debug = true;
		try
		{
			sqlpp::mysql::connection db(config);
			const auto keno_Received = Received{};
			/*string received_transaction_id = HexStrToByte(game.received_transaction_id().c_str(),strlen(game.received_transaction_id().c_str()));
			string payoutTransactionId = HexStrToByte(game.payout_transaction_id().c_str(),strlen(game.payout_transaction_id().c_str()));*/
			db(update(keno_Received).set(keno_Received.payoutTransactionId =game.payout_transaction_id().c_str(),keno_Received.payoutTime=game.payout_time(),keno_Received.payoutBlockNum=game.payout_block_num(),keno_Received.payoutUserId=game.payout_user_id().c_str()).where(keno_Received.receivedTransactionId.in(game.received_transaction_id().c_str())));
			
		}
		catch (const std::exception& e)
		{
			LOG(ERROR) << "Exception: " << e.what() << std::endl;
			return 1;
		}
		
		
		return 0;


} 



int append_game_seed(double t, uint64_t block_num,string id ) {

	uint64_t num = block_num + FLAGS_seed_change_num -1;

	sds type = sdsempty();
	sds value = sdsempty();
	type = sdscatprintf(type, "INSERT INTO `keno_seed` (`id`,`time`, `start_block_num`, `end_block_num`, `server_seed_hash`, `seed`,`seed_hash_transaction`) VALUES ");
	value = sdscatprintf(value, "(NULL, %f, %lu,%lu,'%s','%s','%s') ", t,block_num,num, g_seed_hash.c_str(),  g_seed.c_str(),id.c_str());


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
	LOG(INFO)<<"append_game_seed "<<type<<value; 
	sdsfree(type);
	sdsfree(value);


	return 0;


} 


int append_game_seed_use_sqlpp(double t, uint64_t block_num,string id ) {

	uint64_t num = block_num + FLAGS_seed_change_num -1;

	auto config = std::make_shared<sqlpp::mysql::connection_config>();
	config->user = FLAGS_mysql_user;
	config->host = FLAGS_mysql_host;
	config->database = FLAGS_mysql_name;
	config->password =FLAGS_mysql_password;
	config->debug = true;

	try
	{
		sqlpp::mysql::connection db(config);
		const auto keno_seed = KenoSeed{};
		db(insert_into(keno_seed).set(keno_seed.time = t,keno_seed.startBlockNum = block_num,keno_seed.endBlockNum = num,keno_seed.serverSeedHash = g_seed_hash.c_str(),keno_seed.seed = g_seed.c_str(),keno_seed.seedHashTransaction = id.c_str()));

	}
	catch (const std::exception& e)
	{
		LOG(ERROR) << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;


} 

int append_game_seed_publish_use_sqlpp11(double t, uint64_t block_num,string id,string id2) {



	uint64_t start = block_num - FLAGS_seed_change_num ;
	uint64_t end = block_num - 1 ;

	auto config = std::make_shared<sqlpp::mysql::connection_config>();
	config->user = FLAGS_mysql_user;
	config->host = FLAGS_mysql_host;
	config->database = FLAGS_mysql_name;
	config->password =FLAGS_mysql_password;
	config->debug = true;
	try
	{
		sqlpp::mysql::connection db(config);
		const auto keno_seed = KenoSeedPublish{};

		const auto comma = sqlpp::verbatim(",");
		db(custom_query(sqlpp::insert_into(keno_seed).set(keno_seed.time = t,keno_seed.startBlockNum= start, keno_seed.endBlockNum = end,keno_seed.serverSeedHash= g_seed_hash.c_str(),keno_seed.seed=(id2 == "" ? "" : g_seed.c_str()) ,keno_seed.seedHashTransaction = id.c_str(), keno_seed.seedTransaction= id2.c_str()),
					sqlpp::verbatim(" ON DUPLICATE KEY UPDATE "), keno_seed.time = t,comma,keno_seed.endBlockNum = end,comma,keno_seed.serverSeedHash= g_seed_hash.c_str(),comma,keno_seed.seed=(id2 == "" ? "" : g_seed.c_str()) ,comma,keno_seed.seedHashTransaction = id.c_str(), comma,keno_seed.seedTransaction= id2.c_str()));

	}
	catch (const std::exception& e)
	{
		LOG(ERROR) << "Exception: " << e.what() << std::endl;
		return 1;
	}


	return 0;


} 


int append_game_seed_publish(double t, uint64_t block_num,string id,string id2) {



	uint64_t start = block_num - FLAGS_seed_change_num ;
	uint64_t end = block_num - 1 ;



	sds type = sdsempty();
	sds value = sdsempty();
	type = sdscatprintf(type, "REPLACE INTO `keno_seed_publish` ( `time`, `start_block_num`, `end_block_num`, `server_seed_hash`, `seed`,`seed_hash_transaction`,`seed_transaction`) VALUES ");
	value = sdscatprintf(value, "( %f, %lu,%lu,'%s','%s','%s','%s') ", t,start,end, g_seed_hash.c_str(), (id2 == "" ? "" : g_seed.c_str()), id.c_str(), id2.c_str());

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
	LOG(INFO)<<"append_game_seed_publish "<<type<<value; 
	sdsfree(type);
	sdsfree(value);


	return 0;


} 



int push_game_message (bool isreal, const bytetrade_pb::keno_game& game) {
	//log_trace("info %d",game.recommend);
	//string str = fc::json::to_string(game);//.c_str();

	jsonxx::Object o;

	o << "received_transaction_id" << game.received_transaction_id();
	o << "received_time" << game.received_time();
	o << "shoe" << game.shoe();
	o << "pick" << game.pick();
	o << "hit" << game.hit();
	o << "room_address" << game.room_address();
	o << "user_id" << game.user_id();
	o << "asset_type" << game.asset_type();
	o << "amount" << game.amount();
	o << "status" << game.status();
	o << "win_or_lose" << game.win_or_lose();
	o << "pay_out" << game.pay_out();
	o << "pay_ratio" << game.pay_ratio();


	string r = o.json();

	json_t *message = json_loadb(r.c_str(), r.size(), 0, NULL);
	put_kafka_exe_queue(json_dumps(message, 0));

	//json_decref(tidx);
	json_decref(message);

	return 0;
}

int load_seed_info_from_db(MYSQL *conn)
{
	sds sql = sdsempty();
	sql = sdscatprintf(sql, "SELECT `end_block_num`,`server_seed_hash`,`seed`,`seed_hash_transaction` from `keno_seed` ORDER BY `id` DESC LIMIT 1");
	LOG(INFO)<<"get last seed info";
	LOG(INFO)<<"exec sql: "<<sql;

	int ret = mysql_real_query(conn, sql, sdslen(sql));
	if (ret != 0) {
		LOG(ERROR)<<"exec sql: "<<sql<<" fail: "<<mysql_errno(conn)<<" "<<mysql_error(conn);
		sdsfree(sql);
		return -__LINE__;
	}
	sdsfree(sql);

	MYSQL_RES *result = mysql_store_result(conn);
	size_t num_rows = mysql_num_rows(result);
	if (num_rows != 1) {
		mysql_free_result(result);
		LOG(INFO)<<"exec not found,first time runing";
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(result);
	g_seed_change_num  = strtoull(row[0], NULL, 0) + 1;
	g_seed_hash = row[1];
	g_seed = row[2];
	g_seed_hash_transaction_id = row[3];

	mysql_free_result(result);

	return 0;
}

int init_from_db(void) {

	MYSQL *conn = mysql_bytetrade_init(FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
	if (conn == NULL) {
		LOG(ERROR)<<"connect mysql fail";
		return -__LINE__;
	}

	int ret = load_seed_info_from_db(conn); 
	if (ret < 0) {
		goto cleanup;
		return ret;
	}

	LOG(INFO)<<"g_seed_change_num: "<<g_seed_change_num;
	LOG(INFO)<<"g_seed: "<<g_seed.c_str();
	LOG(INFO)<<"g_seed_hash: "<<g_seed_hash.c_str();


	mysql_close(conn);
	LOG(INFO)<<"load success";


	return 0;

cleanup:
	mysql_close(conn);
	return ret;
}

