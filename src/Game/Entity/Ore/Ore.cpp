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
#include "Utility/UtilMaths.hpp"

#include "Server/Opcodes/Packets/Server/MiscPackets.hpp"
#include "Server/Opcodes/Packets/Server/LoginPackets.hpp"
#include "Server/Opcodes/Packets/Server/ShipPackets.hpp"

#include "Ore.hpp"
#include "Player.hpp"
#include <array> 

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Ore::Ore(OreTypes p_OreType)
    {
        m_Type = p_OreType;

        switch (m_Type)
        {
            case OreTypes::ORE_TYPE_PROMETIUM:
				m_ResourceType = Resource::RESOURCE_PROMETIUM;
				break;
            case OreTypes::ORE_TYPE_ENDURIUM:
                m_ResourceType = Resource::RESOURCE_ENDURIUM;
				break;
			case OreTypes::ORE_TYPE_TERBIUM:
                m_ResourceType = Resource::RESOURCE_TERBIUM;
				break;
        }

        SetType(Type::OBJECT_TYPE_ORE);
        SetGUID(ObjectGUID(GUIDType::Ore));
    }
    /// Deconstructor
    Ore::~Ore()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //                GENERAL
    ///////////////////////////////////////////

    /// Reward Credit on loot
    /// @p_Player : Player being rewarded
    void Ore::RewardCredit(Entity::Player* p_Player)
    {
        /// Prometium, Endurium, Terbium, and the rest
        std::array<uint32, 7> l_CargoResourceTaken{ 0, 0, 0, 0, 0, 0, 0 };

        uint32 l_CargoLeftOver = p_Player->GetCargoSpace();
        int32 l_Resource = static_cast<int32>(GetResource(m_ResourceType));

        if (l_CargoLeftOver == 0)
        {
            p_Player->SendPacket(Server::Packets::Ship::CargoFull().Write());
            return;
        }

        if (l_Resource == 0)
            return;

        if (l_CargoLeftOver >= l_Resource)
            l_CargoResourceTaken[m_ResourceType] = l_Resource;
        else
            l_CargoResourceTaken[m_ResourceType] = l_Resource - l_CargoLeftOver;

        l_Resource = l_Resource - l_CargoResourceTaken[m_ResourceType];

        if (l_Resource < 0)
            l_Resource = 0;

        l_CargoResourceTaken[static_cast<uint32>(m_ResourceType)] = GetResource(m_ResourceType);

        SetResource(m_ResourceType, l_Resource);
        p_Player->SetResource(m_ResourceType, l_CargoResourceTaken[m_ResourceType]);

        p_Player->UpdateOres();

        p_Player->SendPacket(Server::Packets::Misc::Reward().Write(Server::Packets::Misc::RewardType::REWARD_TYPE_CARGO, {
            l_CargoResourceTaken[Entity::Resource::RESOURCE_PROMETIUM],
            l_CargoResourceTaken[Entity::Resource::RESOURCE_ENDURIUM],
            l_CargoResourceTaken[Entity::Resource::RESOURCE_TERBIUM],
            l_CargoResourceTaken[Entity::Resource::RESOURCE_XENOMIT],
            l_CargoResourceTaken[Entity::Resource::RESOURCE_PROMETID],
            l_CargoResourceTaken[Entity::Resource::RESOURCE_DURANIUM],
            l_CargoResourceTaken[Entity::Resource::RESOURCE_PROMERIUM] 
        }));

        // Can now be deleted
        SetScheduleForDelete(true);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone