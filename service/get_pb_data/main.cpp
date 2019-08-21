#include <iostream>
#include "leveldb/write_batch.h"
#include <cstring>
#include <map>
#include "service.pb.h"
#include <fc/io/json.hpp>
#include <fstream>
#include "ts_torrent.hpp"
#include <leveldb/db.h>   
using namespace std;
using namespace btp;
using namespace leveldb;
using namespace bytetrade_pb;
int main(int argc, char* argv[]) {
	/*	map<string,uint32_t> test1;
		cout << "fc json string:"<<fc::json::to_string(test1).c_str()<<endl;*/
	const char* db_path = argv[1];
	leveldb::Options options;
	leveldb::DB* db;  
	options.create_if_missing = true;
	Status status =DB::Open(options,db_path,&db);
	if(!status.ok()){
		cout<<"Failed to open leveldb: "<<db_path<<endl;
		return -1;
	}
	else{
		cout<<"Open successfully!"<<endl;

	}
	leveldb::WriteOptions write_options;
	write_options.sync = true;
	btp::torrent_payment b;
	fc::ripemd160  deal_id ("c79657dfa350bc5a18b270afc03c6566e1e67990");
	string torrent_string;
	leveldb::Status s = db->Get(leveldb::ReadOptions(),"c79657dfa350bc5a18b270afc03c6566e1e67990", &torrent_string);


	if (!s.ok()) {
		cout<<"id is null"<<endl;
		return  1;
	}

	bytetrade_pb::torrent_payment p ;
	p.ParseFromArray(torrent_string.c_str(), torrent_string.size());

	cout << p.payment_id()<<","<<p.user_id()<<endl;
	return 0;
}
