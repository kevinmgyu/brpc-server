use torrent;
DROP TABLE IF EXISTS `torrent`;
	CREATE TABLE `torrent` (
			`id`             VARCHAR(40) CHARACTER SET latin1 NOT NULL PRIMARY KEY,
			`transaction_id` VARCHAR(40) CHARACTER SET latin1 NOT NULL,  
			`time`           DOUBLE NOT NULL NULL default 0,
			`create_block`   BIGINT NOT NULL default 0,
			`last_modify`    BIGINT NOT NULL default 0,
			`user_id`        VARCHAR(42) NOT NULL,
			`asset`          INT NOT NULL,
			`balance`        DECIMAL(40,16) NOT NULL,
			`percent`        INT NOT NULL DEFAULT 0, 
			`pay_times`      INT NOT NULL DEFAULT 0, 
			`finish_times`   INT NOT NULL DEFAULT 0, 
			`is_create`      TINYINT NOT NULL DEFAULT 0,
			INDEX `idx_torrent_userid` (`user_id`),
			INDEX `idx_torrent_transaction_id` (`transaction_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `torrent_payment`;
	CREATE TABLE `torrent_payment` (
			`payment_id`     VARCHAR(40) CHARACTER SET latin1 NOT NULL PRIMARY KEY,
			`torrent_id`     VARCHAR(40) CHARACTER SET latin1 NOT NULL, 
			`time`           DOUBLE NOT NULL NULL default 0,
			`create_time`      BIGINT NOT NULL default 0,
			`selttement_time`      BIGINT NOT NULL default 0,
			`refund_time`      BIGINT NOT NULL default 0,
			`user_id`        VARCHAR(42) NOT NULL,
			`asset`          INT NOT NULL,
			`pieces`         INT NOT NULL DEFAULT 0, 
			`payed_pieces`   INT NOT NULL DEFAULT 0, 
			`sizes`          BIGINT NOT NULL DEFAULT 0, 
			`percent`   INT NOT NULL DEFAULT 0, 
			`is_selttement`      TINYINT NOT NULL DEFAULT 0,
			`is_refund`      TINYINT NOT NULL DEFAULT 0,
			`sign_address`       VARCHAR(42) NOT NULL,
			`pay_pieces`       BLOB,
			`selttement_trx_id`     VARCHAR(40) CHARACTER SET latin1 NOT NULL, 
			`refund_trx_id`     VARCHAR(40) CHARACTER SET latin1 NOT NULL, 
			`receipt_trx_id`     VARCHAR(40) CHARACTER SET latin1 NOT NULL, 
			`increase_trx`     BLOB, 	
			`author_amount`   DECIMAL(40,16) NOT NULL,
			`payment_amount`   DECIMAL(40,16) NOT NULL,
			`transfer_one_piece_amount`   DECIMAL(40,16) NOT NULL,
			`author_one_piece_amount`   DECIMAL(40,16) NOT NULL,
			`receiver_author_one_piece_amount`   DECIMAL(40,16) NOT NULL,
			INDEX `idx_torrent_payment_userid` (`user_id`),
			INDEX `idx_torrent_payment_torrent_id` (`torrent_id`)
	) ENGINE=InnoDB DEFAULT CHARSET=utf8;
