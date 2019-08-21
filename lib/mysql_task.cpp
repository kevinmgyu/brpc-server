#include "mysql_task.h"
bthread_mutex_t g_mutex;
map<string,string> g_mysql_task;
MysqlCheckTask::MysqlCheckTask(const string& host,const string& user,const string& password,const string& name, int port,string& charset,int mysql_exe_time)
{
	m_host = host;
	m_user = user;
	m_password = password;
	m_name = name;
	m_port = port;
	m_charset = charset;
	m_mysql_exe_time = mysql_exe_time;
}
bool MysqlCheckTask::OnTriggeringTask(timespec* next_abstime)
{
        bthread_mutex_lock(&g_mutex);
        //RAW_LOG(ERROR,"enter this branch");
        for(auto &v:g_mysql_task)
        {
                put_mysql_exe_queue_batch(v.first,v.second,m_host.c_str(), m_user.c_str(), m_password.c_str(), m_name.c_str(), m_port, m_charset.c_str());
        }
        g_mysql_task.clear();
        bthread_mutex_unlock(&g_mutex);
        *next_abstime = butil::milliseconds_from_now(m_mysql_exe_time);
        return true;
}
int MysqlCheckTask::Mysql_mutex_init()
{
        return bthread_mutex_init(&g_mutex, NULL);
}
int MysqlCheckTask::Mysql_mutex_destory()
{
        return bthread_mutex_destroy(&g_mutex);
}

