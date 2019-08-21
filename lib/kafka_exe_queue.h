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

#include <librdkafka/rdkafka.h>
using namespace std;

struct kafka_task {
	string message;
};


int  kafka_init(const char* brokers, const char* topic_name);
void kafka_exe_queue_start();
int kafka_queue_exe(void* meta, bthread::TaskIterator<kafka_task> &iter);
void put_kafka_exe_queue(const string& message);
void kafka_exe_queue_stop();
void kafka_finish();
extern bthread::ExecutionQueueId<kafka_task> g_queue_id;

extern rd_kafka_t *rk;
extern rd_kafka_topic_t *g_rkt;
