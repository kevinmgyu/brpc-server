#include "keno_database.hpp"
#include <cstring>
#include "leveldb/write_batch.h"


int keno_database::open( const string& dbdir ) {  
	leveldb::Options options;   
	options.create_if_missing = true;   


	leveldb::Status status = leveldb::DB::Open(options,dbdir, &db);
	if( !status.ok() ) {
		return -__LINE__;
	}

	write_options.sync = true;
	_is_open = true;

	return 0;
}

bool keno_database::is_open()   {
	return _is_open;
}

void keno_database::close() {
	delete db;
} 

int keno_database::store( const bytetrade_pb::keno_game& b ) {
	//std::cout << fc::json::to_string(b) << std::endl;
	string id = b.received_transaction_id();

	leveldb::WriteBatch batch;
	string data;
	b.SerializeToString(&data);
	batch.Put(id, data);

	leveldb::Status s = db->Write(write_options, &batch);


	if( s.ok() ) {
		return 0;
	}

	return 1;
}


//  int keno_database::store_userid_info( const string& user_id,const string& received_id ) {

// 	 leveldb::WriteBatch batch;
// 	 batch.Put(user_id, received_id);

// 	 leveldb::Status s = db->Write(write_options, &batch);

// 	 if( s.ok() ) {
// 	   return 0;
// 	 }

// 	 return 1;
//  }


//  optional<string> keno_database::get_userid_info(const string& user_id) {

// 	 string received_id;

// 	 leveldb::Status s = db->Get(leveldb::ReadOptions(), user_id, &received_id);

// 	 if( !s.ok() ) {
// 	   return optional<string>( );
// 	 }


// 	 return optional<string>(received_id);
//  }


optional<bytetrade_pb::keno_game> keno_database::fetch_by_id(const string& received_id) {

	string block;

	leveldb::Status s = db->Get(leveldb::ReadOptions(), received_id, &block);

	if( !s.ok() ) {
		return optional<bytetrade_pb::keno_game>( );
	}

	bytetrade_pb::keno_game output;
	output.ParseFromArray(block.c_str(), block.size());
	return optional<bytetrade_pb::keno_game>(output);
} 
