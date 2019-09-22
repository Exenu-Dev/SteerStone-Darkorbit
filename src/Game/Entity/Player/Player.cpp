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
#include "Player.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Diagnostic/DiaStopWatch.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_GameSocket : Socket
    Player::Player(Server::GameSocket* p_GameSocket) :
        m_Socket(p_GameSocket ? p_GameSocket->Shared<Server::GameSocket>() : nullptr),
        m_GUID(GUIDType::Player),
        m_Ship(this)
    {
        m_Id                    = 0;
        m_SessionId.clear();
        m_Uridium                = 0;
        m_Credits                = 0;
        m_Jackpot                = 0;
        m_Level                  = 0;
        m_Experience             = 0;
        m_Honor                  = 0;
        m_GatesAchieved          = 0;
        m_ClanId                 = 0;
        m_ClanName.clear();
        m_CompanyId              = Company::NOMAD;
        m_Rank                   = 0;
        m_Premium                = 0;

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
        m_Event           = EventType::EVENT_TYPE_NONE;

        SetType(Type::OBJECT_TYPE_PLAYER);
        SetGUID(ObjectGUID(GUIDType::Player));
    }
    /// Deconstructor
    Player::~Player()
    {
        /// Delete socket
        if (m_Socket)
        {
            m_Socket->CloseSocket();
            m_Socket = nullptr;
        }

        /// Empty Queue
        Server::ClientPacket* l_Packet = nullptr;
        while (m_RecievedQueue.Next(l_Packet))
            delete l_Packet;
    }

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
            m_CompanyId                 = static_cast<Company>(l_Result[10].GetUInt16());
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
            m_SelectedLaser             = l_Result[28].GetUInt32();
            m_SelectedRocket            = l_Result[29].GetUInt32();
            m_DisplayDigits             = l_Result[30].GetBool();
            m_DisplayChat               = l_Result[31].GetBool();
            m_DisplayDrones             = l_Result[32].GetBool();
            m_ShowStarSystem            = l_Result[33].GetBool();
            m_IgnoreCargo               = l_Result[34].GetBool();
            m_IgnoreHostileCargo        = l_Result[35].GetBool();
            m_AutoChangeAmmo            = l_Result[36].GetBool();
            m_EnableBuyFast             = l_Result[37].GetBool();

            /// Now load ship details
            m_Ship.LoadFromDB();

            return true;
        }

        return false;
    }

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
        l_Packet.Id             = m_Id;
        l_Packet.Username       = GetName();
        l_Packet.CompanyId      = static_cast<uint16>(m_CompanyId);
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
        m_Ship.FormulateInitializeShip(l_Packet);

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

    /// Get Ship
    Ship* Player::GetShip()
    {
        return &m_Ship;
    }

    /// Update Player
    /// @p_Diff         : Execution Time
    /// @p_PacketFilter : Type of packet
    bool Player::Update(uint32 p_Diff, Server::PacketFilter& p_PacketFilter)
    {
        Core::Diagnostic::StopWatch l_StopWatch;
        l_StopWatch.Start();

        Server::ClientPacket* l_ClientPacket = nullptr;
        while (m_Socket && m_RecievedQueue.Next(l_ClientPacket, p_PacketFilter))
        {
            Server::OpcodeHandler const* l_OpCodeHandler = sOpCode->GetClientPacket(static_cast<ClientOpCodes>(l_ClientPacket->GetHeader()));
            m_Socket->ExecutePacket(l_OpCodeHandler, l_ClientPacket);
            delete l_ClientPacket;
        }

        if (!m_Socket || m_Socket->IsClosed())
            return false;

        m_OperatorProcessor.ProcessOperators();

        l_StopWatch.Stop();
        if (l_StopWatch.GetElapsed() > 20)
            LOG_WARNING("Player", "Took more than 20ms to update Player!");

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

        LOG_INFO("Packet", "%0", (char*)& p_PacketBuffer->GetContents()[0]);

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