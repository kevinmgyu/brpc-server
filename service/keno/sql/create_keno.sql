use brpc;

DROP TABLE IF EXISTS `received`;
CREATE TABLE `received` (
	`received_transaction_id`         VARCHAR(65)  NOT NULL PRIMARY KEY,
	`received_time`          DOUBLE NOT NULL NULL default 0,
	`received_block_num`         BIGINT NOT NULL default 0,

	`payout_transaction_id`    VARCHAR(65) NOT NULL default "",
	`payout_time`          DOUBLE NOT NULL default 0,
	`payout_block_num`         BIGINT NOT NULL default 0,

	`block_hash`         VARCHAR(65) NOT NULL default "",
	`server_seed_hash`   VARCHAR(65) NOT NULL default "",
	
	`shoe`              VARCHAR(256) NOT NULL default "",
	`pick`              VARCHAR(256) NOT NULL default "",
	`hit`              VARCHAR(256) NOT NULL default "",
	
	`room_address`         VARCHAR(65) NOT NULL default "",
	`user_id`       VARCHAR(42) NOT NULL default "",
	
	 
	`asset_type`         INT NOT NULL  default 0,
	`asset_name`       VARCHAR(42) NOT NULL  default "",
	`amount`        VARCHAR(65) NOT NULL default "",
	`status`        SMALLINT NOT NULL default 0,
	`win_or_lose`       TINYINT(1) NOT NULL default 0,

	`payout_user_id`     VARCHAR(42) NOT NULL default "",
	`hand_type`        INT NOT NULL default 0,
	`pay_ratio`     INT NOT NULL default 0,
	`pay_out`       VARCHAR(65) NOT NULL default "",


	`external`       VARCHAR(1024) NOT NULL default "",
	`channel`       VARCHAR(65) NOT NULL default ""
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
 
 create index received_received_time on received(received_time);
 create index received_userid_received_time on received(user_id,received_time);
