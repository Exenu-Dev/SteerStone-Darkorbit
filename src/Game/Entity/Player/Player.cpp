/*
* Liam Ashdown
* Copyright (C) 2019
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

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_GameSocket : Socket
    Player::Player(Server::GameSocket* p_GameSocket) :
        m_Socket(p_GameSocket ? p_GameSocket->Shared<Server::GameSocket>() : nullptr),
        m_Inventory(this)
    {
        m_Id                    = 0;
        m_SessionId.clear();
        m_Uridium               = 0;
        m_Credits               = 0;
        m_Jackpot               = 0;
        m_Level                 = 0;
        m_Experience            = 0;
        m_Honor                 = 0;
        m_ShipId                = PlayerShips::Phoenix;
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

        m_AttackRange           = sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_PLAYER_ATTACK_RANGE);

        SetType(Type::OBJECT_TYPE_PLAYER);
    }
    /// Deconstructor
    Player::~Player()
    {
        /// Delete socket
        if (m_Socket)
        {
            Server::Packets::LogOut l_Packet;
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
        l_PreparedStatement->PrepareStatement("SELECT accounts.username, accounts.uridium, accounts.credits, accounts.jackpot, accounts.level, accounts.experience, accounts.honor, accounts.gates_achieved, accounts.clan_id, accounts.clan_name, accounts.company, accounts.rank, accounts.premium, "
            "account_settings.display_boost, account_settings.display_damage, account_settings.display_all_las, account_settings.display_exploration, account_settings.display_name, account_settings.display_firm_icon, account_settings.display_alpha_bg, account_settings.ignore_res, "
            "account_settings.ignore_box, account_settings.convert_gates, account_settings.convert_oppo, account_settings.sound_off, account_settings.bg_music_off, account_settings.display_status, account_settings.display_bubble, account_settings.selected_laser, account_settings.selected_rocket, account_settings.display_digits, "
            "account_settings.display_chat, account_settings.display_drones, account_settings.show_star_system, account_settings.ignore_cargo, account_settings.ignore_hostile_cargo, account_settings.auto_change_ammo, account_settings.enable_buy_fast"
            " FROM accounts INNER JOIN account_settings ON account_settings.id = accounts.id WHERE accounts.id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();
            
            SetName(l_Result[0].GetString());
            m_Uridium                   = l_Result[1].GetUInt32();
            m_Credits                   = l_Result[2].GetUInt32();
            m_Jackpot                   = l_Result[3].GetUInt32();
            m_Level                     = l_Result[4].GetUInt32();
            m_Experience                = l_Result[5].GetUInt32();
            m_Honor                     = l_Result[6].GetUInt32();
            m_GatesAchieved             = l_Result[7].GetUInt16();
            m_ClanId                    = l_Result[8].GetUInt32();
            m_ClanName                  = l_Result[9].GetString();
            m_Company                   = static_cast<Company>(l_Result[10].GetUInt16());
            m_Rank                      = l_Result[11].GetUInt16();
            m_Premium                   = l_Result[12].GetBool();
            m_DisplayBoost              = l_Result[13].GetBool();
            m_DisplayDamage             = l_Result[14].GetBool();
            m_DisplayAllLas             = l_Result[15].GetBool();
            m_DisplayExploration        = l_Result[16].GetBool();
            m_DisplayName               = l_Result[17].GetBool();
            m_DisplayFirmIcon           = l_Result[18].GetBool();
            m_DisplayAlphaBG            = l_Result[19].GetBool();
            m_IgnoreRes                 = l_Result[20].GetBool();
            m_IgnoreBox                 = l_Result[21].GetBool();
            m_ConvertGates              = l_Result[22].GetBool();
            m_ConvertOppo               = l_Result[23].GetBool();
            m_SoundOff                  = l_Result[24].GetBool();
            m_BackgroundMusicOff        = l_Result[25].GetBool();
            m_DisplayStatus             = l_Result[26].GetBool();
            m_DisplayBubble             = l_Result[27].GetBool();
            m_SelectedLaser             = l_Result[28].GetUInt16();
            m_SelectedRocket            = l_Result[29].GetUInt16();
            m_DisplayDigits             = l_Result[30].GetBool();
            m_DisplayChat               = l_Result[31].GetBool();
            m_DisplayDrones             = l_Result[32].GetBool();
            m_ShowStarSystem            = l_Result[33].GetBool();
            m_IgnoreCargo               = l_Result[34].GetBool();
            m_IgnoreHostileCargo        = l_Result[35].GetBool();
            m_AutoChangeAmmo            = l_Result[36].GetBool();
            m_EnableBuyFast             = l_Result[37].GetBool();

            if (m_Premium)
                IntervalLogout.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_PREMIUM_LOG_OUT_TIMER));
            else
                IntervalLogout.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_LOG_OUT_TIMER));

            #ifdef  HEADLESS_DEBUG
                if (m_Id == 4)
                {
                    static uint32 l_Counter = 4;
                    m_Id = l_Counter++;
                }
            #endif
                
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
            "position_x, position_y, map_id, max_battery, max_rockets, weapon_state, use_system_font,"
            " battery_lcb_10, battery_mcb_25, battery_mcb_50, battery_ucb_100, battery_sab_50, rocket_r310, rocket_plt_2026, rocket_plt_2021, mines, smart_bombs, instant_shields FROM account_ship WHERE id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

            m_ShipId                    = static_cast<PlayerShips>(l_Result[0].GetUInt16());
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
            m_WeaponState               = l_Result[13].GetUInt16();
            m_UseSystemFont             = l_Result[14].GetBool();

            m_Ammo.m_BatteryLCB10       = l_Result[15].GetInt32();
            m_Ammo.m_BatteryMCB25       = l_Result[16].GetInt32();
            m_Ammo.m_BatteryMCB50       = l_Result[17].GetInt32();
            m_Ammo.m_BatteryUCB100      = l_Result[18].GetInt32();
            m_Ammo.m_BatterySAB50       = l_Result[19].GetInt32();

            m_Ammo.m_RocketR310         = l_Result[20].GetInt32();
            m_Ammo.m_RocketPLT2026      = l_Result[21].GetInt32();
            m_Ammo.m_RocketPLT2021      = l_Result[22].GetInt32();

            m_Ammo.m_Mines              = l_Result[23].GetInt32();
            m_Ammo.m_SmartBombs         = l_Result[24].GetInt32();
            m_Ammo.m_InstantShields     = l_Result[25].GetInt32();
        }
        else
            LOG_ASSERT(false, "Player", "Failed to load ship data for player %0", m_Id);
    }
    /// Load Drone Info from database
    void Player::LoadDrones()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT type, points FROM account_drones WHERE id = ?");
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
        l_PreparedStatement->PrepareStatement("UPDATE accounts INNER JOIN account_settings ON account_settings.id = accounts.id "
            "SET accounts.uridium = ?, accounts.credits = ?, accounts.jackpot = ?, accounts.level = ?, accounts.experience = ?, accounts.honor = ?, accounts.gates_achieved = ?, accounts.clan_id = ?, accounts.clan_name = ?, accounts.company = ?, accounts.rank = ?, accounts.premium = ?, "
            "account_settings.display_boost = ?, account_settings.display_damage = ?, account_settings.display_all_las = ?, account_settings.display_exploration = ?, account_settings.display_name = ?, account_settings.display_firm_icon = ?, account_settings.display_alpha_bg = ?, account_settings.ignore_res = ?, "
            "account_settings.ignore_box = ?, account_settings.convert_gates = ?, account_settings.convert_oppo = ?, account_settings.sound_off = ?, account_settings.bg_music_off = ?, account_settings.display_status = ?, account_settings.display_bubble = ?, account_settings.selected_laser = ?, account_settings.selected_rocket = ?, account_settings.display_digits = ?, "
            "account_settings.display_chat = ?, account_settings.display_drones = ?, account_settings.show_star_system = ?, account_settings.ignore_cargo = ?, account_settings.ignore_hostile_cargo = ?, account_settings.auto_change_ammo = ?, account_settings.enable_buy_fast = ? "
            "WHERE accounts.id = ?");

        l_PreparedStatement->SetUint32(0, m_Uridium);
        l_PreparedStatement->SetUint32(1, m_Credits);
        l_PreparedStatement->SetUint32(2, m_Jackpot);
        l_PreparedStatement->SetUint32(3, m_Level);
        l_PreparedStatement->SetUint32(4, m_Experience);
        l_PreparedStatement->SetUint32(5, m_Honor);
        l_PreparedStatement->SetUint32(6, m_GatesAchieved);
        l_PreparedStatement->SetUint32(7, m_ClanId);
        l_PreparedStatement->SetString(8, m_ClanName);
        l_PreparedStatement->SetUint16(9, static_cast<uint16>(m_Company));
        l_PreparedStatement->SetUint16(10, m_Rank);
        l_PreparedStatement->SetBool(11, m_Premium);
        l_PreparedStatement->SetBool(12, m_DisplayBoost);
        l_PreparedStatement->SetBool(13, m_DisplayDamage);
        l_PreparedStatement->SetBool(14, m_DisplayAllLas);
        l_PreparedStatement->SetBool(15, m_DisplayExploration);
        l_PreparedStatement->SetBool(16, m_DisplayName);
        l_PreparedStatement->SetBool(17, m_DisplayFirmIcon);
        l_PreparedStatement->SetBool(18, m_DisplayAlphaBG);
        l_PreparedStatement->SetBool(19, m_IgnoreRes);
        l_PreparedStatement->SetBool(20, m_IgnoreBox);
        l_PreparedStatement->SetBool(21, m_ConvertGates);
        l_PreparedStatement->SetBool(22, m_ConvertOppo);
        l_PreparedStatement->SetBool(23, m_SoundOff);
        l_PreparedStatement->SetBool(24, m_BackgroundMusicOff);
        l_PreparedStatement->SetBool(25, m_DisplayStatus);
        l_PreparedStatement->SetBool(26, m_DisplayBubble);
        l_PreparedStatement->SetBool(27, m_SelectedLaser);
        l_PreparedStatement->SetBool(28, m_SelectedRocket);
        l_PreparedStatement->SetBool(29, m_DisplayDigits);
        l_PreparedStatement->SetBool(30, m_DisplayChat);
        l_PreparedStatement->SetBool(31, m_DisplayDrones);
        l_PreparedStatement->SetBool(32, m_ShowStarSystem);
        l_PreparedStatement->SetBool(33, m_IgnoreCargo);
        l_PreparedStatement->SetBool(34, m_IgnoreHostileCargo);
        l_PreparedStatement->SetBool(35, m_AutoChangeAmmo);
        l_PreparedStatement->SetBool(36, m_EnableBuyFast);
        l_PreparedStatement->SetUint32(37, m_Id);
        l_PreparedStatement->ExecuteStatement();

        SaveShipToDB();
    }
    /// Save Ship details to database
    void Player::SaveShipToDB()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("UPDATE account_ship SET type = ?, cargo_space = ?, "
            "position_x = ?, position_y = ?, map_id = ?, weapon_state = ?, use_system_font = ?, "
            "battery_lcb_10 = ?, battery_mcb_25 = ?, battery_mcb_50 = ?, battery_ucb_100 = ?, battery_sab_50 = ?, rocket_r310 = ?, " 
            "rocket_plt_2026 = ?, rocket_plt_2021 = ?, mines = ?, smart_bombs = ?, instant_shields = ? WHERE id = ?");

        l_PreparedStatement->SetUint16(0,  static_cast<uint16>(m_ShipId));
        l_PreparedStatement->SetUint32(1,  m_CargoSpace);
        l_PreparedStatement->SetFloat(2,   GetSpline()->GetPositionX());
        l_PreparedStatement->SetFloat(3,   GetSpline()->GetPositionY());
        l_PreparedStatement->SetUint32(4,  GetMap()->GetId());
        l_PreparedStatement->SetUint16(5,  m_WeaponState);
        l_PreparedStatement->SetBool(6,    m_UseSystemFont);
        l_PreparedStatement->SetUint32(7,  m_Ammo.m_BatteryLCB10);
        l_PreparedStatement->SetUint32(8,  m_Ammo.m_BatteryMCB25);
        l_PreparedStatement->SetUint32(9,  m_Ammo.m_BatteryMCB50);
        l_PreparedStatement->SetUint32(10, m_Ammo.m_BatteryUCB100);
        l_PreparedStatement->SetUint32(11, m_Ammo.m_BatterySAB50);
        l_PreparedStatement->SetUint32(12, m_Ammo.m_RocketR310);
        l_PreparedStatement->SetUint32(13, m_Ammo.m_RocketPLT2026);
        l_PreparedStatement->SetUint32(14, m_Ammo.m_RocketPLT2021);
        l_PreparedStatement->SetUint32(15, m_Ammo.m_Mines);
        l_PreparedStatement->SetUint32(16, m_Ammo.m_SmartBombs);
        l_PreparedStatement->SetUint32(17, m_Ammo.m_InstantShields);
        l_PreparedStatement->SetUint32(18, m_Id);

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

    /// Update Player
    /// @p_Diff : Execution Time
    bool Player::Update(uint32 p_Diff)
    {
        Core::Diagnostic::StopWatch l_StopWatch;
        l_StopWatch.Start();

        UpdateSurroundings(p_Diff);

        Unit::Update(p_Diff);

        m_OperatorProcessor.ProcessOperators();

        l_StopWatch.Stop();
        if (l_StopWatch.GetElapsed() > 20)
            LOG_WARNING("Player", "Took more than 20ms to update Player!");

        return true;
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
        Server::Packets::PlayerInfo l_Packet;
        l_Packet.Type                    = "SET";
        l_Packet.DisplayBoost            = m_DisplayBoost;
        l_Packet.DisplayDamage           = m_DisplayDamage;
        l_Packet.DisplayAllLas           = m_DisplayAllLas;
        l_Packet.DisplayExploration      = m_DisplayExploration;
        l_Packet.DisplayName             = m_DisplayName;
        l_Packet.DisplayFirmIcon         = m_DisplayFirmIcon;
        l_Packet.DisplayAlphaBG          = m_DisplayAlphaBG;
        l_Packet.IgnoreRes               = m_IgnoreRes;
        l_Packet.IgnoreBox               = m_IgnoreBox;
        l_Packet.ConvertGates            = m_ConvertGates;
        l_Packet.ConvertOppo             = m_ConvertOppo;
        l_Packet.SoundOff                = m_SoundOff;
        l_Packet.BackgroundMusicOff      = m_BackgroundMusicOff;
        l_Packet.DisplayStatus           = m_DisplayStatus;
        l_Packet.DisplayBubble           = m_DisplayBubble;
        l_Packet.SelectedLaser           = m_SelectedLaser;
        l_Packet.SelectedRocket          = m_SelectedRocket;
        l_Packet.DisplayDigits           = m_DisplayDigits;
        l_Packet.DisplayChat             = m_DisplayChat;
        l_Packet.DisplayDrones           = m_DisplayDrones;
        l_Packet.ShowStarSystem          = m_ShowStarSystem;
        l_Packet.IgnoreCargo             = m_IgnoreCargo;
        l_Packet.IgnoreHostileCargo      = m_IgnoreHostileCargo;
        l_Packet.AutoChangeAmmo          = m_AutoChangeAmmo;
        l_Packet.EnableBuyFast           = m_EnableBuyFast;
        SendPacket(l_Packet.Write());
    }
    /// Send ship details
    void Player::SendInitializeShip()
    {
        Server::Packets::InitializeShip l_Packet;
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
        l_Packet.ShipId         = static_cast<uint16>(m_ShipId);
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

        Server::Packets::LoggedIn l_Packet;
        SendPacket(l_Packet.Write());
    }
    /// Send Account Rank
    void Player::SendAccountRank()
    {
        if (GetRank() == Rank::ADMIN)
        {
            Server::Packets::PlayerInfo l_Packet;
            l_Packet.Type = "ADM";
            l_Packet.EnableDebugWindow = 1;
            SendPacket(l_Packet.Write());

            Server::Packets::DisplaySuccDebug l_DisplaySuccDebugPacket;
            SendPacket(l_DisplaySuccDebugPacket.Write());
        }
    }
    /// Send Display Star System
    void Player::SendDisplayStarSystem()
    {
        Server::Packets::DisplayStarSystem l_Packet;
        SendPacket(l_Packet.Write());
    }
    /// Send Map update
    void Player::SendMapUpdate()
    {
        Server::Packets::MapUpdate l_MapUpdatePacket;
        l_MapUpdatePacket.MapId = GetMap()->GetId();
        SendPacket(l_MapUpdatePacket.Write());

        Server::Packets::MiniMapUpdate l_MiniMapUpdatePacket;
        l_MiniMapUpdatePacket.MapId = GetMap()->GetId();
        SendPacket(l_MiniMapUpdatePacket.Write());
    }
    /// Send Battery, Rocket and mine update
    void Player::SendAmmoUpdate()
    {
        Server::Packets::UpdateRocketMineAmmo l_RocketMineAmmoPacket;
        l_RocketMineAmmoPacket.RocketR310     = m_Ammo.m_RocketR310;
        l_RocketMineAmmoPacket.RocketPLT2021  = m_Ammo.m_RocketPLT2021;
        l_RocketMineAmmoPacket.RocketPLT2026  = m_Ammo.m_RocketPLT2026;
        l_RocketMineAmmoPacket.Mines          = m_Ammo.m_Mines;
        l_RocketMineAmmoPacket.SmartBombs     = m_Ammo.m_SmartBombs;
        l_RocketMineAmmoPacket.InstantShields = m_Ammo.m_InstantShields;
        SendPacket(l_RocketMineAmmoPacket.Write());

        Server::Packets::UpdateBatteryAmmo l_BatteryAmmoPacket;
        l_BatteryAmmoPacket.BatteryLCB10      = m_Ammo.m_BatteryLCB10;
        l_BatteryAmmoPacket.BatteryMCB25      = m_Ammo.m_BatteryMCB25;
        l_BatteryAmmoPacket.BatteryMCB50      = m_Ammo.m_BatteryMCB50;
        l_BatteryAmmoPacket.BatteryUCB100     = m_Ammo.m_BatteryUCB100;
        l_BatteryAmmoPacket.BatterySAB50      = m_Ammo.m_BatterySAB50;
        SendPacket(l_BatteryAmmoPacket.Write());
    }
    /// Send Drone Info
    Server::PacketBuffer const Player::BuildDronePacket()
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

        Server::Packets::MiscInfo l_Packet;
        l_Packet.Type   = "d";
        l_Packet.Id     = GetObjectGUID().GetCounter();
        l_Packet.Misc   = l_Drones;

        return *l_Packet.Write();
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

        Server::Packets::MiscInfo l_Packet;
        l_Packet.Type   = "d";
        l_Packet.Id     = GetObjectGUID().GetCounter();
        l_Packet.Misc = l_Drones;

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
            Server::OpcodeHandler const* l_OpCodeHandler = sOpCode->GetClientPacket(static_cast<ClientOpCodes>(l_ClientPacket->GetHeader()));
            m_Socket->ExecutePacket(l_OpCodeHandler, l_ClientPacket);
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

        LOG_INFO("Packet", "%0", (char*)&p_PacketBuffer->GetContents()[0]);

        m_Socket->SendPacket(p_PacketBuffer);
    }
    /// Kick Player from world
    void Player::KickPlayer()
    {
        if (m_Socket)
            m_Socket->CloseSocket();
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone