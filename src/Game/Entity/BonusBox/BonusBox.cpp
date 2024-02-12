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

#include "Utility/UtilRandom.hpp"

#include "Server/Opcodes/Packets/Server/MiscPackets.hpp"
#include "Server/Opcodes/Packets/Server/LoginPackets.hpp"
#include "Server/Opcodes/Packets/Server/ShipPackets.hpp"
#include "Player.hpp"
#include "BonusBox.hpp"
#include "ZoneManager.hpp"
#include <array> 

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_Type : Type of Bonus Box
    BonusBox::BonusBox(BonusBoxType p_Type)
        : m_BoxType(p_Type)
    {
        m_OwnerId           = -1;
        m_IsFriendlyCargo   = true;
        m_ScheduleForDelete = false;
        m_Looted            = false;
        m_IntervalFriendlyCargo.SetInterval(FRIENDLY_CARGO_TIMER);

        SetType(Type::OBJECT_TYPE_BONUS_BOX);
        SetGUID(ObjectGUID(GUIDType::BonusBox));
    }
    /// Deconstructor
    BonusBox::~BonusBox()
    {
        sZoneManager->RemoveFromMap(this, true);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //                GENERAL
    ///////////////////////////////////////////

    /// Update
    /// @p_Diff : Execution Time
    void BonusBox::Update(uint32 const p_Diff)
    {
        if (NeedToBeUpdated())
            SetNeedToBeUpdated(false);

        m_IntervalFriendlyCargo.Update(p_Diff);
        if (m_IntervalFriendlyCargo.Passed(true))
        {
            if (m_IsFriendlyCargo)
                SetNeedToBeUpdated(true);

            m_IsFriendlyCargo = false;
        }
    }

    /// Reward Credit on kill
    /// @p_Player : Player being rewarded
    void BonusBox::RewardCredit(Entity::Player* p_Player)
    {
        if (m_BoxType == BonusBoxType::BONUS_BOX_TYPE_CARGO)
        {
            // (_root.resource.ore_prometium,_root.resource.ore_endurium,_root.resource.ore_terbium,_root.resource.ore_xenomit,_root.resource.ore_prometid,_root.resource.ore_duranium,_root.resource.ore_promerium);
            std::array<uint32, 7> l_CargoResourceTaken { 0, 0, 0, 0, 0, 0, 0 };

            for (uint32 l_I = 0; l_I < MAX_RESOURCE_COUNTER; l_I++)
            {
                uint32 l_CargoLeftOver = p_Player->GetCargoSpace();
                int32 l_Resource = static_cast<int32>(GetResource(l_I));

                if (l_CargoLeftOver == 0)
                {
                    p_Player->SendPacket(Server::Packets::Ship::CargoFull().Write());
                    break;
                }

                if (l_Resource == 0)
                    continue;

                if (l_CargoLeftOver >= l_Resource)
                    l_CargoResourceTaken[l_I] = l_Resource;
                else
                    l_CargoResourceTaken[l_I] = l_Resource - l_CargoLeftOver;

                l_Resource = l_Resource - l_CargoResourceTaken[l_I];
                    
                if (l_Resource < 0)
                    l_Resource = 0;

                SetResource(l_I, l_Resource);
                p_Player->SetResource(l_I, l_CargoResourceTaken[l_I]);
                p_Player->SetCargoSpace(l_CargoLeftOver - l_CargoResourceTaken[l_I]);
            }

            p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_CARGO, {
                l_CargoResourceTaken[Entity::Resource::RESOURCE_PROMETIUM],
                l_CargoResourceTaken[Entity::Resource::RESOURCE_ENDURIUM],
                l_CargoResourceTaken[Entity::Resource::RESOURCE_TERBIUM],
                l_CargoResourceTaken[Entity::Resource::RESOURCE_XENOMIT],
                l_CargoResourceTaken[Entity::Resource::RESOURCE_PROMETID],
                l_CargoResourceTaken[Entity::Resource::RESOURCE_DURANIUM],
                l_CargoResourceTaken[Entity::Resource::RESOURCE_PROMERIUM] }));

            /// Update Ores
            p_Player->UpdateOres();

            /// If player does not own the cargo, then take away honor also
            if (GetOwnerId() != p_Player->GetObjectGUID().GetCounter() && IsFriendlyCargo())
            {
                p_Player->SetHonor(p_Player->GetHonor() - FRIENDLY_HONOR_TAKE_AWAY);

                p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_HONOUR, {
                    p_Player->GetHonor() }));
            }

            /// Now check if the cargo is empty
            bool l_Empty = true;
            for (uint32 l_I = 0; l_I < MAX_RESOURCE_COUNTER; l_I++)
                if (GetResource(l_I) != 0)
                    l_Empty = false;

            /// It's empty, allow to be removed on next update
            if (l_Empty)
                SetScheduleForDelete(true);
        }
        else if (m_BoxType == BonusBoxType::BONUS_BOX_TYPE_BONUS)
        {
            /// The Unit class does not support giving ammo etc like with the cargo
            /// So we will just work it out on the spot
            std::vector<uint32> l_Credits = { 200, 500, 1000 };
            std::vector<uint32> l_Uridiums = { 20, 50, 75, 100 };
            std::vector<uint32> l_LCB10 = { 10, 15, 20, 50, 75 };
            std::vector<uint32> l_MCB25 = { 5, 25 };
            std::vector<uint32> l_MCB50 = { 15 };
            std::vector<uint32> l_SAB50 = { 5, 10, 25 };

            /// Player is in battle zone, increase the rewards
            if (GetMap()->IsBattleZoneMap())
            {
                l_Credits = { 300, 750, 1500 };
                l_Uridiums = { 30, 75, 150 };
            }

            BonusBoxRewardType l_RewardType = static_cast<BonusBoxRewardType>(Core::Utils::UInt32Random(0, 5));

            switch (l_RewardType)
            {
                case BonusBoxRewardType::BONUS_BOX_REWARD_TYPE_CREDITS:
                {
					uint32 l_Credit = l_Credits[Core::Utils::UInt32Random(0, 2)];
					p_Player->UpdateCredits(l_Credit);

                    p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_CREDIT, {
						l_Credit }));
                }
                break;
                case BonusBoxRewardType::BONUS_BOX_REWARD_TYPE_URIDIUM:
                {
                    uint32 l_Uridium = l_Uridiums[Core::Utils::UInt32Random(0, 3)];
                    p_Player->UpdateUridium(l_Uridium);

                    p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_URIDIUM, {
						l_Uridium }));
                }
                break;
                case BonusBoxRewardType::BONUS_BOX_REWARD_TYPE_LCB10:
                {
					uint32 l_LCB = l_LCB10[Core::Utils::UInt32Random(0, 4)];
					p_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_LCB10, l_LCB);

                    p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_BATTERY, {
                        BatteryType::BATTERY_TYPE_LCB10,
                        l_LCB
                    }));
                }
                break;
                case BonusBoxRewardType::BONUS_BOX_REWARD_TYPE_MCB25:
                {
                    uint32 l_MCB = l_MCB25[Core::Utils::UInt32Random(0, 1)];
                    p_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_MCB25, l_MCB);

                    p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_BATTERY, {
						BatteryType::BATTERY_TYPE_MCB25,
						l_MCB
					}));
                }
                break;
                case BonusBoxRewardType::BONUS_BOX_REWARD_TYPE_MCB50:
                {
					uint32 l_MCB = l_MCB50[0];
					p_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_MCB50, l_MCB);

                    p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_BATTERY, {
                        BatteryType::BATTERY_TYPE_MCB50,
                        l_MCB
                    }));
                }
                break;
                case BonusBoxRewardType::BONUS_BOX_REWARD_TYPE_SAB50:
                {
					uint32 l_SAB = l_SAB50[Core::Utils::UInt32Random(0, 2)];
					p_Player->SetBatteryAmmo(BatteryType::BATTERY_TYPE_SAB50, l_SAB);

                    p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_BATTERY, {
                        BatteryType::BATTERY_TYPE_SAB50,
						l_SAB
					}));
                }
                break;
                default:
                    LOG_ASSERT(false, "BonusBox", "Invalid Reward Type!");
            }

            /// Bonus box can now be deleted
            SetScheduleForDelete(true);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone