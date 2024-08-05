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

#include "Server/MiscPackets.hpp"
#include "Server/MapPackets.hpp"
#include "Server/ShipPackets.hpp"
#include "Server/AttackPackets.hpp"
#include "Socket.hpp"
#include "Portal.hpp"
#include "BonusBox.hpp"
#include "Mine.hpp"
#include "Ore.hpp"
#include "ZoneManager.hpp"
#include "World.hpp"
#include "GameFlags.hpp"
#include "ZoneManager.hpp"
#include "Station.hpp"


namespace SteerStone { namespace Game { namespace Server {

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleObjectMove(ClientPacket* p_Packet)
    {
        /// Client sends NaN when user is logging out
        std::string l_LoggingOut = p_Packet->ReadString();
        if (l_LoggingOut == "NaN")
            return;

        float l_NewPositionX = std::stof(l_LoggingOut);
        float l_NewPositionY = static_cast<float>(p_Packet->ReadUInt32());

        /// The client sends the current position - but we should never trust the client
        /// So skip this
        float l_PrevPositionX = static_cast<float>(p_Packet->ReadUInt32());
        float l_PrevPositionY = static_cast<float>(p_Packet->ReadUInt32());

        m_Player->GetSpline()->Move(l_NewPositionX, l_NewPositionY);
    }
    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleInitializeOpponent(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Attempt to find player in map
        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);
        
        if (!l_Object)
        {
            LOG_WARNING("Map", "Cannot find player Id: %0", l_Id);
            return;
        }

        m_Player->GetMap()->GetGrid(m_Player)->BuildObjectSpawnAndSend(l_Object, m_Player);
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandlePortalJump(ClientPacket* p_Packet)
    {
        Entity::Portal* l_Portal = m_Player->GetGrid()->CanJumpPortal(m_Player);

        /// Okay we can jump
        if (l_Portal)
        {
            if (m_Player->GetLevel() < l_Portal->GetLevel())
            {
                m_Player->SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_SYSTEM_MESSAGE, {
                    "jumplevelfalse",
                    "%",
                    l_Portal->GetLevel()
                }));
                return;
            }

            /// Send Packet
            Server::Packets::JumpPortal l_Packet;
            l_Packet.MapId = l_Portal->GetToMapId();
            l_Packet.PortalId = l_Portal->GetObjectGUID().GetCounter();
            m_Player->SendPacket(l_Packet.Write());

            Game::Map::JumpQueueCordinates l_JumpQueueCordinates;
            l_JumpQueueCordinates.MapId = l_Portal->GetToMapId();
            l_JumpQueueCordinates.PositionX = l_Portal->GetToPositionX();
            l_JumpQueueCordinates.PositionY = l_Portal->GetToPositionY();

            m_Player->GetMap()->AddToJumpQueue(m_Player, l_JumpQueueCordinates);
            m_Player->m_Jumping = true;
        }
        else
            m_Player->SendSystemMessage("jumpgate_failed_no_gate");
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleSelectTarget(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Cannot target ourself
        if (l_Id == m_Player->GetObjectGUID().GetCounter())
            return;

        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);

        if (!l_Object)
        {
            LOG_WARNING("Socket", "Attempted to find object %0 in map but does not exist!", l_Id);
            return;
        }

        if (l_Object->GetType() != Entity::Type::OBJECT_TYPE_MOB && l_Object->GetType() != Entity::Type::OBJECT_TYPE_PLAYER)
        {
            LOG_WARNING("Socket", "Attempted to select a target which is not a unit or a player!", l_Id);
            return;
        }

        m_Player->SetTarget(l_Object->ToUnit());

        Server::Packets::Attack::TargetHealth l_TargetHealthPacket;
        l_TargetHealthPacket.Shield = m_Player->GetTarget()->GetShield();
        l_TargetHealthPacket.MaxShield = m_Player->GetTarget()->GetMaxShield();
        l_TargetHealthPacket.HitPoints = m_Player->GetTarget()->GetHitPoints();
        l_TargetHealthPacket.MaxHitPoints = m_Player->GetTarget()->GetHitMaxPoints();
        m_Player->SendPacket(l_TargetHealthPacket.Write());
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleLaserShoot(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Cannot target ourself
        if (l_Id == m_Player->GetObjectGUID().GetCounter())
            return;

        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);

        if (!l_Object)
        {
            LOG_WARNING("Socket", "Attempted to find object %0 in map but does not exist!", l_Id);
            return;
        }

        if (l_Object->GetType() != Entity::Type::OBJECT_TYPE_MOB && l_Object->GetType() != Entity::Type::OBJECT_TYPE_PLAYER)
        {
            LOG_WARNING("Socket", "Attempted to select a target which is not a unit or a player!", l_Id);
            return;
        }

        // Cannot attack if there's player has no weapons
        if (m_Player->GetInventory()->GetWeaponCount() == 0)
        {
            SendPacket(Server::Packets::Misc::Update().Write(Server::Packets::Misc::InfoUpdate::INFO_UPDATE_MESSAGE, { "You need a weapon to be equipped to attack!" }));
            return;
        }

        m_Player->Attack(l_Object->ToUnit());
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleAbortLaser(ClientPacket* p_Packet)
    {
        m_Player->CancelAttack();
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleShootRocket(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        /// Cannot target ourself
        if (l_Id == m_Player->GetObjectGUID().GetCounter())
            return;

        Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);

        if (!l_Object)
        {
            LOG_WARNING("Socket", "Attempted to find object %0 in map but does not exist!", l_Id);
            return;
        }

        if (!l_Object->IsMob() && !l_Object->IsPlayer())
        {
            LOG_WARNING("Socket", "Attempted to select a target which is not a unit or a player!", l_Id);
            return;
        }

        m_Player->Attack(l_Object->ToUnit(), Entity::ATTACK_TYPE_ROCKET);
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleSellResources(ClientPacket* p_Packet)
    {
        int test = 0;
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleLootCargo(ClientPacket* p_Packet)
    {
        uint32 l_Id = p_Packet->ReadUInt32();

        Entity::Object* l_Object = m_Player->GetGrid()->FindObject(l_Id);

        if (!l_Object)
        {
            SendPacket(Server::Packets::MaterialCollectFail().Write());
            LOG_WARNING("Socket", "Cannot find object %0!", l_Id);
            return;
        }

        if (!l_Object->IsBonusBox())
        {
            SendPacket(Server::Packets::MaterialCollectFail().Write());
            LOG_WARNING("Socket", "Attempted to loot cargo but object is not a bonus box!");
            return;
        }

        if (m_Player->GetCargoSpace() == 0 && l_Object->ToBonusBox()->GetBoxType() == BonusBoxType::BONUS_BOX_TYPE_CARGO)
        {
            m_Player->SendPacket(Server::Packets::Ship::CargoFull().Write());
            return;
        }

        // Get the distance between the player and the cargo
        float l_Distance = m_Player->GetSpline()->GetDistance(l_Object->GetSpline());

        if (l_Distance > 500)
        {
            SendPacket(Server::Packets::MaterialCollectFail().Write());
            return;
        }

        l_Object->ToBonusBox()->RewardCredit(m_Player);
    }

    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleUpdateSettings(ClientPacket* p_Packet)
    {
        if (p_Packet->ReadString() == "SET")
        {
            m_Player->UpdateClientSettings(p_Packet);
        }
    }
    /// Map Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandleChangeMisc(ClientPacket* p_Packet)
    {
        std::string l_Type = p_Packet->ReadString();

        /// Trim linebreaks
        /// For some reason the "ROB" packet includes line breaks, very weird!
        l_Type = Core::Utils::String::RemoveLineBreaks(l_Type);

        if (l_Type == "CFG")
        {
            m_Player->ChangeConfiguration(p_Packet->ReadString()[0] == '1' ? 1 : 2);
        }
        else if (l_Type == "ROB")
        {
            // Check to see if player has a repair bot
            Entity::Item* l_RepairBot = m_Player->GetInventory()->GetRepairBot();

            if (!l_RepairBot)
            {
				m_Player->SendInfoMessage("You do not have a repair bot equipped!");
				return;
			}

            if (!m_Player->CanRepair())
            {
                m_Player->SendInfoMessage("You cannot repair your ship at this time!");
				return;
            }

            m_Player->Repair(!m_Player->IsRepairing());
        }
        else if (l_Type == "J")
        {
            JumpChipType l_JumpChipType = static_cast<JumpChipType>(p_Packet->ReadUInt32());

            Entity::Item* l_JumpChip = m_Player->GetInventory()->GetJumpChip();

            /// Check to see if player actually has a jump chip
            if (!l_JumpChip)
            {
                m_Player->SendInfoMessage("You do not have a jump chip equipped!");
                LOG_WARNING("Socket", "Player %0 attempted to jump without a jump chip equipped!", m_Player->GetName());
                return;
            }

            if (l_JumpChip->GetJumpChipType() != l_JumpChipType)
            {
                m_Player->SendInfoMessage("You do not have the correct jump chip equipped!");
				LOG_WARNING("Socket", "Player %0 attempted to jump with the wrong jump chip equipped!", m_Player->GetName());
				return;
			}

            if (m_Player->IsInCombat())
            {
                m_Player->SendSystemMessage("jump_cpu_failed_attack");
				return;
			}

            if (m_Player->LastCombatTime() < 20)
            {
				m_Player->SendSystemMessage("jump_cpu_failed_attack");
            }

            if (m_Player->IsJumping())
            {
                m_Player->SendInfoMessage("You are already jumping!");
                return;
            }

            switch (l_JumpChipType)
            {
                case JumpChipType::JUMP_CHIP_TYPE_JP_01:
                {
                    if (!m_Player->GetMap()->IsLowerMap())
                    {
                        m_Player->SendSystemMessage("jump_cpu_failed_map");
						return;
					}
                }
                break;
            }

            uint32 l_MapId = m_Player->GetX1CompanyMapId();

            if (l_MapId == m_Player->GetMap()->GetId())
            {
                m_Player->SendSystemMessage("jump_cpu_failed_ontarget");
                return;
            }

            Entity::Station* l_Station = sZoneManager->GetMap(l_MapId)->GetStation();

            LOG_ASSERT(l_Station, "Socket", "Cannot find station for map %0", l_MapId);

            m_Player->Teleport(l_MapId, l_Station->GetSpline()->GetPositionX(), l_Station->GetSpline()->GetPositionY());

            int32 l_JumpsLeft = l_JumpChip->GetMetaData<uint32>("jumps") - 1;

            if (l_JumpsLeft < 0)
            {
                m_Player->GetInventory()->RemoveItem(l_JumpChip);
                return;
            }

            l_JumpChip->UpdateMetaData("jumps", l_JumpsLeft);
        }
        else if (l_Type == "MIN")
        {
            if (m_Player->GetAmmo()->GetMines() <= 0)
            {
                // This is wrong text but cannot find the correct one
                m_Player->SendSystemMessage("smartbomb_failed_mines");
				return;
            }

            if (m_Player->GetAmmo()->GetMineCooldown()->OnCooldown())
            {
                m_Player->SendSystemMessage("mines_cooldown");
                return;
            }

            Entity::Mine* l_Mine = new Entity::Mine(m_Player, MinesType::MINE);
            l_Mine->SetMap(m_Player->GetMap());
            l_Mine->GetSpline()->SetPosition(m_Player->GetSpline()->GetPositionX(), m_Player->GetSpline()->GetPositionY());
            l_Mine->GetMap()->Add(l_Mine);

            m_Player->SetMine(-1);
            m_Player->SendUpdateCooldown(Entity::CooldownType::COOLDOWN_TYPE_MINE);
        }
        else if (l_Type == "SMB")
        {
            if (m_Player->GetAmmo()->GetSmartBombs() <= 0)
            {
                if (m_Player->GetAmmo()->GetMines() < 10)
                {
					m_Player->SendSystemMessage("smartbomb_failed_mines");
					return;
				}

                if (!m_Player->GetResource(Entity::Resource::RESOURCE_XENOMIT) < 100)
                {
					m_Player->SendSystemMessage("smartbomb_failed_xenomit");
					return;
				}
            }

            if (m_Player->GetAmmo()->GetSmartMineCooldown()->OnCooldown())
            {
                m_Player->SendSystemMessage("smartbomb_cooldown");
                return;
            }

            Entity::Mine* l_Mine = new Entity::Mine(m_Player, MinesType::SMART_BOMB);
            l_Mine->SetMap(m_Player->GetMap());
            l_Mine->GetSpline()->SetPosition(m_Player->GetSpline()->GetPositionX(), m_Player->GetSpline()->GetPositionY());
            l_Mine->GetMap()->Add(l_Mine);

            m_Player->SetResource(Entity::Resource::RESOURCE_XENOMIT, -100);
            m_Player->SetMine(-10);
            m_Player->SendAmmoUpdate();
            m_Player->SendUpdateCooldown(Entity::CooldownType::COOLDOWN_TYPE_SMART_MINE);
        }
        else if (l_Type == "ISH")
        {
            if (m_Player->GetAmmo()->GetInstantShields() <= 0)
            {
                if (m_Player->GetAmmo()->GetMines() < 10)
                {
                    m_Player->SendSystemMessage("instashield_failed_mines");
                    return;
                }

                if (!m_Player->GetResource(Entity::Resource::RESOURCE_XENOMIT) < 100)
                {
                    m_Player->SendSystemMessage("instashield_failed_xenomit");
                    return;
                }
            }

            if (m_Player->GetAmmo()->GetInstantShieldCooldown()->OnCooldown())
            {
                m_Player->SendSystemMessage("instashield_cooldown");
                return;
            }

            m_Player->SetResource(Entity::Resource::RESOURCE_XENOMIT, -100);
            m_Player->SetMine(-10);
            m_Player->SendAmmoUpdate();
            m_Player->SendUpdateCooldown(Entity::CooldownType::COOLDOWN_TYPE_INSTANT_SHIELD);
            m_Player->GetGrid()->SendPacketIfInSurrounding(
                Server::Packets::Misc::Info().Write(Server::Packets::Misc::InfoType::INFO_TYPE_INSTANT_SHIELD,
                {
                    m_Player->GetObjectGUID().GetCounter()
                }
            ), m_Player, true);
        }
	}

    void GameSocket::HandleLootOre(ClientPacket* p_Packet)
    {
		uint32 l_Id = p_Packet->ReadUInt32();

		Entity::Object* l_Object = m_Player->GetMap()->FindObject(l_Id);

        if (!l_Object)
        {
            SendPacket(Server::Packets::MaterialCollectFail().Write());
			LOG_WARNING("Socket", "Cannot find object %0!", l_Id);
			return;
		}

        if (!l_Object->IsOre())
        {
            SendPacket(Server::Packets::MaterialCollectFail().Write());
			LOG_WARNING("Socket", "Attempted to loot ore but object is not an ore!");
			return;
		}

        // Get the distance between the player and the ore
        float l_Distance = m_Player->GetSpline()->GetDistance(l_Object->GetSpline());

        if (l_Distance > 200)
        {
			SendPacket(Server::Packets::MaterialCollectFail().Write());
			return;
		}

        m_Player->CheckQuestCondition(ConditionType::CONDITION_TYPE_COLLECT, {
            { AttributeKeys::ATTRIBUTE_KEY_ORE_ID, static_cast<uint16>(l_Object->ToOre()->GetOreType()) }
        });

		l_Object->ToOre()->RewardCredit(m_Player);
	}

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone