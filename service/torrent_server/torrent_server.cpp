#include "torrent_server.h"
using namespace bytetrade_pb;
using namespace std;
int init_fullnode()
{
	FullnodeCheckTask::add_block_callback(handle_block);
	int ret = FullnodeCheckTask::init_fullnode_task(FLAGS_full_node_path);
	if (ret != 0) 
	{
		LOG(ERROR)<<"init_fullnode_task fail: "<<ret;
		return -__LINE__;
	}

	ret = http_client_init(FLAGS_fullnode_server.c_str(), FLAGS_load_balancer.c_str(),g_fullnode_channel_client);
	if (ret < 0) 
	{
		LOG(ERROR)<<"http_client_init fail: "<<ret;
		return -__LINE__;
	}
	PeriodicTaskManager::StartTaskAt(new FullnodeCheckTask(FLAGS_get_block_time, FLAGS_start_num, FLAGS_chain_type,FLAGS_full_node_url),butil::milliseconds_from_now(FLAGS_get_block_time));
	block_exe_queue_start();
	return 0;
	
}

int task_init()
{
	int ret = MysqlCheckTask::Mysql_mutex_init();
	if (ret != 0) 
	{
		LOG(ERROR)<<"init bthread_mutex_init fail:"<<ret;
		return -__LINE__;
	}
	
	PeriodicTaskManager::StartTaskAt(new MysqlCheckTask(FLAGS_mysql_host, FLAGS_mysql_user, FLAGS_mysql_password, FLAGS_mysql_name, FLAGS_mysql_port, FLAGS_mysql_charset,FLAGS_mysql_exe_time),butil::milliseconds_from_now(FLAGS_mysql_exe_time));
		
	/*ret = kafka_init(FLAGS_kafka_brokers.c_str(),FLAGS_kafka_topic.c_str());
	if (ret < 0) 
	{
		LOG(ERROR)<< "kafka_init fail:"<<ret;
		return -__LINE__;
	}
	kafka_exe_queue_start();*/

	ret = mysql_conn_pool_init(FLAGS_mysql_host.c_str(), FLAGS_mysql_user.c_str(), FLAGS_mysql_password.c_str(), FLAGS_mysql_name.c_str(), FLAGS_mysql_port, FLAGS_mysql_charset.c_str());
	if (ret < 0)
	{
		LOG(ERROR)<<"mysql_conn_pool_init: "<<ret;
		return -__LINE__;
	}
	/*ret = mysql_con_pool_mutex_init();
	  if (ret < 0)
	  {
	  LOG(ERROR)<<"mysql_con_pool_mutex_init: "<<ret;
	  return -__LINE__;
	  }*/
	//sqlpp::mysql::global_library_init();

	mysql_exe_queue_start_batch();
	
	return 0;
	
}

void task_fini()
{
	mysql_exe_queue_stop_batch();
	mysql_conn_pool_fini();
	//kafka_exe_queue_stop();
	//kafka_finish();
	MysqlCheckTask::Mysql_mutex_destory();
	//mysql_con_pool_mutex_destory();
	google::ShutdownGoogleLogging();
}

void fini_full_node()
{
	block_exe_queue_stop();
	FullnodeCheckTask::fini_fullnode_task();
}

int main(int argc, char* argv[]) {
	//FLAGS_log_dir="./log";   
	//FLAGS_logtostderr = 1;  
	//FLAGS_minloglevel = 0;  
	google::InitGoogleLogging(argv[0]); 
	GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
	
	brpc::Server server;
	BytetradeServiceImpl torrent_service_impl;
	int ret = task_init();
	if (ret < 0) 
	{
		LOG(ERROR)<<"init fail: "<<ret;
		return -__LINE__;
	}
	
	ret = init_torrent();
	if (ret < 0) 
	{
		LOG(ERROR)<<"init_torrent fail: "<<ret;
		return -__LINE__;
	}

	ret = init_fullnode();
	if (ret < 0) 
	{
		LOG(ERROR)<<"init_fullnode fail: "<<ret;
		return -__LINE__;
	}
	
	server.set_version("torrent_server");
	std::string mapping_tmp =
		"/create_torrent_history => get_create_history, "
		"/user => get_account, "
		"/torrent => get_torrent, "
		"/payment_info => get_torrent_payment, "
		"/torrent_payment_history =>get_payment_history";
	const char* const HTTP_SERVICE_RESTFUL_MAPPINGS = mapping_tmp.c_str();
	if (server.AddService(&torrent_service_impl,
				brpc::SERVER_DOESNT_OWN_SERVICE,
				HTTP_SERVICE_RESTFUL_MAPPINGS) != 0) {
		LOG(ERROR) << "Fail to add http service";
		return -1;
	}	
	brpc::ServerOptions options;
	//options.idle_timeout_sec = FLAGS_idle_timeout_s;
	if (server.Start(FLAGS_port, &options) != 0) {
		LOG(ERROR) << "Fail to start torrentServer";
		return -1;
	}
	
	server.RunUntilAskedToQuit();
	fini_full_node();
	fini_torrent();
	task_fini();

	return 0;
}
