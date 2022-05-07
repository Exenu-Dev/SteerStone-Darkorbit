/*
* Liam Ashdown
* Copyright (C) 2021
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

#include "Packets/Server/LoginPackets.hpp"
#include "Packets/Server/MapPackets.hpp"
#include "Packets/Server/ShipPackets.hpp"
#include "Packets/Server/MiscPackets.hpp"
#include "World.hpp"
#include "ZoneManager.hpp"
#include "Diagnostic/DiaStopWatch.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_GameSocket : Socket
    Player::Player(Server::GameSocket* p_GameSocket) :
        m_Socket(p_GameSocket ? p_GameSocket->Shared<Server::GameSocket>() : nullptr),
        m_Inventory(this)
    {
        m_Id                    = 0;
        m_SessionId.clear();
        m_Jackpot               = 0;
        m_Level                 = 0;
        m_CargoSpace            = 0;
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

        m_LoggedIn              = false;
        m_Jumping               = false;
        m_LoggingOut            = false;
        m_Event                 = EventType::EVENT_TYPE_NONE;
        m_AttackType            = AttackType::ATTACK_TYPE_NONE;

        /// Timers
        m_IntervalNextSave.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_SAVE_PLAYER_TO_DATABASE));
        m_IntervalRadiation.SetInterval(RADIATION_TIMER);
        ConfigTimer.SetInterval(CONFIG_TIMER);

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

        /// Empty Queue
        Server::ClientPacket* l_Packet = nullptr;
        while (m_RecievedQueue.Next(l_Packet))
            delete l_Packet;

        sZoneManager->RemoveFromMap(this, true);
        
        SaveToDB();
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
        l_PreparedStatement->PrepareStatement("SELECT users.username, users.uridium, users.credits, users.jackpot, users.experience, users.honor, users.gates_achieved, users.clan_id, users.company, users.rank, users.premium, "
            "user_settings.display_boost, user_settings.display_damage, user_settings.display_all_las, user_settings.display_exploration, user_settings.display_name, user_settings.display_firm_icon, user_settings.display_alpha_bg, user_settings.ignore_res, "
            "user_settings.ignore_box, user_settings.convert_gates, user_settings.convert_oppo, user_settings.sound_off, user_settings.bg_music_off, user_settings.display_status, user_settings.display_bubble, user_settings.selected_laser, user_settings.selected_rocket, user_settings.display_digits, "
            "user_settings.display_chat, user_settings.display_drones, user_settings.show_star_system, user_settings.ignore_cargo, user_settings.ignore_hostile_cargo, user_settings.auto_change_ammo, user_settings.enable_buy_fast"
            " FROM users INNER JOIN user_settings ON user_settings.user_id = users.id WHERE users.id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
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
            m_ClanId                    = l_Result[7].GetUInt32();
            // m_ClanName                  = l_Result[8].GetString(); ///< TODO
            m_Company                   = static_cast<Company>(l_Result[8].GetUInt16());
            m_Rank                      = l_Result[9].GetUInt16();
            m_Premium                   = l_Result[10].GetBool();
            m_DisplayBoost              = l_Result[11].GetBool();
            m_DisplayDamage             = l_Result[12].GetBool();
            m_DisplayAllLas             = l_Result[13].GetBool();
            m_DisplayExploration        = l_Result[14].GetBool();
            m_DisplayName               = l_Result[15].GetBool();
            m_DisplayFirmIcon           = l_Result[16].GetBool();
            m_DisplayAlphaBG            = l_Result[17].GetBool();
            m_IgnoreRes                 = l_Result[18].GetBool();
            m_IgnoreBox                 = l_Result[10].GetBool();
            m_ConvertGates              = l_Result[20].GetBool();
            m_ConvertOppo               = l_Result[21].GetBool();
            m_SoundOff                  = l_Result[22].GetBool();
            m_BackgroundMusicOff        = l_Result[23].GetBool();
            m_DisplayStatus             = l_Result[24].GetBool();
            m_DisplayBubble             = l_Result[25].GetBool();
            m_SelectedLaser             = l_Result[26].GetUInt16();
            m_SelectedRocket            = l_Result[27].GetUInt16();
            m_DisplayDigits             = l_Result[28].GetBool();
            m_DisplayChat               = l_Result[29].GetBool();
            m_DisplayDrones             = l_Result[30].GetBool();
            m_ShowStarSystem            = l_Result[31].GetBool();
            m_IgnoreCargo               = l_Result[32].GetBool();
            m_IgnoreHostileCargo        = l_Result[33].GetBool();
            m_AutoChangeAmmo            = l_Result[34].GetBool();
            m_EnableBuyFast             = l_Result[35].GetBool();

            m_Level = CalculateLevel();

            if (m_Premium)
                IntervalLogout.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_PREMIUM_LOG_OUT_TIMER));
            else
                IntervalLogout.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_LOG_OUT_TIMER));
                
            LoadShipFromDB();
            LoadDrones();
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
        l_PreparedStatement->SetUint32(0, m_Id);
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
            m_CargoSpace                = l_Result[6].GetUInt32();
            m_MaxCargoSpace             = l_Result[7].GetUInt32();

            SetGUID(ObjectGUID(GUIDType::Player, 0, m_Id));
            GetSpline()->SetPosition(l_Result[8].GetFloat(), l_Result[9].GetFloat(), l_Result[8].GetFloat(), l_Result[9].GetFloat());
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
    /// Save Player details to database
    void Player::SaveToDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("UPDATE users INNER JOIN user_settings ON user_settings.user_id = users.id "
            "SET users.uridium = ?, users.credits = ?, users.jackpot = ?, users.experience = ?, users.honor = ?, users.gates_achieved = ?, users.clan_id = ?, users.company = ?, users.rank = ?, users.premium = ?, "
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
        l_PreparedStatement->SetUint32(6, m_ClanId);
        l_PreparedStatement->SetUint16(7, static_cast<uint16>(m_Company));
        l_PreparedStatement->SetUint16(8, m_Rank);  
        l_PreparedStatement->SetBool(9, m_Premium);
        l_PreparedStatement->SetBool(10, m_DisplayBoost);
        l_PreparedStatement->SetBool(11, m_DisplayDamage);
        l_PreparedStatement->SetBool(12, m_DisplayAllLas);
        l_PreparedStatement->SetBool(13, m_DisplayExploration);
        l_PreparedStatement->SetBool(14, m_DisplayName);
        l_PreparedStatement->SetBool(15, m_DisplayFirmIcon);
        l_PreparedStatement->SetBool(16, m_DisplayAlphaBG);
        l_PreparedStatement->SetBool(17, m_IgnoreRes);
        l_PreparedStatement->SetBool(18, m_IgnoreBox);
        l_PreparedStatement->SetBool(19, m_ConvertGates);
        l_PreparedStatement->SetBool(20, m_ConvertOppo);
        l_PreparedStatement->SetBool(21, m_SoundOff);
        l_PreparedStatement->SetBool(22, m_BackgroundMusicOff);
        l_PreparedStatement->SetBool(23, m_DisplayStatus);
        l_PreparedStatement->SetBool(24, m_DisplayBubble);
        l_PreparedStatement->SetBool(25, m_SelectedLaser);
        l_PreparedStatement->SetBool(26, m_SelectedRocket);
        l_PreparedStatement->SetBool(27, m_DisplayDigits);
        l_PreparedStatement->SetBool(28, m_DisplayChat);
        l_PreparedStatement->SetBool(29, m_DisplayDrones);
        l_PreparedStatement->SetBool(30, m_ShowStarSystem);
        l_PreparedStatement->SetBool(31, m_IgnoreCargo);
        l_PreparedStatement->SetBool(32, m_IgnoreHostileCargo);
        l_PreparedStatement->SetBool(33, m_AutoChangeAmmo);
        l_PreparedStatement->SetBool(34, m_EnableBuyFast);
        l_PreparedStatement->SetUint32(35, m_Id);
        l_PreparedStatement->ExecuteStatement();

        SaveShipToDB();
    }
    /// Save Ship details to database
    /// @p_Asynchronous : Use Async connection
    void Player::SaveShipToDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("UPDATE user_ships SET type = ?, cargo_space = ?, "
            "position_x = ?, position_y = ?, map_id = ?, use_system_font = ?, "
            "prometium = ?, endurium = ?, terbium = ?, xenomit = ?, prometid = ?, duranium = ?, promerium = ?, palladium = ?, seprom = ?, "
            "battery_lcb_10 = ?, battery_mcb_25 = ?, battery_mcb_50 = ?, battery_ucb_100 = ?, battery_sab_50 = ?, rocket_r310 = ?, " 
            "rocket_plt_2026 = ?, rocket_plt_2021 = ?, mines = ?, smart_bombs = ?, instant_shields = ? WHERE user_id = ?");

        l_PreparedStatement->SetUint16(0,  m_ShipType);
        l_PreparedStatement->SetUint32(1,  m_CargoSpace);
        l_PreparedStatement->SetFloat(2,   GetSpline()->GetPositionX());
        l_PreparedStatement->SetFloat(3,   GetSpline()->GetPositionY());
        l_PreparedStatement->SetUint32(4,  GetMap()->GetId());
        l_PreparedStatement->SetBool(5,    m_UseSystemFont);
        l_PreparedStatement->SetUint32(6,  m_Resources[0]);
        l_PreparedStatement->SetUint32(7,  m_Resources[1]);
        l_PreparedStatement->SetUint32(8,  m_Resources[2]);
        l_PreparedStatement->SetUint32(9,  m_Resources[3]);
        l_PreparedStatement->SetUint32(10, m_Resources[4]);
        l_PreparedStatement->SetUint32(11, m_Resources[5]);
        l_PreparedStatement->SetUint32(12, m_Resources[6]);
        l_PreparedStatement->SetUint32(13, m_Resources[7]);
        l_PreparedStatement->SetUint32(14, m_Resources[8]);
        l_PreparedStatement->SetUint32(15, m_Ammo.m_BatteryLCB10);
        l_PreparedStatement->SetUint32(16, m_Ammo.m_BatteryMCB25);
        l_PreparedStatement->SetUint32(17, m_Ammo.m_BatteryMCB50);
        l_PreparedStatement->SetUint32(18, m_Ammo.m_BatteryUCB100);
        l_PreparedStatement->SetUint32(19, m_Ammo.m_BatterySAB50);
        l_PreparedStatement->SetUint32(20, m_Ammo.m_RocketR310);
        l_PreparedStatement->SetUint32(21, m_Ammo.m_RocketPLT2026);
        l_PreparedStatement->SetUint32(22, m_Ammo.m_RocketPLT2021);
        l_PreparedStatement->SetUint32(23, m_Ammo.m_Mines);
        l_PreparedStatement->SetUint32(24, m_Ammo.m_SmartBombs);
        l_PreparedStatement->SetUint32(25, m_Ammo.m_InstantShields);
        l_PreparedStatement->SetUint32(26, m_Id);
        l_PreparedStatement->ExecuteStatement();
    }
    /// Return Drone Level
    /// @p_Drone : Drone
    uint16 Player::GetDroneLevel(Drone& p_Drone)
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
            DealDamage(this, Core::Utils::CalculatePercentage(m_IntervalRadiation.GetTick() > 5 ? 5 : m_IntervalRadiation.GetTick(), GetHitMaxPoints()), false);
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
            m_SelectedLaser,
            m_SelectedRocket,
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
        l_Packet.ClanId         = m_ClanId;
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
        l_Packet.Shield         = m_Shield;
        l_Packet.MaxShield      = m_MaxShield;
        l_Packet.HitPoints      = m_HitPoints;
        l_Packet.MaxHitPoints   = m_MaxHitPoints;
        l_Packet.PositionX      = GetSpline()->GetPositionX();
        l_Packet.PositionY      = GetSpline()->GetPositionY();
        l_Packet.MapId          = GetMap()->GetId();
        l_Packet.WeaponState    = m_WeaponState;
        l_Packet.ShipId         = m_ShipType;
        l_Packet.CargoSpace     = m_CargoSpace;
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
        l_RocketMineAmmoPacket.SmartBombs     = m_Ammo.m_SmartBombs;
        l_RocketMineAmmoPacket.InstantShields = m_Ammo.m_InstantShields;
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

            SendPacket(Server::Packets::Login::PlayerInfo().Write(Server::Packets::Login::InfoType::INFO_TYPE_SET_SHIELD_HEALTH,
                {
                    (uint32)GetHitPoints(),
                    (uint32)GetHitMaxPoints(),
                    (uint32)GetShield(),
                    (uint32)GetMaxShield(),
                }));

            return; 
        }

        SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { "You cannot change config yet!" }));
    }
    /// Send Drone Info
    Server::PacketBuffer const* Player::BuildDronePacket()
    {
        /// TODO; Clean this code, I don't understand how drones are meant to be parsed,
        /// current implementation is a hack fix and only works for 8 drones
        uint32 l_DroneGroupCount = static_cast<uint32>(((std::ceil(static_cast<float>(m_Drones.size()) / 4.0f))));

        std::string l_Drones;
        uint32 l_GroupCount = 1;        ///< Group count starts at 1

        switch (l_DroneGroupCount)
        {
            /// Back drones
        case 1:
        {
            uint32 l_Counter = 1;

            l_Drones = std::to_string(l_DroneGroupCount) + "/4";

            for (auto l_Itr : m_Drones)
            {
                if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                {
                    if (l_Counter == 1)
                        l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr)) + ",";
                    else
                        l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr)) + ",";
                }
                else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                {
                    if (l_Counter == 1)
                        l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                    else
                        l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                }

                l_Counter++;
            }
        }
        default:
        {
            uint32 l_Counter = 1;

            uint32 l_DroneSize = m_Drones.size();

            switch (l_DroneSize)
            {
            case 5:
            {
                l_Drones += "3/1";

                for (auto l_Itr : m_Drones)
                {
                    if (l_Counter == 2)
                        l_Drones += "0/4";

                    if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                    {
                        if (l_Counter <= 2)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                    }
                    else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                    {
                        if (l_Counter <= 2)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                    }

                    l_Counter++;
                }
            }
            break;
            case 6:
            {
                l_Drones += "3/2";

                for (auto l_Itr : m_Drones)
                {
                    if (l_Counter == 3)
                        l_Drones += "0/4";
                    else if (l_Counter == 7)
                        l_Drones += "0/2";

                    if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                    {
                        if (l_Counter == 3 || l_Counter == 7)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                    }
                    else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                    {
                        if (l_Counter == 3 || l_Counter == 7)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                    }

                    l_Counter++;
                }
            }
            break;
            case 7:
            {
                l_Drones += "3/1";

                for (auto l_Itr : m_Drones)
                {
                    if (l_Counter == 2)
                        l_Drones += "0/4";
                    else if (l_Counter == 6)
                        l_Drones += "0/2";

                    if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                    {
                        if (l_Counter == 2 || l_Counter == 6)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                    }
                    else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                    {
                        if (l_Counter == 2 || l_Counter == 6)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                    }

                    l_Counter++;
                }
            }
            break;
            case 8:
            {
                l_Drones += "3/2";

                for (auto l_Itr : m_Drones)
                {
                    if (l_Counter == 3)
                        l_Drones += "0/4";
                    else if (l_Counter == 7)
                        l_Drones += "0/2";

                    if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                    {
                        if (l_Counter == 3 || l_Counter == 7 || l_Counter == 1)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                    }
                    else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                    {
                        if (l_Counter == 3 || l_Counter == 7 || l_Counter == 1)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                    }

                    l_Counter++;
                }
            }
            break;
            }
        }
        break;
        }

        l_Drones.pop_back();

        return Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_DRONES, { GetObjectGUID().GetCounter(), l_Drones});
    }
    /// Send Drone Info
    void Player::SendDrones()
    {
        /// TODO; Clean this code, I don't understand how drones are meant to be parsed,
        /// current implementation is a hack fix and only works for 8 drones

        if (m_Drones.empty())
            return;

        uint32 l_DroneGroupCount = static_cast<uint32>(((std::ceil(static_cast<float>(m_Drones.size()) / 4.0f))));

        std::string l_Drones;
        uint32 l_GroupCount = 1;        ///< Group count starts at 1
        
        switch (l_DroneGroupCount)
        {
            /// Back drones
            case 1:
            {
                uint32 l_Counter = 1;

                l_Drones = std::to_string(l_DroneGroupCount) + "/4";

                for (auto l_Itr : m_Drones)
                {
                    if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                    {
                        if (l_Counter == 1)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr)) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr)) + ",";
                    }
                    else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                    {
                        if (l_Counter == 1)
                            l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                        else
                            l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                    }

                    l_Counter++;
                }
            }
            default:
            {
                uint32 l_Counter = 1;

                uint32 l_DroneSize = m_Drones.size();

                switch (l_DroneSize)
                {
                    case 5:
                    {
                        l_Drones += "3/1";

                        for (auto l_Itr : m_Drones)
                        {
                            if (l_Counter == 2)
                                l_Drones += "0/4";

                            if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                            {
                                if (l_Counter <= 2)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                            }
                            else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                            {
                                if (l_Counter <= 2)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                            }

                            l_Counter++;
                        }
                    }
                    break;
                    case 6:
                    {
                        l_Drones += "3/2";

                        for (auto l_Itr : m_Drones)
                        {
                            if (l_Counter == 3)
                                l_Drones += "0/4";
                            else if (l_Counter == 7)
                                l_Drones += "0/2";

                            if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                            {
                                if (l_Counter == 3 || l_Counter == 7)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                            }
                            else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                            {
                                if (l_Counter == 3 || l_Counter == 7)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                            }

                            l_Counter++;
                        }
                    }
                    break;
                    case 7:
                    {
                        l_Drones += "3/1";

                        for (auto l_Itr : m_Drones)
                        {
                            if (l_Counter == 2)
                                l_Drones += "0/4";
                            else if (l_Counter == 6)
                                l_Drones += "0/2";

                            if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                            {
                                if (l_Counter == 2 || l_Counter == 6)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                            }
                            else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                            {
                                if (l_Counter == 2 || l_Counter == 6)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                            }

                            l_Counter++;
                        }
                    }
                    break;
                    case 8:
                    {
                        l_Drones += "3/2";

                        for (auto l_Itr : m_Drones)
                        {
                            if (l_Counter == 3)
                                l_Drones += "0/4";
                            else if (l_Counter == 7)
                                l_Drones += "0/2";

                            if (l_Itr.Type == DroneType::DRONE_TYPE_FLAK)
                            {
                                if (l_Counter == 3 || l_Counter == 7 || l_Counter == 1)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 9) + ",";
                            }
                            else if (l_Itr.Type == DroneType::DRONE_TYPE_IRIS)
                            {
                                if (l_Counter == 3 || l_Counter == 7 || l_Counter == 1)
                                    l_Drones += "-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                                else
                                    l_Drones += "0-" + std::to_string(GetDroneLevel(l_Itr) + 19) + ",";
                            }

                            l_Counter++;
                        }
                    }
                    break;
                }
            }
            break;
        }

        l_Drones.pop_back();

        SendPacket(Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_DRONES, { GetObjectGUID().GetCounter(), l_Drones }));
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
    }

    /// Update Experience
    /// @p_Experience : Experience to be added
    void Player::UpdateExperience(uint32 const p_Experience)
    {
        m_Experience += p_Experience;

        uint32 l_Level = CalculateLevel();

        /// Update level and send to client
        if (m_Level < l_Level)
        {
            SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_LEVEL_UP, { l_Level, LevelExperience::All[l_Level] - m_Experience }));
            m_Level = l_Level;
        }
    }

    /// Set Battery Ammo
    ///@p_BatteryType : Battery Type
    ///@p_Amount : Amount to add
    void Player::SetBatteryAmmo(BatteryType const p_BatteryType, const uint32 p_Amount)
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

        SendAmmoUpdate();
    }
    /// Set Rocket Ammo
    ///@p_RocketType : Rocket Type
    ///@p_Amount : Amount to add
    void Player::SetRocketAmmo(RocketType const p_RocketType, const uint32 p_Amount)
    {
        switch (p_RocketType)
        {
        case RocketType::ROCKET_TYPE_R310:
            m_Ammo.m_RocketR310 += p_Amount;
            break;
        case RocketType::ROCKET_TYPE_PLT_2026:
            m_Ammo.m_RocketPLT2026 += p_Amount;
            break;
        case RocketType::ROCKET_TYPE_PLT_2021:
            m_Ammo.m_RocketPLT2021 += p_Amount;
            break;
        default:
            LOG_ASSERT(false, "Player", "Cannot update ammo with rocket type %0", static_cast<uint16>(p_RocketType));
            break;
        }

        SendAmmoUpdate();
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone