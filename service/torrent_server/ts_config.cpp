# include "ts_config.h"
//# include "chain_api.h"
//# include "bmath.hpp"


std::map<int, simple_asset> assets;

optional<simple_asset> get_asset_by_id(int id) {
	if( assets.find(id) == assets.end() ) {
    	return optional<simple_asset>();
    }
    return optional<simple_asset>(assets[id]);
}
int init_asset() {
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
 


