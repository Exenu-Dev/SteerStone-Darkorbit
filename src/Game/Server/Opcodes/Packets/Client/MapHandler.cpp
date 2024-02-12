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

#include "Server/MiscPackets.hpp"
#include "Server/MapPackets.hpp"
#include "Server/ShipPackets.hpp"
#include "Server/AttackPackets.hpp"
#include "Socket.hpp"
#include "Portal.hpp"
#include "BonusBox.hpp"
#include "ZoneManager.hpp"
#include "World.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleObjectMove(ClientPacket* p_Packet)
    {
        /// Client sends NaN when user is logging out
        std::string l_LoggingOut = p_Packet->ReadString();
        if (l_LoggingOut == "NaN")
            return;

        float l_NewPositionX = std::stof(l_LoggingOut);
        float l_NewPositionY = static_cast<float>(p_Packet->ReadUInt32());

        /// The client sends the current position - but we should never trust the client
        /// So skip this
        float l_PrevPositionX = static_cast<float>(p_Packet->ReadUInt32());
        float l_PrevPositionY = static_cast<float>(p_Packet->ReadUInt32());

        m_Player->GetSpline()->Move(l_NewPositionX, l_NewPositionY);
    }
    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleInitializeOpponent(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Attempt to find player in map
        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);
        
        if (!l_Object)
        {
            LOG_WARNING("Map", "Cannot find player Id: %0", l_Id);
            return;
        }

        m_Player->GetMap()->GetGrid(m_Player)->BuildObjectSpawnAndSend(l_Object, m_Player);
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandlePortalJump(ClientPacket* p_Packet)
    {
        Entity::Portal* l_Portal = m_Player->GetGrid()->CanJumpPortal(m_Player);

        /// Okay we can jump
        if (l_Portal)
        {
            /// Send Packet
            Server::Packets::JumpPortal l_Packet;
            l_Packet.MapId      = l_Portal->GetToMapId();
            l_Packet.PortalId   = l_Portal->GetObjectGUID().GetCounter();
            m_Player->SendPacket(l_Packet.Write());

            Game::Map::JumpQueueCordinates l_JumpQueueCordinates;
            l_JumpQueueCordinates.MapId = l_Portal->GetToMapId();
            l_JumpQueueCordinates.PositionX = l_Portal->GetToPositionX();
            l_JumpQueueCordinates.PositionY = l_Portal->GetToPositionY();

            m_Player->GetMap()->AddToJumpQueue(m_Player, l_JumpQueueCordinates);
            m_Player->m_Jumping = true;
        }
        else
            SendPacket(Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { "You are not near a portal!" }));
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleSelectTarget(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Cannot target ourself
        if (l_Id == m_Player->GetObjectGUID().GetCounter())
            return;

        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);

        if (!l_Object)
        {
            LOG_WARNING("Socket", "Attempted to find object %0 in map but does not exist!", l_Id);
            return;
        }

        if (l_Object->GetType() != Entity::Type::OBJECT_TYPE_MOB && l_Object->GetType() != Entity::Type::OBJECT_TYPE_PLAYER)
        {
            LOG_WARNING("Socket", "Attempted to select a target which is not a unit or a player!", l_Id);
            return;
        }

        m_Player->SetTarget(l_Object->ToUnit());

        Server::Packets::Attack::TargetHealth l_TargetHealthPacket;
        l_TargetHealthPacket.Shield = m_Player->GetTarget()->GetShield();
        l_TargetHealthPacket.MaxShield = m_Player->GetTarget()->GetMaxShield();
        l_TargetHealthPacket.HitPoints = m_Player->GetTarget()->GetHitPoints();
        l_TargetHealthPacket.MaxHitPoints = m_Player->GetTarget()->GetHitMaxPoints();
        m_Player->SendPacket(l_TargetHealthPacket.Write());
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleLaserShoot(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Cannot target ourself
        if (l_Id == m_Player->GetObjectGUID().GetCounter())
            return;

        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);

        if (!l_Object)
        {
            m_Player->SendClearRocketCooldown();
            LOG_WARNING("Socket", "Attempted to find object %0 in map but does not exist!", l_Id);
            return;
        }

        if (l_Object->GetType() != Entity::Type::OBJECT_TYPE_MOB && l_Object->GetType() != Entity::Type::OBJECT_TYPE_PLAYER)
        {
            m_Player->SendClearRocketCooldown();
            LOG_WARNING("Socket", "Attempted to select a target which is not a unit or a player!", l_Id);
            return;
        }

        // Cannot attack if there's player has no weapons
        if (m_Player->GetInventory()->GetWeaponCount() == 0)
        {
            m_Player->SendClearRocketCooldown();
            SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { "You need a weapon to be equipped to attack!" }));
            return;
        }

        m_Player->Attack(l_Object->ToUnit());
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleAbortLaser(ClientPacket* p_Packet)
    {
        m_Player->CancelAttack();
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleShootRocket(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Cannot target ourself
        if (l_Id == m_Player->GetObjectGUID().GetCounter())
            return;

        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);

        if (!l_Object)
        {
            LOG_WARNING("Socket", "Attempted to find object %0 in map but does not exist!", l_Id);
            return;
        }

        if (l_Object->GetType() != Entity::Type::OBJECT_TYPE_MOB && l_Object->GetType() != Entity::Type::OBJECT_TYPE_PLAYER)
        {
            LOG_WARNING("Socket", "Attempted to select a target which is not a unit or a player!", l_Id);
            return;
        }

        m_Player->Attack(l_Object->ToUnit(), Entity::ATTACK_TYPE_ROCKET);
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleSellResources(ClientPacket* p_Packet)
    {
        int test = 0;
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleLootCargo(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        if (m_Player->GetCargoSpace() == 0)
        {
            m_Player->SendPacket(Server::Packets::Ship::CargoFull().Write());
            return;
        }

        Entity::Object* l_Object = m_Player->GetGrid()->FindObject(l_Id);

        if (!l_Object)
        {
            LOG_WARNING("Socket", "Cannot find object %0!", l_Id);
            return;
        }

        if (!l_Object->IsBonusBox())
        {
            LOG_WARNING("Socket", "Attempted to loot cargo but object is not a bonus box!");
            return;
        }

        l_Object->ToBonusBox()->RewardCredit(m_Player);
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleUpdateSettings(ClientPacket* p_Packet)
    {
        if (p_Packet->ReadString() == "SET")
        {
            m_Player->UpdateClientSettings(p_Packet);
        }
    }

    void GameSocket::HandleMine(ClientPacket* p_Packet)
    {
        PacketBuffer l_PacketBuffer;
        l_PacketBuffer.AppendUInt32(0);
        l_PacketBuffer.AppendChar("n");
        l_PacketBuffer.AppendChar("SMB");
        l_PacketBuffer.AppendUInt32(m_Player->GetId());

        l_PacketBuffer.AppendEndSplitter();
        l_PacketBuffer.AppendCarriage();

        SendPacket(&l_PacketBuffer);
	}

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone