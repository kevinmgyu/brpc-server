#pragma once
#include "asset.hpp"

namespace btp {
 

struct torrent {
    torrent_id_type id;
    transaction_id_type tx_id;   
    account_id_type creator;
    share_type      amount;
    asset_id_type   asset_type;
    block_num_type  create_time;
    block_num_type  last_modify;
    uint32_t        percent;
    uint32_t        pay_times;
    uint32_t        finish_times;
    
    torrent() {
       
    }

    torrent(const torrent &o) {
        id = o.id;
        tx_id = o.tx_id;
        
        creator = o.creator;
        amount = o.amount;
        asset_type = o.asset_type;
        create_time = o.create_time;
        last_modify = o.last_modify;

        percent = o.percent;
        pay_times = o.pay_times;
        finish_times = o.finish_times;
    }
};
 
struct torrent_payment {
    torrent_payment_id_type  id;
    torrent_id_type          torrent_id;
    account_id_type          creator;
    asset_id_type            asset_type;
    share_type               author_amount;
    uint32_t                 percent;
    share_type               payment_amount;
    bool                     is_selttement;
    bool                     is_refund;
    // vector<account_id_type>  users;
    // vector<share_type>       transfer_amounts;
    map<account_id_type, uint32_t>  pay_pieces;
   // vector<uint32_t>         user_pieces;
    block_num_type           create_time;
    block_num_type           selttement_time;
    block_num_type           refund_time;
    uint32_t                 pieces;
    uint64_t                 sizes;
    uint32_t                 payed_pieces;
    share_type               transfer_one_piece_amount;
    share_type               author_one_piece_amount;
    share_type               receiver_author_one_piece_amount;

    std::string              sign_address;
	transaction_id_type      selttement_trx_id;
	transaction_id_type      refund_trx_id;
	transaction_id_type      receipt_trx_id;
	map<std::string,share_type>  increase_trx;
   
    torrent_payment() {
       
    }

    torrent_payment(const torrent_payment &o) {
        id = o.id;
        torrent_id = o.torrent_id;
        creator = o.creator;
        asset_type = o.asset_type;
        author_amount = o.author_amount;
        percent = o.percent;
        payment_amount = o.payment_amount;
        is_selttement = o.is_selttement;
        is_refund = o.is_refund;
        pay_pieces.clear();
        for( auto t : o.pay_pieces) {
            pay_pieces.insert(t);
        }
        create_time = o.create_time;
        selttement_time = o.selttement_time;
        refund_time = o.refund_time;
        pieces = o.pieces;
        sizes = o.sizes;
        payed_pieces = o.payed_pieces;

        transfer_one_piece_amount = o.transfer_one_piece_amount;
        author_one_piece_amount = o.author_one_piece_amount;
        receiver_author_one_piece_amount = o.receiver_author_one_piece_amount;

        sign_address = o.sign_address;

		selttement_trx_id = o.selttement_trx_id;
		refund_trx_id = o.refund_trx_id;
		receipt_trx_id = o.receipt_trx_id;
		increase_trx.clear();
        for( auto t : o.increase_trx) {
            increase_trx.insert(t);
        }
    }

    

};
 

}

FC_REFLECT(btp::torrent,  (id)(tx_id)(creator)(asset_type)(amount)(create_time)(last_modify)(pay_times)(finish_times)(percent))
FC_REFLECT(btp::torrent_payment, (id)(torrent_id)(creator)(asset_type)(author_amount)(percent)(payment_amount)(is_selttement)(is_refund)(pay_pieces)(create_time)(selttement_time)(refund_time)(pieces)(sizes)(payed_pieces)(transfer_one_piece_amount)(author_one_piece_amount)(receiver_author_one_piece_amount)(sign_address)(selttement_trx_id)(refund_trx_id)(receipt_trx_id)(increase_trx))
   
 
