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
#include "MobPool.hpp"
#include "BonusBoxPool.hpp"
#include "Mob/Mob.hpp"
#include "Player.hpp"
#include "BonusBox/BonusBox.hpp"
#include "Map.hpp"
#include "Utility/UtilRandom.hpp"
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Map : Reference of map
    PoolManager::PoolManager(Base* p_Map)
    {
        m_Map = p_Map;

        m_Pool[POOL_TYPE_MOB]       = new MobPool();
        m_Pool[POOL_TYPE_BONUS_BOX] = new BonusBoxPool();
    }
    /// Deconstructor
    PoolManager::~PoolManager()
    {
        for (auto l_Itr : m_Pool)
            delete l_Itr.second;

        m_Pool.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Initialize Pool for map
    void PoolManager::Initialize()
    {
        InitializeMobs();
    }
    /// Load Mobs into pool
    void PoolManager::InitializeMobs()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT entry, min_count, max_count FROM mobs WHERE map_id = ?");
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
                float l_RadiusX = (m_Map->GetMapSizeX() / MAX_GRIDS) * GRID_CELLS;
                float l_RadiusY = (m_Map->GetMapSizeY() / MAX_GRIDS) * GRID_CELLS;

                for (uint32 l_Y = 1; l_Y < GRID_CELLS + 1; l_Y++)
                {
                    float l_MaxY = (l_RadiusY * l_Y);

                    for (uint32 l_X = 1; l_X < GRID_CELLS + 1; l_X++)
                    {
                        float l_MaxX = (l_RadiusX * l_X);

                        for (uint32 l_I = 0; l_I < Core::Utils::UInt32Random(l_Result[1].GetUInt32(), l_Result[2].GetUInt32()); l_I++)
                        {
                            Entity::Mob* l_Mob          = new Entity::Mob();
                            l_Mob->m_Entry              = l_MobTemplate->Entry;
                            l_Mob->m_ShipType           = l_MobTemplate->Type;
                            l_Mob->m_WeaponState        = l_MobTemplate->WeaponState;
                            l_Mob->m_HitPoints          = l_MobTemplate->HitPoints;
                            l_Mob->m_MaxHitPoints       = l_MobTemplate->HitPoints;
                            l_Mob->m_ShieldResistance   = l_MobTemplate->ShieldResistance;
                            l_Mob->m_Shield             = l_MobTemplate->Shield;
                            l_Mob->m_MinDamage          = l_MobTemplate->MinDamage;
                            l_Mob->m_MaxShield          = l_MobTemplate->Shield;
                            l_Mob->m_MaxDamage          = l_MobTemplate->MaxDamage;
                            l_Mob->m_Behaviour          = l_MobTemplate->Behaviour;
                            l_Mob->m_RespawnTimer       = l_MobTemplate->RespawnTimer;
                            l_Mob->m_Experience         = l_MobTemplate->Experience;
                            l_Mob->m_Honor              = l_MobTemplate->Honor;
                            l_Mob->m_Credits            = l_MobTemplate->Credits;
                            l_Mob->m_Uridium            = l_MobTemplate->Uridium;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_PROMETIUM] = l_MobTemplate->Prometium;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_ENDURIUM]  = l_MobTemplate->Endurium;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_TERBIUM]   = l_MobTemplate->Terbium;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_XENOMIT]   = l_MobTemplate->Xenomit;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_PROMETID]  = l_MobTemplate->Prometid;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_DURANIUM]  = l_MobTemplate->Duranium;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_PROMETIUM] = l_MobTemplate->Prometium;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_PALLADIUM] = l_MobTemplate->Palladium;
                            l_Mob->m_Resources[Entity::Resource::RESOURCE_SEPROM]    = l_MobTemplate->Seprom;
                            l_Mob->m_MoveTimeMin        = l_MobTemplate->MinMovementTime;
                            l_Mob->m_MoveTimeMax        = l_MobTemplate->MaxMovementTime;
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
                            m_Pool[POOL_TYPE_MOB]->Add(l_Mob);
                        }

                        l_PreviousRadiusX = l_MaxX;
                    }

                    l_PreviousRadiusX = 1.0f;
                    l_PreviousRadiusY = l_MaxY;
                }

            } while (l_PreparedResultSet->GetNextRow());
        }
    }

    /// Update Pool
    /// @p_Diff : Execution Time
    void PoolManager::Update(uint32 const p_Diff)
    {
        for (auto l_Itr : m_Pool)
            l_Itr.second->Update(p_Diff);
    }

    /// Add Bonus box to map
    /// @p_Unit    : Unit taking away the resources
    /// @p_Type    : Type of Bonus Box
    /// @p_OwnerId : Owner of cargo box
    void PoolManager::AddBonuxBox(Entity::Unit* p_Unit, BonusBoxType p_Type, uint32 const p_OwnerId)
    {
        /// Note; we do not need to send cargo box packet, this will be sent on next object build update
        /// so just add cargo box to map and pool
        
        Entity::BonusBox* l_BonusBox = new Entity::BonusBox(p_Type);

        /// TODO; Player does not give all their cargo, only some of it, right now it gives all of it and doesn't take away
        l_BonusBox->m_OwnerId   = p_OwnerId;
        l_BonusBox->m_Prometium = p_Unit->GetResource(Entity::Resource::RESOURCE_PROMETIUM);
        l_BonusBox->m_Endurium  = p_Unit->GetResource(Entity::Resource::RESOURCE_ENDURIUM);
        l_BonusBox->m_Terbium   = p_Unit->GetResource(Entity::Resource::RESOURCE_TERBIUM);
        l_BonusBox->m_Xenomit   = p_Unit->GetResource(Entity::Resource::RESOURCE_XENOMIT);
        l_BonusBox->m_Prometid  = p_Unit->GetResource(Entity::Resource::RESOURCE_PROMETID);
        l_BonusBox->m_Duranium  = p_Unit->GetResource(Entity::Resource::RESOURCE_DURANIUM);
        l_BonusBox->m_Promerium = p_Unit->GetResource(Entity::Resource::RESOURCE_PROMERIUM);
        l_BonusBox->m_Palladium = p_Unit->GetResource(Entity::Resource::RESOURCE_PALLADIUM);
        l_BonusBox->m_Seprom    = p_Unit->GetResource(Entity::Resource::RESOURCE_SEPROM);

        l_BonusBox->SetMap(m_Map);
        l_BonusBox->GetSpline()->SetPosition(p_Unit->GetSpline()->GetPositionX(), p_Unit->GetSpline()->GetPositionY());
        
        m_Map->Add(l_BonusBox);

        m_Pool[POOL_TYPE_BONUS_BOX]->Add(l_BonusBox);
    }
    /// Remove Bonus Box
    /// @p_Object : Object Bonus Box
    void PoolManager::RemoveBonusBox(Entity::Object* p_Object)
    {
        m_Pool[POOL_TYPE_BONUS_BOX]->Remove(p_Object);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone