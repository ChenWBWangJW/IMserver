DROP DATABASE IF EXISTS chatdb;
create database chatdb default character set utf8 collate utf8_bin;

flush privileges;

use chatdb;
SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

SET FOREIGN_KEY_CHECKS = 0;
-- -----------------------------
-- Table structure for qqnum
-- -----------------------------
DROP TABLE IF EXISTS `qqnum`;
CREATE TABLE `qqnum` (
`id`   int(11)     NOT NULL      AUTO_INCREMENT,
`name` varchar(50) DEFAULT NULL,
PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
