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
  `level` int(10) unsigned NOT NULL COMMENT 'Account Ship Level',
  `experience` int(10) unsigned NOT NULL COMMENT 'Account Ship Experience',
  `honor` int(10) unsigned NOT NULL COMMENT 'Account Ship Honor level',
  `gates_achieved` smallint(5) unsigned NOT NULL COMMENT 'Account Ship Gates Achieved',
  `clan_id` int(10) unsigned NOT NULL COMMENT 'Account Ship Clan Id',
  `company` smallint(5) unsigned NOT NULL COMMENT 'Account Ship Company',
  `rank` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'Account Ship Rank',
  `premium` tinyint(1) unsigned NOT NULL COMMENT 'Account Premium',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

-- Dumping data for table game.accounts: ~0 rows (approximately)
/*!40000 ALTER TABLE `accounts` DISABLE KEYS */;
INSERT INTO `accounts` (`id`, `session_id`, `username`, `uridium`, `credits`, `jackpot`, `level`, `experience`, `honor`, `gates_achieved`, `clan_id`, `company`, `rank`, `premium`) VALUES
	(2, '95412faa0b504375075f561adeaaa104', 'Quadral', 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0);
/*!40000 ALTER TABLE `accounts` ENABLE KEYS */;

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
  `selected_laser` int(10) unsigned NOT NULL DEFAULT '1',
  `selected_rocket` int(10) unsigned NOT NULL DEFAULT '1',
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

-- Dumping data for table game.account_settings: ~0 rows (approximately)
/*!40000 ALTER TABLE `account_settings` DISABLE KEYS */;
INSERT INTO `account_settings` (`id`, `display_boost`, `display_damage`, `display_all_las`, `display_exploration`, `display_name`, `display_firm_icon`, `display_alpha_bg`, `ignore_res`, `ignore_box`, `convert_gates`, `convert_oppo`, `sound_off`, `bg_music_off`, `display_status`, `display_bubble`, `selected_laser`, `selected_rocket`, `display_digits`, `display_chat`, `display_drones`, `show_star_system`, `ignore_cargo`, `ignore_hostile_cargo`, `auto_change_ammo`, `enable_buy_fast`) VALUES
	(2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
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
  `weapon_state` smallint(5) unsigned NOT NULL DEFAULT '0',
  `use_system_font` tinyint(1) unsigned NOT NULL DEFAULT '1',
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
  KEY `account_ship` (`id`),
  CONSTRAINT `account_ship` FOREIGN KEY (`id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping data for table game.account_ship: ~1 rows (approximately)
/*!40000 ALTER TABLE `account_ship` DISABLE KEYS */;
INSERT INTO `account_ship` (`id`, `type`, `speed`, `shield`, `max_shield`, `hit_points`, `max_hit_points`, `cargo_space`, `cargo_space_max`, `position_x`, `position_y`, `map_id`, `max_battery`, `max_rockets`, `weapon_state`, `use_system_font`, `battery_lcb_10`, `battery_mcb_25`, `battery_mcb_50`, `battery_ucb_100`, `battery_sab_50`, `rocket_r310`, `rocket_plt_2026`, `rocket_plt_2021`, `mines`, `smart_bombs`, `instant_shields`) VALUES
	(2, 9, 320, 2000, 2000, 4000, 4000, 0, 100, 0, 0, 1, 2000, 100, 0, 1, 10, 20, 30, 40, 50, 10, 10, 10, 5, 5, 10);
/*!40000 ALTER TABLE `account_ship` ENABLE KEYS */;

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

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
