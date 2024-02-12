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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.f
*/

#include "Socket.hpp"
#include <iomanip>

namespace SteerStone { namespace Game { namespace Server {

    SINGLETON_P_I(OpCodes);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    OpCodes::OpCodes()
    {

    }
    /// Deconstructor
    OpCodes::~OpCodes()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    OpcodeHandler const OpCodes::m_EmptyHandler =
    {
        "NULL",
        PacketStatus::STATUS_UNHANDLED,
        PacketProcess::PROCESS_PLAYER_THREAD,
        &GameSocket::HandleNULL
    };
    
    /// InitializePackets
    /// Load our packets into storages to be accessed later
    void OpCodes::InitializePackets()
    {
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_LOGIN,             "CLIENT_PACKET_LOGIN",              PacketStatus::STATUS_AUTHENTICATION, PacketProcess::PROCESS_WORLD_THREAD,  &GameSocket::HandleLogin               );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_CHANGE_LASER,      "CLIENT_PACKET_CHANGE_LASER",       PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_WORLD_THREAD,  &GameSocket::HandleChangeLaser         );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_CHANGE_ROCKET,     "CLIENT_PACKET_CHANGE_ROCKET",      PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleChangeRocket        );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_PING,              "CLIENT_PACKET_PING",               PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleClient              );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_OBJECT_MOVE,       "CLIENT_PACKET_OBJECT_MOVE",        PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleObjectMove          );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_INITIALIZE_SHIP,   "CLIENT_PACKET_INITIALIZE_SHIP",    PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleInitializeOpponent  );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_PORTAL_JUMP,       "CLIENT_PACKET_PORTAL_JUMP",        PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandlePortalJump          );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_TRADE_ORE,         "CLIENT_PACKET_TRADE_ORE",          PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleNULL                );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_SELECT_TARGET,     "CLIENT_PACKET_SELECT_TARGET",      PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleSelectTarget        );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_LASER_SHOOT ,      "CLIENT_PACKET_LASER_SHOOT",        PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleLaserShoot          );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_ABORT_LASER,       "CLIENT_PACKET_ABORT_LASER",        PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleAbortLaser          );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_LOG_OUT,           "CLIENT_PACKET_LOG_OUT",            PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_WORLD_THREAD,  &GameSocket::HandleLogout              );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_CANCEL_LOG_OUT,    "CLIENT_PACKET_CANCEL_LOG_OUT",     PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_WORLD_THREAD,  &GameSocket::HandleCancelLogout        );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_LOOT_CARGO,        "CLIENT_PACKET_LOOT_CARGO",         PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleLootCargo           );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_UPDATE_SETTINGS,   "CLIENT_PACKET_UPDATE_SETTINGS",    PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleUpdateSettings      );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_CHANGE_CONFIG,     "CLIENT_PACKET_CHANGE_CONFIG",      PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleChangeConfig        );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_UNKNOWN,           "CLIENT_PACKET_UNKNOWN",            PacketStatus::STATUS_UNHANDLED,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleNULL                );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_ROCKET_ATTACK,     "CLIENT_PACKET_ROCKET_ATTACK",      PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleShootRocket);
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_SELL_RESOURCES,    "CLIENT_PACKET_SELL_RESOURCES",     PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleSellResources       );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_ORE_PRICES,        "CLIENT_PACKET_ORE_PRICES",         PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleOrePrices           );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_SELL_ORE,          "CLIENT_PACKET_SELL_ORE",           PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleSellOre             );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_MINE,              "CLIENT_PACKET_MINE",               PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleMine                );

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        StoreClientPacket(ClientOpCodes::WEB_PACKET_CHECK_ONLINE,          "WEB_PACKET_CHECK_ONLINE",         PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleCheckOnline          );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_GET_ONLINE_COUNT,      "WEB_PACKET_GET_ONLINE_COUNT",     PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleGetOnlineCount       );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_CAN_EQUIP,             "WEB_PACKET_CAN_EQUIP",            PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleCanEquip             );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_UPDATE_INVENTORY,      "WEB_PACKET_UPDATE_INVENTORY",     PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleUpdateInventory      );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_UPDATE_SHIP,           "WEB_PACKET_UPDATE_SHIP",          PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleUpdateShip           );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_BROUGHT_AMMO,          "WEB_PACKET_BROUGHT_AMMO",         PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleBroughtAmmo          );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_UPDATE_DRONES,         "WEB_PACKET_UPDATE_DRONES",        PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleUpdateDrones         );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_UPDATE_BOOSTERS,       "WEB_PACKET_UPDATE_BOOSTERS",      PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleUpdateBoosters       );
        StoreClientPacket(ClientOpCodes::WEB_PACKET_UPDATE_CLANS,          "WEB_PACKET_UPDATE_CLANS",         PacketStatus::STATUS_WEB,           PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleUpdateClans          );


        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        StoreServerPacket(ServerOpCodes::SERVER_PACKET_INITIALIZE_SHIP,     "SERVER_PACKET_INITIALIZE_SHIP",        &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MAP_UPDATE,          "SERVER_PACKET_MAP_UPDATE",             &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MINI_MAP_UPDATE,     "SERVER_PACKET_MINI_MAP_UPDATE",        &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_UPDATE_ROCKET_MINE,  "SERVER_PACKET_MISC_AMMO_UPDATE",       &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_UPDATE_BATTERY,      "SERVER_PACKET_UPDATE_BATTERY",         &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_LOGGED_IN,           "SERVER_PACKET_LOGGED_IN",              &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SPAWN_SHIP,          "SERVER_PACKET_SPAWN_SHIP",             &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_DESPAWN_SHIP,        "SERVER_PACKET_DESPAWN_SHIP",           &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_OBJECT_MOVE,         "SERVER_PACKET_OBJECT_MOVE",            &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CREATE_PORTAL,       "SERVER_PACKET_CREATE_PORTAL",          &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CREATE_STATION,      "SERVER_PACKET_CREATE_STATION",         &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MISC_UPDATE,         "SERVER_PACKET_MISC_UPDATE",            &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_EVENT,               "SERVER_PACKET_EVENT",                  &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_JUMP_PORTAL,         "SERVER_PACKET_SEND_MESSAGE",           &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_DISPLAY_STAR_SYSTEM, "SERVER_PACKET_DISPLAY_STAR_SYSTEM",    &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_LASER_SHOOT,         "SERVER_PACKET_LASER_SHOOT",            &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_LOG_OUT,             "SERVER_PACKET_LOG_OUT",                &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ATTACK_OUT_OF_RANGE, "SERVER_PACKET_ATTACK_OUT_OF_RANGE",    &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ATTACK_IN_RANGE,     "SERVER_PACKET_ATTACK_IN_RANGE",        &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CANCEL_LASTER_SHOOT, "SERVER_PACKET_CANCEL_LASTER_SHOOT",    &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_RECIEVED_DAMAGE,     "SERVER_PACKET_RECIEVED_DAMAGE",        &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MAKE_DAMAGE,         "SERVER_PACKET_MAKE_DAMAGE",            &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_KILL,                "SERVER_PACKET_KILL",                   &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MISS_SELF,           "SERVER_PACKET_MISS_SELF",              &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MISS_TARGET,         "SERVER_PACKET_MISS_TARGET",            &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ESCAPED_THE_ATTACK,  "SERVER_PACKET_ESCAPED_THE_ATTACK",     &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MISC_INFO,           "SERVER_PACKET_MISC_INFO",              &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_REWARD,              "SERVER_PACKET_REWARD",                 &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CARGO,               "SERVER_PACKET_CARGO",                  &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_REMOVE_CARGO,        "SERVER_PACKET_REMOVE_CARGO",           &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CROSS_HAIR,          "SERVER_PACKET_CROSS_HAIR",             &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_TARGET_HEALTH,       "SERVER_PACKET_TARGET_HEALTH",          &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CHANGE_CONFIG,       "SERVER_PACKET_CHANGE_CONFIG",          &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CARGO_BAY_FULL,      "SERVER_PACKET_CARGO_BAY_FULL",         &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ROCKET_ATTACK,       "SERVER_PACKET_ROCKET_ATTACK",          &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_NO_AMMUNITION,       "SERVER_PACKET_NO_AMMUNITION",          &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CHANGE_ROCK_AMMO,    "SERVER_PACKET_CHANGE_ROCK_AMMO",       &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_UPDATE_ORE,          "SERVER_PACKET_UPDATE_ORE",             &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SET_ORE_PRICES,      "SERVER_PACKET_SET_ORE_PRICES",         &GameSocket::HandleServer);

        /// Web Packets
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CHECK_ONLINE,        "SERVER_PACKET_CHECK_ONLINE",           &GameSocket::HandleServer);

        /// Debug Headless Packets
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_HEADLESS_MOVE,       "SERVER_PACKET_HEADLESS_MOVE",      &GameSocket::HandleServer);

        LOG_INFO("OpCodes", "Loaded %0 Client Packets", m_ClientOpCodes.size());
        LOG_INFO("OpCodes", "Loaded %0 Server Packets", m_ServerOpCodes.size());
    } 
    
    /// Store Client packet into storage
    /// @p_Opcode : Opcode Id
    /// @p_Name : Name of Opcode
    /// @p_Status : Status of Opcode
    /// @p_Process : When to process the packet
    /// @p_Handler : Handler Function which we will be accessing too
    void OpCodes::StoreClientPacket(uint8 const p_OpCode, char const* p_Name, PacketStatus const p_Status, PacketProcess p_Process, void(GameSocket::* p_Handler)(ClientPacket* p_Packet))
    {
        OpcodeHandler& l_Ref = m_ClientOpCodes[p_OpCode];
        l_Ref.Name = p_Name;
        l_Ref.Status = p_Status;
        l_Ref.Handler = p_Handler;
    }
    /// Store Server packet into storage
    /// @p_Opcode : Opcode Id
    /// @p_Name : Name of Opcode
    /// @p_Handler : Handler Function which we will be accessing too
    void OpCodes::StoreServerPacket(uint8 const p_OpCode, char const* p_Name, void(GameSocket::* p_Handler)(ClientPacket* p_Packet))
    {
        OpcodeHandler& l_Ref = m_ServerOpCodes[p_OpCode];
        l_Ref.Name = p_Name;
        l_Ref.Handler = p_Handler;
    }

    /// Get Client Packet
    /// @p_Id : Id of client packet we are searching for
    OpcodeHandler const* OpCodes::GetClientPacket(ClientOpCodes p_Id)
    {
        return &m_ClientOpCodes[p_Id];
    }
    /// Get Server Packet
    /// @p_Id : Id of server packet we are searching for
    OpcodeHandler const* OpCodes::GetServerPacket(ServerOpCodes p_Id)
    {
        return &m_ServerOpCodes[p_Id];
    }

    /// Get Client Opcode Name
    std::string OpCodes::GetClientOpCodeName(ClientOpCodes p_Opcode)
    {
        std::ostringstream l_StringStream;
        l_StringStream << '[';

        if (static_cast<uint32>(p_Opcode) < ClientOpCodes::CLIENT_MAX_OPCODE)
        {
            OpcodeHandler const* l_Handler = GetClientPacket(p_Opcode);

            if (l_Handler && l_Handler->Name)
                l_StringStream << l_Handler->Name;
            else
                l_StringStream << "UNKNOWN OPCODE";
        }
        else
            l_StringStream << "INVALID OPCODE";

        l_StringStream << " (" << p_Opcode << ")]";
        return l_StringStream.str();
    }
    /// Get Server Opcode Name
    std::string OpCodes::GetServerOpCodeName(ServerOpCodes p_Opcode)
    {
        std::ostringstream l_StringStream;
        l_StringStream << '[';

        if (static_cast<uint32>(p_Opcode) < ClientOpCodes::CLIENT_MAX_OPCODE)
        {
            if (OpcodeHandler const* l_Handler = GetServerPacket(p_Opcode))
                l_StringStream << l_Handler->Name;
            else
                l_StringStream << "UNKNOWN OPCODE";
        }
        else
            l_StringStream << "INVALID OPCODE";

        l_StringStream << " (" << p_Opcode << ")]";
        return l_StringStream.str();
    }

}   ///< namespace Packet
}   ///< namespace Game
}   ///< namespace SteerStone