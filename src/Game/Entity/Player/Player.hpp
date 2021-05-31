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
#include "Core/Core.hpp"

#include "Utility/UtiLockedQueue.hpp"
#include "SurroundingObject.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Inventory.hpp"

#define RADIATION_TIMER 1000

namespace SteerStone { namespace Game { namespace Server {
    class ClientPacket;
    class GameSocket;
    class PacketFilter;
}
}
}

namespace SteerStone { namespace Game { namespace Entity {

    class Player;

    typedef std::unordered_map<uint64, std::unique_ptr<SurroundingObject>> SurroundingMap;

    /// Holds Ammo data
    struct Ammo
    {
        friend class Player;

    public:
        /// Constructor
        Ammo()
        {
            m_BatteryLCB10   = 0;
            m_BatteryMCB25   = 0;
            m_BatteryMCB50   = 0;
            m_BatteryUCB100  = 0;
            m_BatterySAB50   = 0;

            m_RocketR310     = 0;
            m_RocketPLT2026  = 0;
            m_RocketPLT2021  = 0;

            m_Mines          = 0;
            m_SmartBombs     = 0;
            m_InstantShields = 0;
        }
        /// Deconstructor
        ~Ammo()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        int32 m_BatteryLCB10;
        int32 m_BatteryMCB25;
        int32 m_BatteryMCB50;
        int32 m_BatteryUCB100;
        int32 m_BatterySAB50;

        int32 m_RocketR310;
        int32 m_RocketPLT2026;
        int32 m_RocketPLT2021;

        int32 m_Mines;
        int32 m_SmartBombs;
        int32 m_InstantShields;
    };

    /// Player Drone Info
    struct Drone
    {
    public:
        /// Constructor
        Drone()
        {
            Type   = 0;
            Points = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint16 Type;
        uint16 Points;
    };

    /// Main entry for session in world
    class Player : public Unit
    {
    public:
        friend class Server::GameSocket;

    public:
        /// Constructor
        /// @p_GameSocket : Socket
        Player(Server::GameSocket* p_GameSocket);
        /// Deconstructor
        ~Player();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //              GENERAL
        ///////////////////////////////////////////
    public:
        /// Load Player Info from database
        bool LoadFromDB();
        /// Load Ship Info from database
        void LoadShipFromDB();
        /// Load Drone Info from database
        void LoadDrones();
    public:
        /// Save Player details to database
        /// @p_Asynchronous : Use Async connection
        void SaveToDB(bool p_Asynchronous = false);
        /// Save Ship details to database
        /// @p_Asynchronous : Use Async connection
        void SaveShipToDB(bool p_Asynchronous = false);
    private:
        /// Return Drone Level
        /// @p_Drone : Drone
        uint16 GetDroneLevel(Drone& p_Drone);
        /// Returns Level
        uint32 CalculateLevel();
        /// Check whether player is in radiation zone
        /// p_Diff : Execution Time
        void UpdateRadiationZone(uint32 const p_Diff);
    public:
        /// Update Player
        /// @p_Diff : Execution Time
        bool Update(uint32 p_Diff);

        ///////////////////////////////////////////
        //              LOG BOOK
        ///////////////////////////////////////////
    public:
        /// Update Log Book (used for website)
        /// @p_Log : Log text
        void UpdateLogBook(std::string p_Log);

        ///////////////////////////////////////////
        //        SURROUNDING SYSTEM
        ///////////////////////////////////////////
    public:
        /// Add Object to surrounding
        /// @p_Object : Object being added
        void AddToSurrounding(Object* p_Object);
        /// Remove Object from being despawned
        /// @p_Object : Object
        void RemoveScheduleDespawn(Object* p_Object);
        /// Check if Object is in surrounding
        /// @p_Object     : Object being checked
        bool IsInSurrounding(Object* p_Object);
        /// Schedule all surroundings to be despawned
        void ScheduleSurroundingsForDespawn();
        /// Clear and despawn all surroundings
        void ClearSurroundings();
        /// Send Packet to surroundings
        /// @p_PacketBuffer : Packet being sent
        void SendPacketToSurroundings(Server::PacketBuffer const* p_PacketBuffer);
    private:
        /// Update Surroundings
        /// @p_Diff : Execution Time
        void UpdateSurroundings(uint32 const p_Diff);

        ///////////////////////////////////////////
        //              PACKETS
        ///////////////////////////////////////////
    public:
        /// Send Client In-game settings
        void SendClientSettings();
        /// Update Client settings
        void UpdateClientSettings(Server::ClientPacket* p_Packet);
        /// Send ship details
        void SendInitializeShip();
        /// Send Logged In
        void SendLoggedIn();
        /// Send Account Rank
        void SendAccountRank();
        /// Send Display Star System
        void SendDisplayStarSystem();
        /// Send Map update
        void SendMapUpdate();
        /// Send Battery, Rocket and mine update
        void SendAmmoUpdate();
        /// Send Drone Info
        Server::PacketBuffer const* BuildDronePacket();
        /// Send Drone Info
        void SendDrones();
        /// Process Packets
        /// @p_PacketFilter : Type of packet
        bool ProcessPacket(Server::PacketFilter& p_PacketFilter);
        /// Queue Packet
        /// @_ClientPacket : Packet being queued
        void QueuePacket(Server::ClientPacket* p_ClientPacket);
        /// Send Packet
        /// @p_PacketBuffer : Packet Buffer
        void SendPacket(Server::PacketBuffer const* p_PacketBuffer);
        /// Kick Player
        void KickPlayer();

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        /// Getters Function
        uint32 GetId()             const     { return m_Id;               }
        std::string GetSessionId() const     { return m_SessionId;        }
        uint32 GetUridium()        const     { return m_Uridium;          }
        uint32 GetCredits()        const     { return m_Credits;          }
        uint32 GetJackPot()        const     { return m_Jackpot;          }
        uint32 GetLevel()          const     { return m_Level;            }
        uint32 GetExperience()     const     { return m_Experience;       }
        int32 GetHonor()           const     { return m_Honor;            }
        bool IsPremium()           const     { return m_Premium;          }
        bool IsLoggingOut()        const     { return m_LoggingOut;       }
        bool IsLoggedIn()          const     { return m_LoggedIn;         }
        bool IsJumping()           const     { return m_Jumping;          }
        uint32 GetCargoSpace()     const     { return m_CargoSpace;       }
        uint32 GetMaxCargoSpace()  const     { return m_MaxCargoSpace;    }
        uint32 GetMaxBattery()     const     { return m_MaxBattery;       }
        EventType GetEvent()       const     { return m_Event;            } 
        bool HasLoggedOut() const            { return m_LoggingOut;       }
        bool HasDrones()           const     { return !m_Drones.empty();  }
        Ammo const* GetAmmo()      const     { return &m_Ammo;            }
        Inventory* GetInventory()            { return &m_Inventory;       }
        std::shared_ptr<Server::GameSocket> ToSocket() { return m_Socket; }

        /// Setters Function
        void SetEventType(EventType const p_EventType)    { m_Event = p_EventType;         }
        void SetIsJumping(bool const p_Jumping)           { m_Jumping = p_Jumping;         }
        void SetLogout(bool const p_LoggedOut)            { m_LoggingOut = p_LoggedOut;    }
        void UpdateCredits(uint32 const p_Credits)        { m_Credits += p_Credits;        }
        void UpdateUridium(uint32 const p_Uridium)        { m_Uridium += p_Uridium;        }
        void UpdateExperience(uint32 const p_Experience);
        void UpdateHonor(uint32 const p_Honour)           { m_Honor += p_Honour;           }
        void SetCargoSpace(uint32 const p_CargoSpace)     { m_CargoSpace = p_CargoSpace;   }
        Core::Diagnostic::IntervalTimer IntervalLogout;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        /// Stored Player Info
        uint32 m_Id;
        ObjectGUID m_GUID;
        std::string m_SessionId;
        uint32 m_Jackpot;
        uint32 m_Level;
        uint32 m_MaxCargoSpace;
        uint32 m_MaxBattery;
        uint32 m_MaxRockets;
        uint32 m_CargoSpace;
        bool m_Premium;

        /// Player Settings
        bool m_DisplayBoost;
        bool m_DisplayDamage;
        bool m_DisplayAllLas;
        bool m_DisplayExploration;
        bool m_DisplayName;
        bool m_DisplayFirmIcon;
        bool m_DisplayAlphaBG;
        bool m_IgnoreRes;
        bool m_IgnoreBox;
        bool m_ConvertGates;
        bool m_ConvertOppo;
        bool m_SoundOff;
        bool m_BackgroundMusicOff;
        bool m_DisplayStatus;
        bool m_DisplayBubble;
        bool m_DisplayDigits;
        bool m_DisplayChat;
        bool m_DisplayDrones;
        bool m_ShowStarSystem;
        bool m_IgnoreCargo;
        bool m_IgnoreHostileCargo;
        bool m_AutoChangeAmmo;
        bool m_UseSystemFont;
        bool m_EnableBuyFast;

        bool m_LoggedIn;
        bool m_Jumping;
        bool m_LoggingOut;
        EventType m_Event;
        Ammo m_Ammo;
        Inventory m_Inventory;
        Core::Diagnostic::IntervalTimer m_IntervalNextSave;              ///< Save to database
        Core::Diagnostic::IntervalTimer m_IntervalRadiation;             ///< Save to database

        std::vector<Drone> m_Drones;                                     ///< Ship Drones
        SurroundingMap m_Surroundings;                                   ///< Objects surrounding player
        std::shared_ptr<Server::GameSocket> m_Socket;                    ///< Socket
        Core::Database::OperatorProcessor m_OperatorProcessor;           ///< Process Asynchronous Queries
        Core::Utils::LockedQueue<Server::ClientPacket*> m_RecievedQueue; ///< Packets
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone