#include "redis_client.h"
brpc::Channel g_redis_channel;
int redis_client_init(std::string address)
{
	brpc::ChannelOptions redis_options;
        redis_options.protocol = brpc::PROTOCOL_REDIS;
        if (g_redis_channel.Init(address.c_str(), &redis_options) != 0) {  
                LOG(ERROR) << "Fail to init channel to redis-server";
                return -1;
        }
	return 0;
}
