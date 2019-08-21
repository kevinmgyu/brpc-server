#include "fullnode.h"
#include <jansson.h>
#include <fc/io/json.hpp>
brpc::Channel g_fullnode_channel_client;
block_leveldb g_block_leveldb;
uint64_t g_block_num = 0;
/*static double current_timestamp(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double) tv.tv_sec + tv.tv_usec / 1000000.0;
}*/
FullnodeCheckTask::FullnodeCheckTask(int get_block_time, uint64_t start_num, uint64_t chain_type,const string full_node_url)
{
	m_get_block_time = get_block_time;
	m_start_num = start_num;
	m_chain_type = chain_type;
	m_full_node_url = full_node_url;
}
uint64_t FullnodeCheckTask::get_start_block_num(uint64_t start_num) {
	uint64_t start = g_block_leveldb.getTotalBlockNum();
	if( start < (uint64_t)start_num ) {
		start = (uint64_t)start_num;
	}

	return start;
}
void FullnodeCheckTask::add_block_callback(std::function< int (bool isreal, const signed_block& block) > t) {
    g_block_solved_callback = t;
}

int  FullnodeCheckTask::init_fullnode_task(const string path) {

	int ret =g_block_leveldb.open(path);
	if( ret != 0 ) {
		LOG(ERROR)<<"init_fullnode_task faild path:"<< path.c_str()<<" ret:"<< ret;
		return -__LINE__;
	}
	return 0;
}

int FullnodeCheckTask::fini_fullnode_task()
{
	
    if(g_block_leveldb.is_open()) {
        g_block_leveldb.close();
    }
	return 0;
}
bool FullnodeCheckTask::OnTriggeringTask(timespec* next_abstime)
{

	/*double duration = 0.3;
	  if( total_block_size == 0 ||  (total_block_size - _db.getTotalBlockNum() > 5) ) {
	  duration = 0;
	  }

	  double now = current_timestamp();
	  if( now - last_send_time < duration ) {
	  return;
	  }






	  is_in_ws_request = true;*/

	/*if(g_block_num < g_block_leveldb.getTotalBlockNum())
	  {
	  RAW_LOG(ERROR,"enter error branch ");
	  return true;
	  }
	  else if (g_block_num == g_block_leveldb.getTotalBlockNum())
	  {
	  RAW_LOG(INFO,"now is the latest block,do not need send require ");
	  return true;	
	  }*/
	uint64_t block_num  = get_start_block_num(m_start_num);
	LOG(INFO)<<"get start block num:"<<block_num;
	int limit = 50;	 
	uint64_t time_id = time(NULL); 
	std::ostringstream os;
	os << "{\"method\":\"fullnode.query\",\"params\":";
	os << "[";
	os << "\"";
	os << block_num;
	os << "\",";
	os << limit;
	os << "]";
	os << ",\"id\":";
	os << time_id;
	os << "}";
	string body = os.str();


	brpc::Controller client_cntl;
	client_cntl.http_request().uri() = m_full_node_url;  // 设置为待访问的URL
	client_cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
	client_cntl.request_attachment().append(body);
	LOG(INFO)<<"send full cmd request:"<<body.c_str();
	//last_send_time = current_timestamp();
	g_fullnode_channel_client.CallMethod(NULL, &client_cntl, NULL, NULL, NULL/*done*/);
	//g_stub.Echo(&client_cntl, &client_request, &client_response, NULL);
	if (!client_cntl.Failed()) {
		/*LOG(ERROR) << "client Received response from " << client_cntl.remote_side()
		  << " to " << client_cntl.local_side()
		  << " (attached="<< client_cntl.response_attachment() << ")"
		  << " latency=" << client_cntl.latency_us() << "us";*/
		json_t *body = json_loadb(client_cntl.response_attachment().to_string().c_str(), client_cntl.response_attachment().length(), 0, NULL);
		json_t *error = json_object_get(body,"error");


		if( json_is_null(error) == false ) {
			const char *message_str = json_string_value(json_object_get(error, "message"));

			LOG(ERROR)<<"CMD_FULL_NODE error message:"<<message_str;

			json_decref(body);
			*next_abstime = butil::milliseconds_from_now(m_get_block_time);
			return true;
		}
		json_t *result = json_object_get(body,"result");

		const char *chain_type_str = json_string_value(json_object_get(result, "chain_type"));
		const char *block_size_str = json_string_value(json_object_get(result, "block_size"));
		uint64_t chain_type;
		sscanf(chain_type_str,"%lu",&chain_type);

		if( chain_type != m_chain_type ) {
			LOG(ERROR)<<"CMD_FULL_NODE chain_type error me request is:"<<m_chain_type<< "return is:"<< chain_type;

			json_decref(body);
			*next_abstime = butil::milliseconds_from_now(m_get_block_time);
			return true;
		}

		uint64_t block_size;
		sscanf(block_size_str,"%lu",&block_size);
		g_block_num = block_size;
		LOG(INFO)<<"CMD_FULL_NODE chain_type"<<chain_type<<" total_block_size :"<<block_size;

		json_t *blocks = json_object_get(result,"blocks");

		size_t block_length = json_array_size(blocks);

		for( size_t i = 0; i < block_length; ++i ) {
			const char *block_str = json_string_value(json_array_get(blocks, i));

			LOG(INFO)<<"CMD_FULL_NODE  block : "<<block_str;
			try {
				signed_block block = fc::json::from_string(block_str).as<signed_block>();

				if( block.block_num == get_start_block_num(m_start_num) ) {

					int ret = g_block_leveldb.store_totalnum(block);
					if( ret != 0 ) {
						LOG(ERROR)<<"CMD_FULL_NODE_ERROR storeblock:"<<block.block_num;
						break;
					}

					/*for( size_t j = 0; j < block_solved_callbacks.size(); ++j ) {
					  block_solved_callbacks[j](true, block);
					  }*/
					put_block_exe_queue(block);


				} else {
					LOG(ERROR)<<"CMD_FULL_NODE_ERROR db_total_num"<<get_start_block_num(m_start_num) <<" block_number "<<block.block_num;
					break;
				}
			} catch(fc::exception e) {
				LOG(ERROR)<<"CMD_FULL_NODE_PARSE_ERROR:"<<block_str;
			}
		}

		json_decref(body);
	} else {
		LOG(ERROR) << client_cntl.ErrorText();
	}

	*next_abstime = butil::milliseconds_from_now(m_get_block_time);
	return true;
}

