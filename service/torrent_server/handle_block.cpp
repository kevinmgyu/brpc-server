#include "handle_block.h"


btp::torrent_database g_torrent_database;
account_database g_account_database;


static share_type get_ratio_value(mpd_t* r1, const share_type& amount, uint64_t base_precision, uint8_t save_precision) {

	//log_trace("torrent amoutn %s %lu %d",to_string(amount).c_str(),base_precision,save_precision);
	mpd_t * amount1 = devide_int64_to_mpd( amount, base_precision,save_precision );
	//  log_trace("torrent mpd_sci %s",mpd_to_sci(amount1,18));
	//  log_trace("torrent mpd_sci %s",mpd_to_sci(r1,18));
	mpd_div(amount1,amount1,r1,&mpd_ctx);
	//   log_trace("torrrent mpd_sci %s",mpd_to_sci(amount1,18));
	share_type f1 = mul_mpd_to_int64( amount1,  base_precision, save_precision);
	mpd_del(amount1);

	return f1;
}

static std::string get_ratio(uint32_t pieces) {
	if( pieces < 256 ) {
		return "0.995";
	} else if( pieces < 512 ) {
		return "0.99";
	} else if( pieces < 1024 ) {
		return "0.985";
	} else if( pieces < 2048 ) {
		return "0.98";
	} else if( pieces < 4096 ) {
		return "0.97";
	} else {
		return "0.96";
	}
}


int handle_block(bool isreal, const signed_block& block) {
	LOG(INFO)<<"print block store block :"<< block.block_num;
	if( isreal == false ) {
		return 0;
	}

	assert(block.block_type == 3);

	for (size_t i = 0; i < block.transactions.size(); ++i) {
		processed_transaction trx = block.transactions[i];


		if( trx.operations.size() != 1 ) {
			continue;
		}

		int which = trx.operations[0].which();

		if( trx.validate_type == 1 ) {


			if( which == OI_SETTLEMENT_OPERATION) {

				settlement_operation op = trx.operations[0].get<settlement_operation>();

				optional<bytetrade_pb::torrent_payment> tp = g_torrent_database.get_torrent_payment(*trx.proposal_transaction_id);
				if( !tp ) {
					continue;
				}

				mpd_t* ratio = decimal(get_ratio(tp->pieces()).c_str(),18);
				char* ratio_str = mpd_to_sci(ratio,10);
				for(size_t i = 0; i < op.account.size(); ++i ) {
					if( op.business_type[i] != TORRENT_RECEIPT_TRANSFER ) {
						continue;
					}

					share_type t = get_ratio_value(ratio, op.amount[i], 1000000000000000000, 8);
					LOG(INFO)<<"settlement op.amount[i]:"<<to_string(op.amount[i]).c_str()<<" t:"<<to_string(t).c_str()<<" ratio"<<ratio_str;
					uint64_t piece = (t / ((share_type)tp->transfer_one_piece_amount() + (share_type)tp->receiver_author_one_piece_amount())).to_uint64();
					LOG(INFO)<<"account:"<<to_string(op.account[i]).c_str()<<"piece:"<<piece<<" one_piece_amount:"<<tp->transfer_one_piece_amount().c_str()<<" receiver_author_one_piece_amount"<<tp->receiver_author_one_piece_amount().c_str();

					if( piece * ((share_type)tp->transfer_one_piece_amount() + (share_type)tp->receiver_author_one_piece_amount()) != t ) {
						LOG(ERROR)<<"piece error";
						continue;
					} else {
						LOG(INFO)<<"piece correct";
					}


					if( tp->pay_pieces().find(op.account[i]) == tp->pay_pieces().end() ) {
						(*tp->mutable_pay_pieces())[op.account[i].c_str()] = 0;
					}
					(*tp->mutable_pay_pieces())[op.account[i]] += piece;
					LOG(INFO)<<"settlement account:"<<op.account[i].c_str()<<" piece:"<<piece;
					// tp->pay_history.push_back()
				}

				free(ratio_str);
				mpd_del(ratio);


				int ret = g_torrent_database.save_or_update_torrent_payment(*tp);
				if( ret != 0 ) {
					LOG(ERROR)<<"save or update torrent payment failed :"<<ret;
					continue;
				}
				append_torrent_payment(*tp);

				LOG(INFO)<<"settlement_operation execute succed id:"<<tp->payment_id().c_str()<<"trxid:"<<trx.id().str().c_str();


			}

		} else {

			if( trx.operation_results.size() != 1) {
				continue;
			}

			if( trx.operation_results[0].reason != ORR_OK ) {
				continue;
			}

			if( which == OI_TORRENT_CREATE_OPERATION) {

				bytetrade::torrent_create_operation op = trx.operations[0].get<bytetrade::torrent_create_operation>();

				bytetrade_pb::torrent t;
				t.set_id(op.id.str().c_str());
				t.set_transaction_id(trx.id().str().c_str()); 
				t.set_user_id(op.creator.c_str());
				t.set_asset(op.asset_type);
				t.set_balance(to_string(op.amount).c_str());
				t.set_last_modify(block.block_num);
				t.set_percent(op.percent);
				t.set_pay_times(0);
				t.set_finish_times(0);
				t.set_create_block(block.block_num);

				int ret = g_torrent_database.save_or_update_torrent(t);
				if( ret != 0 ) {
					LOG(ERROR)<<"save or update torrent failed:"<<ret;
					continue;
				}
				append_torrent_create(t);

				LOG(INFO)<<"create torrent succed:"<<t.id().c_str();
			} else if( which == OI_TORRENT_UPDATE_OPERATION) {

				bytetrade::torrent_update_operation op = trx.operations[0].get<bytetrade::torrent_update_operation>();

				optional<bytetrade_pb::torrent> t = g_torrent_database.get_torrent(op.id.str());
				if( !t ) {
					continue;
				}

				t->set_balance(to_string(op.amount).c_str());
				t->set_last_modify(block.block_num);
				t->set_percent(op.percent);


				int ret = g_torrent_database.save_or_update_torrent(*t);
				if( ret != 0 ) {
					LOG(ERROR)<<"save or update torrent failed:"<<ret;
					continue;
				}
				append_torrent_create(*t);

				LOG(INFO)<<"update torrent succed:"<<t->id().c_str();


			} else if( which == OI_TORRENT_PAYMENT_OPERATION) {

				bytetrade::torrent_payment_operation op = trx.operations[0].get<bytetrade::torrent_payment_operation>();

				optional<bytetrade_pb::torrent> t = g_torrent_database.get_torrent(op.torrent_id.str());
				if( !t ) {
					continue;
				}

				bytetrade_pb::torrent_payment tp;
				tp.set_payment_id( trx.id().str().c_str());
				tp.set_torrent_id(op.torrent_id.str().c_str());
				tp.set_user_id(op.creator.c_str());
				tp.set_asset(op.asset_type);
				tp.set_author_amount(t->balance().c_str());
				tp.set_percent(t->percent());
				tp.set_payment_amount(to_string(op.amount).c_str());
				tp.set_is_selttement(0);
				tp.set_is_refund(0);
				(*tp.mutable_pay_pieces()).clear();
				tp.set_create_time(block.block_num);
				tp.set_selttement_time(0);
				tp.set_refund_time(0);


				tp.set_pieces(op.pieces);
				tp.set_sizes(op.sizes);
				tp.set_payed_pieces(op.payed_pieces);

				tp.set_transfer_one_piece_amount(to_string(op.one_piece_amount).c_str());

				share_type author_one_piece_amount = (share_type)t->balance() / op.pieces;

				share_type author_one_piece_amount_real = author_one_piece_amount / TORRENT_AUTHOR_PERCENT * t->percent() ;

				share_type receiver_author_one_piece_amount = author_one_piece_amount - author_one_piece_amount_real;

				tp.set_author_one_piece_amount(to_string(author_one_piece_amount).c_str());
				tp.set_receiver_author_one_piece_amount(to_string(receiver_author_one_piece_amount).c_str());
				tp.set_sign_address(op.sign_address.c_str());

				t->set_pay_times(t->pay_times()+1);

				int ret = g_torrent_database.save_or_update_torrent_payment(tp);
				if( ret != 0 ) {

					LOG(ERROR)<<"save or update torrent payment failed:"<<ret;
					continue;
				}

				ret = g_torrent_database.save_or_update_torrent(*t);
				append_torrent_payment(tp);
				append_torrent_create(*t);


				LOG(INFO)<<"torrent_payment_operation execute succed [id]: "<<tp.payment_id().c_str();
			} else if( which == OI_TORRENT_PAYMENT_SELTTMENT_OPERATION) {
				bytetrade::torrent_payment_selttement_operation op = trx.operations[0].get<bytetrade::torrent_payment_selttement_operation>();

				optional<bytetrade_pb::torrent_payment> tp = g_torrent_database.get_torrent_payment(op.payment_id);
				if( !tp ) {
					continue;
				}

				optional<bytetrade_pb::torrent> t = g_torrent_database.get_torrent(tp->torrent_id());
				if( !t ) {
					continue;
				}

				t->set_finish_times(t->finish_times()+1);

				tp->set_is_selttement(true);
				tp->set_selttement_time(block.block_num);
				tp->set_selttement_trx_id(trx.id().str().c_str());

				// for( size_t i = 0; i < op.users.size(); ++i ) {
				//     tp->users.push_back(op.users[i]);
				// }

				// for( size_t i = 0; i < op.amounts.size(); ++i ) {
				//     tp->amounts.push_back(op.amounts[i]);
				// }

				int ret = g_torrent_database.save_or_update_torrent_payment(*tp);
				if( ret != 0 ) {
					LOG(ERROR)<<"save or update torrent payment failed:"<<ret;
					continue;
				}
				ret = g_torrent_database.save_or_update_torrent(*t);
				append_torrent_payment(*tp);
				append_torrent_create(*t);

				LOG(INFO)<<"torrent_payment_selttement_operation execute succed [id]: "<<tp->payment_id().c_str();

			} else if( which == OI_TORRENT_RECEIPT_OPERATION) {

				bytetrade::torrent_receipt_operation op = trx.operations[0].get<bytetrade::torrent_receipt_operation>();

				optional<bytetrade_pb::torrent_payment> tp = g_torrent_database.get_torrent_payment(op.payment_id);
				if( !tp ) {
					continue;
				}

				tp->set_receipt_trx_id (trx.id().str().c_str());
				int ret = g_torrent_database.save_or_update_torrent_payment(*tp);
				if( ret != 0 ) {
					LOG(ERROR)<<"save or update torrent payment failed:"<<ret;
					continue;
				}
				append_torrent_payment(*tp);

				LOG(INFO)<<"torrent_payment_receipt_operation execute succed [id]: "<<tp->payment_id().c_str();


			} else if( which == OI_TORRENT_PAYMENT_REFUND_OPERATION ) {
				bytetrade::torrent_payment_refund_operation op = trx.operations[0].get<bytetrade::torrent_payment_refund_operation>();

				optional<bytetrade_pb::torrent_payment> tp = g_torrent_database.get_torrent_payment(op.payment_id);
				if( !tp ) {
					continue;
				}

				tp->set_is_refund(true);
				tp->set_refund_trx_id(trx.id().str().c_str());
				tp->set_refund_time(block.block_num);

				int ret = g_torrent_database.save_or_update_torrent_payment(*tp);
				if( ret != 0 ) {
					LOG(ERROR)<<"save or update torrent payment failed:"<<ret;
					continue;
				}
				append_torrent_payment(*tp);

				LOG(INFO)<<"torrent_payment_refund_operation execute succed [id]: "<<tp->payment_id().c_str();

			} else if( which == OI_TORRENT_PAYMENT_INCREASE_OPERATION) {

				bytetrade::torrent_payment_increase_operation op = trx.operations[0].get<bytetrade::torrent_payment_increase_operation>();

				optional<bytetrade_pb::torrent_payment> tp = g_torrent_database.get_torrent_payment(op.payment_id);
				if( !tp ) {
					continue;
				}

				tp->set_payed_pieces(tp->payed_pieces() + op.pieces);
				tp->set_payment_amount(to_string((share_type)tp->payment_amount() + op.amount).c_str());

				(*tp->mutable_increase_trx())[trx.id().str()]=op.amount;


				int ret = g_torrent_database.save_or_update_torrent_payment(*tp);
				if( ret != 0 ) {
					LOG(ERROR)<<"save or update torrent payment failed:"<<ret;
					continue;
				}
				append_torrent_payment(*tp);

				LOG(INFO)<<"torrent_payment_refund_operation execute succed [id]: "<<tp->payment_id().c_str();

			} else {

			}
		}



	}


	return 0;
}



