#include "http_client.h"
//bytetrade::BytetradeService_Stub g_stub(&channel_client);
int http_client_init(const string server, const string load_balancer,brpc::Channel& channel_client)
{
	brpc::ChannelOptions options_client;
	options_client.protocol = brpc::PROTOCOL_HTTP;
	/*options_client.protocol = FLAGS_protocol;
	options_client.connection_type = FLAGS_connection_type;
	options_client.timeout_ms = FLAGS_timeout_ms
	options_client.max_retry = FLAGS_max_retry;*/


	if (channel_client.Init(server.c_str(), load_balancer.c_str(), &options_client) != 0) {
		LOG(ERROR) << "Fail to initialize channel";
		return -1;
	}
	return 0;
}
