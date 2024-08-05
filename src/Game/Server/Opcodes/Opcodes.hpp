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

#ifndef _OPCODES_OPCODES_HPP_
#define _OPCODES_OPCODES_HPP_
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Singleton/Singleton.hpp"
#include "PacketFilter.hpp"
#endif /* _OPCODES_OPCODES_HPP_ */

enum class PacketStatus
{
    STATUS_UNHANDLED,           ///< Packet not handled
    STATUS_AUTHENTICATION,      ///< Player has not been created yet
    STATUS_WEB,                 ///< Packet has came through the web
    STATUS_LOGGED_IN            ///< Player is created and in world        
};

enum class PacketProcess
{  
    PROCESS_WORLD_THREAD,       ///< Process Packet in Player::Update           (Thread Unsafe)
    PROCESS_MAP_THREAD,         ///< Process Packet in Map::Update              (Thread Safe)
    PROCESS_PLAYER_THREAD       ///< Process Packet in Socket::ReadIncomingData (Network Thread; For non implemented packets) 
                                ///< TODO; Call this network thread? Because the CMS uses this to send packets
};

enum ClientOpCodes : uint8
{
    /// Client
    CLIENT_PACKET_LOGIN                 = 0x21,
    CLIENT_PACKET_CHANGE_LASER          = 0x75,
    CLIENT_PACKET_CHANGE_ROCKET         = 0x64,
    CLIENT_PACKET_PING                  = 0x50,
    CLIENT_PACKET_OBJECT_MOVE           = 0x31,
    CLIENT_PACKET_INITIALIZE_SHIP       = 0x69,
    CLIENT_PACKET_PORTAL_JUMP           = 0x6A,
    CLIENT_PACKET_TRADE_ORE             = 0x62,
    CLIENT_PACKET_SELECT_TARGET         = 0x4C,
    CLIENT_PACKET_LASER_SHOOT           = 0x61,
    CLIENT_PACKET_ABORT_LASER           = 0X47,
    CLIENT_PACKET_LOG_OUT               = 0x6C,
    CLIENT_PACKET_CANCEL_LOG_OUT        = 0x6F,
    CLIENT_PACKET_LOOT_CARGO            = 0x78,
    CLIENT_PACKET_UPDATE_SETTINGS       = 0x41,
    CLIENT_PACKET_UPDATE_MISC           = 0x53, ///< This could be named better, not sure what to call this, handles config change, jump etc...
    CLIENT_PACKET_UNKNOWN               = 0x62,
    CLIENT_PACKET_ROCKET_ATTACK         = 0x76,
    CLIENT_PACKET_SELL_RESOURCES        = 0x62,
    CLIENT_PACKET_SELL_ORE              = 0x54,
    CLIENT_PACKET_ORE_PRICES            = 0x62,
    CLIENT_PACKET_COLLECT_ORE           = 0x77,
    CLIENT_PACKET_QUEST_HUD             = 0x39,

    /// Web
    WEB_PACKET_CHECK_ONLINE             = 0x22,
    WEB_PACKET_GET_ONLINE_COUNT         = 0x23,
    WEB_PACKET_CAN_EQUIP                = 0x24,
    WEB_PACKET_UPDATE_INVENTORY         = 0x25,
    WEB_PACKET_UPDATE_SHIP              = 0x29,
    WEB_PACKET_BROUGHT_AMMO             = 0x26,
    WEB_PACKET_UPDATE_DRONES            = 0x27,
    WEB_PACKET_UPDATE_BOOSTERS          = 0x28,
    WEB_PACKET_UPDATE_CLANS             = 0x32,
    WEB_PAKET_UPDATE_QUESTS			    = 0x2E,
    WEB_PACKET_CLOAK					= 0x2F,

    CLIENT_MAX_OPCODE                   = 0x7B
};

enum ServerOpCodes : uint8
{
    SERVER_PACKET_INITIALIZE_SHIP       = 0x49,
    SERVER_PACKET_MAP_UPDATE            = 0x69,
    SERVER_PACKET_MINI_MAP_UPDATE       = 0x6D,
    SERVER_PACKET_UPDATE_ROCKET_MINE    = 0x33,
    SERVER_PACKET_UPDATE_BATTERY        = 0x42,
    SERVER_PACKET_LOGGED_IN             = 0x38,
    SERVER_PACKET_SPAWN_SHIP            = 0x43,
    SERVER_PACKET_DESPAWN_SHIP          = 0x52,
    SERVER_PACKET_OBJECT_MOVE           = 0x31,
    SERVER_PACKET_DISPLAY_SUCC_DEBUG    = 0x53,
    SERVER_PACKET_CREATE_PORTAL         = 0x70,
    SERVER_PACKET_CREATE_STATION        = 0x73,
    SERVER_PACKET_MISC_UPDATE           = 0x41,
    SERVER_PACKET_EVENT                 = 0x44,
    SERVER_PACKET_JUMP_PORTAL           = 0x55,
    SERVER_PACKET_DISPLAY_STAR_SYSTEM   = 0x7A,
    SERVER_PACKET_LASER_SHOOT           = 0x61,
    SERVER_PACKET_LOG_OUT               = 0x6C,
    SERVER_PACKET_ATTACK_OUT_OF_RANGE   = 0x4F,
    SERVER_PACKET_ATTACK_IN_RANGE       = 0x58,
    SERVER_PACKET_CANCEL_LASTER_SHOOT   = 0x47,
    SERVER_PACKET_RECIEVED_DAMAGE       = 0x59,
    SERVER_PACKET_MAKE_DAMAGE           = 0x48,
    SERVER_PACKET_KILL                  = 0x4B,
    SERVER_PACKET_MISS_SELF             = 0x4D,
    SERVER_PACKET_MISS_TARGET           = 0x5A,
    SERVER_PACKET_ESCAPED_THE_ATTACK    = 0x56,
    SERVER_PACKET_MISC_INFO             = 0x6E,
    SERVER_PACKET_REWARD                = 0x79,
    SERVER_PACKET_CARGO                 = 0x63,
    SERVER_PACKET_REMOVE_CARGO          = 0x32,
    SERVER_PACKET_TARGET_HEALTH         = 0x4E,
    SERVER_PACKET_CARGO_BAY_FULL        = 0x66,
    SERVER_PACKET_SHOOT_ROCKET          = 0x77,
    SERVER_PACKET_CROSS_HAIR            = 0x36,
    SERVER_PACKET_CHANGE_CONFIG         = 0x53,
    SERVER_PACKET_ROCKET_ATTACK         = 0x76,
    SERVER_PACKET_NO_AMMUNITION         = 0x57,
    SERVER_PACKET_CHANGE_ROCK_AMMO      = 0x34,
    SERVER_PACKET_UPDATE_ORE            = 0x45,
    SERVER_PACKET_SET_ORE_PRICES        = 0x67,
    SERVER_PACKET_SPAWN_ORE             = 0x72,
    SERVER_PACKET_SPAWN_MINE            = 0x4C,
    SERVER_PACKET_DESPAWN_ORE           = 0x71,
    SERVER_PACKET_MATERIAL_COLLECT_FAIL = 0x68,
    SERVER_PACKET_QUEST_HUD			    = 0x39,

    /// Web Packets
    SERVER_PACKET_CHECK_ONLINE          = 0x22,

    /// Debug Headless Packets
    SERVER_PACKET_HEADLESS_MOVE         = 0x21,

    SERVER_MAX_OPCODE                   = 0x7B
};

namespace SteerStone { namespace Game { namespace Server {

    class GameSocket;

    /// Opcode Handler
    struct OpcodeHandler
    {
        char const* Name;
        PacketStatus Status;
        PacketProcess Process;
        void (GameSocket::*Handler)(ClientPacket* p_Packet);
    };

    using OpCodeMap =  std::map<uint64, OpcodeHandler>;

    /// Stores Opcodes handlers for server and client
    class OpCodes
    {
        SINGLETON_P_D(OpCodes);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Initialize Client and Server Handlers
            void InitializePackets();

            /// Get Client Packet
            /// @p_Id : Id of client packet we are searching for
            OpcodeHandler const* GetClientPacket(ClientOpCodes p_Id);
            /// Get Server Packet
            /// @p_Id : Id of server packet we are searching for
            OpcodeHandler const* GetServerPacket(ServerOpCodes p_Id);

            /// Get Client Opcode Name
            std::string GetClientOpCodeName(ClientOpCodes p_Opcode);
            /// Get Server Opcode Name
            std::string GetServerOpCodeName(ServerOpCodes p_Opcode);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            /// Store Client packet into storage
            /// @p_Opcode : Opcode Id
            /// @p_Name : Name of Opcode
            /// @p_Status : Status of Opcode
            /// @p_Process : When to process the packet
            /// @p_Handler : Handler Function which we will be accessing too
            void StoreClientPacket(uint8 const p_OpCode, char const* p_Name, PacketStatus const p_Status, PacketProcess p_Process, void (GameSocket::*handler)(ClientPacket* p_Packet));
            /// Store Server packet into storage
            /// @p_Opcode : Opcode Id
            /// @p_Name : Name of Opcode
            /// @p_Handler : Handler Function which we will be accessing too
            void StoreServerPacket(uint8 const p_OpCode, char const* p_Name, void (GameSocket::* p_Handler)(ClientPacket* p_Packet));

        private:
            static OpcodeHandler const m_EmptyHandler;      ///< Empty handler if client packet has not been given a handler yet
            OpCodeMap m_ClientOpCodes;                      ///< Holds client packets
            OpCodeMap m_WebOpCodes;                         ///< Holds web packets
            OpCodeMap m_ServerOpCodes;                      ///< Holds server packets
    };

}   ///< namespace Packet
}   ///< namespace Game
}   ///< namespace SteerStone

#define sOpCode SteerStone::Game::Server::OpCodes::GetSingleton()