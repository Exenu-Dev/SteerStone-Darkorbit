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

#include "PoolManager.hpp"
#include "ObjectManager.hpp"
#include "Mob/Mob.hpp"
#include "Map.hpp"
#include "Utility/UtilRandom.hpp"
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Map : Reference of map
    PoolManager::PoolManager(Base* p_Map)
    {
        m_Map = p_Map;
    }
    /// Deconstructor
    PoolManager::~PoolManager()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Initialize Pool for map
    void PoolManager::Initialize()
    {
        InitializeMobs();
    }

    /// Update Pool
    /// @p_Diff : Execution Time
    void PoolManager::Update(uint32 const p_Diff)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Load Mobs into pool
    void PoolManager::InitializeMobs()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT entry, min_movement_time, max_movement_time, min_count, max_count FROM mobs WHERE map_id = ?");
        l_PreparedStatement->SetUint32(0, m_Map->GetId());
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::MobTemplate const* l_MobTemplate = sObjectManager->GetMobTemplate(l_Result[0].GetUInt32());
                if (!l_MobTemplate)
                {
                    LOG_WARNING("PoolManager", "Could not find entry %0 in mob template!", l_Result[0].GetUInt32());
                    return;
                }

                float l_PreviousRadiusX = 1.0f;
                float l_PreviousRadiusY = 1.0f;
                float l_RadiusX         = (m_Map->GetMapSizeX() / MAX_GRIDS) * GRID_CELLS;
                float l_RadiusY         = (m_Map->GetMapSizeY() / MAX_GRIDS) * GRID_CELLS;

                for (uint32 l_Y = 1; l_Y < GRID_CELLS + 1; l_Y++)
                {
                    float l_MaxY = (l_RadiusY * l_Y);

                    for (uint32 l_X = 1; l_X < GRID_CELLS + 1; l_X++)
                    {
                        float l_MaxX = (l_RadiusX * l_X);

                        for (uint32 l_I = 0; l_I < Core::Utils::Int32Random(l_Result[3].GetUInt32(), l_Result[4].GetUInt32()); l_I++)
                        {
                            Entity::Mob* l_Mob = new Entity::Mob();
                            l_Mob->m_Entry              = l_MobTemplate->Entry;
                            l_Mob->m_ShipType           = l_MobTemplate->Type;
                            l_Mob->m_WeaponState        = l_MobTemplate->WeaponState;
                            l_Mob->m_HitPoints          = l_MobTemplate->HitPoints;
                            l_Mob->m_MaxHitPoints       = l_MobTemplate->HitPoints;
                            l_Mob->m_ShieldResistance   = l_MobTemplate->ShieldResistance;
                            l_Mob->m_Shield             = l_MobTemplate->Shield;
                            l_Mob->m_MaxShield          = l_MobTemplate->Shield;
                            l_Mob->m_MinDamage          = l_MobTemplate->MinDamage;
                            l_Mob->m_MaxDamage          = l_MobTemplate->MaxDamage;
                            l_Mob->m_Behaviour          = l_MobTemplate->Behaviour;
                            l_Mob->m_RespawnTimer       = l_MobTemplate->RespawnTimer;
                            l_Mob->m_Experience         = l_MobTemplate->Experience;
                            l_Mob->m_Honor              = l_MobTemplate->Honor;
                            l_Mob->m_Credits            = l_MobTemplate->Credits;
                            l_Mob->m_Uridium            = l_MobTemplate->Uridium;
                            l_Mob->m_Prometium          = l_MobTemplate->Prometium;
                            l_Mob->m_Endurium           = l_MobTemplate->Endurium;
                            l_Mob->m_Terbium            = l_MobTemplate->Terbium;
                            l_Mob->m_Prometid           = l_MobTemplate->Prometid;
                            l_Mob->m_Duranium           = l_MobTemplate->Duranium;
                            l_Mob->m_Promerium          = l_MobTemplate->Promerium;
                            l_Mob->m_Xenomit            = l_MobTemplate->Xenomit;
                            l_Mob->m_Seprom             = l_MobTemplate->Seprom;
                            l_Mob->m_Palladium          = l_MobTemplate->Palladium;
                            l_Mob->m_MoveTimeMin        = l_Result[1].GetUInt32();
                            l_Mob->m_MoveTimeMax        = l_Result[2].GetUInt32();
                            l_Mob->GetSpline()->SetSpeed(l_MobTemplate->Speed);
                            l_Mob->SetName(l_MobTemplate->Name);
                            l_Mob->m_IntervalMoveTimer.SetInterval(Core::Utils::FloatRandom(l_Mob->m_MoveTimeMin, l_Mob->m_MoveTimeMin));
                            l_Mob->m_IntervalRespawnTimer.SetInterval(l_MobTemplate->RespawnTimer);
                            l_Mob->SetMap(m_Map);

                            float l_PositionX = Core::Utils::FloatRandom(l_PreviousRadiusX, l_MaxX - DISTANCE_AWAY_FROM_BORDER);
                            float l_PositionY = Core::Utils::FloatRandom(l_PreviousRadiusY, l_MaxY - DISTANCE_AWAY_FROM_BORDER);
                            l_Mob->GetSpline()->SetPosition(l_PositionX, l_PositionY, l_PositionX, l_PositionY);
                            l_Mob->m_HomePositionX = l_PositionX;
                            l_Mob->m_HomePositionY = l_PositionY;

                            m_Map->Add(l_Mob);
                            m_MobPool.Add(m_Map->GetGrid(l_Mob), l_Mob);
                        }

                        l_PreviousRadiusX = l_MaxX;
                    }
                    
                    l_PreviousRadiusX = 1.0f;
                    l_PreviousRadiusY = l_MaxY;
                }

            } while (l_PreparedResultSet->GetNextRow());
        }
    }

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone