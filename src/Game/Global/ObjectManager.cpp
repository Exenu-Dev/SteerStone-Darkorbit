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

#include "World.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Database/DatabaseTypes.hpp"
#include "ObjectManager.hpp"

namespace SteerStone { namespace Game { namespace Global {

    SINGLETON_P_I(Manager);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    Manager::Manager()
    {

    }
    /// Deconstructor
    Manager::~Manager()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Load Mob Template
    void Manager::LoadMobTemplate()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT entry, name, type, weapon_state, hit_points, shield, shield_resistance, min_damage, max_damage, behaviour, respawn_timer, min_movement_time, max_movement_time, "
            "speed, experience, honor, credits, uridium, "
            "prometium, endurium, terbium, prometid, duranium, promerium, xenomit, seprom, palladium "
            "FROM mob_templates");
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        uint32 l_Counter = 0;

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::MobTemplate* l_MobTemplate = new Entity::MobTemplate();

                l_MobTemplate->Entry            = l_Result[0].GetUInt64();
                l_MobTemplate->Name             = l_Result[1].GetString();
                l_MobTemplate->Type             = l_Result[2].GetUInt16();
                l_MobTemplate->WeaponState      = l_Result[3].GetUInt16();
                l_MobTemplate->HitPoints        = l_Result[4].GetInt32();
                l_MobTemplate->Shield           = l_Result[5].GetInt32();
                l_MobTemplate->ShieldResistance = l_Result[6].GetInt32();
                l_MobTemplate->MinDamage        = l_Result[7].GetUInt32();
                l_MobTemplate->MaxDamage        = l_Result[8].GetUInt32();
                l_MobTemplate->Behaviour        = l_Result[9].GetUInt16();
                l_MobTemplate->RespawnTimer     = l_Result[10].GetUInt32();
                l_MobTemplate->MinMovementTime  = l_Result[11].GetUInt32();
                l_MobTemplate->MaxMovementTime  = l_Result[12].GetUInt32();
                l_MobTemplate->Speed            = l_Result[13].GetUInt32();
                l_MobTemplate->Experience       = l_Result[14].GetUInt32();
                l_MobTemplate->Honor            = l_Result[15].GetUInt32();
                l_MobTemplate->Credits          = l_Result[16].GetUInt32();
                l_MobTemplate->Uridium          = l_Result[17].GetUInt32();
                l_MobTemplate->Prometium        = l_Result[18].GetUInt32();
                l_MobTemplate->Endurium         = l_Result[19].GetUInt32();
                l_MobTemplate->Terbium          = l_Result[20].GetUInt32();
                l_MobTemplate->Prometid         = l_Result[21].GetUInt32();
                l_MobTemplate->Duranium         = l_Result[22].GetUInt32();
                l_MobTemplate->Promerium        = l_Result[23].GetUInt32();
                l_MobTemplate->Xenomit          = l_Result[24].GetUInt32();
                l_MobTemplate->Seprom           = l_Result[25].GetUInt32();
                l_MobTemplate->Palladium        = l_Result[26].GetUInt32();

                m_MobTemplate[l_MobTemplate->Entry] = l_MobTemplate;

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("ObjectManager", "Loaded %0 Mob Templates in %1 ms", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()));
    }
    /// Load Portal Template
    void Manager::LoadPortalTemplate()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT portal_id, name, company, type, map_id, position_x, position_y, to_map_id, to_position_x, to_position_y "
            "FROM portal_templates");
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();
        
        uint32 l_Counter = 0;

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::PortalTemplate* l_PortalTemplate = new Entity::PortalTemplate();
                l_PortalTemplate->PortalId      = l_Result[0].GetUInt32();
                l_PortalTemplate->Name          = l_Result[1].GetString();
                l_PortalTemplate->Company       = l_Result[2].GetUInt16();
                l_PortalTemplate->Type          = l_Result[3].GetUInt16();
                l_PortalTemplate->MapId         = l_Result[4].GetUInt32();
                l_PortalTemplate->PositionX     = l_Result[5].GetDouble();
                l_PortalTemplate->PositionY     = l_Result[6].GetDouble();
                l_PortalTemplate->ToMapId       = l_Result[7].GetUInt32();
                l_PortalTemplate->ToPositionX   = l_Result[8].GetDouble();
                l_PortalTemplate->ToPositionY   = l_Result[9].GetDouble();

                m_PortalTemplate[l_PortalTemplate->MapId].push_back(l_PortalTemplate);

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("ObjectManager", "Loaded %0 Portal Templates in %1 ms", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()));
    }
    /// Load Station Template
    void Manager::LoadStationTemplate()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, name, company, type, map_id, peace, position_x, position_y "
            "FROM station_templates");
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        uint32 l_Counter = 0;

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::StationTemplate* l_StationTemplate = new Entity::StationTemplate();
                l_StationTemplate->Id        = l_Result[0].GetUInt64();
                l_StationTemplate->Name      = l_Result[1].GetString();
                l_StationTemplate->Company   = l_Result[2].GetUInt16();
                l_StationTemplate->Type      = l_Result[3].GetUInt16();
                l_StationTemplate->MapId     = l_Result[4].GetUInt32();
                l_StationTemplate->Peace     = l_Result[5].GetBool();
                l_StationTemplate->PositionX = l_Result[6].GetDouble();
                l_StationTemplate->PositionY = l_Result[7].GetDouble();

                m_StationTemplate[l_StationTemplate->MapId] = l_StationTemplate;

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("ObjectManager", "Loaded %0 Station Templates in %1 ms", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()));
    }
    /// Load Item Template
    void Manager::LoadItemTemplate()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT entry, name, type, gfx, value, percentage_value, credits, uridium "
            "FROM item_templates");
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        uint32 l_Counter = 0;

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::ItemTemplate* l_ItemTemplate = new Entity::ItemTemplate();
                l_ItemTemplate->Id              = l_Result[0].GetUInt64();
                l_ItemTemplate->Name            = l_Result[1].GetString();
                l_ItemTemplate->Type            = l_Result[2].GetUInt32();
                l_ItemTemplate->GFX             = l_Result[3].GetUInt32();
                l_ItemTemplate->Value           = l_Result[4].GetInt32();
                l_ItemTemplate->ValuePercentage = l_Result[5].GetInt32();
                l_ItemTemplate->Credits         = l_Result[6].GetDouble();
                l_ItemTemplate->Uridium         = l_Result[7].GetDouble();

                m_ItemTemplate[l_ItemTemplate->Id] = l_ItemTemplate;

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("ObjectManager", "Loaded %0 Item Templates in %1 ms", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()));
    }
    /// Load Ship Template
    void Manager::LoadShipTemplate()
    {
        uint32 l_StartTime = sServerTimeManager->GetServerTime();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT entry, name, description, hit_points, nano_hull, speed, cargo, lasers, generators, "
            "rockets, ammo, extras, credits, uridium, disabled FROM ship_templates");
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        uint32 l_Counter = 0;

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                Entity::ShipTemplate* l_ShipTemplate = new Entity::ShipTemplate();
                l_ShipTemplate->Entry           = l_Result[0].GetUInt64();
                l_ShipTemplate->Name            = l_Result[1].GetString();
                l_ShipTemplate->Description     = l_Result[2].GetString();
                l_ShipTemplate->HitPoints       = l_Result[3].GetUInt32();
                l_ShipTemplate->NanoHull        = l_Result[4].GetUInt32();
                l_ShipTemplate->Cargo           = l_Result[5].GetUInt32();
                l_ShipTemplate->Lasers          = l_Result[6].GetUInt32();
                l_ShipTemplate->Generators      = l_Result[7].GetUInt32();
                l_ShipTemplate->Rockets         = l_Result[8].GetUInt32();
                l_ShipTemplate->Ammo            = l_Result[9].GetUInt32();
                l_ShipTemplate->Extras          = l_Result[10].GetUInt32();
                l_ShipTemplate->Credits         = l_Result[11].GetUInt32();
                l_ShipTemplate->Uridium         = l_Result[12].GetUInt32();
                l_ShipTemplate->Disabled        = l_Result[13].GetUInt32(); ///GetBool();

                m_ShipTemplate[l_ShipTemplate->Entry] = l_ShipTemplate;

                l_Counter++;

            } while (l_PreparedResultSet->GetNextRow());
        }

        LOG_INFO("ObjectManager", "Loaded %0 Ship Templates in %1 ms", l_Counter, sServerTimeManager->GetTimeDifference(l_StartTime, sServerTimeManager->GetServerTime()));
    }

    /// Returns Mob Template
    /// @p_Entry : Entry
    Entity::MobTemplate const* Manager::GetMobTemplate(uint32 const p_Entry)
    {
        auto const l_Itr = m_MobTemplate.find(p_Entry);
        if (l_Itr != m_MobTemplate.end())
            return l_Itr->second;

        return nullptr;
    }
    /// Returns storage
    /// @p_MapId : Map Id
    std::vector<Entity::PortalTemplate*> const& Manager::GetPortalTemplate(uint32 const p_MapId)
    {
        auto const l_Itr = m_PortalTemplate.find(p_MapId);
        if (l_Itr != m_PortalTemplate.end())
            return l_Itr->second;

        return std::vector<Entity::PortalTemplate*>{};
    }
    /// Returns Station Template
    /// @p_MapId : Map Id
    Entity::StationTemplate const* Manager::GetStationTemplate(uint32 const p_MapId)
    {
        auto const l_Itr = m_StationTemplate.find(p_MapId);
        if (l_Itr != m_StationTemplate.end())
            return l_Itr->second;

        return nullptr;
    }
    /// Returns Item Template
    /// @p_Entry : Entry Id
    Entity::ItemTemplate const* Manager::GetItemTemplate(uint32 const p_Entry)
    {
        auto const l_Itr = m_ItemTemplate.find(p_Entry);
        if (l_Itr != m_ItemTemplate.end())
            return l_Itr->second;

        return nullptr;
    }
    /// Returns Ship Template
    /// @p_Entry : Entry Id
    Entity::ShipTemplate const* Manager::GetShipTemplate(uint32 const p_Entry)
    {
        auto const l_Itr = m_ShipTemplate.find(p_Entry);
        if (l_Itr != m_ShipTemplate.end())
            return l_Itr->second;

        return nullptr;
    }
}   ///< namespace Global
}   ///< namespace Game
}   ///< namespace Steerstone
