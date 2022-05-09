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

#include "Player/Player.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Constructor
    /// @p_Player : Player
    PlayerMapFilter::PlayerMapFilter(Entity::Player* p_Player)
        : PacketFilter(p_Player)
    {
    }
    /// Process Packet in Player::Update
    /// @p_ClientPacket : Packet being processed
    bool PlayerMapFilter::Process(ClientPacket* p_ClientPacket)
    {
        OpcodeHandler const* l_OpCodeHandler = sOpCode->GetClientPacket(static_cast<ClientOpCodes>(p_ClientPacket->GetHeader()));

        if (l_OpCodeHandler->Process != PacketProcess::PROCESS_MAP_THREAD)
            return false;

        Entity::Player* l_Player = m_Player;
        if (!l_Player)
            return false;

        return true;
    }

    /// Constructor
    /// @p_Player : Player
    WorldUpdateFilter::WorldUpdateFilter(Entity::Player* p_Player) 
        : PacketFilter(p_Player)
    {
    }

    /// Process Packet in Player::Update
    /// @p_ClientPacket : Packet being processed
    bool WorldUpdateFilter::Process(ClientPacket* p_ClientPacket)
    {
        OpcodeHandler const* l_OpCodeHandler = sOpCode->GetClientPacket(static_cast<ClientOpCodes>(p_ClientPacket->GetHeader()));

        if (l_OpCodeHandler->Process != PacketProcess::PROCESS_WORLD_THREAD)
            return false;

        Entity::Player* l_Player = m_Player;
        if (!l_Player)
            return false;

        return true;
    }

    /// Constructor
    /// @p_Player : Player
    PacketFilter::PacketFilter(Entity::Player* p_Player)
        : m_Player(p_Player)
    {
    }

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone