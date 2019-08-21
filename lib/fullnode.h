#pragma once
#include "block_leveldb.h"
#include <brpc/periodic_task.h>
#include <sstream>
#include "http_client.h"
#include "block_task.h"
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/redis.h>
#include <brpc/channel.h>
#include <brpc/server.h>
#include <librdkafka/rdkafka.h>
#include "glog/logging.h"
#include "glog/raw_logging.h"
using namespace brpc;
extern brpc::Channel g_fullnode_channel_client;
class FullnodeCheckTask : public PeriodicTask {
	public:
		FullnodeCheckTask(int get_block_time, uint64_t start_num, uint64_t chain_type,const string full_node_url);
		bool OnTriggeringTask(timespec* next_abstime);
		void OnDestroyingTask(){};
		uint64_t get_start_block_num(uint64_t start_num);
		static int  init_fullnode_task(const string path);
		static int fini_fullnode_task();
		static void add_block_callback(std::function< int (bool isreal, const signed_block& block) > t);
	private:
		int m_get_block_time;
		uint64_t m_start_num;
		uint64_t m_chain_type;
		string m_full_node_url;
};              
