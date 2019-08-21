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
	// usage : ./main ./json_path  ./pb_path 
	const char* db_path = argv[1];
	const char* db_path_new = argv[2];
	leveldb::Options options;
	leveldb::DB* db;  
	leveldb::DB* db_new;  
	options.create_if_missing = true;
	Status status =DB::Open(options,db_path,&db);
	if(!status.ok()){
		cout<<"Failed to open leveldb: "<<db_path<<endl;
		return -1;
	}
	else{
		cout<<"Open successfully!"<<endl;

	}
	  Status status_new =DB::Open(options,db_path_new,&db_new);
        if(!status.ok()){
                cout<<"Failed to open leveldb: "<<db_path_new<<endl;
                return -1;
        }
        else{
                cout<<"Open successfully!"<<endl;

        }

	leveldb::WriteOptions write_options;
	write_options.sync = true;
	leveldb::WriteOptions write_options_new;
	write_options_new.sync = true;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		//cout<< "enter"<<endl;
		cout << it->key().ToString() <<":" << it->value().ToString() << endl;
		string key = it->key().ToString();
		string value = it->value().ToString();
		size_t found = value.find("receiver_author_one_piece_amount"); 
		int torrent_flag = 0;
		int torrent_payment_flag =0;
    		if (found != string::npos)
		{
			torrent_payment_flag = 1;	
		}
		else
		{
			torrent_flag = 1;
		} 
		// delete levedb json data
		Status s = db->Delete(WriteOptions(),key);
		assert(s.ok());

		btp::torrent t;
		btp::torrent_payment tp;
		if(torrent_flag == 1)
		{
		// json string to  torrent data structure
		try {
			t = fc::json::from_string(value).as<btp::torrent>();
			cout<<"torrent"<<endl;
		} catch (fc::exception e) {
			cout<<"not torrent"<<endl;
		}
		}

		if(torrent_payment_flag ==1)
		{
		try {
			tp = fc::json::from_string(value).as<btp::torrent_payment>();
			cout<<"torrent_payment"<<endl;
		} catch (fc::exception e) {
			cout<<"not torrent_payment"<<endl;
		}
		}

		// torrent data structure to pb struct
		if (torrent_flag == 1)
		{
			bytetrade_pb::torrent torrent_pb;
			torrent_pb.set_id(t.id.str());
			torrent_pb.set_transaction_id(t.tx_id.str());
			torrent_pb.set_create_block(t.create_time);
			torrent_pb.set_last_modify(t.last_modify);
			torrent_pb.set_user_id(t.creator);
			torrent_pb.set_asset(t.asset_type);
			torrent_pb.set_balance(t.amount);
			torrent_pb.set_percent(t.percent);
			torrent_pb.set_pay_times(t.pay_times);
			torrent_pb.set_finish_times(t.finish_times);
			string torrent_id = torrent_pb.id();
			string torrent_string;
			torrent_pb.SerializeToString(&torrent_string);

			leveldb::Status s_save = db_new->Put(write_options_new, torrent_id, torrent_string);
			if (s_save.ok()) {
				continue;
			}
			else
			{
				cout<<"save leveldb failed :"<<torrent_string.c_str()<<endl;
				continue;
			}
		}

		if(torrent_payment_flag == 1)
		{
			bytetrade_pb::torrent_payment torrent_payment_pb;
			torrent_payment_pb.set_payment_id(tp.id.str());
			torrent_payment_pb.set_torrent_id(tp.torrent_id.str());
			torrent_payment_pb.set_create_time(tp.create_time);
			torrent_payment_pb.set_selttement_time(tp.selttement_time);
			torrent_payment_pb.set_refund_time(tp.refund_time);
			torrent_payment_pb.set_user_id(tp.creator);


			torrent_payment_pb.set_asset(tp.asset_type);
			torrent_payment_pb.set_pieces(tp.pieces);
			torrent_payment_pb.set_payed_pieces(tp.payed_pieces);
			torrent_payment_pb.set_sizes(tp.sizes);
			torrent_payment_pb.set_percent(tp.percent);
			torrent_payment_pb.set_is_selttement(tp.is_selttement);
			torrent_payment_pb.set_is_refund(tp.is_refund);


			torrent_payment_pb.set_sign_address(tp.sign_address);
			for(auto &tmp:tp.pay_pieces)
			{
				(*torrent_payment_pb.mutable_pay_pieces())[tmp.first] = tmp.second;
			}
			//torrent_payment_pb.set_pay_pieces();// to do
			torrent_payment_pb.set_selttement_trx_id(tp.selttement_trx_id.str());
			torrent_payment_pb.set_refund_trx_id(tp.refund_trx_id.str());
			torrent_payment_pb.set_receipt_trx_id(tp.receipt_trx_id.str());
			for(auto &tmp:tp.increase_trx)
			{
				(*torrent_payment_pb.mutable_increase_trx())[tmp.first] = to_string(tmp.second);
			}
			//torrent_payment_pb.set_increase_trx();// to do
			torrent_payment_pb.set_author_amount(tp.author_amount);
			torrent_payment_pb.set_payment_amount(tp.payment_amount);
			torrent_payment_pb.set_transfer_one_piece_amount(tp.transfer_one_piece_amount);
			torrent_payment_pb.set_author_one_piece_amount(tp.author_one_piece_amount);
			torrent_payment_pb.set_receiver_author_one_piece_amount(tp.receiver_author_one_piece_amount);
			string torrent_payment_id = torrent_payment_pb.payment_id();
			string torrent_payment_string;
			torrent_payment_pb.SerializeToString(&torrent_payment_string);
			
			leveldb::Status s_save = db_new->Put(write_options_new, torrent_payment_id, torrent_payment_string);
			if (s_save.ok()) {
				continue;
			}
			else
			{
				cout<<"save leveldb failed :"<<torrent_payment_string.c_str()<<endl;
				continue;
			}
		}


	}
	return 0;
}
