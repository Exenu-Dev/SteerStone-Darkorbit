/*
* Liam Ashdown
* Copyright (C) 2022
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <boost/algorithm/string.hpp>

#include "Packets/Server/LoginPackets.hpp"
#include "Packets/Server/MapPackets.hpp"
#include "Packets/Server/ShipPackets.hpp"
#include "Packets/Server/MiscPackets.hpp"
#include "Packets/Server/QuestPackets.hpp"
#include "World.hpp"
#include "QuestManager.hpp"
#include "ObjectManager.hpp"
#include "ClanManager.hpp"
#include "ZoneManager.hpp"
#include "Diagnostic/DiaStopWatch.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_GameSocket : Socket
    Player::Player(Server::GameSocket* p_GameSocket) :
        m_Socket(p_GameSocket ? p_GameSocket->Shared<Server::GameSocket>() : nullptr),
        m_Inventory(this), m_Ammo(this)
    {
        m_Id                    = 0;
        m_SessionId.clear();
        m_Jackpot               = 0;
        m_Level                 = 0;
        m_MaxCargoSpace         = 0;
        m_MaxBattery            = 0;
        m_MaxRockets            = 0;
        m_UseSystemFont         = 0;
        m_Premium               = 0;

        m_DisplayBoost          = false;
        m_DisplayDamage         = false;
        m_DisplayAllLas         = false;
        m_DisplayExploration    = false;
        m_DisplayName           = false;
        m_DisplayFirmIcon       = false;
        m_DisplayAlphaBG        = false;
        m_IgnoreRes             = false;
        m_IgnoreBox             = false;
        m_ConvertGates          = false;
        m_ConvertOppo           = false;
        m_SoundOff              = false;
        m_BackgroundMusicOff    = false;
        m_DisplayStatus         = false;
        m_DisplayBubble         = false;
        m_SelectedLaser         = 0;
        m_SelectedRocket        = 0;
        m_DisplayDigits         = false;
        m_DisplayChat           = false;
        m_DisplayDrones         = false;
        m_ShowStarSystem        = false;
        m_IgnoreCargo           = false;
        m_IgnoreHostileCargo    = false;
        m_AutoChangeAmmo        = false;
        m_EnableBuyFast         = false;
        m_Repairing			    = false;
        m_IsCloaked             = false;

        m_LoggedIn              = false;
        m_Jumping               = false;
        m_LoggingOut            = false;
        m_Event                 = EventType::EVENT_TYPE_NONE;
        m_AttackType            = AttackType::ATTACK_TYPE_NONE;
        m_LabUpgrades           = {};

        /// Timers
        m_IntervalNextSave.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_SAVE_PLAYER_TO_DATABASE));
        m_IntervalRadiation.SetInterval(RADIATION_TIMER);
        m_RepairBotTimer.SetInterval(REPAIR_BOT_TIMER);
        ConfigTimer.SetInterval(CONFIG_TIMER);
        m_IntervalQuestUpdate.SetInterval(QUEST_UPDATE_TIMER);

        m_AttackRange           = sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_PLAYER_ATTACK_RANGE);

        SetType(Type::OBJECT_TYPE_PLAYER);
    }
    /// Deconstructor
    Player::~Player()
    {
        /// Delete socket
        if (m_Socket)
        {
            Server::Packets::Login::LogOut l_Packet;
            SendPacket(l_Packet.Write());

            m_Socket->CloseSocket();
            m_Socket = nullptr;
        }

        SaveToDB();

        /// Empty Queue
        Server::ClientPacket* l_Packet = nullptr;
        while (m_RecievedQueue.Next(l_Packet))
            delete l_Packet;

        /// TODO; Clear surroundings?

        Unit::CleanupsBeforeDelete();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //              GENERAL
    ///////////////////////////////////////////

    /// Load player details from database
    bool Player::LoadFromDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT users.username, users.uridium, users.credits, users.jackpot, users.experience, users.honor, users.gates_achieved, users.company, users.rank, users.premium, "
            "user_settings.display_boost, user_settings.display_damage, user_settings.display_all_las, user_settings.display_exploration, user_settings.display_name, user_settings.display_firm_icon, user_settings.display_alpha_bg, user_settings.ignore_res, "
            "user_settings.ignore_box, user_settings.convert_gates, user_settings.convert_oppo, user_settings.sound_off, user_settings.bg_music_off, user_settings.display_status, user_settings.display_bubble, user_settings.selected_laser, user_settings.selected_rocket, user_settings.display_digits, "
            "user_settings.display_chat, user_settings.display_drones, user_settings.show_star_system, user_settings.ignore_cargo, user_settings.ignore_hostile_cargo, user_settings.auto_change_ammo, user_settings.enable_buy_fast"
            " FROM users INNER JOIN user_settings ON user_settings.user_id = users.id WHERE users.id = ?");
        l_PreparedStatement->SetUint64(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();
            
            SetName(l_Result[0].GetString());
            m_Uridium                   = l_Result[1].GetUInt32();
            m_Credits                   = l_Result[2].GetUInt32();
            m_Jackpot                   = l_Result[3].GetUInt32();
            m_Experience                = l_Result[4].GetUInt32();
            m_Honor                     = l_Result[5].GetInt32();
            m_GatesAchieved             = l_Result[6].GetUInt16();
            m_Company                   = static_cast<Company>(l_Result[7].GetUInt16());
            m_Rank                      = l_Result[8].GetUInt16();
            m_Premium                   = l_Result[9].GetBool();
            m_DisplayBoost              = l_Result[10].GetBool();
            m_DisplayDamage             = l_Result[11].GetBool();
            m_DisplayAllLas             = l_Result[12].GetBool();
            m_DisplayExploration        = l_Result[13].GetBool();
            m_DisplayName               = l_Result[14].GetBool();
            m_DisplayFirmIcon           = l_Result[15].GetBool();
            m_DisplayAlphaBG            = l_Result[16].GetBool();
            m_IgnoreRes                 = l_Result[17].GetBool();
            m_IgnoreBox                 = l_Result[18].GetBool();
            m_ConvertGates              = l_Result[19].GetBool();
            m_ConvertOppo               = l_Result[20].GetBool();
            m_SoundOff                  = l_Result[21].GetBool();
            m_BackgroundMusicOff        = l_Result[22].GetBool();
            m_DisplayStatus             = l_Result[23].GetBool();
            m_DisplayBubble             = l_Result[24].GetBool();
            m_SelectedLaser             = l_Result[25].GetUInt16();
            m_SelectedRocket            = l_Result[26].GetUInt16();
            m_DisplayDigits             = l_Result[27].GetBool();
            m_DisplayChat               = l_Result[28].GetBool();
            m_DisplayDrones             = l_Result[29].GetBool();
            m_ShowStarSystem            = l_Result[30].GetBool();
            m_IgnoreCargo               = l_Result[31].GetBool();
            m_IgnoreHostileCargo        = l_Result[32].GetBool();
            m_AutoChangeAmmo            = l_Result[33].GetBool();
            m_EnableBuyFast             = l_Result[34].GetBool();

            m_Clan                      = sClanManager->GetClanByPlayerId(m_Id);

            m_Level = CalculateLevel();

            if (m_Premium)
                IntervalLogout.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_PREMIUM_LOG_OUT_TIMER));
            else
                IntervalLogout.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_LOG_OUT_TIMER));
                
            LoadShipFromDB();
            LoadBoosters();
            LoadDrones();
            LoadLabUpgrades();
            LoadQuests();
            m_Inventory.LoadInventory();

            return true;
        }

        return false;
    }
    /// Load Ship Info from database
    void Player::LoadShipFromDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT type, speed, shield, max_shield, hit_points, max_hit_points, cargo_space, cargo_space_max, "
            "position_x, position_y, map_id, max_battery, max_rockets, use_system_font,"
            " prometium, endurium, terbium, xenomit, prometid, duranium, promerium, palladium, seprom,"
            " battery_lcb_10, battery_mcb_25, battery_mcb_50, battery_ucb_100, battery_sab_50, rocket_r310, rocket_plt_2026, rocket_plt_2021, mines, smart_bombs, instant_shields, preset FROM user_ships WHERE user_id = ?");
        l_PreparedStatement->SetUint64(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

            m_ShipType                  = l_Result[0].GetUInt16();
            GetSpline()->SetSpeed(l_Result[1].GetUInt32());
            m_Shield                    = l_Result[2].GetInt32();
            m_MaxShield                 = l_Result[3].GetUInt32();
            m_HitPoints                 = l_Result[4].GetInt32();
            m_MaxHitPoints              = l_Result[5].GetUInt32();
            l_Result[6].GetUInt32(); ///< No longer used
            m_MaxCargoSpace             = l_Result[7].GetUInt32();

            SetGUID(ObjectGUID(GUIDType::Player, 0, m_Id));
            GetSpline()->SetPosition(l_Result[8].GetDouble(), l_Result[9].GetDouble(), l_Result[8].GetDouble(), l_Result[9].GetDouble());
            SetMap(sZoneManager->GetMap(l_Result[10].GetUInt32()));

            m_MaxBattery                = l_Result[11].GetUInt32();
            m_MaxRockets                = l_Result[12].GetUInt32();
            m_UseSystemFont             = l_Result[13].GetBool();

            m_Resources[0]              = l_Result[14].GetUInt32();
            m_Resources[1]              = l_Result[15].GetUInt32();
            m_Resources[2]              = l_Result[16].GetUInt32();
            m_Resources[3]              = l_Result[17].GetUInt32();
            m_Resources[4]              = l_Result[18].GetUInt32();
            m_Resources[5]              = l_Result[19].GetUInt32();
            m_Resources[6]              = l_Result[20].GetUInt32();
            m_Resources[7]              = l_Result[21].GetUInt32();
            m_Resources[8]              = l_Result[22].GetUInt32();

            m_Ammo.m_BatteryLCB10       = l_Result[23].GetInt32();
            m_Ammo.m_BatteryMCB25       = l_Result[24].GetInt32();
            m_Ammo.m_BatteryMCB50       = l_Result[25].GetInt32();
            m_Ammo.m_BatteryUCB100      = l_Result[26].GetInt32();
            m_Ammo.m_BatterySAB50       = l_Result[27].GetInt32();

            m_Ammo.m_RocketR310         = l_Result[28].GetInt32();
            m_Ammo.m_RocketPLT2026      = l_Result[29].GetInt32();
            m_Ammo.m_RocketPLT2021      = l_Result[30].GetInt32();

            m_Ammo.m_Mines              = l_Result[31].GetInt32();
            m_Ammo.m_SmartBombs         = l_Result[32].GetInt32();
            m_Ammo.m_InstantShields     = l_Result[33].GetInt32();

            m_Preset                    = l_Result[34].GetUInt8();
        }
        else
            LOG_ASSERT(false, "Player", "Failed to load ship data for player %0", m_Id);
    }
    /// Load Drone Info from database
    void Player::LoadDrones()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT type, points FROM user_drones WHERE user_id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        /// Reset Drones incase this is called from CMS
        m_Drones = std::vector<Drone>();

        if (l_PreparedResultSet)
        {
            uint32 l_DroneCount = 0;

            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                if (l_DroneCount > 8) ///< 16 is max drone count
                {
                    LOG_WARNING("Player", "Player %0 has more than 8 drones! only loading 8!", GetId());
                    break;
                }

                Drone l_Drone;
                l_Drone.Type    = l_Result[0].GetUInt16();
                l_Drone.Points  = l_Result[1].GetUInt16();
                l_DroneCount++;

                m_Drones.push_back(l_Drone);

            } while (l_PreparedResultSet->GetNextRow());
        }
    }
    /// Load Boosters
    void Player::LoadBoosters()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, entry_id, value FROM user_boosters WHERE user_id = ? ORDER BY created_at ASC");
        l_PreparedStatement->SetUint32(0, m_Id);

        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            m_Boosters.clear();

            do
            {
				Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

				const uint32 l_Id           = l_Result[0].GetUInt32();
                const uint32 l_BoosterId    = l_Result[1].GetUInt32();
				const int32 l_Duration      = l_Result[1].GetInt32();

                if (ItemTemplate const* l_ItemTemplate = sObjectManager->GetItemTemplate(l_BoosterId))
                {
					Booster l_Booster;
                    l_Booster.Id                    = l_Id;
                    l_Booster.BoosterItemTemplate   = l_ItemTemplate;
                    l_Booster.Duration              = l_Duration;

                    m_Boosters.push_back(l_Booster);
				}
                else
                {
					LOG_WARNING("Player", "Cannot find Booster with entry %0 deleting from player inventory!", l_BoosterId);

					Core::Database::PreparedStatement* l_PreparedStatementDelete = GameDatabase.GetPrepareStatement();
					l_PreparedStatementDelete->PrepareStatement("DELETE FROM user_boosters WHERE entry_id = ? AND user_id = ?");
					l_PreparedStatementDelete->SetUint32(0, l_BoosterId);
                    l_PreparedStatementDelete->SetUint32(1, m_Id);
					l_PreparedStatementDelete->ExecuteStatement();
				}

			} while (l_PreparedResultSet->GetNextRow());
		}
    }
    /// Load Lab Upgrades
    void Player::LoadLabUpgrades()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
		l_PreparedStatement->PrepareStatement("SELECT id, type, item_type, value FROM user_lab_upgrades WHERE user_id = ?");
		l_PreparedStatement->SetUint32(0, m_Id);

		std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

		if (l_PreparedResultSet)
		{
			m_LabUpgrades.clear();

			do
			{
				Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                LabUpgrade l_LabUpgrade;

				const uint32 l_Id       = l_Result[0].GetUInt32();
				std::string l_Type      = l_Result[1].GetString();
                std::string l_ItemType  = l_Result[2].GetString();
                int32 l_Value           = l_Result[3].GetInt32();

				l_LabUpgrade.Id         = l_Id;
                l_LabUpgrade.Type       = l_Type;
				l_LabUpgrade.ItemType   = l_ItemType;

                /// Cheeky way of doing this
                /// Since we store the item type by string, change it to enum
                /// as it's easier to work with
                if (l_ItemType == "shields")
                    l_LabUpgrade.UpgradeType = LabUpgradeType::LAB_UPGRADE_TYPE_SHIELDS;
                else if (l_ItemType == "engines")
					l_LabUpgrade.UpgradeType = LabUpgradeType::LAB_UPGRADE_TYPE_ENGINES;
                else if (l_ItemType == "lasers")
					l_LabUpgrade.UpgradeType = LabUpgradeType::LAB_UPGRADE_TYPE_LASERS;
                else if (l_ItemType == "rockets")
				    l_LabUpgrade.UpgradeType = LabUpgradeType::LAB_UPGRADE_TYPE_ROCKETS;
                else
                    LOG_ASSERT(false, "Player", "Unknown Lab Upgrade Type %0", l_ItemType);

                if (l_LabUpgrade.IsShieldOrEngine())
                    l_Value *= 1000; ///< Convert to milliseconds

                l_LabUpgrade.Value   = l_Value;

                m_LabUpgrades.push_back(l_LabUpgrade);

			} while (l_PreparedResultSet->GetNextRow());
		}
    }
    /// Load Quests
    void Player::LoadQuests()
    {
        // Clear existing quests and delete them
        for (auto l_Quest : m_Quests)
			delete l_Quest.second;

        m_Quests.clear();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, quest_template_id, completed FROM user_quests WHERE user_id = ? AND completed = 0");
        l_PreparedStatement->SetUint32(0, m_Id);

        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                uint32 l_QuestTemplateId = l_Result[1].GetUInt32();

                Game::Quest::Quest* l_Quest = sQuestManager->GetQuestById(l_QuestTemplateId);

                if (!l_Quest)
                {
					LOG_WARNING("Player", "Failed to find quest with id %0", l_QuestTemplateId);
					continue;
				}

                Quest* l_UserQuest = new Quest(this);
                l_UserQuest->m_Id               = l_Result[0].GetUInt32();
                l_UserQuest->m_Completed        = l_Result[2].GetBool();
                l_UserQuest->m_Quest            = l_Quest;
                l_UserQuest->LoadFromDB();

                m_Quests[l_UserQuest->m_Quest->GetId()] = l_UserQuest;
               
            } while (l_PreparedResultSet->GetNextRow());
        } 
    }
    /// Save Player details to database
    void Player::SaveToDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("UPDATE users INNER JOIN user_settings ON user_settings.user_id = users.id "
            "SET users.uridium = ?, users.credits = ?, users.jackpot = ?, users.experience = ?, users.honor = ?, users.gates_achieved = ?, users.company = ?, users.rank = ?, users.premium = ?, "
            "user_settings.display_boost = ?, user_settings.display_damage = ?, user_settings.display_all_las = ?, user_settings.display_exploration = ?, user_settings.display_name = ?, user_settings.display_firm_icon = ?, user_settings.display_alpha_bg = ?, user_settings.ignore_res = ?, "
            "user_settings.ignore_box = ?, user_settings.convert_gates = ?, user_settings.convert_oppo = ?, user_settings.sound_off = ?, user_settings.bg_music_off = ?, user_settings.display_status = ?, user_settings.display_bubble = ?, user_settings.selected_laser = ?, user_settings.selected_rocket = ?, user_settings.display_digits = ?, "
            "user_settings.display_chat = ?, user_settings.display_drones = ?, user_settings.show_star_system = ?, user_settings.ignore_cargo = ?, user_settings.ignore_hostile_cargo = ?, user_settings.auto_change_ammo = ?, user_settings.enable_buy_fast = ? "
            "WHERE users.id = ?");

        l_PreparedStatement->SetUint32(0, m_Uridium);
        l_PreparedStatement->SetUint32(1, m_Credits);
        l_PreparedStatement->SetUint32(2, m_Jackpot);
        l_PreparedStatement->SetUint32(3, m_Experience);
        l_PreparedStatement->SetInt32(4, m_Honor);
        l_PreparedStatement->SetUint32(5, m_GatesAchieved);
        l_PreparedStatement->SetUint16(6, static_cast<uint16>(m_Company));
        l_PreparedStatement->SetUint16(7, m_Rank);  
        l_PreparedStatement->SetBool(8, m_Premium);
        l_PreparedStatement->SetBool(9, m_DisplayBoost);
        l_PreparedStatement->SetBool(10, m_DisplayDamage);
        l_PreparedStatement->SetBool(11, m_DisplayAllLas);
        l_PreparedStatement->SetBool(12, m_DisplayExploration);
        l_PreparedStatement->SetBool(13, m_DisplayName);
        l_PreparedStatement->SetBool(14, m_DisplayFirmIcon);
        l_PreparedStatement->SetBool(15, m_DisplayAlphaBG);
        l_PreparedStatement->SetBool(16, m_IgnoreRes);
        l_PreparedStatement->SetBool(17, m_IgnoreBox);
        l_PreparedStatement->SetBool(18, m_ConvertGates);
        l_PreparedStatement->SetBool(19, m_ConvertOppo);
        l_PreparedStatement->SetBool(20, m_SoundOff);
        l_PreparedStatement->SetBool(21, m_BackgroundMusicOff);
        l_PreparedStatement->SetBool(22, m_DisplayStatus);
        l_PreparedStatement->SetBool(23, m_DisplayBubble);
        l_PreparedStatement->SetBool(24, m_SelectedLaser);
        l_PreparedStatement->SetBool(25, m_SelectedRocket);
        l_PreparedStatement->SetBool(26, m_DisplayDigits);
        l_PreparedStatement->SetBool(27, m_DisplayChat);
        l_PreparedStatement->SetBool(28, m_DisplayDrones);
        l_PreparedStatement->SetBool(29, m_ShowStarSystem);
        l_PreparedStatement->SetBool(30, m_IgnoreCargo);
        l_PreparedStatement->SetBool(31, m_IgnoreHostileCargo);
        l_PreparedStatement->SetBool(32, m_AutoChangeAmmo);
        l_PreparedStatement->SetBool(33, m_EnableBuyFast);
        l_PreparedStatement->SetUint32(34, m_Id);
        l_PreparedStatement->ExecuteStatement();

        SaveShipToDB();
        SaveLabUpgrades();
        SaveQuestsToDB();
        GetInventory()->SaveInventory();
    }
    /// Save Ship details to database
    /// @p_Asynchronous : Use Async connection
    void Player::SaveShipToDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("UPDATE user_ships SET type = ?, cargo_space = ?, cargo_space_max = ?, "
            "position_x = ?, position_y = ?, map_id = ?, use_system_font = ?, "
            "prometium = ?, endurium = ?, terbium = ?, xenomit = ?, prometid = ?, duranium = ?, promerium = ?, palladium = ?, seprom = ?, "
            "battery_lcb_10 = ?, battery_mcb_25 = ?, battery_mcb_50 = ?, battery_ucb_100 = ?, battery_sab_50 = ?, rocket_r310 = ?, " 
            "rocket_plt_2026 = ?, rocket_plt_2021 = ?, mines = ?, smart_bombs = ?, instant_shields = ? WHERE user_id = ?");

        l_PreparedStatement->SetUint16(0,  m_ShipType);
        l_PreparedStatement->SetUint32(1,  0); ///< No longer used
        l_PreparedStatement->SetUint32(2,  m_MaxCargoSpace);
        l_PreparedStatement->SetFloat(3,   GetSpline()->GetPositionX());
        l_PreparedStatement->SetFloat(4,   GetSpline()->GetPositionY());
        l_PreparedStatement->SetUint32(5,  GetMap()->GetId());
        l_PreparedStatement->SetBool(6,    m_UseSystemFont);
        l_PreparedStatement->SetUint32(7,  m_Resources[0]);
        l_PreparedStatement->SetUint32(8,  m_Resources[1]);
        l_PreparedStatement->SetUint32(9,  m_Resources[2]);
        l_PreparedStatement->SetUint32(10, m_Resources[3]);
        l_PreparedStatement->SetUint32(11, m_Resources[4]);
        l_PreparedStatement->SetUint32(12, m_Resources[5]);
        l_PreparedStatement->SetUint32(13, m_Resources[6]);
        l_PreparedStatement->SetUint32(14, m_Resources[7]);
        l_PreparedStatement->SetUint32(15, m_Resources[8]);
        l_PreparedStatement->SetUint32(16, m_Ammo.m_BatteryLCB10);
        l_PreparedStatement->SetUint32(17, m_Ammo.m_BatteryMCB25);
        l_PreparedStatement->SetUint32(18, m_Ammo.m_BatteryMCB50);
        l_PreparedStatement->SetUint32(19, m_Ammo.m_BatteryUCB100);
        l_PreparedStatement->SetUint32(20, m_Ammo.m_BatterySAB50);
        l_PreparedStatement->SetUint32(21, m_Ammo.m_RocketR310);
        l_PreparedStatement->SetUint32(22, m_Ammo.m_RocketPLT2026);
        l_PreparedStatement->SetUint32(23, m_Ammo.m_RocketPLT2021);
        l_PreparedStatement->SetUint32(24, m_Ammo.m_Mines);
        l_PreparedStatement->SetUint32(25, m_Ammo.m_SmartBombs);
        l_PreparedStatement->SetUint32(26, m_Ammo.m_InstantShields);
        l_PreparedStatement->SetUint32(27, m_Id);
        l_PreparedStatement->ExecuteStatement();
    }

    /// Send Info Message to Player
    /// @p_Message : Message to send
    void Player::SendInfoMessage(std::string const p_Message)
    {
        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { p_Message }));
    }
    /// Send System Message to Player
    /// @p_Message : Message to send
    void Player::SendSystemMessage(std::string const p_Message)
    {
        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_SYSTEM_MESSAGE, { p_Message }));
    }
    /// Get Company X-1 Map Id based on Player Company
    uint32 Player::GetX1CompanyMapId() const
    {
        switch (m_Company)
        {
			case Company::MMO:
                return CompanyMapId::COMPANY_MAP_ID_MMO_1X1;
			case Company::EARTH:
				return CompanyMapId::COMPANY_MAP_ID_EIC_2X1;
			case Company::VRU:
				return CompanyMapId::COMPANY_MAP_ID_VRU_3X1;
		}
    }
    /// Return Drone Level
    /// @p_Drone : Drone
    uint16 Player::GetDroneLevel(Drone& p_Drone) const
    {
        uint16 l_Level = DroneLevel::DRONE_LEVEL_1;

        if (p_Drone.Points >= 100 && p_Drone.Points < 200)
            l_Level = DroneLevel::DRONE_LEVEL_2;
        else if (p_Drone.Points >= 200 && p_Drone.Points < 400)
            l_Level = DroneLevel::DRONE_LEVEL_3;
        else if (p_Drone.Points >= 400 && p_Drone.Points < 800)
            l_Level = DroneLevel::DRONE_LEVEL_4;
        else if (p_Drone.Points >= 800 && p_Drone.Points < 1600)
            l_Level = DroneLevel::DRONE_LEVEL_5;
        else if (p_Drone.Points >= 1600)
            l_Level = DroneLevel::DRONE_LEVEL_6;

        return l_Level;
    }
    /// Returns Level
    uint32 Player::CalculateLevel()
    {
        uint32 l_Level = 1;

        for (uint32 l_I = 0; l_I < LevelExperience::Level::MAX_LEVEL; l_I++)
        {
            if (m_Experience >= LevelExperience::All[l_I])
                l_Level = l_I + 1;
        }

        return l_Level;
    }
    /// Check whether player is in radiation zone
    /// p_Diff : Execution Time
    void Player::UpdateRadiationZone(uint32 const p_Diff)
    {
        m_IntervalRadiation.Update(p_Diff);
        if (!m_IntervalRadiation.Passed())
            return;

        /// Set we are in radiation zone
        if (IsInRadiationZone() && GetEvent() != EventType::EVENT_TYPE_RADIATION_ZONE)
        {
            Server::Packets::Event l_Packet;
            l_Packet.PositionX           = GetSpline()->GetPositionX();
            l_Packet.PositionY           = GetSpline()->GetPositionY();
            l_Packet.InDemolitionZone    = false;
            l_Packet.InRadiationZone     = true;
            l_Packet.PlayRepairAnimation = false;
            l_Packet.InTradeZone         = false;
            l_Packet.InJumpZone          = false;
            l_Packet.Repair              = false;
            SendPacket(l_Packet.Write());

            SetEventType(EventType::EVENT_TYPE_RADIATION_ZONE);
        }
        /// Set we are not in radiation zone
        else if (!IsInRadiationZone() && GetEvent() == EventType::EVENT_TYPE_RADIATION_ZONE)
        {
            SetEventType(EventType::EVENT_TYPE_NONE);

            Server::Packets::Event l_Packet;
            l_Packet.PositionX           = GetSpline()->GetPositionX();
            l_Packet.PositionY           = GetSpline()->GetPositionY();
            l_Packet.InDemolitionZone    = false;
            l_Packet.InRadiationZone     = false;
            l_Packet.PlayRepairAnimation = false;
            l_Packet.InTradeZone         = false;
            l_Packet.InJumpZone          = false;
            l_Packet.Repair              = false;
            SendPacket(l_Packet.Write());

            m_IntervalRadiation.Reset();
        }
        /// In Radiation Zone
        else if (GetEvent() == EventType::EVENT_TYPE_RADIATION_ZONE)
        {
            int32 l_Damage = 10;//Core::Utils::CalculatePercentage(m_IntervalRadiation.GetTick() > 5 ? 5 : m_IntervalRadiation.GetTick(), GetHitMaxPoints());

            CheckQuestCondition(ConditionType::CONDITION_TYPE_TAKE_DAMAGE, {
                { AttributeKeys::ATTRIBUTE_KEY_DAMANGE_TYPE, static_cast<int32>(DamageType::DAMAGE_TYPE_RADIATION) },
                { AttributeKeys::ATTRIBUTE_KEY_MISC_COND_TYPE, l_Damage }
            });

            DealDamage(this, l_Damage, false);
        }
    }
    /// Update Repairing
    /// @p_Diff : Execution Time
    void Player::UpdateRepairing(uint32 const p_Diff)
    {
        m_RepairBotTimer.Update(p_Diff);
        if (!m_RepairBotTimer.Passed())
            return;

        if (!CanRepair())
        {
            Repair(false);
            return;
        }

        if (!IsRepairing())
            return;

        Item* l_RepairBot = GetInventory()->GetItemByType(ItemType::ITEM_TYPE_REPAIR_BOT);

        if (!l_RepairBot)
        {
            m_Repairing = false;
            LOG_WARNING("Player", "Player %0 is repairing but does not have a repair bot!", GetId());
            return;
        }

        /// Check here is it not really needed, but might aswell
        /// as we depend on this
        if (l_RepairBot->GetItemTemplate()->GetValue() <= 0)
        {
            m_Repairing = false;
            return;
        }

        uint32 l_RepairTimeInSeconds = l_RepairBot->GetItemTemplate()->GetValue();
        uint32 l_Health = std::round((GetHitMaxPoints() / l_RepairTimeInSeconds));

        if (GetHitPoints() + l_Health > GetHitMaxPoints())
			l_Health = GetHitMaxPoints() - GetHitPoints();

        Heal(this, l_Health);
    }
    /// Update Cooldowns
    /// @p_Diff : Execution Time
    void Player::UpdateCooldowns(uint32 const p_Diff)
    {
        m_Ammo.Update(p_Diff);
    }
    /// Send Update Cooldown
    /// @p_CooldownType : Cooldown Type
    void Player::SendUpdateCooldown(CooldownType const p_CooldownType)
    {
        std::string l_CooldownType = "unknown";
        uint32 l_CoolDownDuration = 0;

        switch (p_CooldownType)
        {
            case CooldownType::COOLDOWN_TYPE_MINE:
            {
                /// We usually check out side this function, but good to double check here
                if (m_Ammo.GetMineCooldown()->OnCooldown())
					return;

                l_CooldownType = "MIN";
                l_CoolDownDuration = MINE_COOLDOWN / 1000;

                m_Ammo.GetMineCooldown()->StartCooldown();

            }
			break;
            case CooldownType::COOLDOWN_TYPE_SMART_MINE:
            {
                if (m_Ammo.GetSmartMineCooldown()->OnCooldown())
                    return;

                l_CooldownType = "SMB";
                l_CoolDownDuration = MINE_COOLDOWN / 1000;

                m_Ammo.GetSmartMineCooldown()->StartCooldown();
            }
            break;
            case CooldownType::COOLDOWN_TYPE_INSTANT_SHIELD:
            {
                if (m_Ammo.GetInstantShieldCooldown()->OnCooldown())
                    return;

                l_CooldownType = "ISH";
                l_CoolDownDuration = MINE_COOLDOWN / 1000;

                m_Ammo.GetInstantShieldCooldown()->StartCooldown();
            }
            break;
        }

        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_COOLDOWNS, {
            l_CooldownType,
            l_CoolDownDuration
        }));
    }
    /// Update Speed
    /// Informs the client the new speed
    void Player::SendUpdateSpeed()
    {
        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_SPEED, { GetSpline()->GetSpeed() }));
    }

    /// Update Player
    /// @p_Diff : Execution Time
    bool Player::Update(uint32 p_Diff)
    {
        Core::Diagnostic::StopWatch l_StopWatch;
        l_StopWatch.Start();

        switch (m_DeathState)
        {
            case DeathState::ALIVE:
            {
                UpdateRadiationZone(p_Diff);
                UpdateSurroundings(p_Diff);
                UpdateBoosters(p_Diff);
                UpdateLabUpgrades(p_Diff);
                UpdateRepairing(p_Diff);
                UpdateQuests(p_Diff);
                UpdateCooldowns(p_Diff);

                Unit::Update(p_Diff);
            }
            break;
            case DeathState::JUST_DIED:
            {
                m_DeathState = DeathState::DEAD;

                CancelAttack();

                /// It's easier if we just remove mob from map, so we don't need to do extra
                /// checks whether object is dead etc...
                sZoneManager->RemoveFromMap(this);

                uint32 l_RespawnMapId = 0;
                switch (m_Company)
                {
                    case Company::MMO:
                    {
                        l_RespawnMapId = 1;
                    }
                    break;
                    case Company::EARTH:
                    {
                        l_RespawnMapId = 5;
                    }
                    break;
                    case Company::VRU:
                    {
                        l_RespawnMapId = 9;
                    }
                    break;
                }
            }
            break;
        }

        /// Timers
        ConfigTimer.Update(p_Diff);

        m_IntervalNextSave.Update(p_Diff);
        if (m_IntervalNextSave.Passed())
            SaveToDB();

        m_OperatorProcessor.ProcessOperators();

        l_StopWatch.Stop();
        if (l_StopWatch.GetElapsed() > 20)
            LOG_WARNING("Player", "Took more than 20ms to update Player!");

        return true;
    }

    ///////////////////////////////////////////
    //              LOG BOOK
    ///////////////////////////////////////////

    /// Update Log Book (used for website)
    /// @p_Log : Log text
    /// @p_LogBookType : Type of Log Book
    void Player::UpdateLogBook(std::string p_Log, LogBookType const p_LogBookType /*= LogBookType::LOG_BOOK_TYPE_DETAILED*/)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("INSERT INTO log_books (entry_id, type, text, date) VALUES (?, ?, ?, NOW())");
        l_PreparedStatement->SetUint32(0, m_Id);
        l_PreparedStatement->SetUint16(1, static_cast<uint16>(p_LogBookType));
        l_PreparedStatement->SetString(2, p_Log);
        l_PreparedStatement->ExecuteStatement();
    }

    ///////////////////////////////////////////
    //             QUESTS
    ///////////////////////////////////////////

    void Player::SendQuests()
    {
        for (auto l_Itr = m_Quests.begin(); l_Itr != m_Quests.end(); l_Itr++)
        {
            Server::Packets::Quest::QuestHud l_Packet;
            l_Packet.Type = Server::Packets::Quest::QuestHudTypes::QUEST_HUD_TYPE_INI;
            Quest* l_Quest = l_Itr->second;

            QuestProgressVec l_Progress = l_Quest->GetProgress();

            Server::Packets::Quest::Case l_Case;
            l_Case.Id = l_Quest->GetQuestData()->GetId();
            l_Case.Active = true;
            l_Case.Mandatory = false;
            l_Case.Ordered = l_Itr->second->GetQuestData()->IsOrder() && l_Progress.size() > 1;

            for (auto l_JItr = l_Progress.begin(); l_JItr != l_Progress.end(); l_JItr++)
            {
				Server::Packets::Quest::Condition l_Condition;
                l_Condition.Id              = l_JItr->Id;
                l_Condition.Completed       = l_JItr->Completed;
                l_Condition.ConditionType   = static_cast<uint32>(l_JItr->ConditionType);
                l_Condition.Values          = l_Itr->second->GetConditionAttributes(l_JItr->Id);
                l_Case.Conditions.push_back(l_Condition);
			}

            l_Packet.Cases.push_back(l_Case);

            SendPacket(l_Packet.Write());
		}
    }

    /// Save Quests to Database
    void Player::SaveQuestsToDB()
    {
        for (auto l_Itr = m_Quests.begin(); l_Itr != m_Quests.end(); l_Itr++)
		{
			l_Itr->second->SaveToDB();
		}
    }

    void Player::CheckQuestCondition(ConditionType const p_ConditionType, std::initializer_list<std::pair<AttributeKeys, std::variant<int32, std::string>>> p_List)
    {
        auto GetListAttribute = [&](std::initializer_list<std::pair<AttributeKeys, std::variant<int32, std::string>>> p_List, AttributeKeys p_Attribute) -> std::variant<int32, std::string>
            {
                for (auto l_Itr : p_List)
                {
				if (l_Itr.first == p_Attribute)
					return l_Itr.second;
			}

			return std::variant<int32, std::string>();
		};

        for (auto& l_Itr = m_Quests.begin(); l_Itr != m_Quests.end();)
        {
            for (auto& l_JItr : l_Itr->second->GetProgress())
            {
                if (l_JItr.ConditionType == p_ConditionType)
                {
                    if (l_Itr->second->GetQuestData()->IsOrder())
                    {
                        // Check if previous conditions are completed
                        bool l_AllCompleted = true;

                        for (auto l_KItr = l_Itr->second->GetProgress().begin(); l_KItr != l_Itr->second->GetProgress().end(); l_KItr++)
                        {
							if (l_KItr->Id == l_JItr.Id)
								break;

                            if (!l_KItr->Completed)
                            {
								l_AllCompleted = false;
								break;
							}
						}

                        if (!l_AllCompleted)
							continue;
                    }

                    switch (p_ConditionType)
                    {
                        case ConditionType::CONDITION_TYPE_COLLECT:
                        {
                            if (std::get<int32>(l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_ORE_ID))
								== std::get<int32>(GetListAttribute(p_List, AttributeKeys::ATTRIBUTE_KEY_ORE_ID)))
                            {
                                l_JItr.UpdateProgress(std::get<int32>(l_JItr.Progress) + 1);

								// Check if we are above the required amount
								if (std::get<int32>(l_JItr.Progress) >= std::get<int32>(l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_COUNT_TARGET)))
									l_JItr.Completed = true;

                                SendUpdateQuestCondition(
                                    l_Itr->second->GetQuestData()->GetId(),
                                    l_JItr.Id,
                                    std::get<int32>(l_JItr.Progress)
                                );
                            }
                        }
                        break;
                        case ConditionType::CONDITION_TYPE_KILL_NPC:
                        {
                            l_JItr.UpdateProgress(std::get<int32>(l_JItr.Progress) + 1);

                            // Check if we need to mark this condition as completed
                            if (std::get<int32>(l_JItr.Progress) >= std::get<int32>(l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_COUNT_TARGET)))
                            {
								l_JItr.Completed = true;
                            }

                            SendUpdateQuestCondition(
                                l_Itr->second->GetQuestData()->GetId(),
                                l_JItr.Id,
								std::get<int32>(l_JItr.Progress)
							);
                        }
                        break;
                        case ConditionType::CONDITION_TYPE_VISIT_MAP:
                        {
                            std::variant<int32, std::string> l_MapId = l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_MAP_ID);

                            // Check if the map id is a string
                            if (auto l_MapIdString = std::get_if<std::string>(&l_MapId))
                            {
                                bool l_DynamicMap = boost::algorithm::contains(*l_MapIdString, "X-");

                                if (l_DynamicMap)
                                {
                                    /// Replace X with the player company map id
                                    switch (GetCompany())
                                    {
                                    case Company::MMO:
                                        boost::replace_all(*l_MapIdString, "X", "1");
                                        break;
                                    case Company::EARTH:
                                        boost::replace_all(*l_MapIdString, "X", "2");
                                        break;
                                    case Company::VRU:
                                        boost::replace_all(*l_MapIdString, "X", "3");
                                        break;
                                    }
                                }

                                // Convert the map string into the map id
                                l_MapId = sZoneManager->MapStringToId(*l_MapIdString);
                            }
                            else
                                l_MapId = std::get<int32>(GetListAttribute(p_List, AttributeKeys::ATTRIBUTE_KEY_MAP_ID));

                            if (*std::get_if<int32>(&l_MapId) == *std::get_if<int32>(&GetListAttribute(p_List, AttributeKeys::ATTRIBUTE_KEY_MAP_ID)))
							{ 
                                l_JItr.Completed = true;

                                SendUpdateQuestCondition(
                                    l_Itr->second->GetQuestData()->GetId(),
                                    l_JItr.Id,
                                    l_MapId
                                );
                            }
                        }
                        break;
                        case ConditionType::CONDITION_TYPE_TAKE_DAMAGE:
                        {
                            DamageType l_DamageType = static_cast<DamageType>(std::get<int32>(GetListAttribute(p_List, AttributeKeys::ATTRIBUTE_KEY_DAMANGE_TYPE)));

                            if (static_cast<DamageType>(std::get<int32>(l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_DAMANGE_TYPE)))
                                == l_DamageType
                            )
                            {
                                l_JItr.UpdateProgress(std::get<int32>(l_JItr.Progress) + std::get<int32>(GetListAttribute(p_List, AttributeKeys::ATTRIBUTE_KEY_MISC_COND_TYPE)));

                                // Check if we are above the required damage
                                if (std::get<int32>(l_JItr.Progress) >= std::get<int32>(l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_COUNT_TARGET)))
                                    l_JItr.Completed = true;				

                                SendUpdateQuestCondition(
                                    l_Itr->second->GetQuestData()->GetId(),
                                    l_JItr.Id,
                                    std::get<int32>(l_JItr.Progress)
                                );

                            }
                        }
                        break;
                        case ConditionType::CONDITION_TYPE_TIMER:
                        {
							l_JItr.UpdateProgress(std::get<int32>(l_JItr.Progress) + std::get<int32>(GetListAttribute(p_List, AttributeKeys::ATTRIBUTE_KEY_MISC_COND_TYPE)));

							// Check if we've completed all conditions within the timer
                            if (std::get<int32>(l_JItr.Progress) >= std::get<int32>(l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_COUNT_TARGET)))
                            {
                                bool l_AllCompleted = true;

                                for (auto l_KItr = l_Itr->second->GetProgress().begin(); l_KItr != l_Itr->second->GetProgress().end(); l_KItr++)
                                {
                                    if (l_KItr->Id != l_JItr.Id)
                                    {
                                        if (!l_KItr->Completed)
                                        {
                                            l_AllCompleted = false;
                                            break;
                                        }
                                    }
                                }

                                if (l_AllCompleted)
									l_JItr.Completed = true;
                            }

                            SendUpdateQuestCondition(
								l_Itr->second->GetQuestData()->GetId(),
								l_JItr.Id,
								(std::get<int32>(l_JItr.GetAttribute(AttributeKeys::ATTRIBUTE_KEY_TIME_TARGET)) - std::get<int32>(l_JItr.Progress)) * 1000
							);
						}
                    }
                }
            }

            // Has all conditions been completed?
            bool l_AllCompleted = true;

            for (auto& l_JItr : l_Itr->second->GetProgress())
            {
                // We don't want to check for timer conditions
                if (!l_JItr.Completed && l_JItr.ConditionType != ConditionType::CONDITION_TYPE_TIMER)
                {
					l_AllCompleted = false;
					break;
				}
			}

            if (l_AllCompleted)
            {
                FinishQuest(l_Itr->second->GetQuestData()->GetId());

                // Remove quest from map
                delete m_Quests[l_Itr->second->GetQuestData()->GetId()];

                l_Itr = m_Quests.erase(l_Itr);
            }
			else
				l_Itr++;
        }
    }

    void Player::SendUpdateQuestCondition(uint32 const p_QuestId, uint32 const p_ConditionId, std::variant<int32, std::string> const p_Value)
    {
        Server::Packets::Quest::QuestHud l_Packet;
        l_Packet.Type = Server::Packets::Quest::QUEST_HUD_TYPE_UPDATE;

        Server::Packets::Quest::UpdateCondition l_UpdateCondition;
        l_UpdateCondition.QuestId = p_QuestId;
        l_UpdateCondition.Id = p_ConditionId;
        l_UpdateCondition.Value = p_Value;
        l_UpdateCondition.Visibility = 1;
        l_UpdateCondition.ActivityState = 1;
        l_Packet.Update = l_UpdateCondition;

        SendPacket(l_Packet.Write());
    }

    void Player::FinishQuest(uint32 const p_QuestId)
    {
        if (m_Quests.find(p_QuestId) == m_Quests.end())
            return;

        Quest* l_Quest = m_Quests[p_QuestId];
        l_Quest->Finish();

        Server::Packets::Quest::QuestHud l_Packet;
        l_Packet.Type = Server::Packets::Quest::QUEST_HUD_TYPE_FINISH;

        Server::Packets::Quest::QuestFinish l_FinishQuest;
        l_FinishQuest.QuestId = l_Quest->GetQuestData()->GetId();

        l_Packet.Finish = l_FinishQuest;

        SendPacket(l_Packet.Write());
    }

    void Player::FailQuest(uint32 const p_QuestId)
    {
        if (m_Quests.find(p_QuestId) == m_Quests.end())
			return;

		Quest* l_Quest = m_Quests[p_QuestId];
		l_Quest->Fail();

		Server::Packets::Quest::QuestHud l_Packet;
		l_Packet.Type = Server::Packets::Quest::QUEST_HUD_TYPE_FAIL;

		Server::Packets::Quest::QuestFail l_FailQuest;
		l_FailQuest.QuestId = l_Quest->GetQuestData()->GetId();
		l_Packet.Fail = l_FailQuest;

		SendPacket(l_Packet.Write());
    }

    void Player::CancelQuest(uint32 const p_QuestId)
    {
        if (m_Quests.find(p_QuestId) == m_Quests.end())
			return;

        Quest* l_Quest = m_Quests[p_QuestId];

        Server::Packets::Quest::QuestHud l_Packet;
        l_Packet.Type = Server::Packets::Quest::QUEST_HUD_TYPE_CANCEL;

        Server::Packets::Quest::CancelQuest l_CancelQuest;
        l_CancelQuest.QuestId = l_Quest->GetQuestData()->GetId();

        l_Packet.Cancel = l_CancelQuest;

        SendPacket(l_Packet.Write());

        l_Quest->Cancel();

        /// Delete quest and remove from map
        delete m_Quests[p_QuestId];
        m_Quests.erase(p_QuestId);

        // Load the new quests
        SaveToDB();
        LoadQuests();
        SendQuests();
    }

    void Player::UpdateQuests(uint32 const p_Diff)
    {
        m_IntervalQuestUpdate.Update(p_Diff);

        if (!m_IntervalQuestUpdate.Passed())
			return;

        CheckQuestCondition(ConditionType::CONDITION_TYPE_TIMER, { { AttributeKeys::ATTRIBUTE_KEY_MISC_COND_TYPE, 1 } });
    }

    ///////////////////////////////////////////
    //             BOOSTERS
    ///////////////////////////////////////////

    /// Update the Booster Times
    /// @p_Diff : Execution Time
    void Player::UpdateBoosters(uint32 const p_Diff)
    {
        /// Used to check if we need to send the boosters packet
        /// and save the player to the database
        bool l_Updated = false;

        for (auto l_Itr = m_Boosters.begin(); l_Itr != m_Boosters.end();)
        {
			if (l_Itr->Update(p_Diff))
				l_Itr++;
            else
            {
                l_Itr = m_Boosters.erase(l_Itr);
                RemoveBooster(l_Itr->Id);
            }
		}
    }
    /// Check to see if the player has a booster
    /// @p_BoosterType : Booster Type
    bool Player::HasBooster(BoosterTypes const p_BoosterType) const
    {
        for (auto const& l_Booster : m_Boosters)
        {
			if (l_Booster.GetBoosterType() == p_BoosterType)
				return true;
		}

        return false;
    }
    /// Get Booster Value
    /// @p_BoosterType : Booster Type
    int32 Player::GetBoosterValue(BoosterTypes const p_BoosterType) const
    {
        for (auto const& l_Booster : m_Boosters)
        {
            if (l_Booster.GetBoosterType() == p_BoosterType)
                return l_Booster.GetBoosterValue();
        }

        /// TODO; Show what booster is missing
        LOG_ASSERT(false, "Player", "Player %0 does not have a booster of type");
    }
    /// Get Booster Name
    /// @p_BoosterType : Booster Type
    std::string Player::GetBoosterName(BoosterTypes const p_BoosterType) const
    {
        for (auto const& l_Booster : m_Boosters)
		{
            if (l_Booster.GetBoosterType() == p_BoosterType)
                return l_Booster.GetItemTemplate()->Name;
		}
    }
    /// Get Booster Duration
    /// @p_BoosterType : Booster Type
    int32 Player::GetBoosterDuration(BoosterTypes const p_BoosterType) const
    {
        for (auto const& l_Booster : m_Boosters)
		{
			if (l_Booster.GetBoosterType() == p_BoosterType)
				return l_Booster.GetDuration();
		}

		/// TODO; Show what booster is missing
		LOG_ASSERT(false, "Player", "Player %0 does not have a booster of type");
    }
    /// Send the Boosters Packet
    void Player::SendBoosters(bool const p_ShowLastOne/* = false*/)
    {
        /// Notes
        /// Packet is sent in the following format
        /// XP/Honor/Damage/Shield

        std::string l_BoosterString = "";

        std::vector<int32> l_Boosters;

        auto SendBoosterFound = [this](uint32 l_I) {
            SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_SYSTEM_MESSAGE, {
                "booster_found",
                "%BOOSTERNAME%",
                GetBoosterName(static_cast<BoosterTypes>(l_I)),
                "%HOURS%",
                GetBoosterDuration(static_cast<BoosterTypes>(l_I))
                }));
            };

        for (int l_I = static_cast<uint8>(BoosterTypes::BOOSTER_TYPE_XP_B01); l_I < static_cast<uint8>(BoosterTypes::MAX_BOOSTER); l_I++)
        {
            if (HasBooster(static_cast<BoosterTypes>(l_I)))
            {
                if (!p_ShowLastOne)
                    SendBoosterFound(l_I);

                l_Boosters.push_back(GetBoosterValue(static_cast<BoosterTypes>(l_I)));
            }
            else
                l_Boosters.push_back(0);
		}

        for (auto const& l_Booster : l_Boosters)
        {
            l_BoosterString += std::to_string(l_Booster) + "/";
        }

        if (p_ShowLastOne)
        {
            // Get the last booster
            SendBoosterFound(static_cast<uint32>(m_Boosters.back().GetBoosterType()));
        }

        l_BoosterString.pop_back();

        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_BOOSTERS, { l_BoosterString }));
    }

    /// Remove Booster
    /// @p_BoosterId : Booster Id
    void Player::RemoveBooster(uint32 const p_BoosterId)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("DELETE FROM user_boosters WHERE id = ?");
        l_PreparedStatement->SetUint32(0, p_BoosterId);
        l_PreparedStatement->ExecuteStatement();

        /// Also update the player
        SendBoosters();
        SaveToDB();
    }

    ///////////////////////////////////////////
    //             LAB UPGRADES
    ///////////////////////////////////////////

    /// Update Lab Upgrades
    /// @p_Diff : Execution Time
    void Player::UpdateLabUpgrades(uint32 const p_Diff)
    {
        for (auto l_Itr = m_LabUpgrades.begin(); l_Itr != m_LabUpgrades.end();)
        {
            if (l_Itr->Update(p_Diff))
                l_Itr++;
            else
            {
                /// Expired, remove from player
                RemoveLabUpgrade(l_Itr->Id);
                l_Itr = m_LabUpgrades.erase(l_Itr);
            }
        }
    }
    /// Remove Lab Upgrade
    /// @p_Id : Id of Lab Upgrade
    void Player::RemoveLabUpgrade(uint32 const p_Id)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("DELETE FROM user_lab_upgrades WHERE id = ?");
        l_PreparedStatement->SetUint32(0, p_Id);
        l_PreparedStatement->ExecuteStatement();

        /// Also update the player
        SaveToDB();
        /// Also recalculate user inventory stats
        GetInventory()->CalculateStats();
    }

    /// Get Lab Upgrade
    /// @p_LabUpgradeType : Lab Upgrade Type
    LabUpgrade* Player::GetLabUpgrade(LabUpgradeType const p_LabUpgradeType)
    {
        for (auto& l_LabUpgrade : m_LabUpgrades)
		{
			if (l_LabUpgrade.GetLabUpgradeType() == p_LabUpgradeType)
				return &l_LabUpgrade;
		}

        return nullptr;
    }

    /// Save Lab Upgrades
    void Player::SaveLabUpgrades()
    {
		for (auto const& l_LabUpgrade : m_LabUpgrades)
		{
            Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
			l_PreparedStatement->PrepareStatement("UPDATE user_lab_upgrades SET value = ? WHERE id = ?");

            float l_Value = l_LabUpgrade.GetValue();

            if (l_LabUpgrade.IsShieldOrEngine())
                l_Value /= 1000; ///< Convert to seconds

            l_PreparedStatement->SetInt32(0, std::round(l_Value));
			l_PreparedStatement->SetUint32(1, l_LabUpgrade.GetId());
			l_PreparedStatement->ExecuteStatement();
		}
    }

    ///////////////////////////////////////////
    //        SURROUNDING SYSTEM
    ///////////////////////////////////////////

    /// Add Object to surrounding
    /// @p_Object : Object being added
    void Player::AddToSurrounding(Object* p_Object)
    {
        auto l_Itr = m_Surroundings.find(p_Object->GetGUID());
        if (l_Itr == m_Surroundings.end())
            m_Surroundings[p_Object->GetGUID()] = std::make_unique<SurroundingObject>(p_Object, this);
    }
    /// Remove Object from surrounding
    /// @p_Object : Object being removed
    void Player::RemoveFromSurrounding(Object* p_Object)
    {
        auto l_Itr = m_Surroundings.find(p_Object->GetGUID());
        if (l_Itr != m_Surroundings.end())
        {
            l_Itr->second->Despawn(true);
            m_Surroundings.erase(l_Itr);
        }
    }
    /// Remove Object from being despawned
    /// @p_Object : Object
    void Player::RemoveScheduleDespawn(Object* p_Object)
    {
        auto l_Itr = m_Surroundings.find(p_Object->GetGUID());
        if (l_Itr != m_Surroundings.end())
            l_Itr->second->RemoveScheduleFromDespawn();
    }
    /// Check if Object is in surrounding
    /// @p_Object : Object being checked
    bool Player::IsInSurrounding(Object* p_Object)
    {
        auto l_Itr = m_Surroundings.find(p_Object->GetGUID());
        if (l_Itr != m_Surroundings.end())
            return true;

        return false;
    }
    /// Schedule all surroundings to be despawned
    void Player::ScheduleSurroundingsForDespawn()
    {
        for (auto l_Itr = m_Surroundings.begin(); l_Itr != m_Surroundings.end(); l_Itr++)
            l_Itr->second->ScheduleForDespawn();
    }
    /// Clear and despawn all surroundings
    void Player::ClearSurroundings()
    {
        for (auto l_Itr = m_Surroundings.begin(); l_Itr != m_Surroundings.end(); l_Itr++)
            l_Itr->second->Despawn();
       
        m_Surroundings.clear();
    }
    /// Send Packet to surroundings
    /// @p_PacketBuffer : Packet being sent
    void Player::SendPacketToSurroundings(Server::PacketBuffer const* p_PacketBuffer)
    {
        for (auto l_Itr = m_Surroundings.begin(); l_Itr != m_Surroundings.end(); l_Itr++)
            l_Itr->second->SendPacket(p_PacketBuffer);
    }
    /// Update Surroundings
    /// @p_Diff : Execution Time
    void Player::UpdateSurroundings(uint32 const p_Diff)
    {
        for (auto l_Itr = m_Surroundings.begin(); l_Itr != m_Surroundings.end();)
        {
            if (!l_Itr->second->Update(p_Diff))
            {
                l_Itr = m_Surroundings.erase(l_Itr);
                continue;
            }
            else
                l_Itr++;
        }
    }

    ///////////////////////////////////////////
    //              PACKETS
    ///////////////////////////////////////////

    /// Send Client In-game settings
    void Player::SendClientSettings()
    {
        Server::Packets::Login::PlayerInfo l_Packet;
        SendPacket(Server::Packets::Login::PlayerInfo().Write(Server::Packets::Login::InfoType::INFO_TYPE_SET_SETTINGS,
            { m_DisplayBoost,
            m_DisplayDamage,
            m_DisplayAllLas,
            m_DisplayExploration,
            m_DisplayName,
            m_DisplayFirmIcon,
            m_DisplayAlphaBG,
            m_IgnoreRes,
            m_IgnoreBox,
            m_ConvertGates,
            m_ConvertOppo,
            m_SoundOff,
            m_BackgroundMusicOff,
            m_DisplayStatus,
            m_DisplayBubble,
            (int32)m_SelectedLaser,
            (int32)m_SelectedRocket,
            m_DisplayDigits,
            m_DisplayChat,
            m_DisplayDrones,
            m_ShowStarSystem,
            m_IgnoreCargo,
            m_IgnoreHostileCargo,
            m_AutoChangeAmmo,
            m_EnableBuyFast,
            }));
    }
    /// Update Client settings
    void Player::UpdateClientSettings(Server::ClientPacket* p_Packet)
    {
        m_DisplayBoost          = p_Packet->ReadUInt8();
        m_DisplayDamage         = p_Packet->ReadUInt8();
        m_DisplayAllLas         = p_Packet->ReadUInt8();
        m_DisplayExploration    = p_Packet->ReadUInt8();
        m_DisplayName           = p_Packet->ReadUInt8();
        m_DisplayFirmIcon       = p_Packet->ReadUInt8();
        m_DisplayAlphaBG        = p_Packet->ReadUInt8();
        m_IgnoreRes             = p_Packet->ReadUInt8();
        m_IgnoreBox             = p_Packet->ReadUInt8();
        m_ConvertGates          = p_Packet->ReadUInt8();
        m_ConvertOppo           = p_Packet->ReadUInt8();
        m_SoundOff              = p_Packet->ReadUInt8();
        m_BackgroundMusicOff    = p_Packet->ReadUInt8();
        m_DisplayStatus         = p_Packet->ReadUInt8();
        m_DisplayBubble         = p_Packet->ReadUInt8();
        m_SelectedLaser         = p_Packet->ReadUInt8();
        m_SelectedRocket        = p_Packet->ReadUInt8();
        m_DisplayDigits         = p_Packet->ReadUInt8();
        m_DisplayChat           = p_Packet->ReadUInt8();
        m_DisplayDrones         = p_Packet->ReadUInt8();
        m_ShowStarSystem        = p_Packet->ReadUInt8();
        m_IgnoreCargo           = p_Packet->ReadUInt8();
        m_IgnoreHostileCargo    = p_Packet->ReadUInt8();
        m_AutoChangeAmmo        = p_Packet->ReadUInt8();
        m_EnableBuyFast         = p_Packet->ReadUInt8();

        SendClientSettings();
    }
    /// Send ship details
    void Player::SendInitializeShip()
    {
        Server::Packets::Login::InitializeShip l_Packet;
        l_Packet.Id             = GetObjectGUID().GetCounter();
        l_Packet.Username       = GetName();
        l_Packet.CompanyId      = static_cast<uint16>(m_Company);

        if (GetClan())
        {
            l_Packet.ClanId     = GetClan()->GetID();
            l_Packet.ClanTag    = GetClan()->GetTag();
        }

        l_Packet.IsPremium      = m_Premium;
        l_Packet.Experience     = m_Experience;
        l_Packet.Honour         = m_Honor;
        l_Packet.GatesAchieved  = m_GatesAchieved;
        l_Packet.Level          = m_Level;
        l_Packet.Credits        = m_Credits;
        l_Packet.Uridium        = m_Uridium;
        l_Packet.JackPot        = m_Jackpot;
        l_Packet.Rank           = m_Rank;
        l_Packet.Speed          = GetSpline()->GetSpeed();
        l_Packet.Shield         = GetShield();
        l_Packet.MaxShield      = GetMaxShield();
        l_Packet.HitPoints      = m_HitPoints;
        l_Packet.MaxHitPoints   = m_MaxHitPoints;
        l_Packet.PositionX      = GetSpline()->GetPositionX();
        l_Packet.PositionY      = GetSpline()->GetPositionY();
        l_Packet.MapId          = GetMap()->GetId();
        l_Packet.WeaponState    = m_WeaponState;
        l_Packet.ShipId         = m_ShipType;
        l_Packet.CargoSpace     = GetCargoSpace();
        l_Packet.MaxCargoSpace  = m_MaxCargoSpace;
        l_Packet.MaxBattery     = m_MaxBattery;
        l_Packet.MaxRockets     = m_MaxRockets;

        SendPacket(l_Packet.Write());
    }
    /// Send Logged In
    void Player::SendLoggedIn()
    {
        m_LoggedIn = true;

        SendPacket(Server::Packets::Login::LoggedIn().Write());
    }
    /// Send Account Rank
    void Player::SendAccountRank()
    {
        if (GetRank() == Rank::ADMIN)
        {
            SendPacket(Server::Packets::Login::PlayerInfo().Write(Server::Packets::Login::InfoType::INFO_TYPE_SET_ADMIN, { (bool)1 }));
            SendPacket(Server::Packets::Misc::DisplaySuccDebug().Write());
        }
    }
    /// Send Display Star System
    void Player::SendDisplayStarSystem()
    {
        SendPacket(Server::Packets::DisplayStarSystem().Write());
    }
    /// Send Map update
    void Player::SendMapUpdate()
    {
        Server::Packets::Ship::MapUpdate l_MapUpdatePacket;
        l_MapUpdatePacket.MapId = GetMap()->GetId();
        SendPacket(l_MapUpdatePacket.Write());

        Server::Packets::Ship::MiniMapUpdate l_MiniMapUpdatePacket;
        l_MiniMapUpdatePacket.MapId = GetMap()->GetId();
        SendPacket(l_MiniMapUpdatePacket.Write());
    }
    /// Send Battery, Rocket and mine update
    void Player::SendAmmoUpdate()
    {
        Server::Packets::Ship::UpdateRocketMineAmmo l_RocketMineAmmoPacket;
        l_RocketMineAmmoPacket.RocketR310     = m_Ammo.m_RocketR310;
        l_RocketMineAmmoPacket.RocketPLT2021  = m_Ammo.m_RocketPLT2021;
        l_RocketMineAmmoPacket.RocketPLT2026  = m_Ammo.m_RocketPLT2026;
        l_RocketMineAmmoPacket.Mines          = m_Ammo.m_Mines;
        l_RocketMineAmmoPacket.SmartBombs     = m_Ammo.GetSmartBombs();
        l_RocketMineAmmoPacket.InstantShields = m_Ammo.GetInstantShields();
        SendPacket(l_RocketMineAmmoPacket.Write());

        Server::Packets::Ship::UpdateBatteryAmmo l_BatteryAmmoPacket;
        l_BatteryAmmoPacket.BatteryLCB10      = m_Ammo.m_BatteryLCB10;
        l_BatteryAmmoPacket.BatteryMCB25      = m_Ammo.m_BatteryMCB25;
        l_BatteryAmmoPacket.BatteryMCB50      = m_Ammo.m_BatteryMCB50;
        l_BatteryAmmoPacket.BatteryUCB100     = m_Ammo.m_BatteryUCB100;
        l_BatteryAmmoPacket.BatterySAB50      = m_Ammo.m_BatterySAB50;
        SendPacket(l_BatteryAmmoPacket.Write());
    }
    /// Send Clear Rocket Cooldown
    void Player::SendClearRocketCooldown()
    {
        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_CLEAR_ROCKET));
    }
    /// Change Configuration
    void Player::ChangeConfiguration(const uint16 p_Config)
    {
        if (ConfigTimer.Passed())
        {
            m_Preset = p_Config;

            m_Inventory.LoadInventory();
            m_Inventory.CalculateStats();

            Server::Packets::Ship::ChangeConfig l_Packet;
            l_Packet.Config = p_Config;
            SendPacket(l_Packet.Write());

            SendHealthAndShield();
            return; 
        }

        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_SYSTEM_MESSAGE, { "config_change_failed_time" }));
    }
    /// On Trade Ore
    /// @p_ResourceId : Resource Id which we are trading
    /// @p_OreAmount : The Ore amount we are trading
    void Player::OnTradeOre(const uint32 p_ResourceId, const uint32 p_OreAmount)
    {
        uint32 l_OreWorth = 0;

        switch (p_ResourceId)
        {
        case Resource::RESOURCE_PROMETIUM:
            l_OreWorth = 10;
            break;
        case Resource::RESOURCE_ENDURIUM:
            l_OreWorth = 15;
            break;
        case Resource::RESOURCE_TERBIUM:
            l_OreWorth = 25;
            break;
        case Resource::RESOURCE_PROMETID:
        case Resource::RESOURCE_DURANIUM:
            l_OreWorth = 200;
            break;
        case Resource::RESOURCE_PROMERIUM:
            l_OreWorth = 500;
            break;
        default:
            LOG_ASSERT(false, "Player", "Received unknown resource on OnTradeOre");
            break;
        }

        l_OreWorth = std::floor(l_OreWorth * (1 + GetHonor() / ORE_CALCULATION_AMOUNT));

        uint32 l_CreditsRewarded = p_OreAmount * l_OreWorth;

        m_Credits += l_CreditsRewarded;
        m_Resources[p_ResourceId] -= p_OreAmount;

        SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_CREDIT, { l_CreditsRewarded, m_Credits }));
        
        UpdateOres();
        UpdateMaxCargoSpace();
    }
    /// Send Drone Info
    void Player::SendDrones()
    {
        if (!m_Drones.size())
            return;

        SendPacket(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_DRONES, { GetObjectGUID().GetCounter(), BuildDronesString() }));
    }
    /// Send Cloak
    void Player::SendCloak()
    {
        GetMap()->SendPacketToNearByGridsIfInSurrounding(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_INVISIBLE, {
            GetObjectGUID().GetCounter(),
            (bool)IsCloaked()
        }), this, true);
    }
    /// Build Drones Packet
    std::string Player::BuildDronesString() const
    {
        std::string l_Drones;
        uint32 l_DroneCount = m_Drones.size();

        // Determine the number of drone groups
        uint32 l_DroneGroupCount = (l_DroneCount <= 4) ? 1 : 3;

        uint32 l_Counter = 0;
        uint32 l_GroupCounter = 0;

        for (auto l_Itr : m_Drones)
        {
            uint32 l_DroneLevel = GetDroneLevel(l_Itr) - 1;

            if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                l_DroneLevel += 10;
            else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                l_DroneLevel += 20;

            if (l_Counter == 0)
            {
                l_Drones = std::to_string(l_DroneGroupCount) + "/" + std::to_string((l_DroneCount <= 4) ? l_DroneCount : (l_DroneCount <= 8 ? (l_DroneCount == 5 || l_DroneCount == 7 ? 1 : 2) : 4)) + "-" + std::to_string(l_DroneLevel) + ",";
            }
            else if (l_Counter < l_DroneCount - 1)
            {
                l_Drones += std::to_string((l_Counter % 2 == 0 && l_DroneGroupCount == 3 && l_Counter != 1) ? l_GroupCounter++ : 0) + "-" + std::to_string(l_DroneLevel) + ",";
            }
            else
            {
                l_Drones += std::to_string(l_GroupCounter) + "-" + std::to_string(l_DroneLevel) + ",";
            }

            l_Counter++;
        }

        // Trim the last comma and add final zero as per the string format rules
        if (!l_Drones.empty())
            l_Drones.pop_back(); // Remove the last comma

        l_Drones += "/0";

        return l_Drones;
    }

    /// Update Cargo Max Space
    void Player::UpdateMaxCargoSpace()
    {
        // SendPacket(Server::Packets::Login::PlayerInfo().Write(Server::Packets::Login::InfoType::INFO_TYPE_SET_CARGO_SPACE, { (int32)GetCargoSpace() }));
        SendPacket(Server::Packets::Login::PlayerInfo().Write(Server::Packets::Login::InfoType::INFO_TYPE_SET_CARGO_SPACE, { (int32)GetMaxCargoSpace() }));
    }
    /// Update Ores
    /// Note this updates the cargo bay on the client
    void Player::UpdateOres()
    {
        Server::Packets::Ship::UpdateOre l_Packet;
        l_Packet.Prometium = GetResource(Resource::RESOURCE_PROMETIUM);
        l_Packet.Endurium = GetResource(Resource::RESOURCE_ENDURIUM);
        l_Packet.Terbium = GetResource(Resource::RESOURCE_TERBIUM);
        l_Packet.Xenomit = GetResource(Resource::RESOURCE_XENOMIT);
        l_Packet.Prometid = GetResource(Resource::RESOURCE_PROMETID);
        l_Packet.Duranium = GetResource(Resource::RESOURCE_DURANIUM);
        l_Packet.Promerium = GetResource(Resource::RESOURCE_PROMERIUM);
        l_Packet.Palladium = GetResource(Resource::RESOURCE_PALLADIUM);
        l_Packet.Seprom = GetResource(Resource::RESOURCE_SEPROM);

        SendPacket(l_Packet.Write());
    }
    /// Set Ore Prices
    void Player::SetOrePrices()
    {
        Server::Packets::Ship::OrePrices l_Packet;
        l_Packet.Prometium = std::floor(10 * (1 + GetHonor() / ORE_CALCULATION_AMOUNT));
        l_Packet.Endurium = std::floor(15 * (1 + GetHonor() / ORE_CALCULATION_AMOUNT));
        l_Packet.Terbium = std::floor(25 * (1 + GetHonor() / ORE_CALCULATION_AMOUNT));
        l_Packet.Prometid = std::floor(200 * (1 + GetHonor() / ORE_CALCULATION_AMOUNT));
        l_Packet.Duranium = std::floor(200 * (1 + GetHonor() / ORE_CALCULATION_AMOUNT));;
        l_Packet.Promerium = std::floor(500 * (1 + GetHonor() / ORE_CALCULATION_AMOUNT));;

        SendPacket(l_Packet.Write());
    }
    /// Process Packets
    /// @p_PacketFilter : Type of packet
    bool Player::ProcessPacket(Server::PacketFilter& p_PacketFilter)
    {
        if (!m_Socket || m_Socket->IsClosed())
            return false;

        Server::ClientPacket* l_ClientPacket = nullptr;
        while (m_RecievedQueue.Next(l_ClientPacket, p_PacketFilter))
        {
            Server::OpcodeHandler const* l_OpcodeHandler = sOpCode->GetClientPacket(static_cast<ClientOpCodes>(l_ClientPacket->GetHeader()));

            if (l_OpcodeHandler && l_OpcodeHandler->Name)
                m_Socket->ExecutePacket(l_OpcodeHandler, l_ClientPacket);
            else
                LOG_WARNING("Opcode", "Received unhandled opcode %0", l_ClientPacket->GetHeader());
            
            delete l_ClientPacket;
        }

        return true;
    }
    /// Queue Packet
    /// @_ClientPacket : Packet being queued
    void Player::QueuePacket(Server::ClientPacket* p_ClientPacket)
    {
        m_RecievedQueue.Add(p_ClientPacket);
    }
    /// Send Packet
    /// @p_PacketBuffer : Packet Buffer
    void Player::SendPacket(Server::PacketBuffer const* p_PacketBuffer)
    {
        if (!m_Socket || !p_PacketBuffer)
            return;

        m_Socket->SendPacket(p_PacketBuffer);
    }
    /// Kick Player
    void Player::KickPlayer()
    {
        Server::PacketBuffer l_Buffer;
        l_Buffer.AppendChar("0");
        l_Buffer.AppendChar("KIK");
        l_Buffer.AppendEndSplitter();
        l_Buffer.AppendCarriage();
        SendPacket(&l_Buffer);

        /// Close the socket
        m_Socket->CloseSocket();
    }

    /// Teleport Player to Map
    /// @p_MapId : Map Id to teleport to
    /// @p_PositionX : Position X to teleport to
    /// @p_PositionY : Position Y to teleport to
    void Player::Teleport(const uint32 p_MapId, const int32 p_PositionX /*= 0*/, const int32 p_PositionY /*= 0*/)
    {
        if (p_MapId == GetMap()->GetId())
			return;

        Map::Base* l_Map = sZoneManager->GetMap(p_MapId);

        LOG_ASSERT(l_Map, "Player", "Map %0 does not exist", p_MapId);

        SendInfoMessage("Teleporting to map " + l_Map->GetName() + " in 5 seconds");

        Game::Map::JumpQueueCordinates l_JumpQueueCordinates;
        l_JumpQueueCordinates.MapId = p_MapId;
        l_JumpQueueCordinates.PositionX = p_PositionX;
        l_JumpQueueCordinates.PositionY = p_PositionY;

        GetMap()->AddToJumpQueue(this, l_JumpQueueCordinates);
    }
    /// Update Items
    /// This updates the cloak, jump cpu, etc
    void Player::UpdateExtrasInfo()
    {
        JumpChipType l_JumpChipType = GetInventory()->GetJumpChipType();

        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_EXTRAS_INFO, {
            1, ///< updateDroneCpuIndicator (3)
            0, ///< diploscan Portable Trade Ore (4)
            static_cast<uint8>(l_JumpChipType), // jump_cpu_id (5)
            0, // (6)
            GetInventory()->GetRepairBot() != nullptr ? true : false, // (7)
            0, // hm7_drone_on_board(8)
            7, // rt_doubbler (9)
            GetInventory()->HasSmartBomb(), // smartbomb_cpu_on_board (10)
            GetInventory()->HasInstantShield(), // instashield_cpu_on_board (11)
            0, // mine_turbo_level (12)
            0, ///< AIM CPU (aiming_cpu_level) (13)
            GetInventory()->HasAutoRocket(), ///< ROCKET CPU Automatically fires; TODO arol_cpu_on_board (14)
            0, ///< Cloak CPU, cloak_cpu_on_board (15)
        }));

        if (l_JumpChipType == JumpChipType::JUMP_CHIP_TYPE_NONE)
            return;

        Item* l_JumpChip = GetInventory()->GetJumpChip();

        /// This is double checking, but always good to do
        if (!l_JumpChip)
            return;

        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_CPU_JUMP_CHIP, {
            static_cast<uint8>(l_JumpChipType),
            0, ///< Not sure what this is yet
            l_JumpChip->GetMetaData<uint32>("jumps")
        }));
    }
    /// Send the packet to update players health and shield
    void Player::SendHealthAndShield()
    {
        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_SET_SHIELD_HEALTH,
            {
                GetHitPoints(),
                (int32)GetHitMaxPoints(),
                GetShield(),
                (int32)GetMaxShield(),
            }));
    }
    /// Check to see if the player can repair based on a series of checks
    bool Player::CanRepair()
    {
        if (IsInCombat())
            return false;

        if (GetHitPoints() >= GetHitMaxPoints())
            return false;

        Item* l_RepairBot = GetInventory()->GetItemByType(ItemType::ITEM_TYPE_REPAIR_BOT);

        if (!l_RepairBot)
            return false;

        /// Check here is it not really needed, but might aswell
        /// as we depend on this
        if (l_RepairBot->GetItemTemplate()->GetValue() <= 0)
            return false;

        return true;
    }

    /// Update Experience
    /// @p_Experience : Experience to be added
    void Player::UpdateExperience(uint32 p_Experience)
    {
        if (HasBooster(BoosterTypes::BOOSTER_TYPE_XP_B01))
            p_Experience =+ Core::Utils::CalculatePercentage(p_Experience, ToPlayer()->GetBoosterValue(BoosterTypes::BOOSTER_TYPE_XP_B01));

        m_Experience += p_Experience;

        uint32 l_Level = CalculateLevel();

        /// Update level and send to client
        if (m_Level < l_Level)
        {
            SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_LEVEL_UP, { l_Level, LevelExperience::All[l_Level] - m_Experience }));
            m_Level = l_Level;
        }
    }

    /// Update the Drone Experience
    /// @p_Object : Object
    void Player::UpdateDroneExperience(Entity::Object* p_Object)
    {
        if (!m_Drones.size())
            return;

        uint32 l_Experience = 0;

        switch (GetShipType())
        {
            case PlayerShips::Phoenix:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
						case NpcShips::Streuner:
							l_Experience = 4;
						break;
                        case NpcShips::Lordakia:
                            l_Experience = 6;
                        break;
                        case NpcShips::Saimon:
							l_Experience = 10;
                        break;
                        case NpcShips::Mordon:
                            l_Experience = 16;
                            break;
                        case NpcShips::Devolarium:
                            l_Experience = 40;
                            break;
                        case NpcShips::Sibelon:
							l_Experience = 50;
							break;
                        case NpcShips::Sibelonit:
							l_Experience = 20;
							break;
                        case NpcShips::Lordakium:
                            l_Experience = 60;
							break;
                        case NpcShips::Kristallin:
                            l_Experience = 30;
                            break;
                        case NpcShips::Kristallon:
							l_Experience = 75;
							break;
					}
				}
            }
            break;
            case PlayerShips::Yamato:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 2;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 4;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 8;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 12;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 30;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 40;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 16;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 50;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 20;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 65;
                        break;
                    }
                }
            }
            break;
            case PlayerShips::Defcom:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 1;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 3;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 7;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 10;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 25;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 35;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 14;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 45;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 18;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 60;
                        break;
                    }
                }
            }
            break;
            case PlayerShips::Liberator:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 0;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 2;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 6;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 8;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 20;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 30;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 12;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 40;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 16;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 55;
                        break;
                    }
                }
            }
            break;
            case PlayerShips::Piranha:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 0;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 1;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 5;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 6;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 15;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 25;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 10;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 35;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 14;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 50;
                        break;
                    }
                }
            }
            break;
            case PlayerShips::Nostromo:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 0;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 0;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 4;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 4;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 10;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 22;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 8;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 30;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 12;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 40;
                        break;
                    }
                }
            }
            break;
            case PlayerShips::Vengeance:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 0;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 0;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 3;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 2;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 9;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 19;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 6;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 25;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 10;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 30;
                        break;
                    }
                }
            }
            break;
            case PlayerShips::Bigboy:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 0;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 0;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 2;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 1;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 8;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 17;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 4;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 20;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 7;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 24;
                        break;
                    }
                }
            }
            break;
            case PlayerShips::Goliath:
            {
                if (p_Object->IsMob())
                {
                    switch (p_Object->ToUnit()->GetShipType())
                    {
                    case NpcShips::Streuner:
                        l_Experience = 0;
                        break;
                    case NpcShips::Lordakia:
                        l_Experience = 0;
                        break;
                    case NpcShips::Saimon:
                        l_Experience = 1;
                        break;
                    case NpcShips::Mordon:
                        l_Experience = 1;
                        break;
                    case NpcShips::Devolarium:
                        l_Experience = 7;
                        break;
                    case NpcShips::Sibelon:
                        l_Experience = 12;
                        break;
                    case NpcShips::Sibelonit:
                        l_Experience = 3;
                        break;
                    case NpcShips::Lordakium:
                        l_Experience = 15;
                        break;
                    case NpcShips::Kristallin:
                        l_Experience = 4;
                        break;
                    case NpcShips::Kristallon:
                        l_Experience = 18;
                        break;
                    }
                }
            }
            break;
        }

        if (l_Experience)
        {
            for (auto& l_Itr : m_Drones)
            {
                l_Itr.AddExperience(l_Experience);
			}

            /// Send Drones packet, this will update the drone level
            /// We might aswell do this each time the drone gets experience
            SendDrones();
        }
    }
    /// Set Battery Ammo
    ///@p_BatteryType : Battery Type
    ///@p_Amount : Amount to add
    void Player::SetBatteryAmmo(BatteryType const p_BatteryType, const int32 p_Amount)
    {
        switch (p_BatteryType)
        {
            case BatteryType::BATTERY_TYPE_LCB10:
                m_Ammo.m_BatteryLCB10 += p_Amount;

                if (m_Ammo.m_BatteryLCB10 < 0) {
                    m_Ammo.m_BatteryLCB10 = 0;
                }
                break;
            case BatteryType::BATTERY_TYPE_MCB25:
                m_Ammo.m_BatteryMCB25 += p_Amount;

                if (m_Ammo.m_BatteryMCB25 < 0) {
                    m_Ammo.m_BatteryMCB25 = 0;
                }
                break;
            case BatteryType::BATTERY_TYPE_MCB50:
                m_Ammo.m_BatteryMCB50 += p_Amount;

                if (m_Ammo.m_BatteryMCB50 < 0) {
                    m_Ammo.m_BatteryMCB50 = 0;
                }
                break;
            case BatteryType::BATTERY_TYPE_SAB50:
                m_Ammo.m_BatterySAB50 += p_Amount;

                if (m_Ammo.m_BatterySAB50 < 0) {
                    m_Ammo.m_BatterySAB50 = 0;
                }
                break;
            case BatteryType::BATTERY_TYPE_UCB100:
                m_Ammo.m_BatteryUCB100 += p_Amount;

                if (m_Ammo.m_BatteryUCB100 < 0) {
                    m_Ammo.m_BatteryUCB100 = 0;
                }
                break;
            default:
                LOG_ASSERT(false, "Player", "Cannot update ammo with battery type %0", static_cast<uint16>(p_BatteryType));
                break;
        }

        if (LabUpgrade* l_LabUpgrade = GetLabUpgrade(LabUpgradeType::LAB_UPGRADE_TYPE_LASERS))
            l_LabUpgrade->SetValue(p_Amount);

        SendAmmoUpdate();
    }
    /// Set Rocket Ammo
    ///@p_RocketType : Rocket Type
    ///@p_Amount : Amount to add
    void Player::SetRocketAmmo(RocketType const p_RocketType, const int32 p_Amount)
    {
        switch (p_RocketType)
        {
        case RocketType::ROCKET_TYPE_R310:
            m_Ammo.m_RocketR310 += p_Amount;

            if (m_Ammo.m_RocketR310 <= 0)
                m_Ammo.m_RocketR310 = 0;
            break;
        case RocketType::ROCKET_TYPE_PLT_2026:
            m_Ammo.m_RocketPLT2026 += p_Amount;

            if (m_Ammo.m_RocketPLT2026 <= 0)
                m_Ammo.m_RocketPLT2026 = 0;
            break;
        case RocketType::ROCKET_TYPE_PLT_2021:
            m_Ammo.m_RocketPLT2021 += p_Amount;

            if (m_Ammo.m_RocketPLT2021 <= 0)
                m_Ammo.m_RocketPLT2021 = 0;
            break;
        case RocketType::ROCKET_TYPE_NONE:
            /// Don't do anything
            return;
            break;
        default:
            LOG_ASSERT(false, "Player", "Cannot update ammo with rocket type %0", static_cast<uint16>(p_RocketType));
            break;
        }

        if (LabUpgrade* l_LabUpgrade = GetLabUpgrade(LabUpgradeType::LAB_UPGRADE_TYPE_ROCKETS))
            l_LabUpgrade->SetValue(p_Amount);

        SendAmmoUpdate();
    }
    void Player::SetMine(const int32 p_Amount)
    {
        m_Ammo.m_Mines += p_Amount;

		if (m_Ammo.m_Mines <= 0)
			m_Ammo.m_Mines = 0;

		SendAmmoUpdate();
    }
    /// Update Honor
    /// @p_Honor : Honor to Update
    void Player::UpdateHonor(uint32 p_Honor)
    {
        if (HasBooster(BoosterTypes::BOOSTER_TYPE_HON_B01))
            p_Honor = Core::Utils::CalculatePercentage(p_Honor, ToPlayer()->GetBoosterValue(BoosterTypes::BOOSTER_TYPE_HON_B01));

		m_Honor += p_Honor;
	}
    /// Set the player to repairing mode
    /// @p_Repair : Repair
    void Player::Repair(bool p_Repair)
    {
        SetIsRepairing(p_Repair);

        /// Trigger the event to update
        // e.g if player is already at the station
        // then trigger the station again but this time we are repairing
        SetUpdateEvent(true);
    }
    uint32 Player::CalculateSmartBombs() const
    {
        if (!m_Inventory.HasSmartBomb())
            return 0;

        uint32 l_Xenomit = GetResource(Resource::RESOURCE_XENOMIT);
        uint32 l_Mines = m_Ammo.GetMines();
        uint32 l_SmartBombs = 0;

        while (l_Xenomit >= 100 && l_Mines >= 1)
        {
            l_Xenomit -= 100;
            l_Mines -= 1;
            l_SmartBombs += 1;
        }

        return l_SmartBombs;
    }
    uint32 Player::CalculateInstantShield() const
    {
        if (!m_Inventory.HasInstantShield())
            return 0;

        uint32 l_Xenomit = GetResource(Resource::RESOURCE_XENOMIT);
        uint32 l_Mines = m_Ammo.GetMines();
        uint32 l_SmartBombs = 0;

        while (l_Xenomit >= 100 && l_Mines >= 1)
        {
            l_Xenomit -= 100;
            l_Mines -= 1;
            l_SmartBombs += 1;
        }

        return l_SmartBombs;
    }
    /// Reward Credits
    /// This will also send the reward to the client
    void Player::RewardCredits(int32 p_Credits)
    {
        UpdateCredits(p_Credits);

		SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_CREDIT, { p_Credits, m_Credits }));
    }
    /// Reward Uridium
    /// This will also send the reward to the client
    void Player::RewardUridium(int32 p_Uridium)
    {
		UpdateUridium(p_Uridium);

        SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_URIDIUM, { p_Uridium, m_Uridium }));
    }
    /// Reward Honor
    /// This will also send the reward to the client
    void Player::RewardHonor(int32 p_Honor)
    {
        UpdateHonor(p_Honor);

		SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_HONOUR, { p_Honor, m_Honor }));
    }
    /// Reward Experience
    /// This will also send the reward to the client and also the level up packet if the player levels up
    void Player::RewardExperience(int32 p_Experience)
    {
		UpdateExperience(p_Experience);

        SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_EXPERIENCE, { p_Experience, m_Experience, m_Level }));
	}

    void Player::ToggleCloak()
    {
        if (IsCloaked())
            SendSystemMessage("msg_uncloaked");

        SetCloaked(!IsCloaked());
        SendCloak();
    }

    ///////////////////////////////////////////
    //        BOOSTERS
    ///////////////////////////////////////////

    /// Get Booster Type
    BoosterTypes Booster::GetBoosterType() const
    {
        return static_cast<BoosterTypes>(BoosterItemTemplate->Id);
    }
    /// Get Booster Value
    int32 Booster::GetBoosterValue() const
    {
        return BoosterItemTemplate->Value != 0 ? BoosterItemTemplate->Value : BoosterItemTemplate->ValuePercentage;
	}
    uint32 Ammo::GetInstantShields() const
    {
        return m_Player->CalculateInstantShield();
    }
    uint32 Ammo::GetSmartBombs() const
    {
        return m_Player->CalculateSmartBombs();
    }
    void Ammo::Update(uint32 const p_Diff)
    {
        if (m_MineCooldown.Update(p_Diff))
            m_Player->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_CLEAR_COOLDOWNS, { "MIN" }));

        if (m_SmartMineCooldown.Update(p_Diff))
            m_Player->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_CLEAR_COOLDOWNS, { "SMB" }));
    }
}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone