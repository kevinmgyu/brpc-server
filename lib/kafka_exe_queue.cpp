#include "kafka_exe_queue.h"
rd_kafka_t *rk;
rd_kafka_topic_t *g_rkt;
bthread::ExecutionQueueId<kafka_task> g_queue_id;

void on_delivery(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque) {
	if (rkmessage->err) {
		LOG(ERROR)<<"Message delivery failed:"<< rd_kafka_err2str(rkmessage->err);
	} else {
		LOG(INFO)<<"Message delivered topic:" <<rd_kafka_topic_name(rkmessage->rkt)<<","<<rkmessage->len<<" bytes, partition"<< rkmessage->partition;
	}
}

void on_logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf) {
	LOG(ERROR)<<"RDKAFKA-"<<level<<"-"<<fac<<": "<<rd_kafka_name(rk)<<": "<<buf;
}
int  kafka_init(const char* brokers, const char* topic_name)
{

	char errstr[1024];
	rd_kafka_conf_t *conf = rd_kafka_conf_new();
	if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
		LOG(ERROR)<<"Set kafka brokers:"<<brokers<<"fail: "<<errstr;
		return -__LINE__;
	}
	if (rd_kafka_conf_set(conf, "queue.buffering.max.ms", "1", errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
		LOG(ERROR)<<"Set kafka buffering:"<<brokers<<"fail:"<<errstr;
		return -__LINE__;
	}
	rd_kafka_conf_set_log_cb(conf, on_logger);
	rd_kafka_conf_set_dr_msg_cb(conf, on_delivery);

	rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
	if (rk == NULL) {
		LOG(ERROR)<<"Failed to create new producer: "<<errstr;
		return -__LINE__;
	}


	g_rkt = rd_kafka_topic_new(rk, topic_name, NULL);
	if (g_rkt == NULL) {
		LOG(ERROR)<<"Failed to create topic object:"<<rd_kafka_err2str(rd_kafka_last_error());
		return -__LINE__;
	}
	return 0;
}

void kafka_exe_queue_start() {
	bthread::ExecutionQueueOptions options;
	bthread::execution_queue_start(&g_queue_id, &options, kafka_queue_exe, NULL);
}

int kafka_queue_exe(void* meta, bthread::TaskIterator<kafka_task> &iter) {
	for (; iter; ++iter) {

		LOG(INFO)<<"push "<<rd_kafka_topic_name(g_rkt)<<"message:"<<iter->message.c_str();


		int ret = rd_kafka_produce(g_rkt, 0, RD_KAFKA_MSG_F_COPY, const_cast<char*>(iter->message.c_str()), iter->message.size(), NULL, 0, NULL);
		if (ret == -1) {
			LOG(ERROR)<<"Failed to produce:"<<iter->message.c_str()<<" to topic "<<rd_kafka_topic_name(g_rkt)<<":"<<rd_kafka_err2str(rd_kafka_last_error());
		 	if (rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL) 
		 	{
				LOG(ERROR)<<"RD_KAFKA_RESP_ERR__QUEUE_FULL";		  
			}
			continue; // kevin to consider or return ?
		}
	}
	rd_kafka_poll(rk, 0);
	return 0;
}
void put_kafka_exe_queue(const string& message) {
	kafka_task t;
	t.message = message;
	bthread::execution_queue_execute(g_queue_id, t, NULL);
}

void kafka_exe_queue_stop() {
	bthread::execution_queue_stop(g_queue_id);
	//bthread::execution_queue_join(g_queue_id);
}
void kafka_finish(){
	LOG(INFO)<<"fini_message start";


	rd_kafka_flush(rk, 1000);

	rd_kafka_topic_destroy(g_rkt);


	rd_kafka_destroy(rk);

	LOG(INFO)<<"fini_message finished";

}

/*
   int main(int argc, char const *argv[]) {
   Hello hl;
   hl.start();
   hl.execute();
   cout<< "===============" <<endl;
   hl.stop();
   cout<< "+++++++++++++++" <<endl;
   return 0;
   }*/
