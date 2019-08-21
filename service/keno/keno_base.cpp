#include "keno_base.h"
#include "utils.h"
#include "keno_config.h"
#include "keno_seed.h"
#include "keno_game.h"
#include "types.hpp"

//#include "vp_jacks_or_better.h"
//#include "vp_hand.h"
//#include "vp_anaylse.h"


/*string to_string(const share_type itttt) {

  std::stringstream ss;
  ss << string(itttt);
  return ss.str();
  }
 */
vector<string> split(const string &s, const string &seperator){
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while(i != s.size()){
		int flag = 0;
		while(i != s.size() && flag == 0){
			flag = 1;
			for(string_size x = 0; x < seperator.size(); ++x)
				if(s[i] == seperator[x]){
					++i;
					flag = 0;
					break;
				}
		}

		flag = 0;
		string_size j = i;
		while(j != s.size() && flag == 0){
			for(string_size x = 0; x < seperator.size(); ++x)
				if(s[j] == seperator[x]){
					flag = 1;
					break;
				}
			if(flag == 0) 
				++j;
		}
		if(i != j){
			result.push_back(s.substr(i, j-i));
			i = j;
		}
	}
	return result;
}

share_type mul_mpd_to_int64(string amount, string ratio)
{

	int prec = 0;
	mpd_t *base = decimal(amount.c_str(), 0);
	mpd_t *mulp = decimal(ratio.c_str(), 4);

	mpd_t *result = mpd_new(&mpd_ctx);
	// log_trace("1 %s",mpd_to_sci(base,0));
	// log_trace("2 %s",mpd_to_sci(a,0));
	mpd_mul(result, base, mulp, &mpd_ctx);
	//log_trace("3 %s",mpd_to_sci(result,0));
	if (prec)
	{
		mpd_rescale(result, result, -prec, &mpd_ctx);
	}
	//log_trace("4 %s",mpd_to_sci(result,0));
	char *str = mpd_to_sci(result, 0);
	string t(str);
	share_type t1(t);
	//log_trace("5 %s",t.c_str());
	//    uint64_t res;
	//    sscanf(t.c_str(),"%lu",&res);
	//uint64_t res = boost::lexical_cast<uint64_t>(t);

	free(str);
	mpd_del(base);
	mpd_del(mulp);
	mpd_del(result);

	return t1;
}

bonus_adr::bonus_adr()
{
	adr = "";
	base = 0;
	rate = 0;
}

bonus_adr::bonus_adr(const bonus_adr &g)
{ 
	adr = g.adr;
	base = g.base;
	rate = g.rate;
}

keno_area::keno_area()
{
	address = "";
	//  private_key ="";

	//asset_type = 0;
	//asset_name = "";
	//  chain_contract_address = "";

	// precision = 0;
	// transfer_amount = 0;
	channel = "";
	contract_id = "";
	bonus_adrs.clear();
	area.clear();
	area_list.clear();
	// rooms.clear();
	// rooms_bet.clear();
}

keno_area::keno_area(const keno_area &g)
{
	address = g.address;

	//id = g.id;


	//asset_type = g.asset_type;
	// asset_name = g.asset_name;
	// chain_type = g.chain_type;
	// chain_contract_address = g.chain_contract_address;
	//  precision = g.precision;
	// transfer_amount = g.transfer_amount;
	channel = g.channel;
	contract_id = g.contract_id;

	bonus_adrs.clear();
	for( auto t : g.bonus_adrs ) {
		bonus_adrs.insert(t);
	}
	area.clear();
	for( auto &t : g.area) {
		std::unordered_map<std::string,std::string> tmp ;
		tmp.clear();
		for(auto &t1:t.second){
			tmp.insert(t1);
		}
		area.insert(pair<int,std::unordered_map<std::string,std::string>>(t.first,tmp));
	}

	area_list.clear();
	for( auto t : g.area_list ) {
		area_list.push_back(t);
	}
	// rooms.clear();
	// for( auto t : g.rooms ) {
	//     rooms.insert(t);
	// }

	// rooms_bet.clear();
	// for( auto t : g.rooms_bet ) {
	//     rooms_bet.push_back(t);
	// }
}

bool is_digit(string message) {
	if( message.size() <= 0 || message.size() > 2 ) {
		return false;
	}

	//  if( message.size() <= 0  ) {
	//     return false;
	// }

	for(size_t i = 0; i < message.size(); ++i ) {
		char c = message[i];
		if( c < '0' || c > '9' ) {
			return false;
		}
	}

	return true;
}

bytetrade_pb::keno_game get_game_by_transfer_op_and_room( const transfer_op &op, const keno_area& ar) {
	bytetrade_pb::keno_game g;

	g.set_channel(ar.channel);
	g.set_external("");

	g.set_room_address(op.to);
	g.set_user_id(op.from);

	g.set_received_block_num(op.block_num);
	g.set_received_transaction_id(op.transaction_id);
	g.set_received_time(current_timestamp());

	g.set_payout_block_num(0);
	g.set_payout_transaction_id("");
	g.set_payout_time(0);

	g.set_asset_name("");
	g.set_bet_multiple(0);
	g.set_basic_amount(""); 
	g.set_amount(to_string(op.amount));
	g.set_asset_type(op.asset_type);

	g.set_shoe("");
	g.set_pick("");
	g.set_hit("");

	//  g.asset_name = ar.asset_name;

	g.set_block_hash(op.block_hash);
	g.set_server_seed_hash(g_seed_hash);

	g.set_user_id(op.from);
	g.set_payout_user_id("");
	g.set_pay_ratio(0);

	//g.hand_type = NOTHING;
	g.set_pay_out("0");


	if( ar.area.find(op.asset_type) == ar.area.end() ) {
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	if( !op.message) {
		LOG(ERROR)<<"[KENO_RESULT] message not exists";
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}
	// address||asset_type||basice_amount||beishu
	vector<string> a = split( *op.message, "," );

	if( a.size() != 5 ) {
		LOG(ERROR)<<"size not 5 . is "<<a.size()<<" ,message "<<(*op.message).c_str();
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	if( a[0] != ar.address ) {
		LOG(ERROR)<<"[KENO_RESULT] message error "<<a[0].c_str()<<" "<<ar.address.c_str();
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	if( is_digit(a[1]) == false ) {
		LOG(ERROR)<<"a1 not digit "<<a[1].c_str();
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}
	uint32_t asset_type = atoi(a[1].c_str());
	if( asset_type != op.asset_type) {
		LOG(ERROR)<<"asset type do not match "<<asset_type<<" "<<op.asset_type;
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	auto asset_iter =  ar.area.find(op.asset_type);
	if( asset_iter == ar.area.end()) {
		LOG(ERROR)<<"[KENO_RESULT]do not support asset type "<<op.asset_type;
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	//auto amount_iter = ar.area[op.asset_type].find(a[2]);
	//if( amount_iter == ar.area[op.asset_type].end()) {
	auto amount_iter = asset_iter->second.find(a[2]);
	if( amount_iter == asset_iter->second.end()) {
		LOG(ERROR)<<"[KENO_RESULT]do not support basic amount "<<a[2].c_str();
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	share_type one_bet = (share_type)a[2];

	if( is_digit(a[3]) == false ) {
		LOG(ERROR)<<"a3 not digit "<<a[3].c_str();
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}
	if( op.amount <= 0 ) {
		LOG(ERROR)<<"[KENO_RESULT] amount "<<to_string(op.amount).c_str();
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	share_type bet_numbers = op.amount / one_bet;	
	if (bet_numbers != atoi(a[3].c_str()))
	{
		LOG(ERROR)<<"[VIDEO_RESULT]input not match amount "<<to_string(op.amount).c_str()<<",basic amount "<<a[2].c_str()<<",bet_num "<<a[3].c_str();
		g.set_status(GAME_RECEIVED_FAILED);
		g.set_win_or_lose(false);
		return g;
	}

	set<int> pick;
	vector<string> pp = split( a[4], "_" );
	for( size_t i = 0; i < pp.size(); ++i ) {
		if( is_digit(pp[i]) == false ) {
			LOG(ERROR)<<"[KENO_RESULT] number "<<pp[i].c_str()<<",a[4]:"<<a[4].c_str();
			g.set_status(GAME_RECEIVED_FAILED);
			g.set_win_or_lose(false);
			return g;
		}
		pick.insert(atoi(pp[i].c_str()));
	}

	/*if( pick.size() != bet_numbers.to_integer() ) {
	  LOG(ERROR)<<"[KENO_RESULT] pick "<<pick.size()<<" not equal bet_number "<<bet_numbers.to_integer();
	  g.set_status(GAME_RECEIVED_FAILED);
	  g.set_win_or_lose(false);
	  return g;
	  }*/


	string shoe;
	set<int> hit;
	int pay = bytetrade::get_keno_result(g_seed_hash + op.block_hash + op.transaction_id, pick, shoe, hit);

	g.set_shoe(shoe);
	g.set_pick(fc::json::to_string(pick));
	g.set_hit(fc::json::to_string(hit));

	if( pay > 0 ) {
		share_type o = mul_mpd_to_int64(to_string(op.amount), to_string(pay));

		g.set_status(GAME_RECEIVED);
		g.set_win_or_lose(true);
		g.set_pay_ratio(pay);

		g.set_pay_out(to_string(o));// / ar.min_transfer_amount * ar.min_transfer_amount;
		LOG(INFO)<<"[KENO_RESULT] pay_out "<<to_string(o).c_str()<<" amount "<<g.pay_out().c_str();
	}  else {
		g.set_win_or_lose(false);
		g.set_pay_ratio(0);
		g.set_pay_out("0");
		g.set_status(GAME_FINISHED);

		LOG(INFO)<<"[KENO_RESULT] lose";
	}


	return g;
}


optional<bytetrade_pb::keno_game> game_merge(const bytetrade_pb::keno_game &g, const transfer_op &op)
{
	bytetrade_pb::keno_game g1(g);

	if( g1.status() != GAME_RECEIVED ) {
		LOG(ERROR)<<"game status error "<<g1.received_transaction_id().c_str()<<" "<<g1.status();
		return optional<bytetrade_pb::keno_game>();
	}

	g1.set_status(GAME_FINISHED);

	if (op.reason != ORR_OK) {
		g1.set_status(GAME_PAYOUT_FAILED);
	}

	if (op.amount != (share_type)g.pay_out()) {
		g1.set_status(GAME_PAYOUT_AMOUNT_FAILED);
	}

	g1.set_payout_user_id(op.from);
	g1.set_payout_transaction_id(op.transaction_id);
	g1.set_payout_time(current_timestamp());
	g1.set_payout_block_num(op.block_num);

	return optional<bytetrade_pb::keno_game>(g1);
}
