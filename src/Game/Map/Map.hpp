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

#pragma once
#include <PCH/Precompiled.hpp>
#include "PoolManager.hpp"
#include "Diagnostic/DiaIntervalTimer.hpp"
#include "GameFlags.hpp"
#include <mutex>

namespace SteerStone { namespace Game { namespace Entity {

    class Object;
    class Station;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Map {

    class Entity::Object;

    struct JumpQueueCordinates
    {
        uint32 MapId;
        int32 PositionX;
        int32 PositionY;
	};

    typedef std::unordered_map<Entity::Object*, std::pair<Core::Diagnostic::IntervalTimer, JumpQueueCordinates>> JumpQueueMap;

    /// Map
    class Base
    {
        DISALLOW_COPY_AND_ASSIGN(Base);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    public:
        /// Constructor
        /// @p_Id : Map Id
        Base(uint32 const p_Id);
        /// Deconstructor
        ~Base();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //              GENERAL
        ///////////////////////////////////////////
    public:
        /// Update Maps
        /// @p_Diff : Execution Time
        bool Update(uint32 const p_Diff);

        /// Load Map
        void Load();
        /// Load Portals to map
        void LoadPortals();
        /// Load Stations to map
        void LoadStations();
        /// Load Mobs to map
        void LoadMobs();

        /// Is in company map
        /// @p_Target : Target checking against
        bool IsInCompanyMap(Entity::Unit* p_Target) const;

        /// Get Station on Map
        Entity::Station* GetStation() const;

        ///////////////////////////////////////////
        //            GRID SYSTEM
        ///////////////////////////////////////////
    public:
        /// Get near by grids
        /// @p_GridX : Origin Grid X
        /// @p_GridY : Origin Grid Y
        /// @p_Grids : Storage for the nearby grids
        void GetNearbyGrids(uint32 const p_GridX, uint32 const p_GridY, std::vector<Grid*>& p_Grids);
        /// Return Grid
        /// @p_Object : Object we are getting grid from
        Grid* GetGrid(Entity::Object const* p_Object);
        /// Calculate Grid By Object Position
        /// @p_Object : Object
        std::tuple<int32, int32> CalculateGridByPosition(Entity::Object* p_Object);
        /// Calculate Grid By Position
        /// @p_PositionX : X Axis
        /// @p_PositionY : Y Axis
        std::tuple<int32, int32> CalculateGridByPosition(float const p_PositionX, float const p_PositionY);
        /// Return Grid by Position
        /// @p_PositionX : X Axis
        /// @p_PositionY : Y Axis
        Grid* GetGridByPosition(float const p_PositionX, float const p_PositionY);
        /// This function is only called when object is added to map
        /// Add Object to map
        /// @p_Object       : Object being added to map
        /// @p_SendMovement : Send movement packet when joining
        void Add(Entity::Object* p_Object, bool p_SendMovement = false);
        /// Remove Object from map
        /// @p_Object : Object being removed from map
        /// @p_SendPacket : Send Despawn Packet
        void Remove(Entity::Object* p_Object, bool p_SendPacket = false);
        /// Find Object in map
        /// @p_Counter : Counter of object
        Entity::Object* FindObject(uint32 const p_Counter);
        /// Move Object
        /// @p_Object : Object being moved
        void Move(Entity::Object* p_Object);
        /// Unload Grids
        void UnloadAll();
    private:
        /// Check whether coordinates is within grid
        /// @p_PositionX : X Axis
        /// @p_PositionY : Y Axis
        bool IsWithinGridRange(uint32 const p_PositionX, uint32 const p_PositionY);

        ///////////////////////////////////////////
        //              JUMP QUEUE
        ///////////////////////////////////////////
    public:
        /// Add Player to Jump Queue
        /// @p_ObjectPlayer : Player being added
        /// @p_JumpQueueCordinates : Jump Queue Cordinates (MapId, PositionX, PositionY)
        void AddToJumpQueue(Entity::Object* p_ObjectPlayer, const JumpQueueCordinates p_JumpQueueCordinates);
        /// Process Jump Queue
        /// @p_Diff : Execution Time
        void ProcessJumpQueue(uint32 const p_Diff);

        ///////////////////////////////////////////
        //              PACKETS
        ///////////////////////////////////////////
    public:
        /// Send Packet to everyone in map
        /// @p_PacketBuffer : Packet being sent to
        void SendPacketToMap(Server::PacketBuffer const* p_PacketBuffer);
        /// Send Packet to nearby grids if in surrounding
        /// @p_Object : Object responsible for building packet
        /// @p_SendToSelf : Send Packet to self
        void SendPacketToNearByGridsIfInSurrounding(Server::PacketBuffer const* p_PacketBuffer, Entity::Object* p_Object, bool p_SendToSelf = false);
        /// Send Objects which always appear on map regardless of what grid user is in
        /// @p_Object : Packet being sent to
        void SendConstantObjects(Entity::Object* p_Object);

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        uint32 GetId()          const { return m_Id;            }
        MapType GetMapType()    const { return m_MapType;       }
        uint32 GetGridRadiusX() const { return m_GridRadiusX;   }
        uint32 GetGridRadiusY() const { return m_GridRadiusY;   }
        uint32 GetMapSizeX()    const { return m_MapSizeX;      }
        uint32 GetMapSizeY()    const { return m_MapSizeY;      }

        /// Get all the players on the map
        std::vector<Entity::Player*> GetPlayers();

        std::string GetName()
        {
            switch (m_Id)
            {
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X1:
					return "1-1";
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X2:
				    return "1-2";
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X3:
                    return "1-3";
                case CompanyMapId::COMPANY_MAP_ID_EIC_1X4:
                    return "1-4";
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X1:
				    return "2-1";
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X2:
				    return "2-2";
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X3:
                    return "2-3";
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X4:
        		    return "2-4";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X1:
                    return "3-1";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X2:
				    return "3-2";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X3:
                    return "3-3";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X4:
					return "3-4";
                case CompanyMapId::COMPANY_MAP_ID_NOMAN_4X4:
					return "4-4";
                case CompanyMapId::COMPANY_MAP_ID_MMO_4X1:
					return "4-1";
                case CompanyMapId::COMPANY_MAP_ID_MMO_4X2:
                    return "4-2";
                case CompanyMapId::COMPANY_MAP_ID_MMO_4X3:
                    return "4-3";
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X5:
                    return "1-5";
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X6:
                    return "1-6";
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X7:
					return "1-7";
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X8:
					return "1-8";
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X6:
                    return "2-6";
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X7:
					return "2-7";
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X8:
                    return "2-8";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X5:
					return "3-5";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X6:
                    return "3-6";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X7:
					return "3-7";
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X8:
                    return "3-8";
                case CompanyMapId::COMPANY_MAP_NOMAN_4X5:
					return "4-5";
                default:
                    return "Unknown";
            }
        }

        bool IsBattleZoneMap() const
        {
            switch (m_Id)
            {
                case CompanyMapId::COMPANY_MAP_ID_NOMAN_4X4:
                case CompanyMapId::COMPANY_MAP_ID_MMO_4X1:
                case CompanyMapId::COMPANY_MAP_ID_MMO_4X2:
                case CompanyMapId::COMPANY_MAP_ID_MMO_4X3:
                    return true;
                    break;
                default:
                    return false;
                    break;
            }
        }

        bool IsStarterMap() const
        {
            switch (m_Id)
            {
				case CompanyMapId::COMPANY_MAP_ID_MMO_1X1:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X1:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X1:
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X2:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X2:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X2:
					return true;
				default:
					return false;
			}
		}

        bool IsMidMap() const
        {
            switch (m_Id)
            {
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X2:
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X3:
                case CompanyMapId::COMPANY_MAP_ID_EIC_1X4:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X2:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X3:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X4:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X2:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X3:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X4:
					return true;
				default:
					return false;
			}
		}

        bool IsLowerMap() const
        {
            switch (m_Id)
            {
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X1:
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X2:
                case CompanyMapId::COMPANY_MAP_ID_MMO_1X3:
                case CompanyMapId::COMPANY_MAP_ID_EIC_1X4:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X1:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X2:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X3:
                case CompanyMapId::COMPANY_MAP_ID_EIC_2X4:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X1:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X2:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X3:
                case CompanyMapId::COMPANY_MAP_ID_VRU_3X4:
                    return true;
                default:
                    return false;
            }
        }

        /// Is Main Battle Zone Map
        /// TODO: Better naming could be done here
        bool IsMainBattleZoneMap()
        {
            switch (m_Id)
            {
                case CompanyMapId::COMPANY_MAP_ID_NOMAN_4X4:
                case 29:    ///< 4-5
				    return true;
				default:
					return false;
            }
        }

        PoolManager* GetPoolManager() { return &m_PoolManager;  }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        uint32 m_Id;                                          ///< Map Id
        MapType m_MapType;                                    ///< Map Type
        uint32 m_GridRadiusX;                                 ///< Grid Radius X
        uint32 m_GridRadiusY;                                 ///< Grid Radius Y
        uint32 m_MapSizeX;                                    ///< Map Size X
        uint32 m_MapSizeY;                                    ///< Map Size Y
        Grid* m_Grids[GRID_CELLS][GRID_CELLS];                ///< Grids
        Grid* m_RadiationGrid;                                ///< Radiation Grid
        PoolManager m_PoolManager;                            ///< Pool Manager
        std::vector<Entity::Object*> m_ConstantObjects;       ///< Constant Objects
        JumpQueueMap m_PlayersToJump;                         ///< Process players to jump
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone