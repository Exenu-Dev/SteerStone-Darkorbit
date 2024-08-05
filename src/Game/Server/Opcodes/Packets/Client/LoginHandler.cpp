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
#include "Socket.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "ZoneManager.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Login Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleLogin(ClientPacket* p_Packet)
    {
        m_Player->SendClientSettings();
        m_Player->GetInventory()->CalculateStats();
        m_Player->SendInitializeShip();
        m_Player->UpdateExtrasInfo();
        m_Player->SendBoosters();
        m_Player->SendMapUpdate();
        m_Player->SendAmmoUpdate();
        m_Player->SendAccountRank();
        m_Player->UpdateOres();
        m_Player->UpdateMaxCargoSpace();
        m_Player->SendDrones();
        m_Player->SendLoggedIn();
        m_Player->SendQuests();
        sZoneManager->AddToMap(m_Player);
        return;


        Server::PacketBuffer l_PacketBuffer;
        l_PacketBuffer.AppendChar("0");
        l_PacketBuffer.AppendChar("9");
        l_PacketBuffer.AppendChar("ini");
        //l_PacketBuffer.AppendChar("case,300,1,0,0{cond,60,1,1,1,t,u,v}case,310,1,0,1{cond,70,5,2,3,NULL,NULL,NULL}cond,80,1,1,1,z,a,b}{cond,90,0,0,0,c,d,e}");
        l_PacketBuffer.AppendChar("case,300,1,0,1,{cond,23,23,2,3,a,b,c,1,10}{cond,70,5,2,3,a,b,c}");
        l_PacketBuffer.AppendChar("1");

        l_PacketBuffer.AppendEndSplitter();
        l_PacketBuffer.AppendCarriage();

        m_Player->SendPacket(&l_PacketBuffer);
    }

    /// Login Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleLogout(ClientPacket* p_Packet)
    {
        m_Player->SetLogout(true);
        sWorldManager->AddPlayerToRemoval(m_Player);
    }

    /// Login Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleCancelLogout(ClientPacket* p_Packet)
    {
        m_Player->m_LoggedIn = true;
        m_Player->SetLogout(false);
    }
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone