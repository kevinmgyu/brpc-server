#include "service_config.h"
//DEFINE_string(protocol, "", "Protocol type. Defined in src/brpc/options.proto");
//DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
//DEFINE_string(server, "", "client connect IP Address of server");
DEFINE_string(fullnode_server, "", "client connect IP Address of server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_string(kafka_brokers, "", "The algorithm for load balancing");
DEFINE_string(kafka_topic, "", "The algorithm for load balancing");
DEFINE_string(redis_address, "", "The algorithm for load balancing");
DEFINE_string(mysql_host, "", "The algorithm for load balancing");
DEFINE_string(mysql_user, "", "The algorithm for load balancing");
DEFINE_string(mysql_name, "", "The algorithm for load balancing");
DEFINE_string(mysql_password, "", "The algorithm for load balancing");
DEFINE_string(mysql_charset, "", "The algorithm for load balancing");
DEFINE_string(keno_path, "", "The algorithm for load balancing");
DEFINE_string(full_node_path, "", "The algorithm for load balancing");
DEFINE_string(full_node_url, "", "The algorithm for load balancing");
DEFINE_string(rooms_info, "", "The algorithm for load balancing");
DEFINE_string(sign_ip, "", "The algorithm for load balancing");
DEFINE_string(sign_port, "", "The algorithm for load balancing");
DEFINE_string(http_url, "", "The algorithm for load balancing");
DEFINE_int32(sign_fd_cnt, -1, "RPC timeout in milliseconds");
DEFINE_int32(sign_timeout, -1, "RPC timeout in milliseconds");
DEFINE_int32(mysql_port, -1, "RPC timeout in milliseconds");
DEFINE_int32(mysql_exe_time, -1, "RPC timeout in milliseconds");
DEFINE_int32(get_block_time, -1, "RPC timeout in milliseconds");
DEFINE_uint64(start_num, -1, "RPC timeout in milliseconds");
DEFINE_uint64(chain_type, -1, "RPC timeout in milliseconds");
DEFINE_string(transfer_seed_from, "", "The algorithm for load balancing");
DEFINE_string(transfer_seed_to, "", "The algorithm for load balancing");
DEFINE_uint64(seed_change_num, -1, "RPC timeout in milliseconds");
/*DEFINE_int32(timeout_ms, -1, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, -1, "Max retries(not including the first RPC)");*/


//DEFINE_bool(echo_attachment, false, "Echo attachment as well");
DEFINE_int32(port, 0, "TCP Port of this server");
/*DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
                "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, -1, "Maximum duration of server's LOGOFF state "
                "(waiting for client to close connection before server stops)");
*/
