#include "handle_block.h"
#include "chain_api.h"
#include "get_sign_manager.h"
keno_database g_keno_database;
get_sign_manager g_get_sign_manager;
string g_seed_hash_transaction_id;
string g_previous_seed_hash_transaction_id;

Response transfer2_use_sign_service_with_proposal_info(const account_id_type from, const account_id_type to,const asset_id_type asset_type, const share_type amount, const optional<string> message,const string& id, string& trx_id,string propal_id) {
	signed_transaction trx;
	transfer2_operation t;
	t.fee = 30 * core_asset_fee_precision;
	t.from = from;
	t.to = to;
	t.asset_type = asset_type;
	t.amount = amount;
	t.message = message;


	trx.operations.push_back(t);
	trx.proposal_transaction_id = propal_id;
	trx.timestamp = fc::time_point::now();
	std::string output;
	std::string input = trx.sig_digest(1).str();  // get_chain_id() ,1 
	int flag=0;
	if(!g_get_sign_manager.get_sign(id.c_str(), input, output,flag,
			FLAGS_sign_fd_cnt, FLAGS_sign_timeout, FLAGS_sign_ip, FLAGS_sign_port))
	{
		trx.signatures.push_back(fc::json::from_string(output).as<signature_type>());
		LOG(INFO)<<"input sign string "<<input.c_str()<<", return info is "<<output.c_str();
	}
	else
	{
		LOG(FATAL)<<"sign error";
	}

	trx_id = trx.id().str();

	string res = fc::json::to_string(trx);

	//cout << res << endl;
	Object o;
	o.parse(res);

	return put_method("blockchain.put_transaction", o);
}

Response transfer2_bonus(account_id_type from,asset_id_type asset_type, share_type amount, optional<string> message,string id, const string& dapp,const string& gongxian) {

	signed_transaction trx;
	keno_area area1 = bets[from];
	for(map<int,bonus_adr>::iterator it = area1.bonus_adrs.begin(); it != area1.bonus_adrs.end(); ++it ){
		string ttt = it->second.adr;
		if( ttt == "*" ) {
			if( dapp == "" ) {
				continue;
			}
			ttt = dapp;
		}

		transfer2_operation t;
		t.fee = 30 * core_asset_fee_precision;
		t.from = from;
		t.to = ttt;
		t.asset_type = asset_type;
		t.amount = amount /it->second.base *it->second.rate ;
		t.message = message;
		trx.operations.push_back(t);
	}
	
	trx.timestamp = fc::time_point::now();
	trx.dapp = gongxian;

	std::string output;
	std::string input = trx.sig_digest(1).str();  // get_chain_id() ,1 
	int flag=0;
	if(!g_get_sign_manager.get_sign(id.c_str(), input, output,flag,FLAGS_sign_fd_cnt, FLAGS_sign_timeout, 
			FLAGS_sign_ip, FLAGS_sign_port))
	{
		trx.signatures.push_back(fc::json::from_string(output).as<signature_type>());
		LOG(INFO)<<"input sign string "<<input.c_str()<<", return info is "<<output.c_str();
	}
	else
	{
		LOG(FATAL)<<"sign error";
	}

	string res = fc::json::to_string(trx);

	//cout << res << endl;
	Object o;
	o.parse(res);

	return put_method("blockchain.put_transaction", o);
}


Response transfer2_use_sign_service(const account_id_type from, const account_id_type to,const asset_id_type asset_type, const share_type amount, const optional<string> message,const string& id, string& trx_id) {
	signed_transaction trx;
	transfer2_operation t;
	t.fee = 30 * core_asset_fee_precision;
	t.from = from;
	t.to = to;
	t.asset_type = asset_type;
	t.amount = amount;
	t.message = message;


	trx.operations.push_back(t);

	trx.timestamp = fc::time_point::now();
	std::string output;
	std::string input = trx.sig_digest(1).str();  // get_chain_id() ,1 
	int flag=0;
	if(!g_get_sign_manager.get_sign(id.c_str(), input, output,flag,
	FLAGS_sign_fd_cnt, FLAGS_sign_timeout, FLAGS_sign_ip, FLAGS_sign_port))
	{
		trx.signatures.push_back(fc::json::from_string(output).as<signature_type>());
		LOG(INFO)<<"input sign string "<<input.c_str()<<", return info is "<<output.c_str();
	}
	else
	{
		LOG(FATAL)<<"sign error";
	}

	trx_id = trx.id().str();
	//LOG(ERROR)<<"trx_id:"<<trx_id.c_str();
	string res = fc::json::to_string(trx);

	//cout << res << endl;
	Object o;
	o.parse(res);

	return put_method("blockchain.put_transaction", o);
}

		void process_seed(const signed_block& block)
		{
			if(g_seed.empty() && g_seed_change_num == 0) // first time normal
			{
				g_seed = generate_seed();
				g_seed_hash =  bytetrade::get_sha256(g_seed);
				double t = current_timestamp();
		
				g_seed_change_num = block.block_num + FLAGS_seed_change_num;
				string id;
				string sig_id = FLAGS_transfer_seed_from;
				Response r = transfer2_use_sign_service(FLAGS_transfer_seed_from, FLAGS_transfer_seed_to,
						1, share_type("100000000"), optional<string>(g_seed_hash + "@"+
							std::to_string(block.block_num) + "@" + std::to_string(g_seed_change_num)), 
						sig_id, id );
		
				g_seed_hash_transaction_id	= id;
				append_game_seed_use_sqlpp(t,block.block_num,id);
				append_game_seed_publish_use_sqlpp11(t,g_seed_change_num,id, "");
		
				return;
			}
		
			if (block.block_num >= g_seed_change_num) // reach	to generate new seed 
			{
				g_seed_change_num = block.block_num + FLAGS_seed_change_num;
				string id;
				string sig_id = FLAGS_transfer_seed_from;
				Response r1 = transfer2_use_sign_service(FLAGS_transfer_seed_from, FLAGS_transfer_seed_to,
						1, share_type("100000000"), optional<string>(g_seed), 
						sig_id, id);
				string publish_seed = g_seed;
				string publish_seed_hash = g_seed_hash;
				double t = current_timestamp();
				append_game_seed_publish(t,block.block_num,g_seed_hash_transaction_id,id);
		
				g_seed = "";
				g_seed = generate_seed();
				g_seed_hash =  bytetrade::get_sha256(g_seed);
				string id2;
				Response r2 = transfer2_use_sign_service(FLAGS_transfer_seed_from, FLAGS_transfer_seed_to,
						1, share_type("100000000"), optional<string>(g_seed_hash), 
						sig_id, id2);
				g_seed_hash_transaction_id = id2;
				append_game_seed_use_sqlpp(t,block.block_num,id2);
				append_game_seed_publish_use_sqlpp11(t,g_seed_change_num,id2,"");
				return;
		
			}
			return;
		
		}



int handle_block(bool isreal, const signed_block& block) {
	LOG(INFO)<<"print block store block "<<block.block_num;
	if( isreal == false ) {
		return 0;
	}
	process_seed(block);
	for (size_t i = 0; i < block.transactions.size(); ++i) {
		processed_transaction trx = block.transactions[i];
		if( trx.operations.size() != 1 ) {
			continue;
		}

		int which = trx.operations[0].which();

		transfer_op op;
		// if (which == 0) {
		//     transfer_operation op1 = trx.operations[0].get<transfer_operation>();
		//     op = op1;
		// } else 
		if (which == 28) {
			transfer2_operation op2 = trx.operations[0].get<transfer2_operation>();
			op = op2;
		} else {
			continue;
		}
		op.transaction_id = trx.id().str();
		op.block_id = block.id().str();
		op.block_num = block.block_num;
		op.reason = trx.operation_results[0].reason;
		op.transaction_in_block = i;
		op.block_hash = block.id().str();

		if( trx.proposal_transaction_id ) {
			op.proposal_transaction_id = trx.proposal_transaction_id->str();
		} else {
			op.proposal_transaction_id = optional<string>();
		}


		if( trx.dapp ) {
			op.dapp = *trx.dapp;
		} else {
			op.dapp = "";
		}

		if( op.from == op.to ) {
			continue;
		}

		if( bets.find(op.to) != bets.end() ) {
			
			if( trx.operation_results[0].reason != ORR_OK ) {
				LOG(ERROR)<<"transaction "<<op.transaction_id.c_str()<<" code error "<<op.reason;
				continue;
			}

			keno_area area = bets[op.to];

			

			if( !op.message ) {
				LOG(ERROR)<<"transaction "<<op.transaction_id.c_str()<<" message is null";
				continue;
			}

			LOG(INFO)<<"KENO_RESULT] g_transfer_in_operation add "<<op.transaction_id.c_str();

			put_keno_block_exe_queue(op,KENO_REC_BLOCK);
		}

		if( bets.find(op.from) != bets.end() ) {
			LOG(INFO)<<"KENO_RESULT] g_transfer_out_operation add "<<op.transaction_id.c_str();
			put_keno_block_exe_queue(op,KENO_PAYOUT_BLOCK);
		}

	}

	return 0;
}


void handler_receivd_transactions(const transfer_op& op) {

		keno_area area = bets[op.to];

		if( area.area.find(op.asset_type) ==  area.area.end() ) {
			LOG(ERROR)<<"transaction "<<op.transaction_id.c_str()<<" res error "<<op.asset_type;
			return;
		}
		
		bytetrade_pb::keno_game g = get_game_by_transfer_op_and_room(  op, area );

		optional<bytetrade_pb::keno_game> g1 = g_keno_database.fetch_by_id(g.received_transaction_id());
		if( g1 ) {
			LOG(ERROR)<<"[VIDEO_RESULT] game already exists "<<g.received_transaction_id().c_str();
			return;
		}
		
		append_game_received_use_sqlpp(g);
		push_game_message(true, g);
		
		
		g_keno_database.store(g);
	 
		if( g.status() == GAME_RECEIVED ) {
			
			Response r1 = transfer2_bonus(op.to, op.asset_type, op.amount, optional<string>(area.contract_id),op.to, op.dapp, g.user_id());
			LOG(INFO)<<"[VIDEO_RESULT] id "<<" "<<op.transaction_id.c_str()<<" amount  "<<g.amount().c_str()<<" asset_type  "<<g.asset_type();

			if( g.win_or_lose() && (share_type)g.pay_out() > 0 ) {
				LOG(INFO)<<"[VIDEO_RESULT] id "<<op.transaction_id.c_str()<<" asset_type "<<g.asset_type()<<" amount "<<g.amount().c_str()<<" pay_out "<<g.pay_out().c_str();
				

				
				string trx_id;
				Response r = transfer2_use_sign_service_with_proposal_info(op.to, op.from,
						g.asset_type(), (share_type)g.pay_out(), optional<string>(op.transaction_id),op.to,
						trx_id,
						g.received_transaction_id());
					//cout << r.body << endl;
				

			} else {
				LOG(INFO)<<"[VIDEO_RESULT] id "<<op.transaction_id.c_str()<<" lose asset_type "<<g.asset_type()<<" amount "<<g.amount().c_str();

			}
		} else {
			LOG(INFO)<<"[VIDEO_RESULT] id "<<op.transaction_id.c_str()<<" amount "<<g.amount().c_str()<<" asset_type "<<g.asset_type();
		}
		 
		

}



void handler_payout_transactions(const transfer_op& op) {

		if( !op.proposal_transaction_id ) { 
			LOG(INFO)<<"[VIDEO_RESULT] proposal_transaction_id not exists";
			return;
		}

		if( op.amount <= 0 ) {
			LOG(INFO)<<"[VIDEO_RESULT] op.amount equal 0";
			return;
		}

		optional<bytetrade_pb::keno_game> g1 = g_keno_database.fetch_by_id(*(op.proposal_transaction_id));
		if( !g1 ) {
			LOG(ERROR)<<"[VIDEO_RESULT] preious game not exists "<<op.proposal_transaction_id->c_str();
			return;
		}

		optional<bytetrade_pb::keno_game> g = game_merge(  *g1 , op );
		if( !g ) {
			LOG(ERROR)<<"[VIDEO_RESULT] game merge failed "<<op.proposal_transaction_id->c_str();
			return;
		}

		 
		append_game_payout_use_sqlpp(*g);
		//push_game_message(true, *g);
		 

		g_keno_database.store(*g);

		LOG(INFO)<<"[VIDEO_RESULT] pay_finished "<<g->received_transaction_id().c_str()<<" "<<g->payout_transaction_id().c_str()<<" "<<g->pay_out().c_str();
}

