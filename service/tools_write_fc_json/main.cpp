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
	b.id = deal_id;
	b.torrent_id = fc::sha1("123");
	b.creator = "kevin";
	string torrent_id = b.id.str();
    string torrent_string = fc::json::to_string(b);
 

    leveldb::Status s = db->Put(write_options, torrent_id, torrent_string);
    if (s.ok()) {
        return 0;
    }
    else
	{
		cout << "write failed "<<endl;
	}
	return 0;
}
