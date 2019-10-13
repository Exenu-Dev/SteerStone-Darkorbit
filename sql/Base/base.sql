-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               5.6.44-log - MySQL Community Server (GPL)
-- Server OS:                    Win64
-- HeidiSQL Version:             10.2.0.5599
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


-- Dumping database structure for game
CREATE DATABASE IF NOT EXISTS `game` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `game`;

-- Dumping structure for table game.accounts
CREATE TABLE IF NOT EXISTS `accounts` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Account Id',
  `session_id` varchar(255) NOT NULL COMMENT 'Account Session Id',
  `username` varchar(30) NOT NULL COMMENT 'Account Username',
  `uridium` int(10) unsigned NOT NULL COMMENT 'Account Uridium',
  `credits` int(10) unsigned NOT NULL COMMENT 'Account Credits',
  `jackpot` int(10) unsigned NOT NULL COMMENT 'Account JackPot',
  `experience` int(10) unsigned NOT NULL COMMENT 'Account Ship Experience',
  `honor` int(10) NOT NULL COMMENT 'Account Ship Honor level',
  `gates_achieved` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Account Ship Gates Achieved',
  `clan_id` int(10) unsigned NOT NULL COMMENT 'Account Ship Clan Id',
  `clan_name` varchar(4) NOT NULL DEFAULT '' COMMENT 'Account Ship Clan Name',
  `company` smallint(5) unsigned NOT NULL COMMENT 'Account Ship Company',
  `rank` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'Account Ship Rank',
  `premium` tinyint(1) unsigned NOT NULL COMMENT 'Account Premium',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

-- Dumping data for table game.accounts: ~3 rows (approximately)
/*!40000 ALTER TABLE `accounts` DISABLE KEYS */;
INSERT INTO `accounts` (`id`, `session_id`, `username`, `uridium`, `credits`, `jackpot`, `experience`, `honor`, `gates_achieved`, `clan_id`, `clan_name`, `company`, `rank`, `premium`) VALUES
	(2, '95412faa0b504375075f561adeaaa104', 'Exenu', 30, 10800, 0, 37800, 54, 1, 0, '', 3, 1, 0),
	(3, '95412faa0b504375075f561adeaaa104', 'John', 32323, 3123123, 5, 0, -6000, 4, 1, 'MOTU', 2, 1, 1),
	(4, '95412faa0b504375075f561adeaaa105', 'Dummy', 1, 1, 1, 1, 1, 0, 0, '', 1, 1, 0);
/*!40000 ALTER TABLE `accounts` ENABLE KEYS */;

-- Dumping structure for table game.account_drones
CREATE TABLE IF NOT EXISTS `account_drones` (
  `id` int(10) unsigned NOT NULL,
  `type` mediumint(5) unsigned NOT NULL DEFAULT '0' COMMENT '0 - Flak - 1 Iris',
  `points` mediumint(5) unsigned NOT NULL DEFAULT '0',
  KEY `FK_account_drones_accounts` (`id`),
  CONSTRAINT `FK_account_drones_accounts` FOREIGN KEY (`id`) REFERENCES `accounts` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table game.account_drones: ~8 rows (approximately)
/*!40000 ALTER TABLE `account_drones` DISABLE KEYS */;
INSERT INTO `account_drones` (`id`, `type`, `points`) VALUES
	(3, 1, 200),
	(3, 1, 400),
	(2, 1, 1600),
	(2, 1, 1600),
	(2, 1, 1600),
	(2, 1, 1600),
	(2, 1, 1600),
	(2, 1, 1600),
	(2, 1, 1600),
	(2, 1, 1600);
/*!40000 ALTER TABLE `account_drones` ENABLE KEYS */;

-- Dumping structure for table game.account_inventory
CREATE TABLE IF NOT EXISTS `account_inventory` (
  `id` int(10) unsigned NOT NULL,
  `entry` int(10) unsigned NOT NULL,
  `count` int(10) unsigned NOT NULL,
  `equiped` tinyint(1) unsigned NOT NULL DEFAULT '0',
  KEY `FK__accounts` (`id`),
  CONSTRAINT `FK__accounts` FOREIGN KEY (`id`) REFERENCES `accounts` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table game.account_inventory: ~4 rows (approximately)
/*!40000 ALTER TABLE `account_inventory` DISABLE KEYS */;
INSERT INTO `account_inventory` (`id`, `entry`, `count`, `equiped`) VALUES
	(2, 4, 15, 1),
	(3, 4, 10, 1),
	(3, 15, 2, 1),
	(2, 15, 2, 1);
/*!40000 ALTER TABLE `account_inventory` ENABLE KEYS */;

-- Dumping structure for table game.account_settings
CREATE TABLE IF NOT EXISTS `account_settings` (
  `id` int(10) unsigned NOT NULL,
  `display_boost` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_damage` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_all_las` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_exploration` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_name` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_firm_icon` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_alpha_bg` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `ignore_res` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `ignore_box` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `convert_gates` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `convert_oppo` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `sound_off` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `bg_music_off` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `display_status` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_bubble` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `selected_laser` mediumint(5) unsigned NOT NULL DEFAULT '1',
  `selected_rocket` mediumint(5) unsigned NOT NULL DEFAULT '1',
  `display_digits` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_chat` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `display_drones` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `show_star_system` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `ignore_cargo` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `ignore_hostile_cargo` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `auto_change_ammo` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `enable_buy_fast` tinyint(1) unsigned NOT NULL DEFAULT '0',
  KEY `account_settings.id` (`id`),
  CONSTRAINT `account_settings.id` FOREIGN KEY (`id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table game.account_settings: ~3 rows (approximately)
/*!40000 ALTER TABLE `account_settings` DISABLE KEYS */;
INSERT INTO `account_settings` (`id`, `display_boost`, `display_damage`, `display_all_las`, `display_exploration`, `display_name`, `display_firm_icon`, `display_alpha_bg`, `ignore_res`, `ignore_box`, `convert_gates`, `convert_oppo`, `sound_off`, `bg_music_off`, `display_status`, `display_bubble`, `selected_laser`, `selected_rocket`, `display_digits`, `display_chat`, `display_drones`, `show_star_system`, `ignore_cargo`, `ignore_hostile_cargo`, `auto_change_ammo`, `enable_buy_fast`) VALUES
	(2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1),
	(3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1),
	(4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1);
/*!40000 ALTER TABLE `account_settings` ENABLE KEYS */;

-- Dumping structure for table game.account_ship
CREATE TABLE IF NOT EXISTS `account_ship` (
  `id` int(10) unsigned NOT NULL COMMENT 'Account Id',
  `type` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'Phoenix Default',
  `speed` int(10) unsigned NOT NULL DEFAULT '320',
  `shield` int(10) NOT NULL DEFAULT '2000',
  `max_shield` int(10) unsigned NOT NULL DEFAULT '2000',
  `hit_points` int(10) NOT NULL DEFAULT '4000',
  `max_hit_points` int(10) unsigned NOT NULL DEFAULT '4000',
  `cargo_space` int(10) unsigned NOT NULL DEFAULT '0',
  `cargo_space_max` int(10) unsigned NOT NULL DEFAULT '100',
  `position_x` float unsigned NOT NULL DEFAULT '0',
  `position_y` float unsigned NOT NULL DEFAULT '0',
  `map_id` int(10) unsigned NOT NULL DEFAULT '1',
  `max_battery` int(10) unsigned NOT NULL DEFAULT '2000',
  `max_rockets` int(10) unsigned NOT NULL DEFAULT '100',
  `use_system_font` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `prometium` int(10) unsigned NOT NULL DEFAULT '0',
  `endurium` int(10) unsigned NOT NULL DEFAULT '0',
  `terbium` int(10) unsigned NOT NULL DEFAULT '0',
  `xenomit` int(10) unsigned NOT NULL DEFAULT '0',
  `prometid` int(10) unsigned NOT NULL DEFAULT '0',
  `duranium` int(10) unsigned NOT NULL DEFAULT '0',
  `promerium` int(10) unsigned NOT NULL DEFAULT '0',
  `palladium` int(10) unsigned NOT NULL DEFAULT '0',
  `seprom` int(10) unsigned NOT NULL DEFAULT '0',
  `battery_lcb_10` int(10) NOT NULL DEFAULT '1000',
  `battery_mcb_25` int(10) NOT NULL DEFAULT '0',
  `battery_mcb_50` int(10) NOT NULL DEFAULT '0',
  `battery_ucb_100` int(10) NOT NULL DEFAULT '0',
  `battery_sab_50` int(10) NOT NULL DEFAULT '0',
  `rocket_r310` int(10) NOT NULL DEFAULT '100',
  `rocket_plt_2026` int(10) NOT NULL DEFAULT '0',
  `rocket_plt_2021` int(10) NOT NULL DEFAULT '0',
  `mines` int(10) NOT NULL DEFAULT '0',
  `smart_bombs` int(10) NOT NULL DEFAULT '0',
  `instant_shields` int(10) NOT NULL DEFAULT '0',
  KEY `FK_account_ship_accounts_template` (`id`),
  CONSTRAINT `FK_account_ship_accounts_template` FOREIGN KEY (`id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table game.account_ship: ~3 rows (approximately)
/*!40000 ALTER TABLE `account_ship` DISABLE KEYS */;
INSERT INTO `account_ship` (`id`, `type`, `speed`, `shield`, `max_shield`, `hit_points`, `max_hit_points`, `cargo_space`, `cargo_space_max`, `position_x`, `position_y`, `map_id`, `max_battery`, `max_rockets`, `use_system_font`, `prometium`, `endurium`, `terbium`, `xenomit`, `prometid`, `duranium`, `promerium`, `palladium`, `seprom`, `battery_lcb_10`, `battery_mcb_25`, `battery_mcb_50`, `battery_ucb_100`, `battery_sab_50`, `rocket_r310`, `rocket_plt_2026`, `rocket_plt_2021`, `mines`, `smart_bombs`, `instant_shields`) VALUES
	(2, 9, 380, 2000, 2000, 500000, 4000, 0, 100, 18795, 11213, 1, 2000, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 20, 30, 40, 50, 10, 10, 10, 5, 5, 10),
	(3, 10, 380, 2000, 2000, 500000, 4000, 100, 100, 7948, 3406, 2, 2000, 100, 1, 30, 30, 40, 0, 0, 0, 0, 0, 0, 10, 20, 30, 40, 50, 10, 10, 10, 5, 5, 10),
	(4, 3, 380, 2000, 2000, 500000, 4000, 0, 100, 18500, 10000, 2, 2000, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 20, 30, 40, 50, 10, 10, 10, 5, 5, 10);
/*!40000 ALTER TABLE `account_ship` ENABLE KEYS */;

-- Dumping structure for table game.item_template
CREATE TABLE IF NOT EXISTS `item_template` (
  `entry` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL DEFAULT '',
  `type` mediumint(5) NOT NULL,
  `gfx` mediumint(5) NOT NULL,
  `value` int(10) NOT NULL,
  `percentage_value` int(10) NOT NULL DEFAULT '0',
  `credits` int(10) NOT NULL,
  `uridium` int(10) NOT NULL,
  PRIMARY KEY (`entry`)
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=latin1;

-- Dumping data for table game.item_template: ~13 rows (approximately)
/*!40000 ALTER TABLE `item_template` DISABLE KEYS */;
INSERT INTO `item_template` (`entry`, `name`, `type`, `gfx`, `value`, `percentage_value`, `credits`, `uridium`) VALUES
	(1, 'LF-1', 0, 1, 40, 0, 10000, 0),
	(2, 'MP-1', 0, 2, 60, 0, 40000, 0),
	(3, 'LF-2', 0, 3, 100, 0, 250000, 0),
	(4, 'LF-3', 0, 4, 150, 0, 0, 10000),
	(5, 'G3N-1010', 1, 1, 2, 0, 2000, 0),
	(6, 'G3N-2010', 1, 2, 3, 0, 4000, 0),
	(7, 'G3N-3210', 1, 3, 4, 0, 8000, 0),
	(8, 'G3N-3310', 1, 4, 5, 0, 16000, 0),
	(9, 'G3N-6900', 1, 5, 7, 0, 50000, 0),
	(10, 'G3N-7900', 1, 6, 10, 0, 0, 2000),
	(11, 'SG3N-A01', 2, 7, 1000, 40, 8000, 0),
	(12, 'SG3N-A02', 2, 8, 2000, 50, 16000, 0),
	(13, 'SG3N-B01', 2, 9, 4000, 70, 256000, 0),
	(14, 'SG3M-A03', 2, 10, 5000, 60, 128000, 0),
	(15, 'SG3N-B02', 2, 11, 10000, 80, 0, 10000);
/*!40000 ALTER TABLE `item_template` ENABLE KEYS */;

-- Dumping structure for table game.mobs
CREATE TABLE IF NOT EXISTS `mobs` (
  `entry` int(10) unsigned NOT NULL,
  `map_id` int(10) unsigned NOT NULL,
  `min_count` int(10) unsigned NOT NULL,
  `max_count` int(10) unsigned NOT NULL,
  KEY `FK_mobs_mob_template` (`entry`),
  CONSTRAINT `FK_mobs_mob_template` FOREIGN KEY (`entry`) REFERENCES `mob_template` (`entry`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table game.mobs: ~39 rows (approximately)
/*!40000 ALTER TABLE `mobs` DISABLE KEYS */;
INSERT INTO `mobs` (`entry`, `map_id`, `min_count`, `max_count`) VALUES
	(10, 1, 0, 2),
	(1, 2, 0, 2),
	(2, 2, 0, 2),
	(2, 3, 0, 1),
	(3, 3, 1, 1),
	(4, 3, 0, 1),
	(5, 3, 0, 1),
	(1, 5, 1, 3),
	(1, 6, 1, 3),
	(2, 6, 1, 2),
	(2, 7, 1, 2),
	(3, 7, 1, 2),
	(5, 7, 1, 1),
	(1, 9, 1, 3),
	(1, 10, 1, 3),
	(2, 10, 1, 2),
	(2, 11, 1, 2),
	(3, 11, 1, 2),
	(4, 11, 1, 2),
	(5, 11, 0, 1),
	(3, 4, 1, 1),
	(4, 4, 1, 1),
	(6, 8, 0, 1),
	(2, 4, 1, 2),
	(2, 8, 1, 2),
	(3, 8, 1, 1),
	(4, 8, 1, 1),
	(6, 12, 0, 1),
	(2, 12, 1, 2),
	(3, 12, 1, 1),
	(4, 12, 1, 1),
	(6, 4, 0, 1),
	(2, 17, 1, 3),
	(7, 17, 1, 2),
	(8, 17, 0, 1),
	(2, 21, 1, 3),
	(7, 21, 1, 2),
	(8, 21, 0, 1),
	(2, 25, 1, 3),
	(7, 25, 1, 2),
	(8, 25, 0, 1);
/*!40000 ALTER TABLE `mobs` ENABLE KEYS */;

-- Dumping structure for table game.mob_template
CREATE TABLE IF NOT EXISTS `mob_template` (
  `entry` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `type` smallint(5) unsigned NOT NULL,
  `weapon_state` smallint(5) unsigned NOT NULL DEFAULT '1',
  `hit_points` int(10) NOT NULL,
  `shield` int(10) NOT NULL,
  `shield_resistance` int(10) NOT NULL,
  `min_damage` int(10) unsigned NOT NULL,
  `max_damage` int(10) unsigned NOT NULL,
  `behaviour` smallint(5) unsigned NOT NULL,
  `respawn_timer` int(10) unsigned NOT NULL,
  `min_movement_time` int(10) unsigned NOT NULL,
  `max_movement_time` int(10) unsigned NOT NULL,
  `speed` int(10) unsigned NOT NULL,
  `experience` int(10) unsigned NOT NULL,
  `honor` int(10) unsigned NOT NULL,
  `credits` int(10) unsigned NOT NULL,
  `uridium` int(10) unsigned NOT NULL,
  `prometium` int(10) unsigned NOT NULL,
  `endurium` int(10) unsigned NOT NULL,
  `terbium` int(10) unsigned NOT NULL,
  `prometid` int(10) unsigned NOT NULL,
  `duranium` int(10) unsigned NOT NULL,
  `promerium` int(10) unsigned NOT NULL,
  `xenomit` int(10) unsigned NOT NULL,
  `seprom` int(10) unsigned NOT NULL,
  `palladium` int(10) unsigned NOT NULL,
  PRIMARY KEY (`entry`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=latin1;

-- Dumping data for table game.mob_template: ~12 rows (approximately)
/*!40000 ALTER TABLE `mob_template` DISABLE KEYS */;
INSERT INTO `mob_template` (`entry`, `name`, `type`, `weapon_state`, `hit_points`, `shield`, `shield_resistance`, `min_damage`, `max_damage`, `behaviour`, `respawn_timer`, `min_movement_time`, `max_movement_time`, `speed`, `experience`, `honor`, `credits`, `uridium`, `prometium`, `endurium`, `terbium`, `prometid`, `duranium`, `promerium`, `xenomit`, `seprom`, `palladium`) VALUES
	(1, '-=[ Streuner ]=-', 2, 1, 800, 400, 20, 20, 20, 0, 20000, 0, 5000, 280, 400, 2, 400, 1, 10, 10, 20, 0, 0, 0, 0, 0, 0),
	(2, '-=[ Lordakia ]=-', 71, 1, 1600, 2000, 20, 80, 80, 1, 20000, 0, 6000, 320, 800, 4, 800, 2, 20, 20, 20, 0, 0, 0, 0, 0, 0),
	(3, '-=[ Saimon ]=-', 75, 1, 6000, 3000, 20, 200, 200, 1, 20000, 0, 8000, 320, 1600, 8, 1600, 4, 40, 40, 40, 2, 2, 0, 0, 0, 0),
	(4, '-=[ Mordon ]=-', 73, 1, 20000, 10000, 20, 390, 390, 1, 20000, 0, 16000, 125, 3200, 15, 6400, 8, 80, 80, 80, 8, 8, 1, 0, 0, 0),
	(5, '-=[ Devolarium ]=-', 72, 1, 100000, 100000, 20, 1200, 1200, 1, 20000, 0, 24000, 300, 6400, 32, 51200, 16, 100, 100, 100, 16, 16, 2, 0, 0, 0),
	(6, '-=[ Sibelon ]=-', 74, 1, 200000, 200000, 20, 2650, 2650, 1, 20000, 0, 10000, 100, 12800, 64, 102400, 32, 200, 200, 200, 32, 32, 4, 0, 0, 0),
	(7, '-=[ Sibelonit ]=-', 76, 1, 40000, 40000, 20, 1000, 1500, 1, 20000, 0, 32000, 300, 3200, 16, 12800, 12, 100, 100, 100, 8, 8, 1, 0, 0, 0),
	(8, '-=[ Lordakium ]=-', 77, 1, 300000, 200000, 20, 3150, 3600, 1, 20000, 0, 12000, 300, 25600, 128, 204800, 64, 300, 300, 300, 64, 64, 8, 0, 0, 0),
	(9, '-=[ Kristallin ]=-', 78, 1, 50000, 40000, 20, 1500, 1650, 1, 20000, 0, 8000, 300, 6400, 32, 12800, 16, 100, 100, 100, 16, 16, 1, 0, 0, 0),
	(10, '-=[ Kristallon ]=-', 79, 1, 400000, 300000, 20, 4450, 4450, 0, 20000, 0, 12000, 250, 51200, 256, 409600, 128, 300, 300, 300, 128, 128, 16, 0, 0, 0),
	(11, '-=[ StreuneR ]=-', 4, 3, 20000, 10000, 20, 1000, 1000, 1, 20000, 0, 5000, 300, 6000, 30, 12000, 15, 160, 160, 160, 15, 15, 1, 4, 0, 0),
	(13, '-=[ Cubikon ]=-', 80, 1, 1600000, 1200000, 20, 0, 0, 0, 20000, 0, 60000, 100, 512000, 4096, 1638400, 1024, 1200, 1200, 1200, 512, 512, 64, 128, 0, 0);
/*!40000 ALTER TABLE `mob_template` ENABLE KEYS */;

-- Dumping structure for table game.portal_template
CREATE TABLE IF NOT EXISTS `portal_template` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `portal_id` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(6) NOT NULL DEFAULT 'Portal',
  `company` smallint(5) unsigned NOT NULL,
  `type` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'Normal Portal',
  `map_id` int(10) unsigned NOT NULL,
  `position_x` float unsigned NOT NULL,
  `position_y` float unsigned NOT NULL,
  `to_map_id` int(10) unsigned NOT NULL,
  `to_position_x` float unsigned NOT NULL,
  `to_position_y` float unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=83 DEFAULT CHARSET=latin1;

-- Dumping data for table game.portal_template: ~82 rows (approximately)
/*!40000 ALTER TABLE `portal_template` DISABLE KEYS */;
INSERT INTO `portal_template` (`id`, `portal_id`, `name`, `company`, `type`, `map_id`, `position_x`, `position_y`, `to_map_id`, `to_position_x`, `to_position_y`) VALUES
	(1, 1, 'Portal', 1, 1, 1, 18500, 11500, 2, 2000, 2000),
	(2, 1, 'Portal', 1, 1, 2, 2000, 2000, 1, 18500, 11500),
	(3, 3, 'Portal', 1, 1, 2, 18500, 11500, 4, 2000, 2000),
	(4, 2, 'Portal', 1, 1, 2, 18500, 2000, 3, 2000, 11500),
	(5, 1, 'Portal', 1, 1, 3, 2000, 11500, 2, 18500, 2000),
	(6, 2, 'Portal', 1, 1, 3, 18500, 11500, 4, 18500, 2000),
	(7, 3, 'Portal', 1, 1, 3, 18500, 2000, 7, 2000, 11500),
	(8, 1, 'Portal', 1, 1, 4, 2000, 2000, 2, 18500, 11500),
	(9, 2, 'Portal', 1, 1, 4, 18500, 2000, 3, 18500, 11500),
	(10, 3, 'Portal', 1, 1, 4, 18500, 11500, 12, 2000, 2000),
	(11, 4, 'Portal', 1, 1, 4, 19500, 7000, 13, 2000, 7000),
	(12, 1, 'Portal', 2, 1, 5, 7000, 11500, 6, 18500, 2000),
	(13, 1, 'Portal', 2, 1, 6, 18500, 2000, 5, 2000, 11500),
	(14, 2, 'Portal', 2, 1, 6, 18500, 11500, 8, 2000, 2000),
	(15, 2, 'Portal', 2, 1, 7, 18500, 2000, 6, 2000, 11500),
	(16, 1, 'Portal', 2, 1, 7, 2000, 11500, 3, 18500, 2000),
	(17, 3, 'Portal', 2, 1, 7, 18500, 11500, 8, 18500, 2000),
	(18, 1, 'Portal', 2, 1, 8, 2000, 2000, 6, 18500, 11500),
	(19, 4, 'Portal', 2, 1, 8, 18500, 2000, 7, 18500, 11500),
	(20, 2, 'Portal', 2, 1, 8, 2000, 11500, 11, 2000, 2000),
	(21, 3, 'Portal', 2, 1, 8, 10000, 12000, 11, 10700, 1200),
	(22, 1, 'Portal', 2, 1, 9, 2000, 2000, 10, 18500, 11500),
	(23, 2, 'Portal', 3, 1, 10, 18500, 11500, 9, 2000, 2000),
	(24, 3, 'Portal', 3, 1, 10, 18500, 2000, 11, 18500, 11500),
	(25, 1, 'Portal', 3, 1, 10, 2000, 2000, 12, 18500, 11500),
	(26, 3, 'Portal', 3, 1, 11, 18500, 11500, 10, 18500, 2000),
	(27, 2, 'Portal', 3, 1, 11, 2000, 11500, 12, 18500, 2000),
	(28, 1, 'Portal', 3, 1, 11, 2000, 2000, 8, 2000, 11500),
	(29, 2, 'Portal', 3, 1, 12, 18500, 2000, 10, 2000, 2000),
	(30, 3, 'Portal', 3, 1, 12, 18500, 2000, 11, 2000, 11500),
	(31, 1, 'Portal', 3, 1, 12, 2000, 2000, 4, 18500, 11500),
	(32, 1, 'Portal', 0, 1, 13, 2000, 7000, 4, 19500, 7000),
	(33, 2, 'Portal', 0, 1, 13, 10700, 7000, 16, 19000, 15000),
	(34, 3, 'Portal', 0, 1, 13, 18500, 2000, 14, 2000, 11500),
	(35, 4, 'Portal', 0, 1, 13, 18500, 11500, 15, 2000, 11500),
	(36, 1, 'Portal', 0, 1, 14, 2000, 11500, 13, 18500, 2000),
	(37, 4, 'Portal', 0, 1, 14, 18500, 11500, 15, 2000, 2000),
	(38, 3, 'Portal', 0, 1, 14, 10700, 7000, 16, 21400, 11000),
	(39, 2, 'Portal', 0, 1, 14, 10700, 1200, 8, 10000, 12000),
	(40, 3, 'Portal', 0, 1, 15, 10700, 7000, 16, 23800, 15000),
	(41, 4, 'Portal', 0, 1, 15, 19500, 7000, 12, 10000, 1200),
	(42, 1, 'Portal', 0, 1, 15, 2000, 11500, 13, 18500, 11500),
	(43, 2, 'Portal', 0, 1, 15, 2000, 2000, 14, 18500, 11500),
	(44, 6, 'Portal', 0, 1, 16, 23800, 15000, 15, 10700, 7000),
	(45, 3, 'Portal', 0, 1, 16, 5000, 15000, 17, 18500, 6000),
	(46, 2, 'Portal', 0, 1, 16, 21400, 11000, 14, 10700, 7000),
	(47, 1, 'Portal', 0, 1, 16, 19000, 15000, 13, 10700, 7000),
	(48, 5, 'Portal', 0, 1, 16, 32000, 2400, 21, 2000, 11500),
	(49, 4, 'Portal', 0, 1, 16, 32000, 24000, 25, 2000, 2000),
	(50, 2, 'Portal', 1, 1, 17, 2000, 11500, 19, 18500, 2000),
	(51, 4, 'Portal', 1, 1, 17, 18500, 6000, 16, 5000, 15000),
	(52, 1, 'Portal', 1, 1, 17, 2000, 2000, 18, 18500, 11500),
	(53, 3, 'Portal', 1, 1, 17, 10500, 12000, 29, 5500, 13500),
	(54, 1, 'Portal', 1, 1, 18, 2000, 11500, 20, 18500, 2000),
	(55, 2, 'Portal', 1, 1, 18, 18500, 11500, 17, 2000, 2000),
	(56, 3, 'Portal', 1, 1, 19, 2000, 2000, 20, 18500, 2000),
	(57, 4, 'Portal', 1, 1, 19, 18500, 2000, 17, 2000, 11500),
	(58, 1, 'Portal', 1, 1, 20, 18500, 2000, 18, 2000, 11500),
	(59, 2, 'Portal', 1, 1, 20, 18500, 11500, 19, 2000, 2000),
	(60, 2, 'Portal', 2, 1, 21, 2000, 2000, 22, 2000, 11500),
	(61, 3, 'Portal', 2, 1, 21, 18500, 2000, 23, 2000, 11500),
	(62, 1, 'Portal', 2, 1, 21, 2000, 11500, 16, 32000, 2400),
	(63, 4, 'Portal', 2, 1, 21, 18500, 11500, 29, 25000, 2000),
	(64, 2, 'Portal', 2, 1, 22, 18500, 2000, 24, 2000, 11500),
	(65, 1, 'Portal', 2, 1, 22, 2000, 11500, 21, 2000, 2000),
	(66, 2, 'Portal', 2, 1, 23, 18500, 2000, 24, 18500, 11500),
	(67, 1, 'Portal', 2, 1, 23, 2000, 11500, 21, 18500, 2000),
	(68, 2, 'Portal', 2, 1, 24, 18500, 11500, 23, 18500, 2000),
	(69, 1, 'Portal', 2, 1, 24, 2000, 11500, 22, 18500, 2000),
	(70, 2, 'Portal', 3, 1, 25, 2000, 11500, 26, 2000, 2000),
	(71, 3, 'Portal', 3, 1, 25, 18500, 11500, 27, 2000, 11500),
	(72, 1, 'Portal', 3, 1, 25, 2000, 2000, 16, 32000, 24000),
	(73, 4, 'Portal', 3, 1, 25, 18500, 2000, 29, 25000, 25000),
	(74, 2, 'Portal', 3, 1, 26, 18500, 11500, 28, 2000, 11500),
	(75, 1, 'Portal', 3, 1, 26, 2000, 2000, 25, 2000, 11500),
	(76, 2, 'Portal', 3, 1, 27, 18500, 11500, 28, 2000, 2000),
	(77, 1, 'Portal', 3, 1, 27, 2000, 11500, 25, 18500, 11500),
	(78, 2, 'Portal', 3, 1, 28, 2000, 2000, 27, 18500, 11500),
	(79, 1, 'Portal', 3, 1, 28, 2000, 11500, 26, 18500, 11500),
	(80, 1, 'Portal', 0, 1, 29, 5500, 13500, 17, 10500, 12000),
	(81, 2, 'Portal', 0, 1, 29, 25000, 2000, 21, 18500, 11500),
	(82, 3, 'Portal', 0, 1, 29, 25000, 25000, 25, 18500, 2000);
/*!40000 ALTER TABLE `portal_template` ENABLE KEYS */;

-- Dumping structure for table game.ships_info
CREATE TABLE IF NOT EXISTS `ships_info` (
  `id` int(10) unsigned NOT NULL COMMENT 'Ship Id',
  `name` varchar(255) NOT NULL DEFAULT '' COMMENT 'Ship Name',
  `health_points` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Ship Health Points',
  `speed` int(10) unsigned NOT NULL COMMENT 'Ship Speed',
  `cargo` int(10) unsigned NOT NULL COMMENT 'Ship Cargo',
  `lasers` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Ship Lasers',
  `generators` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Ship Generators',
  `batteries` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Ship Batteries',
  `missles` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Ship Missles',
  `extra` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Ship Extra'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table game.ships_info: ~0 rows (approximately)
/*!40000 ALTER TABLE `ships_info` DISABLE KEYS */;
INSERT INTO `ships_info` (`id`, `name`, `health_points`, `speed`, `cargo`, `lasers`, `generators`, `batteries`, `missles`, `extra`) VALUES
	(1, 'Phoenix', 4000, 320, 100, 1, 1, 2000, 100, 1);
/*!40000 ALTER TABLE `ships_info` ENABLE KEYS */;

-- Dumping structure for table game.station_template
CREATE TABLE IF NOT EXISTS `station_template` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(11) NOT NULL,
  `company` smallint(5) unsigned NOT NULL,
  `type` smallint(5) unsigned NOT NULL DEFAULT '1',
  `map_id` int(10) unsigned NOT NULL,
  `peace` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `position_x` float unsigned NOT NULL,
  `position_y` float unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- Dumping data for table game.station_template: ~0 rows (approximately)
/*!40000 ALTER TABLE `station_template` DISABLE KEYS */;
INSERT INTO `station_template` (`id`, `name`, `company`, `type`, `map_id`, `peace`, `position_x`, `position_y`) VALUES
	(1, 'MMOBase', 1, 1, 1, 1, 990, 1200);
/*!40000 ALTER TABLE `station_template` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
