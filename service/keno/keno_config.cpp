# include "keno_config.h"
# include "chain_api.h"
# include "bmath.hpp"

struct keno_settings keno_settings;
std::map<std::string, keno_area> bets;
int read_cfg_str(json_t *root, const char *key, char **val, const char *default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node && default_val) {
        *val = strdup(default_val);
        if (*val == NULL)
            return -__LINE__;
        return 0;
    }
    if (!node || !json_is_string(node))
        return -__LINE__;
    *val = strdup(json_string_value(node));
    if (*val == NULL)
        return -__LINE__;

    return 0;
}
int read_cfg_int(json_t *root, const char *key, int *val, bool required, int default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (!json_is_integer(node))
        return -__LINE__;
    *val = json_integer_value(node);

    return 0;
}

std::map<uint32_t, simple_asset> assets;

static int init_asset() {
	Response r = get_asset_list();
	if( r.code != 200 ) {
		printf("init asset failed %d",r.code);
		return 1;
	}
	Object o;
	o.parse(r.body);
	cout << "init_asset" << " " << get_asset_list().body << endl;

	Array result = o.get<Array>("result");
	//  cout << result.json() << endl;
	for( size_t i = 0; i < result.size();++i ) {
		Object coin =  result.get<Object>(i); 
		//cout << coin.json() << endl;
		simple_asset a;
		a.id = coin.get<Number>("id");
		a.symbol = coin.get<String>("symbol");
		// a.prec = coin.get<Number>("prec");
		if ( coin.has<String>("chain_contract_address") ) {
			a.chain_contract_address = coin.get<String>("chain_contract_address");
		} else {
			a.chain_contract_address = "";
		}

		if ( coin.has<Number>("chain_type") ) {
			a.chain_type = coin.get<Number>("chain_type");
		} else {
			a.chain_type = 0;
		}

		a.base_precision = coin.get<Number>("base_precision");
		a.min_transfer_amount = coin.get<Number>("min_trade_amount");
		cout << a.symbol << endl;

		assets[a.id] = a;

		//assets_map[a.symbol] = a.id;
	}

	return 0;
}

static int load_areas(const string address, json_t *root, const char *key) {
	json_t *node = json_object_get(root, key);
	if (!node || !json_is_array(node)) {
		return -__LINE__;
	}

	size_t area_num = json_array_size(node);
	area_inner *areas = (area_inner*)malloc(sizeof(struct area_inner) * area_num);

	//cout << asset.symbol << " " << room_num << endl;

	for (size_t i = 0; i < area_num; ++i) {

		json_t *row = json_array_get(node, i);
		if (!json_is_object(row))
			return -__LINE__;

		//cout << asset.symbol << " " << i << endl;

		ERR_RET_LN(read_cfg_int(row, "asset_type", &areas[i].asset_type, true, 1));

		if( assets.find(areas[i].asset_type) == assets.end() ) {
			return -__LINE__;
		}

		simple_asset sa = assets[areas[i].asset_type];
		 
		bets[address].area_list.push_back(areas[i].asset_type);
		
		json_t *transfer_amounts = json_object_get(row, "transfer_amount");
		int cnt = 0;

		if (json_is_array(transfer_amounts)) {
			cnt = json_array_size(transfer_amounts);
			if (cnt == 0)
				return -__LINE__;

			for (int j = 0; j < cnt; ++j) {
				json_t *row1 = json_array_get(transfer_amounts, j);
					if (!json_is_string(row1))
						return -__LINE__;
					const char* value = json_string_value(row1);
					std::string key = value;
					LOG(INFO)<<"amount value: "<<value;
					mpd_t* mb_mpd = decimal(value,8);
					string tmp_amount = mul_mpd_to_int64(mb_mpd, sa.base_precision, 8);
					
					bets[address].area[areas[i].asset_type].insert(pair<std::string,std::string>(tmp_amount,key));
					
					mpd_del(mb_mpd);
			}   
		} else {
			return -__LINE__;
		}

		// room r;
		// r.bet = rooms[i].bet;
		// r.pay_ratio = rooms[i].pay_ratio;

		// r.min_bet_string = rooms[i].min_bet;
		/*mpd_t* mb_mpd = decimal(areas[i].transfer_amount,8);
		bets[address].area[areas[i].asset_type] = mul_mpd_to_int64(mb_mpd, sa.base_precision, 8);
		mpd_del(mb_mpd);*/

		// r.max_bet_string = rooms[i].max_bet;
		// mb_mpd = decimal(rooms[i].max_bet,8);
		// r.max_bet = mul_mpd_to_int64(mb_mpd, asset.base_precision, 8);
		// mpd_del(mb_mpd);

		// cout << asset.symbol << " " << r.bet << " " << to_string(r.min_bet) << " " << to_string(r.max_bet) << endl;
		// .rooms[r.bet] = r;
		// bets[address].rooms_bet.push_back(r.bet);
	}

	
		for(auto t:bets[address].area)
		{
			LOG(INFO)<<t.first;
			for(auto t1:t.second)
			{
				LOG(INFO)<<t1.first.c_str();
			}
		}
	return 0;
}

static int load_bonus(const string address, json_t *root, const char *key) {
	json_t *node = json_object_get(root, key);
	if (!node || !json_is_array(node)) {
		return -__LINE__;
	}

	size_t bonus_num = json_array_size(node);
	bonus_adr_inner *bonus = (bonus_adr_inner*)malloc(sizeof(struct bonus_adr_inner) * bonus_num);


	for (size_t i = 0; i < bonus_num; ++i) {

		json_t *row = json_array_get(node, i);
		if (!json_is_object(row))
			return -__LINE__;

		//cout << asset.symbol << " " << i << endl;

		ERR_RET_LN(read_cfg_str(row, "adr", &bonus[i].adr, NULL));
		ERR_RET_LN(read_cfg_int(row, "base", &bonus[i].base, true, 10000));
		ERR_RET_LN(read_cfg_int(row, "rate", &bonus[i].rate, true, 10));

		bonus_adr b;
		b.adr = bonus[i].adr;
		b.base = bonus[i].base;
		b.rate = bonus[i].rate;


		cout <<b.adr.c_str() << " " << b.base << " " <<b.rate <<  endl;
		bets[address].bonus_adrs[i] = b;

	}

	return 0;
}

static int load_rooms(json_t *root, const char *key) {
	json_t *node = json_object_get(root, key);
	if (!node || !json_is_array(node)) {
		return -__LINE__;
	}

	keno_settings.room_num = json_array_size(node);
	keno_settings.rooms = (room_inner*)malloc(sizeof(struct area_inner) * keno_settings.room_num);
	for (size_t i = 0; i < keno_settings.room_num; ++i) {
		json_t *row = json_array_get(node, i);
		if (!json_is_object(row))
			return -__LINE__;

		ERR_RET_LN(read_cfg_str(row, "address", &keno_settings.rooms[i].address, NULL));
		// if( is_in_video_poker_server ) {
		//     ERR_RET_LN(read_cfg_str(row, "id", &video_poker_settings.areas[i].id, NULL));
		// }

		// ERR_RET_LN(read_cfg_int(row, "asset_type", &video_poker_settings.areas[i].asset_type, true, 1));
		// ERR_RET_LN(read_cfg_str(row, "asset_name", &video_poker_settings.areas[i].asset_name, NULL));
		ERR_RET_LN(read_cfg_str(row, "channel", &keno_settings.rooms[i].channel, NULL));
		ERR_RET_LN(read_cfg_str(row, "contract_id", &keno_settings.rooms[i].contract_id, NULL));

		

		keno_area a;
		a.address = keno_settings.rooms[i].address;
		// if( is_in_video_poker_server ) {
		//     a.id = video_poker_settings.areas[i].id;
		// }
		// a.asset_type = video_poker_settings.areas[i].asset_type;
		// a.asset_name = video_poker_settings.areas[i].asset_name;
		// a.chain_type = sa.chain_type;
		// a.chain_contract_address = sa.chain_contract_address;
		a.channel = keno_settings.rooms[i].channel;
		a.contract_id = keno_settings.rooms[i].contract_id;
		// a.precision = sa.base_precision;
		// a.min_transfer_amount = sa.min_transfer_amount;

		bets[a.address] = a;

		LOG(INFO)<<keno_settings.rooms[i].address;

		int ret = load_bonus(a.address, row, "bonus");
		if (ret < 0) {
			printf("load_bonus fail: %d\n", ret);
			return -__LINE__;
		}

		ret = load_areas(a.address, row, "areas");
		if (ret < 0) {
			printf("load_areas fail: %d\n", ret);
			return -__LINE__;
		}
	}

	return 0;
}

static int read_config_from_json(json_t *root) {
	int ret;

  	/*ret = read_cfg_bool(root, "debug", &keno_settings.debug, false, false);
    if (ret < 0) {
        printf("read debug config fail: %d\n", ret);
        return -__LINE__;
    }
    ret = load_cfg_process(root, "process", &keno_settings.process);
    if (ret < 0) {
        printf("load process config fail: %d\n", ret);
        return -__LINE__;
    }
    ret = load_cfg_log(root, "log", &keno_settings.log);
    if (ret < 0) {
        printf("load log config fail: %d\n", ret);
        return -__LINE__;
    }
    // ret = load_cfg_alert(root, "alert", &fl_server_settings.alert);
    // if (ret < 0) {
    //     printf("load alert config fail: %d\n", ret);
    //     return -__LINE__;
    // }
    
    ret = load_cfg_rpc_svr(root, "svr", &keno_settings.svr);
    if (ret < 0) {
        printf("load svr config fail: %d\n", ret);
        return -__LINE__;
    }

	ret = load_cfg_mysql(root, "db_history", &keno_settings.db_history);
	if (ret < 0) {
		printf("load history db config fail: %d\n", ret);
		return -__LINE__;
	}

	ret = read_cfg_int(root, "history_thread", &keno_settings.history_thread, false, 10);
	if (ret < 0) {
		printf("load history_thread fail: %d", ret);
		return -__LINE__;
	}


	ret = read_cfg_str(root, "http_url", &keno_settings.http_url, NULL);
	if (ret < 0) {
		printf("load httpurl fail: %d\n", ret);
		return -__LINE__;
	}


	ret = read_cfg_str(root, "transfer_seed_from", &keno_settings.transfer_seed_from, NULL);
	if (ret < 0) {
		printf("load transfer_seed_from fail: %d\n", ret);
		return -__LINE__;
	}

	// ret = read_cfg_str(root, "seed_id", &video_poker_settings.seed_id, NULL);
	// if (ret < 0) {
	// 	printf("load seed_id fail: %d\n", ret);
	// 	return -__LINE__;
	// }


	ret = read_cfg_int(root, "seed_change_num", &keno_settings.seed_change_num, false, 100);
	if (ret < 0) {
		printf("load seed_change_num fail: %d", ret);
		return -__LINE__;
	}

	ret = read_cfg_str(root, "transfer_seed_to", &keno_settings.transfer_seed_to, NULL);
	if (ret < 0) {
		printf("load transfer_seed_to fail: %d\n", ret);
		return -__LINE__;
	}

	ret = read_cfg_str(root, "sign_ip", &keno_settings.sign_ip, NULL);
	if (ret < 0) {
		printf("load sign_ip fail: %d\n", ret);
		return -__LINE__;
	}

	ret = read_cfg_str(root, "sign_port", &keno_settings.sign_port, NULL);
	if (ret < 0) {
		printf("load sign_port fail: %d\n", ret);
		return -__LINE__;
	}

	ret = read_cfg_int(root, "sign_fn_cnt", &keno_settings.sign_fd_cnt, false, 20);
	if (ret < 0) {
		printf("load sign_fn_cnt fail: %d", ret);
		return -__LINE__;
	}

	ret = read_cfg_int(root, "sign_timeout", &keno_settings.sign_timeout, false, 100);
	if (ret < 0) {
		printf("load sign_timeout fail: %d", ret);
		return -__LINE__;
	}*/

	set_btt_base_url(FLAGS_http_url);
	LOG(INFO)<<FLAGS_http_url.c_str()<<"."; 

	ret = init_asset();
	if(ret != 0 ) {
		printf("init asset failed");
		return -__LINE__;
	}

	ret = load_rooms(root, "rooms");
	if (ret < 0) {
		printf("load_areas fail: %d\n", ret);
		return -__LINE__;
	}
 
	/*ret = read_cfg_str(root, "brokers", &keno_settings.brokers, NULL);
	if (ret < 0) {
		printf("load brokers fail: %d\n", ret);
		return -__LINE__;
	}*/
	 
	return 0;
}

int init_keno_rooms_config(const char *path) {

	/*int ret = init_fl_config(path);
	if( ret != 0 ) {
	    printf("init_fl_config failed %s", path);
	    return -__LINE__;
	}*/

	json_error_t error;
	json_t *root = json_load_file(path, 0, &error);
	if (root == NULL) {
		printf("json_load_file from: %s fail: %s in line: %d\n", path, error.text, error.line);
		return -__LINE__;
	}

	if (!json_is_object(root)) {
		json_decref(root);
		return -__LINE__;
	}

	int ret = read_config_from_json(root);
	if (ret < 0) {
		json_decref(root);
		return ret;
	}
	json_decref(root);

	return 0;
}

