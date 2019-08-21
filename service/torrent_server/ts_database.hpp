
#pragma once
#include <iostream>
#include <fstream>
#include "service.pb.h"
#include <leveldb/db.h>   
#include <mutex>
#include <fc/io/json.hpp>

#include "ts_config.h"

namespace btp {
      
class torrent_database {
public:
      
      int open( const fc::path& dbdir );
      bool is_open();
      void close();
 
      int save_or_update_torrent(const bytetrade_pb::torrent &b);
      optional<bytetrade_pb::torrent> get_torrent(const string& torrent_id);
      bool has_torrent(const string& torrent_id);


      int save_or_update_torrent_payment(const bytetrade_pb::torrent_payment &b);
      optional<bytetrade_pb::torrent_payment> get_torrent_payment(const torrent_payment_id_type& torrent_payment_id);
      bool has_torrent_payment(const string& torrent_payment_id);
      
      

private:
      bool _is_open = false;
      leveldb::DB* db;  

      leveldb::WriteOptions write_options;
      fc::path _index_filename;
      std::mutex mut;
};

}
