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
#include "service.pb.h"
#include "keno_base.h"
#include "keno.h"
//#include "service_config.h"
using namespace std;

struct keno_block{
	transfer_op op;
	int type;
};
void keno_block_exe_queue_start();
int keno_block_queue_exe(void* meta, bthread::TaskIterator<keno_block> &iter);
void put_keno_block_exe_queue(const transfer_op& op,int type);
void keno_block_exe_queue_stop();
void keno_block_finish();
extern bthread::ExecutionQueueId<keno_block> g_keno_block_queue_id;
extern std::function< void (const transfer_op& op) > g_rec_block_solved_callback;
extern std::function< void (const transfer_op& op) > g_payout_block_solved_callback;
