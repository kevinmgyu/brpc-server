#include "block_leveldb.h"
#include <fc/io/json.hpp>
#include "block.hpp"
#include <cstring>
#include "types.hpp"
//#include "me_config.h"
#include "leveldb/write_batch.h"
#include "ut_log.h"

int block_leveldb::open( const string& dbdir ) {  
   leveldb::Options options;   
   options.create_if_missing = true;   
   totalnum = 0;

   leveldb::Status status = leveldb::DB::Open(options,dbdir, &db);
   if( !status.ok() ) {
     return -__LINE__;
   }
   
   //block_num_type total = getTotalBlockNum();
  
   write_options.sync = true;
   _is_open = true;

   return 0;
}

bool block_leveldb::is_open()   {
  return _is_open;
}

void block_leveldb::close()
{
  delete db;
}

block_num_type block_leveldb::getTotalBlockNum() {
  if( totalnum > 0 ) {
    return totalnum;
  }

 // log_trace("getTotal BlockNum 1");
  std::string value;
  leveldb::Status s = db->Get(leveldb::ReadOptions(), "totalblock", &value);
 // log_trace("getTotalBlockNum 2");
  if( s.ok() ) {
   // log_trace("getTotalBlockNum 3 %s",value.c_str());
     //log_trace("total block num %s",value.c_str());
   // totalnum = std::stoi(value.c_str());
    totalnum = boost::lexical_cast<block_num_type>( value.c_str() );
    return totalnum;
    //return c;
  }
  //log_trace("getTotalBlockNum 4");
  
  totalnum = 0;
  return totalnum;
}
 
int block_leveldb::store( const signed_block& b ) {
   
    //block_num_type num = getTotalBlockNum()+1;
    block_num_type num = b.block_num + 1;
   
    char totalvalue[20];
    memset(totalvalue,'\0',sizeof(char)*20);
    snprintf(totalvalue,20,"%lu",num);
    totalvalue[19] = '\0';
  
    /*char blocknum_key[20];
    memset(blocknum_key,'\0',sizeof(char)*20);
    snprintf(blocknum_key,20,"b%lu",num);
    blocknum_key[19] = '\0';

    string block_id = b.id().str();
    string block_string =  fc::json::to_string(b);*/

    leveldb::WriteBatch batch;
    batch.Put("totalblock", totalvalue );
    /*batch.Put(blocknum_key, block_id);
    batch.Put(block_id, block_string);*/

    leveldb::Status s = db->Write(write_options, &batch);
    if( s.ok() ) {
      totalnum = num;
      return 0;
    }
    return 1;
}

int block_leveldb::store_totalnum( const signed_block& b ) {
   
    //block_num_type num = getTotalBlockNum()+1;
    block_num_type num = b.block_num + 1;
   
    char totalvalue[20];
    memset(totalvalue,'\0',sizeof(char)*20);
    snprintf(totalvalue,20,"%lu",num);
    totalvalue[19] = '\0';
  
    leveldb::WriteBatch batch;
    batch.Put("totalblock", totalvalue );

    leveldb::Status s = db->Write(write_options, &batch);
    if( s.ok() ) {
      totalnum = num;
      return 0;
    }
    return 1;
}

 /*optional<signed_block> block_leveldb::fetch_by_number(block_num_type num) {
    if( num <= 0 || num > getTotalBlockNum() ) {
      return optional<signed_block>();
    }

    char blocknum_key[20];
    memset(blocknum_key,'\0',sizeof(char)*20);
    snprintf(blocknum_key,20,"b%lu",num);
    blocknum_key[19] = '\0';

    string block_id;
    leveldb::Status s = db->Get(leveldb::ReadOptions(), blocknum_key, &block_id);
   // log_trace("block_id %s",block_id.c_str());

    string block;
    if( !s.ok() ) {
      return optional<signed_block>( );
    }

    s = db->Get(leveldb::ReadOptions(), block_id, &block);
   // log_trace("block %s",block.c_str());

    if( !s.ok() ) {
      return optional<signed_block>( );
    }

    signed_block sb = fc::json::from_string(block).as<signed_block>();
  
    return optional<signed_block>(sb);
}

 optional<string> block_leveldb::fetch_by_string(block_num_type num) {
    if( num <=0 || num > getTotalBlockNum() ) {
      log_error("num is wrong %lu",num);
      return optional<string>();
    }

    char blocknum_key[20];
    memset(blocknum_key,'\0',sizeof(char)*20);
    snprintf(blocknum_key,20,"b%lu",num);
    blocknum_key[19] = '\0';

    string block_id;
    leveldb::Status s = db->Get(leveldb::ReadOptions(), blocknum_key, &block_id);
   // log_trace("block_id %s",block_id.c_str());

    string block;
    if( !s.ok() ) {
      return optional<string>( );
    }

    s = db->Get(leveldb::ReadOptions(), block_id, &block);
   // log_trace("block %s",block.c_str());

    if( !s.ok() ) {
      return optional<string>( );
    }

    return optional<string>(block);
}

optional<signed_block> block_leveldb::last_block() {
    block_num_type block_id = getTotalBlockNum();
    return fetch_by_number(block_id);
}

block_id_type block_leveldb::last_id() {
    return last_block()->id();
}

block_id_type block_leveldb::get_last_n_block_ripemed(block_num_type n) {
    
     block_num_type num = getTotalBlockNum();
     block_num_type start = num - n + 1;
     if ( start <= 0 ) {
        start = 1;
     }

     block_id_type::encoder e;
     for( block_num_type j = start; j <= num; ++j ) {
        block_id_type id = fetch_by_number(j)->id();
        e.write(id.str().c_str(),id.str().size());
     }
 
    return e.result();
 }*/

