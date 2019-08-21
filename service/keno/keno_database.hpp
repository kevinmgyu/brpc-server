
#pragma once
#include <iostream>
#include <fstream>
#include "keno_config.h"
#include <leveldb/db.h>
#include "service.pb.h"
#include <fc/io/json.hpp>

class keno_database {
public:
      int open( const string& dbdir );
      bool is_open() ;
      void close();

      int store( const bytetrade_pb::keno_game& b );
     
      optional<bytetrade_pb::keno_game> fetch_by_id(const string& received_id);
      // int store_userid_info( const string& user_id,const string& received_id ) ;
      // fc::optional<string> get_userid_info(const string& user_id);
      
private:
      bool _is_open = false;
      leveldb::DB* db;
      
      leveldb::WriteOptions write_options;

      fc::path _index_filename;
};
