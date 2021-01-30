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

#include "Packets/Server/DebugPackets.hpp"
#include "Packets/Server/MapPackets.hpp"
#include "Packets/Server/ShipPackets.hpp"
#include "Grid.hpp"
#include "Map.hpp"
#include "BonusBox/BonusBox.hpp"
#include "Mob.hpp"
#include "Portal.hpp"
#include "Station.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Map   : Map who owns the grid
    /// @p_Type  : Type of grid
    /// @p_GridX : Grid X
    /// @p_GridY : Grid Y
    Grid::Grid(Base* p_Map, GridType p_Type, uint32 const p_GridX, uint32 const p_GridY)
        : m_Map(p_Map), m_Type(p_Type), m_GridX(p_GridX), m_GridY(p_GridY)
    {
        /// Grids are active on creation
        m_State = State::Active;

        m_IntervalCheckPlayer.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_CHECK_FOR_PLAYER));
        m_IntervalInteractiveEvents.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_CHECK_FOR_INTERACTIVE_EVENTS));
    }
    /// Deconstructor
    Grid::~Grid()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //               GENERAL
    ///////////////////////////////////////////

    /// Check if Player is near any interactive events
    /// @p_Objects : Portal and stations
    /// @p_Diff : Execution Time
    void Grid::UpdateInteractiveEvents(std::vector<Entity::Object*>& p_Objects, uint32 const p_Diff)
    {
        m_IntervalInteractiveEvents.Update(p_Diff);
        if (!m_IntervalInteractiveEvents.Passed())
            return;

        /// Note; Radiation Event is handled in Player::UpdateRadiationZone because it needs to be handled every 1 second

        /// TODO; Optimize this code
        for (auto l_Itr : m_Players)
        {
            bool l_FoundEvent = false;
            for (auto l_SecondItr : p_Objects)
            {
                if (l_SecondItr->GetType() == Entity::Type::OBJECT_TYPE_PORTAL)
                {
                    if (l_SecondItr->ToPortal()->IsInPortalRadius(l_Itr))
                    {
                        if (l_Itr->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_PORTAL)
                        {
                            l_FoundEvent = true;
                            continue;
                        }

                        Server::Packets::Event l_Packet;
                        l_Packet.PositionX           = l_Itr->GetSpline()->GetPositionX();
                        l_Packet.PositionY           = l_Itr->GetSpline()->GetPositionY();
                        l_Packet.InDemolitionZone    = false;
                        l_Packet.InRadiationZone     = false;
                        l_Packet.PlayRepairAnimation = false;
                        l_Packet.InTradeZone         = false;
                        l_Packet.InJumpZone          = true;
                        l_Packet.Repair              = false;
                        l_Itr->ToPlayer()->SendPacket(l_Packet.Write());

                        l_Itr->ToPlayer()->SetEventType(EventType::EVENT_TYPE_PORTAL);
                        l_FoundEvent = true;

                    }
                }
                else if (l_SecondItr->GetType() == Entity::Type::OBJECT_TYPE_STATION)
                {
                    if (l_SecondItr->ToStation()->IsInStationRadius(l_Itr))
                    {
                        if (l_Itr->ToPlayer()->GetEvent() == EventType::EVENT_TYPE_STATION)
                        {
                            l_FoundEvent = true;
                            continue;
                        }

                        Server::Packets::Event l_Packet;
                        l_Packet.PositionX           = l_Itr->GetSpline()->GetPositionX();
                        l_Packet.PositionY           = l_Itr->GetSpline()->GetPositionY();
                        l_Packet.InDemolitionZone    = false;
                        l_Packet.InRadiationZone     = false;
                        l_Packet.PlayRepairAnimation = false;
                        l_Packet.InTradeZone         = true;
                        l_Packet.InJumpZone          = false;
                        l_Packet.Repair              = false;
                        l_Itr->ToPlayer()->SendPacket(l_Packet.Write());

                        l_Itr->ToPlayer()->SetEventType(EventType::EVENT_TYPE_STATION);
                        l_FoundEvent = true;
                    }
                }
            }

            if (!l_FoundEvent && !l_Itr->IsInRadiationZone())
            {
                Server::Packets::Event l_Packet;
                l_Packet.PositionX           = l_Itr->GetSpline()->GetPositionX();
                l_Packet.PositionY           = l_Itr->GetSpline()->GetPositionY();
                l_Packet.InDemolitionZone    = false;
                l_Packet.InRadiationZone     = false;
                l_Packet.PlayRepairAnimation = false;
                l_Packet.InTradeZone         = false;
                l_Packet.InJumpZone          = false;
                l_Packet.Repair              = false;
                l_Itr->ToPlayer()->SendPacket(l_Packet.Write());

                l_Itr->ToPlayer()->SetEventType(EventType::EVENT_TYPE_NONE);
            }
        }
    }
    /// Update Surrounding objects for players
    void Grid::UpdateSurroundingObjects()
    {
        if (m_Players.empty())
            return;

        /// Get nearby grids and check if they are within range
        std::vector<Grid*> l_Grids;
        m_Map->GetNearbyGrids(m_GridX, m_GridY, l_Grids);

        for (auto l_Itr : m_Players)
        {
            for (auto l_SecondItr : l_Grids)
                l_SecondItr->BuildSurroundingObjects(l_Itr);
        }
    }
    /// Check if a Player is inside the grid
    /// @p_Diff : Execution Time
    void Grid::CheckForPlayer(uint32 const p_Diff)
    {
        m_IntervalCheckPlayer.Update(p_Diff);
        if (!m_IntervalCheckPlayer.Passed())
            return;

        if (m_Players.empty())
            m_State = State::Idle;
    }
    /// Update Grid
    /// @p_Diff : Execution Time
    bool Grid::Update(uint32 const p_Diff)
    {
        CheckForPlayer(p_Diff);
        UpdateSurroundingObjects();

        /// TODO; This isn't really ideal, because the mobs can move to different grids
        /// find a better way instead of copying, but this is okay for now
        std::unordered_map<uint32, Entity::Object*> l_Copy(m_Objects);

        for (auto l_Itr = l_Copy.begin(); l_Itr != l_Copy.end();)
        {
            if (l_Itr->second->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
            {
                Server::PlayerMapFilter l_Filter(l_Itr->second->ToPlayer());
                l_Itr->second->ToPlayer()->ProcessPacket(l_Filter);
                l_Itr->second->ToPlayer()->Update(p_Diff);
            }
            else if (l_Itr->second->GetType() == Entity::Type::OBJECT_TYPE_MOB)
                l_Itr->second->ToMob()->Update(p_Diff);
            else if (l_Itr->second->GetType() == Entity::Type::OBJECT_TYPE_BONUS_BOX)
                l_Itr->second->ToBonusBox()->Update(p_Diff);
            

            l_Itr = l_Copy.erase(l_Itr);
        }

        return false;
    }

    /// Check if near portal
    /// @p_Object : Object being checked
    Entity::Portal* Grid::CanJumpPortal(Entity::Object* p_Object)
    {
        for (auto l_Itr : m_Objects)
        {
            if (l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_PORTAL)
            {
                if (l_Itr.second->ToPortal()->IsInPortalRadius(p_Object))
                    return l_Itr.second->ToPortal();
            }
        }

        return nullptr;
    }

    ///////////////////////////////////////////
    //               OBJECTS
    ///////////////////////////////////////////

    /// Add Object to Grid
    /// @p_Object       : Object being added
    /// @p_SendMovement : Send Movement packet when joining
    void Grid::Add(Entity::Object* p_Object, bool p_SendMovement)
    {
        m_Objects[p_Object->GetObjectGUID().GetCounter()] = p_Object;

        /// If a player is joining then set grid to active
        if (p_Object->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
        {
            m_Players.insert(p_Object->ToPlayer());

            if (m_State == State::Idle)
                m_State = State::Active;
        }

        //LOG_INFO("Grid", "Added GUID: %0 to Grid: X %1 Y %2", p_Object->GetGUID(), m_GridX, m_GridY);
    }
    /// Remove Object from Grid
    /// @p_Object : Object being removed
    /// @p_SendPacket : Send Despawn Packet
    void Grid::Remove(Entity::Object* p_Object, bool p_SendPacket)
    {
        if (p_Object->IsPlayer())
            m_Players.erase(p_Object->ToPlayer());

        if (p_SendPacket)
        {
            if (p_Object->IsBonusBox())
            {
                Server::Packets::RemoveCargo l_Packet;
                l_Packet.Id = p_Object->GetObjectGUID().GetCounter();
                p_Object->GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), p_Object);
            }
            else
            {
                Server::Packets::Ship::DespawnShip l_Packet;
                l_Packet.Id = p_Object->GetObjectGUID().GetCounter();
                p_Object->GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), p_Object, true);
            }
        }
        
        m_Objects.erase(p_Object->GetGUID());

        //LOG_INFO("Grid", "Removed GUID: %0 from Grid: X %1 Y %2", p_Object->GetGUID(), m_GridX, m_GridY);
    }
    /// Unload objects from map
    void Grid::Unload()
    {
        for (auto l_Itr : m_Objects)
        {
            if (l_Itr.second->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
            {
                l_Itr.second->ToPlayer()->ToSocket()->CloseSocket();
            }
        }

        m_Objects.clear();
        m_Players.clear();
    }

    ///////////////////////////////////////////
    //               FINDER
    ///////////////////////////////////////////
    
    /// Find Player
    /// @p_Id : Account Id of Player
    Entity::Object* Grid::FindPlayer(uint32 const p_Id)
    {
        for (auto l_Itr : m_Players)
        {
            if (l_Itr->ToPlayer()->GetId() == p_Id)
                return l_Itr;
        }

        return nullptr;
    }
    /// Find Nearest Player to Object
    /// @p_Object : Object which we are checking against
    /// @p_Distance : Find Object in a specific range
    Entity::Player* Grid::FindNearestPlayer(Entity::Object* p_Object, float p_Distance)
    {
        Entity::Object* l_Object = nullptr;
        float l_MinDistance = 500000.0f;
        for (auto l_Itr : m_Players)
        {
            float l_Distance = Core::Utils::DistanceSquared(p_Object->GetSpline()->GetPositionX(), p_Object->GetSpline()->GetPositionY(),
                l_Itr->GetSpline()->GetPositionX(), l_Itr->GetSpline()->GetPositionY());

            if (l_Distance < p_Distance)
            {
                if (l_MinDistance > l_Distance)
                {
                    l_MinDistance = l_Distance;
                    l_Object = l_Itr;
                }
            }
        }

        return l_Object ? l_Object->ToPlayer() : nullptr;
    }
    /// Find Object
    /// @p_Counter : Counter of Object
    Entity::Object* Grid::FindObject(uint32 const p_Counter)
    {
        for (auto l_Itr : m_Objects)
        {
            if (l_Itr.second->GetObjectGUID().GetCounter() == p_Counter)
                return l_Itr.second;
        }

        return nullptr;
    }

    ///////////////////////////////////////////
    //               PACKETS
    ///////////////////////////////////////////

    /// Send Packet to everyone in grid
    /// @p_Packet : Packet being sent
    void Grid::SendPacketToEveryone(Server::PacketBuffer const* p_Packet)
    {
        for (auto l_Itr : m_Players)
            l_Itr->ToPlayer()->SendPacket(p_Packet);
    }
    /// Send Packet to everyone if object is in surrounding
    /// @p_Packet : Packet being sent
    /// @p_Object : Object being checked
    /// @p_SendToSelf : Send Packet to self
    void Grid::SendPacketIfInSurrounding(Server::PacketBuffer const* p_Packet, Entity::Object* p_Object, bool p_SendToSelf)
    {
        if (p_SendToSelf && p_Object->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
            p_Object->ToPlayer()->SendPacket(p_Packet);
            
        for (auto l_Itr : m_Players)
        {
            if (l_Itr->ToPlayer()->IsInSurrounding(p_Object))
                l_Itr->ToPlayer()->SendPacket(p_Packet);
        }
    }
    /// Move Object
    /// @p_Object : Object being moved
    /// @p_PositionX : New X Axis
    /// @p_PositionY : New Y Axis
    void Grid::Move(Entity::Object* p_Object)
    {
        Server::Packets::Ship::ObjectMove l_Packet;
        l_Packet.Id        = p_Object->GetObjectGUID().GetCounter();
        l_Packet.PositionX = p_Object->GetSpline()->GetPlannedPositionX();
        l_Packet.PositionY = p_Object->GetSpline()->GetPlannedPositionY();
        l_Packet.Time      = p_Object->GetSpline()->GetDestinationTime();
        l_Packet.ShipType  = p_Object->ToUnit()->GetShipType();
        p_Object->GetMap()->SendPacketToNearByGridsIfInSurrounding(l_Packet.Write(), p_Object);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone