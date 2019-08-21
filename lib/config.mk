# Generated by config_brpc.sh, don't modify manually
SYSTEM=Linux
HDRS=/usr/local/include/ /usr/local/include/node/  ../../network/ ../../utils/ ../../base/ ../../httptools/ ../../sign_server/ ../../phxpaxos/include/ ../../bitcoin/ ../../lib/ 
LIBS=/usr/local/lib ../../network ../../utils ../../base  ../../httptools ../../sign_server ../../phxpaxos/lib ../../bitcoin  ../../lib
PROTOC=/usr/local/bin/protoc
PROTOBUF_HDR=/usr/local/include/
CC=gcc
CXX=g++
GCC_VERSION=40805
STATIC_LINKINGS=  -lutils -lnetwork -lbbtbase -lhttptools -lphxpaxos -lglog -lrdkafka -lgflags -lprotobuf -lleveldb -lbtc -lbitcoin -lbitcoin_rpcclient -lbitcoin_util -lbitcoin_crypto_base -lssl -lfc -lsecp256k1 -lev -ljansson -lrestclient-cpp -lcurl -lmpdec -lmysqlclient -lz -lhiredis -lboost_system -lboost_thread -lboost_chrono -lboost_filesystem -lboost_date_time -lboost_context -lboost_coroutine -lboost_iostreams 
DYNAMIC_LINKINGS=-lpthread -lssl -lcrypto -ldl -lz -lrt
CPPFLAGS=-DBRPC_WITH_GLOG=1 -DGFLAGS_NS=google -g
ifeq ($(NEED_LIBPROTOC), 1)
    STATIC_LINKINGS+=-lprotoc
endif
ifeq ($(NEED_GPERFTOOLS), 1)
    $(error "Fail to find gperftools")
endif
#DYNAMIC_LINKINGS+=-lglog
ifeq ($(NEED_GTEST), 1)
    $(error "Fail to find gtest")
endif
