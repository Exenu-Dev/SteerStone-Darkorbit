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
#include "ZoneManager.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Database/Database.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Id : Map Id
    Base::Base(uint32 const p_Id)
        : m_Id(p_Id)
    { 
        /// Define size of map
        switch (p_Id)
        {
        case 29: ///< 4-5
        case 16: ///< 4-4
            m_MapType = MapType::MAP_TYPE_BIG;
            m_GridRadiusX = BIG_GRID_SEARCH_RADIUS;
            m_GridRadiusY = BIG_GRID_SEARCH_RADIUS_HALFED;
            break;
        default:
            m_MapType = MapType::MAP_TYPE_NORMAL;
            m_GridRadiusX = NORMAL_GRID_SEARCH_RADIUS;
            m_GridRadiusY = NORMAL_GRID_SEARCH_RADIUS_HALFED;
            break;
        }

        /// Load Grids
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
                m_Grids[l_X][l_Y] = new Grid(l_X, l_Y);

        m_IntervalJumpPlayer.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_JUMP_DELAY));
    }
    /// Deconstructor
    Base::~Base()
    {
        int i = 0;
    }

    /// Get Map Id
    uint32 Base::GetId() const
    {
        return m_Id;
    }
    /// Get Map Type
    MapType Base::GetMapType() const
    {
        return m_MapType;
    }

    /// Load Map
    void Base::Load()
    {
        LoadPortals();
        LoadStations();
    }

    /// Load Portals to map
    void Base::LoadPortals()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT portal_id, name, company, type, map_id, position_x, position_y, to_map_id, to_position_x, to_position_y "
            "FROM portals WHERE map_id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        uint32 l_Counter = 0;
        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::Portal* l_Portal = new Entity::Portal();
                l_Portal->m_Id           = l_Result[0].GetUInt32();
                l_Portal->m_CompanyId    = static_cast<Company>(l_Result[2].GetUInt16());
                l_Portal->m_Type         = static_cast<PortalType>(l_Result[3].GetUInt16());
                l_Portal->m_ToMapId      = l_Result[7].GetUInt32();
                l_Portal->m_ToPositionX  = l_Result[8].GetFloat();
                l_Portal->m_ToPositionY  = l_Result[9].GetFloat();
                l_Portal->SetName(l_Result[1].GetString());
                l_Portal->SetMap(this);
                l_Portal->GetSpline()->SetPosition(l_Result[5].GetFloat(), l_Result[6].GetFloat());
                l_Portal->SetGUID(Entity::ObjectGUID(Entity::GUIDType::Portal, l_Portal->GetId()));
                Add(l_Portal);

                /// It's faster if we keep a reference of portal because they will always appear on mini map
                m_ConstantObjects.push_back(l_Portal);

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("Map", "Loaded %0 Portals in %1 ms for Map: %2", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()), m_Id);
    }
    /// Load Stations to map
    void Base::LoadStations()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, name, company, type, map_id, peace, position_x, position_y "
            "FROM stations WHERE map_id = ?");
        l_PreparedStatement->SetUint32(0, m_Id);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        uint32 l_Counter = 0;
        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::Station* l_Station  = new Entity::Station();
                l_Station->m_Id             = l_Result[0].GetUInt32();
                l_Station->m_CompanyId      = static_cast<Company>(l_Result[2].GetUInt16());
                l_Station->m_Type           = static_cast<StationType>(l_Result[3].GetUInt16());
                l_Station->m_Peace          = l_Result[5].GetBool();
                l_Station->SetName(l_Result[1].GetString());
                l_Station->GetSpline()->SetPosition(l_Result[6].GetFloat(), l_Result[7].GetFloat());
                l_Station->SetGUID(Entity::ObjectGUID(Entity::GUIDType::Station, l_Station->GetId()));
                l_Station->SetMap(this);
                Add(l_Station);

                /// It's faster if we keep a reference of station because they will always appear on mini map
                m_ConstantObjects.push_back(l_Station);

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("Map", "Loaded %0 Station in %1 ms for Map: %2", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()), m_Id);
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
                l_Packet.Id         = l_Itr->ToPortal()->GetId();
                l_Packet.Type       = static_cast<uint16>(l_Itr->ToPortal()->GetType());
                l_Packet.PositionX  = l_Itr->GetSpline()->GetPositionX();
                l_Packet.PositionY  = l_Itr->GetSpline()->GetPositionY();
                p_Object->ToPlayer()->SendPacket(l_Packet.Write());
            }
            else if (l_Itr->GetType() == Entity::Type::OBJECT_TYPE_STATION)
            {
                Server::Packets::CreateStation l_Packet;
                l_Packet.Id         = l_Itr->ToStation()->GetId();
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
        /// We need to calculate which grid the player will be in
        float l_PositionX = p_Object->GetSpline()->GetPositionX();
        float l_PositionY = p_Object->GetSpline()->GetPositionY();
        uint32 l_PositionOffset = (l_PositionX + l_PositionY) / m_GridRadiusX;

        /// This is a quicker way of determining we are inside the 64 grid range
        if (l_PositionOffset > MAX_GRIDS)
        {
            LOG_WARNING("Map", "Attempted to add Player %0 to grid but grid is larger than max grid! kicking player!", p_Object->ToPlayer()->GetName());
            p_Object->ToPlayer()->KickPlayer();
            return std::make_tuple(0, 0); ///< To make compiler happy
        }

        /// Now grab the Grid X and Grid Y index
        float l_GridX = l_PositionX / m_GridRadiusX;
        if (l_GridX < 1)
            l_GridX = 0;
        else
            l_GridX = std::ceil(((l_GridX / MAX_GRIDS) * 10)) - 1;

        /// The width is bigger than the height
        /// Map Size is 21000x14100, so we need to cut the search radius in half to get the index we are looking for
        float l_GridY = l_PositionY / m_GridRadiusY;
        if (l_GridY < 1)
            l_GridY = 0;
        else
            l_GridY = std::floor((l_GridY / MAX_GRIDS * 10)) - 1;

        if (l_GridX < 1)
            l_GridX = 0;

        if (l_GridY < 1)
            l_GridY = 0;

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
        /// We need to calculate which grid the player will be in
        float l_PositionX = p_PositionX;
        float l_PositionY = p_PositionY;
        int32 l_PositionOffset = (l_PositionX + l_PositionY) / m_GridRadiusX;

        /// This is a quicker way of determining we are inside the 64 grid range
        if (l_PositionOffset > MAX_GRIDS)
        {
            LOG_WARNING("Map", "Attempted to add Player %0 to grid but grid is larger than max grid! kicking player!", p_Object->ToPlayer()->GetName());
            p_Object->ToPlayer()->KickPlayer();
            return std::make_tuple(0, 0); ///< To make compiler happy
        }

        /// Now grab the Grid X and Grid Y index
        float l_GridX = l_PositionX / m_GridRadiusX;
        l_GridX = std::ceil(((l_GridX / MAX_GRIDS) * 10)) - 1;

        /// The width is bigger than the height
        /// Map Size is 21000x14100, so we need to cut the search radius in half to get the index we are looking for
        float l_GridY = l_PositionY / m_GridRadiusY;
        l_GridY = std::floor((l_GridY / MAX_GRIDS * 10)) - 1;

        if (l_GridX < 1)
            l_GridX = 0;

        if (l_GridY < 1)
            l_GridY = 0;

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

    /// Send Packet to everyone in map
    /// @p_PacketBuffer : Packet being sent to
    void Base::SendPacketEveryone(Server::PacketBuffer const* p_PacketBuffer)
    {
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
                m_Grids[l_X][l_Y]->SendPacketEveryone(p_PacketBuffer);
    }

    /// Update Maps
    /// @p_Diff : Execution Time
    bool Base::Update(uint32 const p_Diff)
    {
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
            }
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone