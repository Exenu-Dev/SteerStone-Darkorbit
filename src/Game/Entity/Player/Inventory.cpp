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
        // Clear Inventory
        m_Items.clear();

        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, entry_id, meta_data, unique_id FROM user_inventories WHERE user_id = ? AND equipped = 1 AND preset = ?");
        l_PreparedStatement->SetUint32(0, m_Player->GetId());
        l_PreparedStatement->SetUint16(1, m_Player->GetPreset());
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
                    l_Item.m_Id             = l_Result[0].GetUInt32();
                    l_Item.m_UniqueId       = l_Result[3].GetString();

                    if (!l_Result[2].IsEmpty())
						l_Item.m_MetaData = nlohmann::json::parse(l_Result[2].GetString());
                    
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
                        case InventoryType::INVENTORY_TYPE_EXTRAS:
							l_Item.m_IsExtra = true;
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
    /// Save Inventory to database
    void Inventory::SaveInventory()
    {
        for (auto l_Itr : m_Items)
        {
            if (l_Itr.m_MetaData.is_null() || l_Itr.m_MetaData.empty())
				continue;

            Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();

            /// The Jumps needs to be synced
            if (l_Itr.GetItemTemplate()->Id == static_cast<uint8>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_01) || l_Itr.GetItemTemplate()->Id == static_cast<uint8>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_02))
            {
                l_PreparedStatement->PrepareStatement("UPDATE user_inventories SET meta_data = ? WHERE unique_id = ?");
                l_PreparedStatement->SetString(1, l_Itr.GetUniqueId());
            }
            else
            {
				l_PreparedStatement->PrepareStatement("UPDATE user_inventories SET meta_data = ? WHERE id = ?");
				l_PreparedStatement->SetUint32(1, l_Itr.GetId());
			}

            l_PreparedStatement->SetString(0, l_Itr.m_MetaData.dump());

			l_PreparedStatement->ExecuteStatement();
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
        float  l_HitChance          = HIT_CHANCE;
        m_WeaponCount               = 0;
        m_Player->SetRocketAttackUpdateTimer(ROCKET_ATTACK_UPDATE);

        for (auto l_Itr : m_Items)
        {
            if (l_Itr.IsWeapon())
            {
                l_Damage += l_Itr.GetItemTemplate()->Value;
                /// TODO; Don't check against name as this could change and would break
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
            else if (l_Itr.IsExtra())
            {
                if (l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_AIM_01))
                {
                    l_HitChance += (l_Itr.GetItemTemplate()->Value ? (l_Itr.GetItemTemplate()->Value / 100) : l_Itr.GetItemTemplate()->ValuePercentage);
                }
                else if (l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_ROK_T01))
                {
                    m_Player->SetRocketAttackUpdateTimer(ROCKET_ATTACK_UPDATE / 2);
                }
			}
        }

        m_Player->SetStats(l_Damage, l_Damage, l_Speed, l_Shield, l_ShieldResistence, l_HitChance);

        if (m_WeaponCount == 0)
            m_Player->SetWeaponState(0);
        if (l_LF3WeaponCount >= l_ShipWeaponCount)
            m_Player->SetWeaponState(3);
        else if (m_WeaponCount >= l_ShipWeaponCount)
            m_Player->SetWeaponState(2);
        else
            m_Player->SetWeaponState(1);
    }
    /// Has Auto Buy Ammo
    bool Inventory::HasAutoAmmo() const
    {
        for (auto l_Itr : m_Items)
        {
			if (l_Itr.IsExtra() && l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_AM_CPU))
				return true;
		}

        return false;
    }
    /// Check to see if player has a jump chip
    bool Inventory::HasJumpChip() const
    {
        for (auto l_Itr : m_Items)
        {
            if (l_Itr.IsExtra() && l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_01) || l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_02))
                return true;
        }

        return false;
    }
    /// Get Repair Bot
    Item* Inventory::GetRepairBot() const
    {
        for (auto l_Itr : m_Items)
        {
			if (l_Itr.IsExtra() && l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_REP_1))
				return const_cast<Item*>(&l_Itr);
        }

        return nullptr;
    }
    /// Get Jump Chip
    Item* Inventory::GetJumpChip() const
    {
        for (auto& l_Itr : m_Items)
        {
            if (l_Itr.IsExtra() && l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_01) || l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_02))
                return const_cast<Item*>(&l_Itr);
		}

        return nullptr;
    }
    /// Get Item By Type
    /// @p_ItemType : Item Type
    Item* Inventory::GetItemByType(ItemType p_ItemType) const
    {
        switch (p_ItemType)
        {
            case ItemType::ITEM_TYPE_REPAIR_BOT:
				return GetRepairBot();
        }

        return nullptr;
    }
    /// Get the Jump Chip Type
    /// Returns JUMP_CHIP_TYPE_NONE if no jump chip
    JumpChipType Inventory::GetJumpChipType() const
    {
        for (auto l_Itr : m_Items)
        {
            if (l_Itr.IsExtra() && l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_01))
                return JumpChipType::JUMP_CHIP_TYPE_JP_01;
            else if (l_Itr.IsExtra() && l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_JP_02))
                return JumpChipType::JUMP_CHIP_TYPE_JP_02;
		}

        return JumpChipType::JUMP_CHIP_TYPE_NONE;
    }
    /// Automatically purchase Ammo
    void Inventory::BuyAutoAmmo()
    {
        if (!HasAutoAmmo())
            return;

        for (auto l_Itr : m_Items)
        {
            if (l_Itr.IsExtra() && l_Itr.GetItemTemplate()->Id == static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_AM_CPU))
            {
				if (l_Itr.m_MetaData["ammo_id"].is_null())
					return;

				uint32 l_AmmoId = l_Itr.m_MetaData["ammo_id"].get<uint32>();

                BatteryType l_BatteryType;

                switch (l_AmmoId)
                {
                case static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_LCB_10):
                    l_BatteryType = BatteryType::BATTERY_TYPE_LCB10;
                    break;
                case static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_MCB_25):
                    l_BatteryType = BatteryType::BATTERY_TYPE_MCB25;
                    break;
                case static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_MCB_50):
                    l_BatteryType = BatteryType::BATTERY_TYPE_MCB50;
                    break;
                case static_cast<uint32>(ItemTemplatesId::ITEM_TEMPLATE_ID_SAB_50):
                    l_BatteryType = BatteryType::BATTERY_TYPE_SAB50;
                    break;
                default:
                    LOG_ASSERT(false, "Inventory", "Battery Type: %0 does not exist!", l_AmmoId);
                }

                if (m_Player->GetSelectedBatteryAmmo() != l_BatteryType)
                    return;

                ItemTemplate const* l_ItemTemplate = sObjectManager->GetItemTemplate(l_AmmoId);

                LOG_ASSERT(l_ItemTemplate, "Inventory", "Cannot find ammo with id: %0", l_AmmoId);

                uint32 l_Price = 0;

                if (l_ItemTemplate->GetPriceType() == Entity::PriceType::PRICE_TYPE_CREDITS)
                {
                    l_Price = l_ItemTemplate->Credits * AUTO_AMMO_UNITS;

                    if (l_Price > m_Player->GetCredits())
                    {
						m_Player->SendInfoMessage("Cannot use auto ammo, not enough credits!");
                        return;
					}

                    m_Player->UpdateCredits(-l_Price);
                }
                else if (l_ItemTemplate->GetPriceType() == Entity::PriceType::PRICE_TYPE_URIDIUM)
                {
                    l_Price = l_ItemTemplate->Uridium * AUTO_AMMO_UNITS;

                    if (l_Price > m_Player->GetUridium())
                    {
                        m_Player->SendInfoMessage("Cannot use auto ammo, not enough uridium!");
                        return;
                    }

                    m_Player->UpdateUridium(-l_Price);
                }

                m_Player->SetBatteryAmmo(l_BatteryType, AUTO_AMMO_UNITS);

                m_Player->SendInfoMessage("Brought auto ammo for: " + l_ItemTemplate->Name);
			}
		}
    }
    /// Remove Item From Inventory
    /// @p_Item : Item to remove
    void Inventory::RemoveItem(Item* p_Item)
    {
        for (auto l_Itr = m_Items.begin(); l_Itr != m_Items.end(); ++l_Itr)
        {
            if (&(*l_Itr) == p_Item)
            {
				m_Items.erase(l_Itr);
				break;
			}
		}

        /// Saving the player also saves the inventory
        /// No harm done saving the player to the database
        m_Player->SaveToDB();
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone