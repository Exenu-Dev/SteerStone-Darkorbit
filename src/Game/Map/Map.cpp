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

#include "Server/Opcodes/Packets/Server/MapPackets.hpp"
#include "Map.hpp"
#include "Station.hpp"
#include "Player.hpp"
#include "Portal.hpp"
#include "World.hpp"
#include "ObjectManager.hpp"
#include "ZoneManager.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Database/Database.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Id : Map Id
    Base::Base(uint32 const p_Id)
        : m_Id(p_Id), m_PoolManager(this)
    { 
        /// Define size of map
        switch (p_Id)
        {
        case 29: ///< 4-5
        case 16: ///< 4-4
            m_MapType = MapType::MAP_TYPE_BIG;
            m_GridRadiusX = BIG_MAP_SIZE_X / GRID_CELLS;
            m_GridRadiusY = BIG_MAP_SIZE_Y / GRID_CELLS;
            m_MapSizeX    = BIG_MAP_SIZE_X;
            m_MapSizeY    = BIG_MAP_SIZE_Y;
            break;
        default:
            m_MapType = MapType::MAP_TYPE_NORMAL;   
            m_GridRadiusX = NORMAL_MAP_SIZE_X / GRID_CELLS;
            m_GridRadiusY = NORMAL_MAP_SIZE_Y / GRID_CELLS;
            m_MapSizeX    = NORMAL_MAP_SIZE_X;
            m_MapSizeY    = NORMAL_MAP_SIZE_Y;
            break;
        }

        /// Load Grids
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
                m_Grids[l_X][l_Y] = new Grid(l_X, l_Y);

        m_IntervalJumpPlayer.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_JUMP_DELAY));
        m_IntervalDelayRemoval.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_DELAY_REMOVAL));
    }
    /// Deconstructor
    Base::~Base()
    {

    }

    /// Load Map
    void Base::Load()
    {
        LoadPortals();
        LoadStations();
        LoadMobs();
    }

    /// Load Portals to map
    void Base::LoadPortals()
    {
        for (auto l_Itr : sObjectManager->GetPortalTemplate(m_Id))
        {
            Entity::Portal* l_Portal    = new Entity::Portal();
            l_Portal->m_Id              = l_Itr->PortalId;
            l_Portal->m_CompanyId       = static_cast<Company>(l_Itr->Company);
            l_Portal->m_Type            = static_cast<PortalType>(l_Itr->Type);
            l_Portal->m_ToMapId         = l_Itr->ToMapId;
            l_Portal->m_ToPositionX     = l_Itr->ToPositionX;
            l_Portal->m_ToPositionY     = l_Itr->ToPositionY;
            l_Portal->SetName(l_Itr->Name);
            l_Portal->SetMap(this);
            l_Portal->GetSpline()->SetPosition(l_Itr->PositionX, l_Itr->PositionY);
            l_Portal->SetGUID(Entity::ObjectGUID(Entity::GUIDType::Portal, l_Portal->GetId()));
            Add(l_Portal);

            /// It's faster if we keep a reference of portal because they will always appear on mini map
            m_ConstantObjects.push_back(l_Portal);
        }
    }
    /// Load Stations to map
    void Base::LoadStations()
    {
        if (Entity::StationTemplate const* l_StationTemplate = sObjectManager->GetStationTemplate(m_Id))
        {
            Entity::Station* l_Station  = new Entity::Station();
            l_Station->m_Id             = l_StationTemplate->Id;
            l_Station->m_CompanyId      = static_cast<Company>(l_StationTemplate->Company);
            l_Station->m_Type           = static_cast<StationType>(l_StationTemplate->Type);
            l_Station->m_Peace          = l_StationTemplate->Peace;
            l_Station->SetName(l_StationTemplate->Name);
            l_Station->GetSpline()->SetPosition(l_StationTemplate->PositionX, l_StationTemplate->PositionY);
            l_Station->SetGUID(Entity::ObjectGUID(Entity::GUIDType::Station, l_Station->GetId()));
            l_Station->SetMap(this);
            Add(l_Station);

            /// It's faster if we keep a reference of station because they will always appear on mini map
            m_ConstantObjects.push_back(l_Station);
        }
    }
    /// Load Mobs to map
    void Base::LoadMobs()
    {
        m_PoolManager.Initialize();
    }

    /// Send Objects which always appear on map regardless of what grid user is in
    /// @p_Object : Packet being sent to
    void Base::SendConstantObjects(Entity::Object* p_Object)
    {
        /// Only players
        if (p_Object->GetType() != Entity::Type::OBJECT_TYPE_PLAYER)
            return;

        for (auto l_Itr : m_ConstantObjects)
        {
            if (l_Itr->GetType() == Entity::Type::OBJECT_TYPE_PORTAL)
            {
                Server::Packets::CreatePortal l_Packet;
                l_Packet.Id         = l_Itr->GetObjectGUID().GetCounter();
                l_Packet.Type       = static_cast<uint16>(l_Itr->ToPortal()->GetType());
                l_Packet.PositionX  = l_Itr->GetSpline()->GetPositionX();
                l_Packet.PositionY  = l_Itr->GetSpline()->GetPositionY();
                p_Object->ToPlayer()->SendPacket(l_Packet.Write());
            }
            else if (l_Itr->GetType() == Entity::Type::OBJECT_TYPE_STATION)
            {
                Server::Packets::CreateStation l_Packet;
                l_Packet.Id         = l_Itr->GetObjectGUID().GetCounter();
                l_Packet.Type       = static_cast<uint16>(l_Itr->ToStation()->GetType());
                l_Packet.Name       = l_Itr->GetName();
                l_Packet.CompanyId  = static_cast<uint16>(l_Itr->ToStation()->GetCompany());
                l_Packet.Peace      = l_Itr->ToStation()->IsPeace();
                l_Packet.PositionX  = l_Itr->GetSpline()->GetPositionX();
                l_Packet.PositionY  = l_Itr->GetSpline()->GetPositionY();
                p_Object->ToPlayer()->SendPacket(l_Packet.Write());
            }
        }
    }

    /// Calculate Grid By Object Position
    /// @p_Object : Object
    std::tuple<uint32, uint32> Base::CalculateGridByPosition(Entity::Object* p_Object)
    {
        float l_GridX = std::floor(p_Object->GetSpline()->GetPositionX() / m_GridRadiusX);
        float l_GridY = std::floor(p_Object->GetSpline()->GetPositionY() / m_GridRadiusY);

        /// Is our grid out of bounds?
        if (l_GridX > GRID_CELLS - 1 || l_GridY > GRID_CELLS - 1) ///< Grid starts from 0
            LOG_ASSERT(false, "Map", "Grid X or Y is larger than Grid Cells! Grid X: %0, Grid Y: %1", l_GridX, l_GridY);

        return std::make_tuple(static_cast<uint32>(l_GridX), static_cast<uint32>(l_GridY));
    }

    /// Calculate Grid By Object Position
    /// @p_Object : Object
    /// @p_PositionX : X Axis
    /// @p_PositionY : Y Axis
    std::tuple<uint32, uint32> Base::CalculateGridByNewPosition(Entity::Object* p_Object, float const p_PositionX, float const p_PositionY)
    {
        float l_GridX = std::floor(p_PositionX / m_GridRadiusX);
        float l_GridY = std::floor(p_PositionY / m_GridRadiusY);

        /// Is our grid out of bounds?
        if (l_GridX > GRID_CELLS - 1 || l_GridY > GRID_CELLS - 1) ///< Grid starts from 0
            LOG_ASSERT(false, "Map", "Grid X or Y is larger than Grid Cells! Grid X: %0, Grid Y: %1", l_GridX, l_GridY);

        return std::make_tuple(static_cast<uint32>(l_GridX), static_cast<uint32>(l_GridY));
    }

    /// This function is only called when object is added to map
    /// Add Object to map
    /// @p_Object : Object being added to map
    void Base::Add(Entity::Object* p_Object)
    {
        p_Object->SetGridIndex(CalculateGridByPosition(p_Object));
        m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Add(p_Object);

        /// This must be called after we add object to the grid
        SendConstantObjects(p_Object);
    }
    /// Remove Object from map
    /// @p_Object : Object being removed from map
    void Base::Remove(Entity::Object* p_Object)
    {
        m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Remove(p_Object);
    }
    /// Delay Removal
    /// @p_Player : Player
    /// @p_Object : Object
    void Base::AddToDelayRemoval(Entity::Object* p_Player, Entity::Object* p_Object)
    {
        m_DelayRemoval[p_Player].insert(p_Object);
    }

    /// Unload Maps
    void Base::UnloadAll()
    {
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
        {
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
            {
                Grid* l_Grid = m_Grids[l_X][l_Y];
                l_Grid->Unload();
                delete l_Grid;
            }
        }
    }

    /// Move Object
    /// @p_Object : Object being moved
    void Base::Move(Entity::Object* p_Object)
    {
        std::tuple<uint32, uint32> l_GridIndex = CalculateGridByNewPosition(p_Object, p_Object->GetSpline()->GetPositionX(), p_Object->GetSpline()->GetPositionY());

        /// If both indexes don't match, we've moved to a new grid
        if (std::get<0>(l_GridIndex) != std::get<0>(p_Object->GetGridIndex()) ||
            std::get<1>(l_GridIndex) != std::get<1>(p_Object->GetGridIndex()))
        {
            /// Remove old grid
            m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Remove(p_Object);

            /// Add new grid
            p_Object->SetGridIndex(l_GridIndex);
            m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Add(p_Object);
        }

        m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Move(p_Object);
    }

    /// Add Player to Jump Queue
    /// @p_ObjectPlayer : Player being added
    /// @p_ObjectPortal : Portal
    void Base::AddToJumpQueue(Entity::Object* p_ObjectPlayer, Entity::Object* p_ObjectPortal)
    {
        if (p_ObjectPlayer->ToPlayer()->IsJumping())
            return;

        m_JumpPlayerMutex.lock();
        m_PlayersToJump[p_ObjectPlayer] = p_ObjectPortal;
        m_JumpPlayerMutex.unlock();
    }

    /// Process Jump Queue
    /// @p_Diff : Execution Time
    void Base::ProcessJumpQueue(uint32 const p_Diff)
    {
        m_IntervalJumpPlayer.Update(p_Diff);
        if (!m_IntervalJumpPlayer.Passed())
            return;

        m_JumpPlayerMutex.lock();
        std::unordered_map<Entity::Object*, Entity::Object*> l_Copy(m_PlayersToJump);
        m_JumpPlayerMutex.unlock();

        for (auto l_Itr : l_Copy)
        {
            /// Remove player from map
            sZoneManager->RemoveFromMap(l_Itr.first);

            /// Add to map
            l_Itr.first->SetMap(sZoneManager->GetMap(l_Itr.second->ToPortal()->GetToMapId()));

            l_Itr.first->ToPlayer()->SendDisplayStarSystem();
            l_Itr.first->GetSpline()->SetPosition(l_Itr.second->ToPortal()->GetToPositionX(), l_Itr.second->ToPortal()->GetToPositionY(),
                l_Itr.second->ToPortal()->GetToPositionX(), l_Itr.second->ToPortal()->GetToPositionY());
            l_Itr.first->ToPlayer()->SendInitializeShip();
            l_Itr.first->ToPlayer()->GetShip()->SendMapUpdate();
            l_Itr.first->ToPlayer()->GetShip()->SendAmmoUpdate();
            l_Itr.first->ToPlayer()->SendAccountRank();

            /// Must be called after we send initial login packets
            sZoneManager->AddToMap(l_Itr.first);
            l_Itr.first->ToPlayer()->SetIsJumping(false);
        }

        m_PlayersToJump.clear();
    }

    /// Return Grid
    /// @p_Object : Object we are getting grid from
    Grid* Base::GetGrid(Entity::Object const* p_Object)
    {
        return m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())];
    }

    /// Find Object in map
    /// @p_Counter : Counter of object
    Entity::Object* Base::FindObject(uint32 const p_Counter)
    {
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
        {
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
            {
                Grid* l_Grid = m_Grids[l_X][l_Y];
                
                Entity::Object* l_Object = l_Grid->FindObject(p_Counter);
                if (l_Object)
                    return l_Object;
            }
        }

        return nullptr;
    }

    /// Send Packet to everyone in map
    /// @p_PacketBuffer : Packet being sent to
    void Base::SendPacketEveryone(Server::PacketBuffer const* p_PacketBuffer)
    {
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
                m_Grids[l_X][l_Y]->SendPacketEveryone(p_PacketBuffer);
    }

    void Base::UpdateRemoval(uint32 const p_Diff)
    {
        m_IntervalDelayRemoval.Update(p_Diff);
        if (!m_IntervalDelayRemoval.Passed())
        {
            for (auto l_Itr : m_DelayRemoval)
            {
                for (auto l_SecondItr = l_Itr.second.begin(); l_SecondItr != l_Itr.second.end();)
                {
                    /// If unit is dead, remove from client
                    if ((*l_SecondItr)->ToUnit()->GetDeathState() == Entity::DeathState::DEAD)
                    {
                        Server::Packets::DespawnShip l_Packet;
                        l_Packet.Id = (*l_SecondItr)->GetObjectGUID().GetCounter();
                        l_Itr.first->ToPlayer()->SendPacket(l_Packet.Write());

                        l_SecondItr = l_Itr.second.erase(l_SecondItr);
                    }
                    else
                    {
                        Server::Packets::ObjectMove l_Packet;
                        l_Packet.Id        = (*l_SecondItr)->GetObjectGUID().GetCounter();
                        l_Packet.PositionX = (*l_SecondItr)->GetSpline()->GetPlannedPositionX();
                        l_Packet.PositionY = (*l_SecondItr)->GetSpline()->GetPlannedPositionY();
                        l_Packet.Time      = (*l_SecondItr)->GetSpline()->GetDestinationTime();
                        l_Itr.first->ToPlayer()->SendPacket(l_Packet.Write());

                        l_SecondItr++;
                    }
                }
            }

            return;
        }

        /// Now remove from client
        for (auto l_Itr : m_DelayRemoval)
        {
            for (auto l_SecondItr : l_Itr.second)
            {
                Server::Packets::DespawnShip l_Packet;
                l_Packet.Id = l_SecondItr->GetObjectGUID().GetCounter();
                l_Itr.first->ToPlayer()->SendPacket(l_Packet.Write());
            }
        }

        m_DelayRemoval.clear();
    }

    /// Update Maps
    /// @p_Diff : Execution Time
    bool Base::Update(uint32 const p_Diff)
    {
        ProcessJumpQueue(p_Diff);
        UpdateRemoval(p_Diff);

        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
        {
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
            {
                Grid* l_Grid = m_Grids[l_X][l_Y];

                /// If there's no players inside the grid, then no point in updating
                if (l_Grid->GetState() == State::Idle)
                    continue;

                l_Grid->Update(p_Diff);
            }
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone