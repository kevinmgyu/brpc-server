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
#include <queue>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
using namespace std;

struct mysql_task {
	string type;  // for batch
	string value; // for batch
	string sql;   // for Single execution 
	string host;
	int port;
	string user;
	string password;
	string name;
	string charset;
};
MYSQL*  mysql_bytetrade_init(const char* host, const char* user, const char* pass, const char* name, int port, const char* charset);
void mysql_exe_queue_start();
void mysql_exe_queue_start_batch();

int mysql_queue_exe_batch(void* meta, bthread::TaskIterator<mysql_task> &iter);
int mysql_queue_exe(void* meta, bthread::TaskIterator<mysql_task> &iter);

void put_mysql_exe_queue_batch(const string& type,const string& value,const string host,string user,string password,string name,int port,string charset);
void put_mysql_exe_queue(const string sql,const string host,string user,string password,string name,int port,string charset);
void mysql_exe_queue_stop();
void mysql_exe_queue_stop_batch();
void mysql_finish(MYSQL *conn);
int mysql_conn_pool_init(const char* host, const char* user, const char* pass, const char* name, int port, const char* charset);
int mysql_con_pool_mutex_init();
int mysql_con_pool_mutex_destory();
MYSQL* get_mysql_conn(int& flag,const char* host, const char* user, const char* pass, const char* name, int port, const char* charset);
void release_mysql_conn(MYSQL* conn, int flag);
int mysql_conn_pool_fini();


extern bthread::ExecutionQueueId<mysql_task> g_mysql_queue_id_batch;
extern bthread::ExecutionQueueId<mysql_task> g_mysql_queue_id;
extern queue<MYSQL*> g_mysql_conn_queue;
extern bthread_mutex_t g_con_pool_mutex;
extern int g_max_conn_size;
