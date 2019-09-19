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

#pragma once
#include <PCH/Precompiled.hpp>

#include "Socket.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    class InitializeShip;
    class MapUpdate;
    class MiniMapUpdate;

}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Entity {

    class Player;
    class Server::Packets::InitializeShip;

    /// Holds Ammo data
    struct Ammo
    {
        friend class Ship;

        public:
            /// Constructor
            Ammo()
            {
                m_LaserType         = 1;
                m_BatteryLCB10      = 0;
                m_BatteryMCB25      = 0;
                m_BatteryMCB50      = 0;
                m_BatteryUCB100     = 0;
                m_BatterySAB50      = 0;

                m_RocketType        = 0;
                m_RocketR310        = 0;
                m_RocketPLT2026     = 0;
                m_RocketPLT2021     = 0;

                m_Mines             = 0;
                m_SmartBombs        = 0;
                m_InstantShields    = 0;
            }
            /// Deconstructor
            ~Ammo() 
            {
            }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

            /// Set Laser Type
            /// @p_LaserType : Laser Type
            void SetLaserType(uint16 const p_LaserType) { m_LaserType = p_LaserType; }
            /// Set Rocket Type
            /// @p_RocketType : Rocket Type
            void SetRocketType(uint16 const p_RocketType) { m_RocketType = p_RocketType; }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            uint16 m_LaserType;
            int32 m_BatteryLCB10;
            int32 m_BatteryMCB25;
            int32 m_BatteryMCB50;
            int32 m_BatteryUCB100;
            int32 m_BatterySAB50;

            uint16 m_RocketType;
            int32 m_RocketR310;
            int32 m_RocketPLT2026;
            int32 m_RocketPLT2021;

            int32 m_Mines;
            int32 m_SmartBombs;
            int32 m_InstantShields;
    };

    /// Ship Data for player class
    class Ship
    {
        friend class Player;

        public:
            /// Constructor
            /// @p_Player : Player
            Ship(Player* p_Player);
            /// Deconstructor
            ~Ship();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

            /// Load player details from database
            bool LoadFromDB();

            /// Send ship details
            /// @p_Packet : Packet being formed
            void FormulateInitializeShip(Server::Packets::InitializeShip& p_Packet);
            /// Send Map update
            void SendMapUpdate();
            /// Send Battery, Rocket and mine update
            void SendAmmoUpdate();

            /// Get Ammo
            Ammo* GetAmmo();

            PlayerShips GetShipId()     const { return m_ShipId;            }
            int32 GetShield()           const { return m_Shield;            }
            uint32 GetMaxShield()       const { return m_MaxShield;         }
            int32 GetHitPoints()        const { return m_HitPoints;         }
            uint32 GetHitMaxPoints()    const { return m_MaxHitPoints;      }
            uint32 GetCargoSpace()      const { return m_CargoSpace;        }
            uint32 GetMaxCargoSpace()   const { return m_MaxCargoSpace;     }
            uint32 GetMaxBattery()      const { return m_MaxBattery;        }
            uint16 GetWeaponState()     const { return m_WeaponState;       }
            bool UseSystemFont()        const { return m_UseSystemFont;     }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            PlayerShips m_ShipId;
            int32 m_Shield;
            uint32 m_MaxShield;
            int32 m_HitPoints;
            uint32 m_MaxHitPoints;
            uint32 m_CargoSpace;
            uint32 m_MaxCargoSpace;
            uint32 m_MaxBattery;
            uint32 m_MaxRockets;
            uint16 m_WeaponState;
            bool m_UseSystemFont;
            Ammo m_Ammo;

            Player* m_Player;   ///< Player
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone