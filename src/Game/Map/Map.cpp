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
                m_Grids[l_X][l_Y] = new Grid(this, GridType::GRID_TYPE_NORMAL, l_X, l_Y);

        /// Note; We only need 1 grid for radiation, as the mini map does not cover outside of the grid,
        /// so we just send the packet around in a radius

        m_RadiationGrid = new Grid(this, GridType::GRID_TYPE_RADIATION, 0, 0);
    }
    /// Deconstructor
    Base::~Base()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //              GENERAL
    ///////////////////////////////////////////

    /// Update Maps
    /// @p_Diff : Execution Time
    bool Base::Update(uint32 const p_Diff)
    {
        /// TODO; We are updating the mobs twice, the Unit::Update gets updated on the grid then the pool
        /// updates anything other than the update, we should merge it into one update and this will improve
        /// performance alot.
        /// NOTE: This must always be called first
        m_PoolManager.Update(p_Diff);

        ProcessJumpQueue(p_Diff);

        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
        {
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
            {
                Grid* l_Grid = m_Grids[l_X][l_Y];

                /// If there's no players inside the grid, then no point in updating
                if (l_Grid->GetState() == State::Idle)
                    continue;

                l_Grid->Update(p_Diff);
                l_Grid->UpdateInteractiveEvents(m_ConstantObjects, p_Diff);
            }
        }

        m_RadiationGrid->Update(p_Diff);

        return true;
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

    /// Is in company map
    /// @p_Target : Target checking against
    bool Base::IsInCompanyMap(Entity::Unit* p_Target) const
    {
        Company l_Company = p_Target->GetCompany();
        uint32 l_MapId = p_Target->GetMap()->GetId();

        switch (l_Company)
        {
            case Company::EARTH:
            {
                switch (l_MapId)
                {
                case 5:  ///< 2-1
                case 6:  ///< 2-2
                case 7:  ///< 2-3
                case 8:  ///< 2-4 
                case 21: ///< 2-5
                case 22: ///< 2-6
                case 23: ///< 2-7
                case 24: ///< 2-8
                    return true;
                }
            }
            break;
            case Company::MMO:
            {
                switch (l_MapId)
                {
                case 1:  ///< 1-1
                case 2:  ///< 1-2
                case 3:  ///< 1-3
                case 4:  ///< 1-4 
                case 17: ///< 1-5   
                case 18: ///< 1-6
                case 19: ///< 1-7
                case 20: ///< 1-8
                    return true;
                }
            }
            break;
            case Company::VRU:
            {
                switch (l_MapId)
                {
                case 9:  ///< 3-1
                case 10: ///< 3-2
                case 11: ///< 3-3
                case 12: ///< 3-4 
                case 25: ///< 3-5
                case 26: ///< 3-6
                case 27: ///< 3-7
                case 28: ///< 3-8
                    return true;
                }
            }
            break;
        }

        return false;
    }
    /// Get Station on Map
    Entity::Station* Base::GetStation() const
    {
        for (auto l_Itr : m_ConstantObjects)
        {
			if (l_Itr->GetType() == Entity::Type::OBJECT_TYPE_STATION)
				return l_Itr->ToStation();
		}
        
		return nullptr;
    }

    ///////////////////////////////////////////
    //            GRID SYSTEM
    ///////////////////////////////////////////

    /// Get near by grids
    /// @p_GridX : Origin Grid X
    /// @p_GridY : Origin Grid Y
    /// @p_Grids : Storage for the nearby grids
    void Base::GetNearbyGrids(uint32 const p_GridX, uint32 const p_GridY, std::vector<Grid*>& p_Grids)
    {
        static const int32 l_X[] = { -1, -1, -1,  1, 1, 1,  0, 0 };
        static const int32 l_Y[] = { -1,  0,  1, -1, 0, 1, -1, 1 };

        /// Find neighbour girds
        for (uint32 l_I = 0; l_I < GRID_CELLS; l_I++)
        {
            int32 l_GridX = p_GridX + l_X[l_I];
            int32 l_GridY = p_GridY + l_Y[l_I];

            if (IsWithinGridRange(l_GridX, l_GridY))
                p_Grids.push_back(m_Grids[l_GridX][l_GridY]);
        }

        /// Also Push original grid
        p_Grids.push_back(m_Grids[p_GridX][p_GridY]);
    }
    /// Return Grid
    /// @p_Object : Object we are getting grid from
    Grid* Base::GetGrid(Entity::Object const* p_Object)
    {
        if ((std::get<0>(p_Object->GetGridIndex()) > GRID_CELLS - 1 || std::get<0>(p_Object->GetGridIndex()) < 0) ||
            (std::get<1>(p_Object->GetGridIndex()) > GRID_CELLS - 1 || std::get<1>(p_Object->GetGridIndex()) < 0))
            return m_RadiationGrid;

        return m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())];
    }
    /// Calculate Grid By Object Position
    /// @p_Object : Object
    std::tuple<int32, int32> Base::CalculateGridByPosition(Entity::Object* p_Object)
    {
        float l_GridX = std::floor(p_Object->GetSpline()->GetPositionX() / m_GridRadiusX);
        float l_GridY = std::floor(p_Object->GetSpline()->GetPositionY() / m_GridRadiusY);

        return std::make_tuple(static_cast<int32>(l_GridX), static_cast<int32>(l_GridY));
    }
    /// Calculate Grid By Object Position
    /// @p_Object : Object
    /// @p_PositionX : X Axis
    /// @p_PositionY : Y Axis
    std::tuple<int32, int32> Base::CalculateGridByPosition(float const p_PositionX, float const p_PositionY)
    {
        float l_GridX = std::floor(p_PositionX / m_GridRadiusX);
        float l_GridY = std::floor(p_PositionY / m_GridRadiusY);

        return std::make_tuple(static_cast<int32>(l_GridX), static_cast<int32>(l_GridY));
    }
    /// Return Grid by Position
    /// @p_PositionX : X Axis
    /// @p_PositionY : Y Axis
    Grid* Base::GetGridByPosition(float const p_PositionX, float const p_PositionY)
    {
        float l_GridX = std::floor(p_PositionX / m_GridRadiusX);
        float l_GridY = std::floor(p_PositionY / m_GridRadiusY);

        /// Is our grid out of bounds?
        if (l_GridX > GRID_CELLS - 1 || l_GridY > GRID_CELLS - 1) ///< Grid starts from 0
            LOG_ASSERT(false, "Map", "Grid X or Y is larger than Grid Cells! Grid X: %0, Grid Y: %1", l_GridX, l_GridY);

        return m_Grids[static_cast<uint32>(p_PositionX)][static_cast<uint32>(p_PositionY)];
    }
    /// This function is only called when object is added to map
    /// Add Object to map
    /// @p_Object      : Object being added to map
    /// @p_SendMovement : Send movement packet when joining
    void Base::Add(Entity::Object* p_Object, bool p_SendMovement)
    {
        p_Object->SetGridIndex(CalculateGridByPosition(p_Object));

        if ((std::get<0>(p_Object->GetGridIndex()) > GRID_CELLS - 1 || std::get<0>(p_Object->GetGridIndex()) < 0)
            || std::get<1>(p_Object->GetGridIndex()) > GRID_CELLS - 1 || std::get<1>(p_Object->GetGridIndex()) < 0)
        {
            p_Object->ToUnit()->SetInRadiationZone(true);
            m_RadiationGrid->Add(p_Object);
        }
        else
            m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Add(p_Object);

        /// This must be called after we add object to the grid
        SendConstantObjects(p_Object);
    }
    /// Remove Object from map
    /// @p_Object : Object being removed from map
    /// @p_SendPacket : Send Despawn Packet
    void Base::Remove(Entity::Object* p_Object, bool p_SendPacket)
    {
        if ((std::get<0>(p_Object->GetGridIndex()) > GRID_CELLS - 1 || std::get<0>(p_Object->GetGridIndex()) < 0)
            || std::get<1>(p_Object->GetGridIndex()) > GRID_CELLS - 1 || std::get<1>(p_Object->GetGridIndex()) < 0)
        {
            m_RadiationGrid->Remove(p_Object, p_SendPacket);
            return;
        }

        m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Remove(p_Object, p_SendPacket);
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
    /// Move Object
    /// @p_Object : Object being moved
    void Base::Move(Entity::Object* p_Object)
    {
        std::tuple<int32, int32> l_GridIndex = CalculateGridByPosition(p_Object->GetSpline()->GetPositionX(), p_Object->GetSpline()->GetPositionY());

        /// If we are out of bounds, then we have entered the radiation zone
        if ((std::get<0>(l_GridIndex) > GRID_CELLS - 1 || std::get<0>(l_GridIndex) < 0) || 
            (std::get<1>(l_GridIndex) > GRID_CELLS - 1 || std::get<1>(l_GridIndex) < 0))
        {
            if (!p_Object->ToUnit()->IsInRadiationZone())
            {
                /// Remove old grid
                m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Remove(p_Object);

                m_RadiationGrid->Add(p_Object);
                p_Object->ToUnit()->SetInRadiationZone(true);
            }

            m_RadiationGrid->Move(p_Object);
        }
        else
        {
            /// If both indexes don't match, we've moved to a new grid
            if (std::get<0>(l_GridIndex) != std::get<0>(p_Object->GetGridIndex()) ||
                std::get<1>(l_GridIndex) != std::get<1>(p_Object->GetGridIndex()))
            {
                /// Remove old grid
                if (p_Object->ToUnit()->IsInRadiationZone())
                {
                    m_RadiationGrid->Remove(p_Object);
                    p_Object->ToUnit()->SetInRadiationZone(false);
                }
                else
                    m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Remove(p_Object);
                
                /// Add new grid
                p_Object->SetGridIndex(l_GridIndex);
                m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Add(p_Object, true);
            }

            if (p_Object->ToUnit()->IsInRadiationZone())
                p_Object->ToUnit()->SetInRadiationZone(false);

            m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]->Move(p_Object);
        }
    }
    /// Unload Grids
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
    /// Check whether coordinates is within grid
    /// @p_PositionX : X Axis
    /// @p_PositionY : Y Axis
    bool Base::IsWithinGridRange(uint32 const p_PositionX, uint32 const p_PositionY)
    {
        if (p_PositionX < 0 || p_PositionX >= 8)
            return false;

        if (p_PositionY < 0 || p_PositionY >= 8)
            return false;

        return true;
    }

    ///////////////////////////////////////////
    //              JUMP QUEUE
    ///////////////////////////////////////////

    /// Add Player to Jump Queue
    /// @p_ObjectPlayer : Player being added
    /// @p_JumpQueueCordinates : Jump Queue Cordinates (MapId, PositionX, PositionY)
    void Base::AddToJumpQueue(Entity::Object* p_ObjectPlayer, const JumpQueueCordinates p_JumpQueueCordinates)
    {
        if (p_ObjectPlayer->ToPlayer())
        {
            if (p_ObjectPlayer->ToPlayer()->IsJumping())
				return;

            p_ObjectPlayer->ToPlayer()->SetIsJumping(true);
        }

        m_PlayersToJump[p_ObjectPlayer] = std::make_pair(Core::Diagnostic::IntervalTimer(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_JUMP_DELAY)), p_JumpQueueCordinates);
    }
    /// Process Jump Queue
    /// @p_Diff : Execution Time
    void Base::ProcessJumpQueue(uint32 const p_Diff)
    {
        for (auto l_Itr = m_PlayersToJump.begin(); l_Itr != m_PlayersToJump.end();)
        {
            l_Itr->second.first.Update(p_Diff);
            if (!l_Itr->second.first.Passed())
            {
                l_Itr++;
                continue;
            }

            /// Remove player from map
            sZoneManager->RemoveFromMap(l_Itr->first, true);

            /// Clear target also
            if (l_Itr->first->ToPlayer()->GetTarget())
                l_Itr->first->ToPlayer()->GetTarget()->CancelAttack();

            l_Itr->first->ToPlayer()->CancelAttack();
            l_Itr->first->ToPlayer()->ClearSurroundings();
            l_Itr->first->ToPlayer()->Repair(false);

            /// Add to map
            l_Itr->first->SetMap(sZoneManager->GetMap(l_Itr->second.second.MapId));

            l_Itr->first->ToPlayer()->SendDisplayStarSystem();
            l_Itr->first->GetSpline()->SetPosition(l_Itr->second.second.PositionX, l_Itr->second.second.PositionY,
                l_Itr->second.second.PositionX, l_Itr->second.second.PositionY);
            l_Itr->first->ToPlayer()->SendInitializeShip();
            l_Itr->first->ToPlayer()->UpdateExtrasInfo();
            l_Itr->first->ToPlayer()->SendMapUpdate();
            l_Itr->first->ToPlayer()->SendDrones();
            l_Itr->first->ToPlayer()->SendAmmoUpdate();
            l_Itr->first->ToPlayer()->SendAccountRank();

            /// Must be called after we send initial login packets
            sZoneManager->AddToMap(l_Itr->first);
            l_Itr->first->ToPlayer()->SetIsJumping(false);

            // Save Player Stats to Database
            l_Itr->first->ToPlayer()->SaveToDB();

            l_Itr = m_PlayersToJump.erase(l_Itr);
        }
    }

    ///////////////////////////////////////////
    //              PACKETS
    ///////////////////////////////////////////

    /// Send Packet to everyone in map
    /// @p_PacketBuffer : Packet being sent to
    void Base::SendPacketToMap(Server::PacketBuffer const* p_PacketBuffer)
    {
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
                m_Grids[l_X][l_Y]->SendPacketToEveryone(p_PacketBuffer);
    }
    /// Send Packet to nearby grids if in surrounding
    /// p_Object : Object responsible for building packet
    /// @p_SendToSelf : Send Packet to self
    void Base::SendPacketToNearByGridsIfInSurrounding(Server::PacketBuffer const* p_PacketBuffer, Entity::Object* p_Object, bool p_SendToSelf)
    {
        /// If we are in radiation zone, we don't send packet to other grids
        if (p_Object->IsPlayer())
        {
            if (p_Object->ToPlayer()->IsInRadiationZone())
            {
                p_Object->ToPlayer()->GetGrid()->SendPacketIfInSurrounding(p_PacketBuffer, p_Object, true);
                return;
            }
        }

        static const int32 l_X[] = { -1, -1, -1,  1, 1, 1,  0, 0 };
        static const int32 l_Y[] = { -1,  0,  1, -1, 0, 1, -1, 1 };

        std::vector<Grid*> l_Grids;

        /// Find neighbour girds
        for (uint32 l_I = 0; l_I < GRID_CELLS; l_I++)
        {
            int32 l_GridX = std::get<0>(p_Object->GetGridIndex()) + l_X[l_I];
            int32 l_GridY = std::get<1>(p_Object->GetGridIndex()) + l_Y[l_I];

            if (IsWithinGridRange(l_GridX, l_GridY))
                l_Grids.push_back(m_Grids[l_GridX][l_GridY]);
        }

        /// Push object grid into storage
        l_Grids.push_back(m_Grids[std::get<0>(p_Object->GetGridIndex())][std::get<1>(p_Object->GetGridIndex())]);
        
        if (p_SendToSelf && p_Object->IsPlayer())
            p_Object->ToPlayer()->SendPacket(p_PacketBuffer);

        /// Now send packet to players if object is in their surroundings
        for (auto l_Itr : l_Grids)
            l_Itr->SendPacketIfInSurrounding(p_PacketBuffer, p_Object);
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

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone