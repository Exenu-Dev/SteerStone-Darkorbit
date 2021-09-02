/*
* Liam Ashdown
* Copyright (C) 2021
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

#include "Player.hpp"
#include "Inventory.hpp"
#include "ObjectManager.hpp"
#include "Common.hpp"

#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Entity {
    
    /// Constructor
    /// @p_Player : Player owner
    Inventory::Inventory(Player* p_Player)
        : m_Player(p_Player)
    {
        m_WeaponCount = 0;
    }
    /// Deconstructor
    Inventory::~Inventory()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Load Inventory from database
    void Inventory::LoadInventory()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, entry_id FROM user_inventories WHERE user_id = ? AND equipped = 1 AND preset = 1");
        l_PreparedStatement->SetUint32(0, m_Player->GetId());
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                if (ItemTemplate const* l_ItemTemplate = sObjectManager->GetItemTemplate(l_Result[1].GetUInt32()))
                {
                    Item l_Item;
                    l_Item.m_ItemTemplate   = l_ItemTemplate;
                    
                    switch (l_ItemTemplate->Type)
                    {
                        case InventoryType::INVENTORY_TYPE_BATTERY:
                            l_Item.m_IsWeapon = true;
                            break;
                        case InventoryType::INVENTORY_TYPE_SPEED_GENERATOR:
                            l_Item.m_IsSpeed = true;
                            break;
                        case InventoryType::INVENTORY_TYPE_SHIELD_GENERATOR:
                            l_Item.m_IsShield = true;
                            break;
                        default:
                            LOG_ASSERT(false, "Inventory", "Inventory Type: %0 does not exist!");
                            break;
                    }

                    m_Items.push_back(l_Item);
                }
                else
                {
                    LOG_WARNING("Inventory", "Cannot find Item with entry %0 deleting from player inventory!", l_Result[1].GetUInt32());

                    Core::Database::PreparedStatement* l_PreparedStatementDelete = GameDatabase.GetPrepareStatement();
                    l_PreparedStatementDelete->PrepareStatement("DELETE FROM account_inventory WHERE id = ?");
                    l_PreparedStatementDelete->SetUint32(0, l_Result[0].GetUInt32());
                    l_PreparedStatementDelete->ExecuteStatement();
                }

            } while (l_PreparedResultSet->GetNextRow());
        }

    }

    /// Calculate player stats
    void Inventory::CalculateStats()
    {
        uint32 l_Damage             = 0;
        uint32 l_Speed              = 0;
        uint32 l_Shield             = 0;
        uint32 l_ShieldResistence   = 0;
        uint32 l_ShipWeaponCount    = Common::GetWeaponCountByShip(m_Player->GetShipType());
        uint32 l_LF3WeaponCount     = 0;
        m_WeaponCount               = 0;

        for (auto l_Itr : m_Items)
        {
            if (l_Itr.IsWeapon())
            {

                l_Damage += l_Itr.GetItemTemplate()->Value;
                if (l_Itr.GetItemTemplate()->Name == "LF-3")
                    l_LF3WeaponCount++;

                m_WeaponCount++;
                
            }
            else if (l_Itr.IsSpeed())
            {
                l_Speed += l_Itr.GetItemTemplate()->Value;
            }
            else if (l_Itr.IsShield())
            {
                l_Shield += l_Itr.GetItemTemplate()->Value;

                /// TODO; I think this is correct, we only use the highest percentage damage taken
                if (l_ShieldResistence < l_Itr.GetItemTemplate()->ValuePercentage)
                    l_ShieldResistence = l_Itr.GetItemTemplate()->ValuePercentage;
            }
        }

        m_Player->SetStats(l_Damage, l_Damage, l_Speed, l_Shield, l_ShieldResistence);

        if (m_WeaponCount == 0)
            m_Player->SetWeaponState(0);
        if (l_LF3WeaponCount >= l_ShipWeaponCount)
            m_Player->SetWeaponState(3);
        else if (m_WeaponCount >= l_ShipWeaponCount)
            m_Player->SetWeaponState(2);
        else
            m_Player->SetWeaponState(1);
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone