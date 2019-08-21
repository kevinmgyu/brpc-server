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
#include <fstream>
#include "block.hpp"
using namespace std;

struct block_task {
	signed_block message;
};


int  block_init();
void block_exe_queue_start();
int block_queue_exe(void* meta, bthread::TaskIterator<block_task> &iter);
void put_block_exe_queue(const signed_block& message);
void block_exe_queue_stop();
void block_finish();
extern bthread::ExecutionQueueId<block_task> g_block_queue_id;
extern std::function< int (bool isreal,const signed_block& block) > g_block_solved_callback;
