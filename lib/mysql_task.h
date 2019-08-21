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
#include <brpc/periodic_task.h>
#include <map>
#include "mysql_exe_queue.h"
using namespace std;
using namespace brpc;
class MysqlCheckTask : public PeriodicTask {
        public:
                MysqlCheckTask(const string& host,const string& user,const string& password,const string& name, int port,string& charset,int mysql_exe_time);
                bool OnTriggeringTask(timespec* next_abstime);
                void OnDestroyingTask(){};
                static int  Mysql_mutex_init();
                static int Mysql_mutex_destory();
	private:
		string m_host;
		string m_user;
		string m_password;
		string m_name;
		int m_port;
		string m_charset;
		int m_mysql_exe_time;
};
extern bthread_mutex_t g_mutex;
extern map<string,string> g_mysql_task;
