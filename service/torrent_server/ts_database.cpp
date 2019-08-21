#include "ts_database.hpp"
#include "leveldb/write_batch.h"
#include <cstring>
#include <fc/io/json.hpp>
//#include "me_block.h"
namespace btp {
	int torrent_database::open(const fc::path &dbdir) {
		leveldb::Options options;
		options.create_if_missing = true;

		leveldb::Status status = leveldb::DB::Open(options, dbdir.string(), &db);
		if (!status.ok()) {
			//log_fatal("status %d",status.code());
			return -__LINE__;
		}

		write_options.sync = true;
		_is_open = true;

		return 0;
	}

	bool torrent_database::is_open() {
		return _is_open;
	}

	void torrent_database::close() {
		delete db;
	}

	int torrent_database::save_or_update_torrent(const bytetrade_pb::torrent &b) {
		std::lock_guard<std::mutex> lk(mut);

		string torrent_id = b.id();
		string torrent_string;
		b.SerializeToString(&torrent_string);

		leveldb::Status s = db->Put(write_options, torrent_id, torrent_string);
		if (s.ok()) {
			return 0;
		}

		return 1;
	}


	optional<bytetrade_pb::torrent> torrent_database::get_torrent(const string &torrent_id) {
		std::lock_guard<std::mutex> lk(mut);

		string torrent_string;
		leveldb::Status s = db->Get(leveldb::ReadOptions(), torrent_id, &torrent_string);


		if (!s.ok()) {
			return optional<bytetrade_pb::torrent>();
		}
		bytetrade_pb::torrent p;
		p.ParseFromArray(torrent_string.c_str(), torrent_string.size());
		return optional<bytetrade_pb::torrent>(p);
	}


	bool torrent_database::has_torrent(const string& torrent_id) {
		std::lock_guard<std::mutex> lk(mut);

		string torrent_string;
		leveldb::Status s = db->Get(leveldb::ReadOptions(), torrent_id, &torrent_string);


		if (!s.ok()) {
			return false;
		}

		return true;
	}

	int torrent_database::save_or_update_torrent_payment(const bytetrade_pb::torrent_payment &b) {
		std::lock_guard<std::mutex> lk(mut);

		string torrent_id = b.payment_id();
		string torrent_string;
		b.SerializeToString(&torrent_string);

		leveldb::Status s = db->Put(write_options, torrent_id, torrent_string);
		if (s.ok()) {
			return 0;
		}

		return 1;
	}

	optional<bytetrade_pb::torrent_payment> torrent_database::get_torrent_payment(const torrent_payment_id_type& torrent_payment_id) {
		std::lock_guard<std::mutex> lk(mut);

		string torrent_string;
		leveldb::Status s = db->Get(leveldb::ReadOptions(), torrent_payment_id.str(), &torrent_string);


		if (!s.ok()) {
			return optional<bytetrade_pb::torrent_payment>();
		}

		bytetrade_pb::torrent_payment p ;
		p.ParseFromArray(torrent_string.c_str(), torrent_string.size());

		return optional<bytetrade_pb::torrent_payment>(p);
	}

	bool torrent_database::has_torrent_payment(const string& torrent_payment_id) {
		std::lock_guard<std::mutex> lk(mut);

		string torrent_string;
		leveldb::Status s = db->Get(leveldb::ReadOptions(), torrent_payment_id, &torrent_string);


		if (!s.ok()) {
			return false;
		}

		return true;
	}
}
