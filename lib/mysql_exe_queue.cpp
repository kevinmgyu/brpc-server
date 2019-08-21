#include "mysql_exe_queue.h"
#include<set>
#include<map>
#include<queue>
#include "concurrentqueue.h"
#include "unistd.h"

string g_insert_key_words = "VALUES ";
bthread::ExecutionQueueId<mysql_task> g_mysql_queue_id;
bthread::ExecutionQueueId<mysql_task> g_mysql_queue_id_batch;
set<string> g_key;
map<string,string> g_exe_value;
queue<MYSQL*> g_mysql_conn_queue;
bthread_mutex_t g_con_pool_mutex;
int g_max_conn_size = 64;
moodycamel::ConcurrentQueue<MYSQL*> g_concurrent_q(g_max_conn_size);
MYSQL*  mysql_bytetrade_init(const char* host, const char* user, const char* pass, const char* name, int port, const char* charset)
{
	MYSQL *conn = mysql_init(NULL);
	if (conn == NULL)
		return NULL;

	my_bool reconnect = 1;
	if (mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect) != 0) {
		mysql_close(conn);
		return NULL;
	}
	if (mysql_options(conn, MYSQL_SET_CHARSET_NAME, charset) != 0) {
		mysql_close(conn);
		return NULL;
	}
	if (mysql_real_connect(conn, host, user, pass, name, port, NULL, 0) == NULL) {
		mysql_close(conn);
		return NULL;
	}
	return conn;
}

int mysql_conn_pool_init(const char* host, const char* user, const char* pass, const char* name, int port, const char* charset)
{
	size_t cnt = sysconf(_SC_NPROCESSORS_ONLN)*2;
	MYSQL *conn =NULL;
	for(size_t i=0;i<cnt;++i)
	{	
		conn = mysql_bytetrade_init(host, user, pass, name, port, charset);
		if (conn == NULL) {
			LOG(ERROR)<<"connect mysql fail";
			return __LINE__;
			}
		g_concurrent_q.enqueue(conn);
	}

	return 0;
}

int mysql_conn_pool_fini()
{
	MYSQL *conn =NULL;
	for(int i=0;i<g_max_conn_size;++i)
	{
		g_concurrent_q.try_dequeue(conn);
	}
	conn = NULL;
	return 0;
}
/*int mysql_conn_pool_init(const char* host, const char* user, const char* pass, const char* name, int port, const char* charset)
{
	size_t cnt = sysconf(_SC_NPROCESSORS_ONLN)*2;
	MYSQL *conn =NULL;
	for(size_t i=0;i<cnt;++i)
	{	
		conn = mysql_bytetrade_init(host, user, pass, name, port, charset);
		if (conn == NULL) {
			LOG(ERROR)<<"connect mysql fail";
			return __LINE__;
			}
		g_mysql_conn_queue.push(conn);
	}
	if (g_mysql_conn_queue.size() == cnt)
	{
		return 0;
	}
	else
	{
		LOG(ERROR)<<"g_mysql_conn_queue size is not "<<cnt;
		
		return __LINE__;
	}	
}
*/
void append_key(string key)
{
	set<string>::iterator itr;
	itr = g_key.find(key);
	if(itr == g_key.end())
	{
		g_key.insert(key);	
	}
}

int mysql_con_pool_mutex_init()
{
        return bthread_mutex_init(&g_con_pool_mutex, NULL);
}
int mysql_con_pool_mutex_destory()
{
        return bthread_mutex_destroy(&g_con_pool_mutex);
}
/*MYSQL* get_mysql_conn(int& flag,const char* host, const char* user, const char* pass, const char* name, int port, const char* charset)
{
	MYSQL* conn = NULL;
	if (g_mysql_conn_queue.size() > 0)
	{
		bthread_mutex_lock(&g_con_pool_mutex);
		conn = g_mysql_conn_queue.front();
		g_mysql_conn_queue.pop();
		bthread_mutex_unlock(&g_con_pool_mutex);
	}
	if (NULL == conn)
	{
		LOG(INFO)<<"manual mysql_bytetrade_init branch , conn pool is empty";	
		conn = mysql_bytetrade_init(host, user, pass, name, port, charset);
		if (conn == NULL) {
			LOG(ERROR)<<"manual connect mysql fail";
			return NULL;
		}
		flag = 1;
	}
	return conn;
}

void release_mysql_conn(MYSQL* conn, int flag)
{
	if ( conn!= NULL)
	{
		if (flag == 0)
		{
			bthread_mutex_lock(&g_con_pool_mutex);
			g_mysql_conn_queue.push(conn);
			bthread_mutex_unlock(&g_con_pool_mutex);
		}
		else if (flag == 1)
		{
			mysql_close(conn);
			conn = NULL;
		}
		
	}
	return;
	
}
*/
MYSQL* get_mysql_conn(int& flag,const char* host, const char* user, const char* pass, const char* name, int port, const char* charset)
{
	MYSQL* conn = NULL;
	bool result = false;
	result = g_concurrent_q.try_dequeue(conn);
	if (result == false || NULL == conn)
	{
		LOG(INFO)<<"manual mysql_bytetrade_init branch , conn pool is empty";	
		conn = mysql_bytetrade_init(host, user, pass, name, port, charset);
		if (conn == NULL) {
			LOG(ERROR)<<"manual connect mysql fail";
			return NULL;
		}
		flag = 1;
	}
	return conn;
}

void release_mysql_conn(MYSQL* conn, int flag)
{
	if ( conn!= NULL)
	{
		if (flag == 0)
		{
			g_concurrent_q.enqueue(conn);
		}
		else if (flag == 1)
		{
			mysql_close(conn);
			conn = NULL;
		}
		
	}
	return;
	
}
void mysql_exe_queue_start_batch() {
	bthread::ExecutionQueueOptions options;
	bthread::execution_queue_start(&g_mysql_queue_id_batch, &options, mysql_queue_exe_batch, NULL);
}

void mysql_exe_queue_start() {
	bthread::ExecutionQueueOptions options;
	bthread::execution_queue_start(&g_mysql_queue_id, &options, mysql_queue_exe, NULL);
}
//batch only support replace into or  insert into command 
/*int mysql_queue_exe_batch(void* meta, bthread::TaskIterator<mysql_task> &iter) {
	//string sql = "";
	MYSQL*conn = NULL;
	int flag = 0;
	if (iter)
	{   
		conn = get_mysql_conn(flag,iter->host.c_str(), iter->user.c_str(), iter->password.c_str(), iter->name.c_str(), iter->port, iter->charset.c_str());
		if (conn == NULL)
		{
			LOG(ERROR)<<"get conn is null";	
			return __LINE__;
		}	
	}

	string sql = "";
	g_exe_value.clear();

	for (; iter; ++iter) {
		map<string,string>::iterator it;
		it = g_exe_value.find(iter->type);
		if(it == g_exe_value.end())
		{
			g_exe_value[iter->type] =  iter->value;	
		}
		else
		{
			g_exe_value[iter->type] = g_exe_value[iter->type] +","+iter->value;	
		}
	}
	for(auto&v:g_exe_value)	
	{
		string sql="";
		sql = v.first+v.second; 
		while(1)
		{
			LOG(INFO)<<"mysql_real_query:"<<sql.c_str()<<","<<sql.size();
			int ret = mysql_real_query(conn, sql.c_str(), sql.size());
			if (ret != 0 && mysql_errno(conn) != 1062) {
				LOG(ERROR)<<"exec sql: "<<sql.c_str()<<" fail:"<<mysql_errno(conn) <<" "<<mysql_error(conn);
				usleep(1000 * 1000); // kevin this need to do ?
				continue;
			}
			else if (ret !=0)
			{
				LOG(ERROR)<<"exec sql: "<<sql.c_str()<<" fail:"<<mysql_errno(conn) <<" "<<mysql_error(conn);
				// need process error branch ?
			}
			//exe_flag = 1;
			break;
		}
	}
	release_mysql_conn(conn, flag);
	return 0;
}*/

int mysql_queue_exe_batch(void* meta, bthread::TaskIterator<mysql_task> &iter) {
	//string sql = "";
	MYSQL*conn = NULL;
	int flag = 0;
	if (iter)
	{   
		conn = get_mysql_conn(flag,iter->host.c_str(), iter->user.c_str(), iter->password.c_str(), iter->name.c_str(), iter->port, iter->charset.c_str());
		if (conn == NULL)
		{
			LOG(ERROR)<<"get conn is null";	
			return __LINE__;
		}	
	}

	for (; iter; ++iter) {
		string sql="";
		sql = iter->type + iter->value; 
		while(1)
		{
			LOG(INFO)<<"mysql_real_query:"<<sql.c_str()<<","<<sql.size();
			int ret = mysql_real_query(conn, sql.c_str(), sql.size());
			if (ret != 0 && mysql_errno(conn) != 1062) {
				LOG(ERROR)<<"exec sql: "<<sql.c_str()<<" fail:"<<mysql_errno(conn) <<" "<<mysql_error(conn);
				bthread_usleep(1000 * 1000); // kevin this need to do ?
				continue;
			}
			else if (ret !=0)
			{
				LOG(ERROR)<<"exec sql: "<<sql.c_str()<<" fail:"<<mysql_errno(conn) <<" "<<mysql_error(conn);
				// need process error branch ?
			}
			//exe_flag = 1;
			break;
		}
	}
	release_mysql_conn(conn, flag);
	return 0;
}

int mysql_queue_exe(void* meta, bthread::TaskIterator<mysql_task> &iter) {
	//string sql = "";
	MYSQL*conn = NULL;
	int flag = 0;
	if (iter)
	{
		conn = get_mysql_conn(flag,iter->host.c_str(), iter->user.c_str(), iter->password.c_str(), iter->name.c_str(), iter->port, iter->charset.c_str());
		if (conn == NULL)
		{
			LOG(ERROR)<<"conn is null";	
			return __LINE__;
		}	
	}
	for (; iter; ++iter) {
		//sql = sql + iter->sql;
		//num = num + 1;
		string sql ="";
		sql = iter->sql;
		while(1 )//&& num == cnt)
		{
			LOG(INFO)<<"mysql_real_query:"<<sql.c_str()<<","<<sql.size();
			
			int ret = mysql_real_query(conn, sql.c_str(), sql.size());
			if (ret != 0 && mysql_errno(conn) != 1062) {
				LOG(ERROR)<<"exec sql: "<<sql.c_str()<<" fail:"<<mysql_errno(conn) <<" "<<mysql_error(conn);
				bthread_usleep(1000 * 1000);
				continue;
			}
			else if (ret !=0)
			{
				LOG(ERROR)<<"exec sql: "<<sql.c_str()<<" fail:"<<mysql_errno(conn) <<" "<<mysql_error(conn);
			}
			//mysql_close(conn);
			//exe_flag = 1;
			break;
		}
		/*if (exe_flag == 1)
		  {
		  num =0;
		  sql = "";
		  }*/
	}
	release_mysql_conn(conn, flag);
	return 0;
}
void put_mysql_exe_queue_batch(const string& type,const string& value,const string host,string user,string password,string name,int port,string charset) {
	mysql_task t;
	t.type = type;
	t.value = value;
	t.host = host;
	t.user = user;
	t.password = password;
	t.name = name;
	t.port = port;
	t.charset = charset;
	bthread::execution_queue_execute(g_mysql_queue_id_batch, t, NULL);
}

void put_mysql_exe_queue(const string sql,const string host,string user,string password,string name,int port,string charset) {
	mysql_task t;
	t.sql = sql;
	t.host = host;
	t.user = user;
	t.password = password;
	t.name = name;
	t.port = port;
	t.charset = charset;
	bthread::execution_queue_execute(g_mysql_queue_id, t, NULL);
}
void mysql_exe_queue_stop() {
	bthread::execution_queue_stop(g_mysql_queue_id);
	//bthread::execution_queue_join(g_mysql_queue_id);
}
void mysql_exe_queue_stop_batch() {
	bthread::execution_queue_stop(g_mysql_queue_id_batch);
	//bthread::execution_queue_join(g_mysql_queue_id_batch);
}
void mysql_finish(MYSQL *conn){
	mysql_close(conn);	
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
