#pragma once
#include <iostream>
#include <fstream>
#include "block.hpp"
#include <leveldb/db.h>   

class block_leveldb {
public:
      int open( const string& dbdir );
      bool is_open() ;
      void close();

      int store( const signed_block& b );
      int store_totalnum( const signed_block& b );
      block_num_type getTotalBlockNum();
      //void remove( const block_id_type& id );
     /* boost::optional<signed_block> last_block();
      block_id_type last_id();
      boost::optional<signed_block> fetch_by_number(block_num_type num);
      boost::optional<string> fetch_by_string(block_num_type num);
      block_id_type get_last_n_block_ripemed(block_num_type n);*/
      
private:
      bool _is_open = false;
      leveldb::DB* db;  
      block_num_type totalnum;
      
      leveldb::WriteOptions write_options;

      string _index_filename;
};

