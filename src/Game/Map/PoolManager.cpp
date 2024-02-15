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

#include "PoolManager.hpp"
#include "Database/DatabaseTypes.hpp"
#include "ObjectManager.hpp"
#include "Map.hpp"
#include "BonusBoxPool.hpp"
#include "OrePool.hpp"
#include "MobPool.hpp"
#include "Mob.hpp"
#include "Ore.hpp"
#include "BonusBox.hpp"
#include "Utility/UtilRandom.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Map : Reference of map
    PoolManager::PoolManager(Base* p_Map)
    {
        m_Map = p_Map;

        m_Pool[POOL_TYPE_MOB]       = new MobPool();
        m_Pool[POOL_TYPE_BONUS_BOX] = new BonusBoxPool();
        m_Pool[POOL_TYPE_ORE]       = new OrePool();
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
        InitializeBonusBoxes();
        InitializeOres();
    }
    /// Load Mobs into pool
    void PoolManager::InitializeMobs()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT entry, chance FROM mobs WHERE map_id = ?");
        l_PreparedStatement->SetUint32(0, m_Map->GetId());
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::MobTemplate const* l_MobTemplate = sObjectManager->GetMobTemplate(l_Result[0].GetUInt64());
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

                        if (Core::Utils::RoleChanceFloat(l_Result[1].GetDouble())) 
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
                            l_Mob->GetSpline()->SetSpeed(l_MobTemplate->Speed);
                            l_Mob->SetName(l_MobTemplate->Name);
                            l_Mob->m_IntervalMoveTimer.SetInterval(Core::Utils::UInt32Random(l_MobTemplate->MinMovementTime, l_MobTemplate->MaxMovementTime));
                            l_Mob->m_IntervalRespawnTimer.SetInterval(l_MobTemplate->RespawnTimer);
                            l_Mob->SetMap(m_Map);

                            float l_PositionX = Core::Utils::FloatRandom(l_PreviousRadiusX, l_MaxX - DISTANCE_AWAY_FROM_BORDER);
                            float l_PositionY = Core::Utils::FloatRandom(l_PreviousRadiusY, l_MaxY - DISTANCE_AWAY_FROM_BORDER);
                            l_Mob->GetSpline()->SetPosition(l_PositionX, l_PositionY, l_PositionX, l_PositionY);
                            l_Mob->m_HomePositionX = l_PositionX;
                            l_Mob->m_HomePositionY = l_PositionY;
                            l_Mob->m_Fleeing       = false;

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

    /// Load Bonus Boxes into pool
    void PoolManager::InitializeBonusBoxes()
    {
        /// We don't generate bonus boxes for main battle zone maps
        if (m_Map->IsMainBattleZoneMap())
            return;

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

                if (Core::Utils::RoleChanceFloat(80.f))
                {
					Entity::BonusBox* l_BonusBox = new Entity::BonusBox(BonusBoxType::BONUS_BOX_TYPE_BONUS);

					l_BonusBox->SetMap(m_Map);
					l_BonusBox->GetSpline()->SetPosition(Core::Utils::FloatRandom(l_PreviousRadiusX, l_MaxX - DISTANCE_AWAY_FROM_BORDER), Core::Utils::FloatRandom(l_PreviousRadiusY, l_MaxY - DISTANCE_AWAY_FROM_BORDER));
					m_Map->Add(l_BonusBox);
					m_Pool[POOL_TYPE_BONUS_BOX]->Add(l_BonusBox);
                }
             
                l_PreviousRadiusX = l_MaxX;
            }

            l_PreviousRadiusX = 1.0f;
            l_PreviousRadiusY = l_MaxY;
        }

        LOG_INFO("PoolManager", "Initialized Bonus Boxes for Map %0", m_Map->GetId());
    }

    /// Load Ores into pool
    void PoolManager::InitializeOres()
    {
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

                for (uint8 l_I = static_cast<uint8>(OreTypes::ORE_TYPE_PROMETIUM); l_I < static_cast<uint8>(OreTypes::MAX_ORE); l_I++)
                {
                    if (Core::Utils::RoleChanceFloat(80.f))
                    {
                        if (l_I == static_cast<uint8>(OreTypes::ORE_TYPE_PROMETIUM) && !m_Map->IsStarterMap())
							continue;

                        if (l_I == static_cast<uint8>(OreTypes::ORE_TYPE_TERBIUM) && !m_Map->IsMidMap())
							continue;

                        if (l_I == static_cast<uint8>(OreTypes::ORE_TYPE_ENDURIUM) && !m_Map->IsLowerMap())
                            continue;

                        OreTypes l_OreType = static_cast<OreTypes>(l_I);
                        Entity::Ore* l_Ore = new Entity::Ore(l_OreType);
                        l_Ore->SetMap(m_Map);
                        l_Ore->GetSpline()->SetPosition(Core::Utils::FloatRandom(l_PreviousRadiusX, l_MaxX - DISTANCE_AWAY_FROM_BORDER), Core::Utils::FloatRandom(l_PreviousRadiusY, l_MaxY - DISTANCE_AWAY_FROM_BORDER));
                        l_Ore->SetResource(l_Ore->GetResourceType(), 1);
                        m_Map->Add(l_Ore);
                        m_Pool[POOL_TYPE_ORE]->Add(l_Ore);
                    }
                }

				l_PreviousRadiusX = l_MaxX;
			}

			l_PreviousRadiusX = 1.0f;
			l_PreviousRadiusY = l_MaxY;
		}

		LOG_INFO("PoolManager", "Initialized Ores for Map %0", m_Map->GetId());
    }

    /// Update Pool
    /// @p_Diff : Execution Time
    void PoolManager::Update(uint32 const p_Diff)
    {
        for (auto l_Itr : m_Pool)
            l_Itr.second->Update(p_Diff);
    }

    /// Add Bonus box to map
    /// @p_Victim   : Victim
    /// @p_Owner    : Owner of cargo box
    void PoolManager::AddBonuxBox(Entity::Unit* p_Victim, BonusBoxType p_Type, Entity::Unit* p_Owner)
    {
        /// Note; we do not need to send cargo box packet, this will be sent on next object build update
        /// so just add cargo box to map and pool
        
        Entity::BonusBox* l_BonusBox = new Entity::BonusBox(p_Type);

        /// TODO; Player does not give all their cargo, only some of it, right now it gives all of it and doesn't take away
        l_BonusBox->m_OwnerId = p_Owner->GetObjectGUID().GetCounter();
        l_BonusBox->m_Company = p_Owner->GetCompany();
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_PROMETIUM, p_Victim->GetResource(Entity::Resource::RESOURCE_PROMETIUM));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_ENDURIUM,  p_Victim->GetResource(Entity::Resource::RESOURCE_ENDURIUM));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_TERBIUM,   p_Victim->GetResource(Entity::Resource::RESOURCE_TERBIUM));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_XENOMIT,   p_Victim->GetResource(Entity::Resource::RESOURCE_XENOMIT));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_PROMETID,  p_Victim->GetResource(Entity::Resource::RESOURCE_PROMETID));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_DURANIUM,  p_Victim->GetResource(Entity::Resource::RESOURCE_DURANIUM));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_PROMERIUM, p_Victim->GetResource(Entity::Resource::RESOURCE_PROMERIUM));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_PALLADIUM, p_Victim->GetResource(Entity::Resource::RESOURCE_PALLADIUM));
        l_BonusBox->SetResource(Entity::Resource::RESOURCE_SEPROM,    p_Victim->GetResource(Entity::Resource::RESOURCE_SEPROM));

        l_BonusBox->SetMap(m_Map);
        l_BonusBox->GetSpline()->SetPosition(p_Victim->GetSpline()->GetPositionX(), p_Victim->GetSpline()->GetPositionY());
        
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