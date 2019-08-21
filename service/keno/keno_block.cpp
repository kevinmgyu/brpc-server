#include "keno_block.h"
bthread::ExecutionQueueId<keno_block> g_keno_block_queue_id;
std::function< void (const transfer_op& op) > g_rec_block_solved_callback;
std::function< void (const transfer_op& op) > g_payout_block_solved_callback;





void keno_block_exe_queue_start() {
	bthread::ExecutionQueueOptions options;
	bthread::execution_queue_start(&g_keno_block_queue_id, &options, keno_block_queue_exe, NULL);
}

int keno_block_queue_exe(void* meta, bthread::TaskIterator<keno_block> &iter) {
	for (; iter; ++iter) {
		if (iter->type == KENO_REC_BLOCK)
		{
			 g_rec_block_solved_callback(iter->op);
		}
		else if(iter->type == KENO_PAYOUT_BLOCK)
		{
			 g_payout_block_solved_callback(iter->op);
		}
		else
		{
			LOG(ERROR)<<"invalid type "<< iter->type;
		}
		
	}
	return 0;
}
void put_keno_block_exe_queue(const transfer_op& op, int type) {
	keno_block tmp;
	tmp.op = op;
	tmp.type = type;
	bthread::execution_queue_execute(g_keno_block_queue_id, tmp, NULL);
}

void keno_block_exe_queue_stop() {
	bthread::execution_queue_stop(g_keno_block_queue_id);
	//bthread::execution_queue_join(g_keno_block_queue_id);
}
void keno_block_finish(){
	LOG(INFO)<<"fini_message start";



	LOG(INFO)<<"fini_message finished";

}


