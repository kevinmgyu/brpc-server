use brpc;

DROP TABLE IF EXISTS `keno_seed`;
CREATE TABLE `keno_seed` (
		`id`            BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
		`time`          DOUBLE NOT NULL,
		`start_block_num`     BIGINT NOT NULL,
		`end_block_num`     BIGINT NOT NULL,
		`server_seed_hash`   VARCHAR(65) NOT NULL,
		`seed`    VARCHAR(1024) NOT NULL,
		`seed_hash_transaction`  VARCHAR(65) NOT NULL
		) ENGINE=InnoDB DEFAULT CHARSET=utf8;
create index seed_block_num on keno_seed(start_block_num,end_block_num);

DROP TABLE IF EXISTS `keno_seed_publish`;
CREATE TABLE `keno_seed_publish` (
		`start_block_num`  BIGINT UNSIGNED NOT NULL PRIMARY KEY  ,
		`time`          DOUBLE NOT NULL,
		`end_block_num`     BIGINT NOT NULL,
		`server_seed_hash`   VARCHAR(65) NOT NULL,
		`seed`    VARCHAR(1024) NOT NULL,
		`seed_hash_transaction`  VARCHAR(65) NOT NULL,
		`seed_transaction`    VARCHAR(65) NOT NULL
		) ENGINE=InnoDB DEFAULT CHARSET=utf8;
create index seed_block_num on keno_seed_publish(start_block_num,end_block_num);
