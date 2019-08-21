#include "block_task.h"
bthread::ExecutionQueueId<block_task> g_block_queue_id;
std::function< int (bool isreal,const signed_block& block) > g_block_solved_callback;


int  block_init()
{
	return 0;
}

void block_exe_queue_start() {
	bthread::ExecutionQueueOptions options;
	bthread::execution_queue_start(&g_block_queue_id, &options, block_queue_exe, NULL);
}

int block_queue_exe(void* meta, bthread::TaskIterator<block_task> &iter) {
	for (; iter; ++iter) {
		g_block_solved_callback(true, iter->message);
	}
	return 0;
}
void put_block_exe_queue(const signed_block& message) {
	block_task t;
	t.message = message;
	bthread::execution_queue_execute(g_block_queue_id, t, NULL);
}

void block_exe_queue_stop() {
	bthread::execution_queue_stop(g_block_queue_id);
}
void block_finish(){
	LOG(INFO)<<"fini_message start";



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
