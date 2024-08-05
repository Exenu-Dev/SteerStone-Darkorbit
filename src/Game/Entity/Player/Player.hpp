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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"

#include "Utility/UtiLockedQueue.hpp"
#include "SurroundingObject.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Inventory.hpp"
#include "Quest.hpp"

#define RADIATION_TIMER 1000
#define CONFIG_TIMER 5000 ///< TODO; Move to config settings under player?
#define ORE_CALCULATION_AMOUNT 500000
#define REPAIR_BOT_TIMER 3000
#define QUEST_UPDATE_TIMER 1000
#define MINE_COOLDOWN 20000

namespace SteerStone { namespace Game { namespace Entity {

    class Server::ClientPacket;
    class Player;

    typedef std::unordered_map<uint64, std::unique_ptr<SurroundingObject>> SurroundingMap;
    typedef std::unordered_map<uint32, Quest*> QuestMap;

    enum class CooldownType
    {
    	COOLDOWN_TYPE_MINE,
        COOLDOWN_TYPE_SMART_MINE,
        COOLDOWN_TYPE_INSTANT_SHIELD
    };

    struct Booster
    {
    public:
        /// Constructor
        Booster()
        {
            Id              = 0;
            BoosterItemTemplate    = nullptr;
            Duration        = 0;
		}

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        ItemTemplate const* GetItemTemplate() const { return BoosterItemTemplate; }

        /// Get Item Template
        ItemTemplate const* GetItemTemplate()
        {
			return BoosterItemTemplate;
		}

        /// Update Duration
        /// @p_Diff : Execution Time
        bool Update(uint32 const p_Diff)
        {
			if (Duration > 0)
				Duration -= p_Diff;

            return Duration <= 0;
		}

        int32 GetDuration() const
		{
			return Duration;
		}

        /// Get Booster Type
        BoosterTypes GetBoosterType() const;
        /// Get Booster Value
        int32 GetBoosterValue() const;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 Id;
        ItemTemplate const* BoosterItemTemplate;
        int32 Duration;
	};

    struct Cooldown
    {
    public:
        Cooldown()
        {
            m_IsOnCooldown = false;
            m_Timer.SetInterval(0);
        }

        bool OnCooldown()
        {
            return m_IsOnCooldown;
        }

        bool Update(uint32 const p_Diff)
        {
            m_Timer.Update(p_Diff);

            if (!m_Timer.Passed())
                return false;

            if (m_IsOnCooldown)
            {
                m_IsOnCooldown = false;
                return true;
            }

            return false;
		}

        void StartCooldown()
        {
			m_Timer.Reset();
			m_IsOnCooldown = true;
		}

        void SetCooldown(uint32 const p_Cooldown)
        {
			m_Timer.SetInterval(p_Cooldown);
		}

    private:

        Core::Diagnostic::IntervalTimer m_Timer;
        bool m_IsOnCooldown;
    };

    /// Holds Ammo data
    struct Ammo
    {
        friend class Player;

    public:
        /// Constructor
        Ammo(Player* p_Player)
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

            m_MineCooldown.SetCooldown(MINE_COOLDOWN);
            m_SmartMineCooldown.SetCooldown(MINE_COOLDOWN);
            m_InstantShieldCooldown.SetCooldown(MINE_COOLDOWN);

            m_Player = p_Player;
        }
        /// Deconstructor
        ~Ammo()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    public:
        /// Getters Setters
        int32 GetLCB10()   const { return m_BatteryLCB10;  }
        int32 GetMCB25()   const { return m_BatteryMCB25;  }
        int32 GetMCB50()   const { return m_BatteryMCB50;  }
        int32 GetUCB100()  const { return m_BatteryUCB100; }
        int32 GetSAB50()   const { return m_BatterySAB50;  }
        int32 GetMines()   const { return m_Mines;         }

        Cooldown* GetMineCooldown() { return &m_MineCooldown; }
        Cooldown* GetSmartMineCooldown() { return &m_SmartMineCooldown; }
        Cooldown* GetInstantShieldCooldown() { return &m_InstantShieldCooldown; }

        /// Get Next Available Ammo
        BatteryType GetAvailableBattery() const
        {
            if (m_BatteryLCB10)
                return BatteryType::BATTERY_TYPE_LCB10;

            if (m_BatteryMCB25)
                return BatteryType::BATTERY_TYPE_MCB25;

            if (m_BatteryMCB50)
                return BatteryType::BATTERY_TYPE_MCB50;

            if (m_BatteryUCB100)
                return BatteryType::BATTERY_TYPE_UCB100;

            if (m_BatterySAB50)
                return BatteryType::BATTERY_TYPE_SAB50;

            return BatteryType::BATTERY_TYPE_NONE;
        }

        /// Get Available Rocket
        RocketType GetAvailableRocket() const
        {
            if (m_RocketR310)
                return RocketType::ROCKET_TYPE_R310;

            if (m_RocketPLT2021)
                return RocketType::ROCKET_TYPE_PLT_2021;

            if (m_RocketPLT2026)
                return RocketType::ROCKET_TYPE_PLT_2026;

            return RocketType::ROCKET_TYPE_NONE;
        }
        /// Get Total Batteries
        int32 GetTotalBatteries() const
        {
			return m_BatteryLCB10 + m_BatteryMCB25 + m_BatteryMCB50 + m_BatteryUCB100 + m_BatterySAB50;
		}
        /// Get Total Rockets
        int32 GetTotalRockets() const
        {
            return m_RocketR310 + m_RocketPLT2021 + m_RocketPLT2026 + m_Mines;
        }

        uint32 GetInstantShields() const;

        uint32 GetSmartBombs() const;

        void Update(uint32 const p_Diff);

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

        Cooldown m_MineCooldown;
        Cooldown m_SmartMineCooldown;
        Cooldown m_InstantShieldCooldown;

        Player* m_Player;
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

        /// Add Experience
        /// @p_Experience : Experience to add
        void AddExperience(const uint16 p_Experience)
        {
            Points += p_Experience;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint16 Type;
        uint16 Points;
    };

    /// Lab Upgrade
    /// Increases the player speed etc...
    struct LabUpgrade
    {
    public:
        /// Constructor
        LabUpgrade()
        {
            Id              = 0;
            Type            = "";
			ItemType        = "";
            Value           = 0;
			UpgradeType     = LabUpgradeType::LAB_UPGRADE_TYPE_NONE;
			Counter         = 0;
            Timer.SetInterval(1000);
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        bool Update(uint32 const p_Diff)
		{
            Timer.Update(p_Diff);
            if (!Timer.Passed())
				return true;

            /// The Shield and Engine runs on a timer while laser and rocket doesn't
			if (IsShieldOrEngine() && Value > 0)
                Value -= p_Diff;

            return Value > 0;
		}

        uint32 GetId() const
        {
            return Id;
		}

        bool IsShieldOrEngine() const
		{
			return UpgradeType == LabUpgradeType::LAB_UPGRADE_TYPE_SHIELDS || UpgradeType == LabUpgradeType::LAB_UPGRADE_TYPE_ENGINES;
		}

        bool IsLaserOrRocket() const
        {
            return UpgradeType == LabUpgradeType::LAB_UPGRADE_TYPE_LASERS || UpgradeType == LabUpgradeType::LAB_UPGRADE_TYPE_ROCKETS;
        }

        LabUpgradeType GetLabUpgradeType() const
		{
			return UpgradeType;
		}

        void SetValue(int32 const p_Value)
        {
            /// Lasers or Rockets takes 1 unit of resource per 10 units of laser or rocket
            if (IsLaserOrRocket())
            {
                Counter += p_Value;

                if (Counter <= 10)
                    return;
            }

            int32 l_Value = Value + p_Value;

            if (l_Value < 0)
				Value = 0;
			else
				Value = l_Value;
        }

        /// Get the value of the upgrade
        /// This all depends on the item type and the type
        /// Source: https://darkorbit.fandom.com/wiki/Refining_Upgrade
        int32 GetCalculatedValue() const
		{
            switch (UpgradeType)
            {
                case LabUpgradeType::LAB_UPGRADE_TYPE_SHIELDS:
                case LabUpgradeType::LAB_UPGRADE_TYPE_ENGINES:
                {
                    if (Type == "duranium")
                        return 10;

                    if (Type == "promerium")
						return 20;

                    return 1;
                }
                break;
                case LabUpgradeType::LAB_UPGRADE_TYPE_ROCKETS:
                case LabUpgradeType::LAB_UPGRADE_TYPE_LASERS:
                {
                    if (Type == "prometid")
                        return 15;

                    if (Type == "promerium")
                        return 30;

					return 1;
				}
            }
		}

        int32 GetValue() const
        {
			return Value;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 Id;
        std::string Type;
        std::string ItemType;
        int32 Value;
        LabUpgradeType UpgradeType;
        int32 Counter;
        Core::Diagnostic::IntervalTimer Timer;
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
        /// Load Boosters
        void LoadBoosters();
        /// Load Lab Upgrades
        void LoadLabUpgrades();
        /// Load Quests
        void LoadQuests();
    public:
        /// Save Player details to database
        void SaveToDB();
        /// Save Ship details to database
        void SaveShipToDB();
    public:
        /// Send Info Message to Player
        /// @p_Message : Message to send
        void SendInfoMessage(std::string const p_Message);
        /// Send System Message to Player
        /// @p_Message : Message to send
        void SendSystemMessage(std::string const p_Message);
        /// Get Company X-1 Map Id based on Player Company
        uint32 GetX1CompanyMapId() const;
        /// Update Speed
        /// Informs the client the new speed
        void SendUpdateSpeed();
    private:
        /// Return Drone Level
        /// @p_Drone : Drone
        uint16 GetDroneLevel(Drone& p_Drone) const;
        /// Returns Level
        uint32 CalculateLevel();
        /// Check whether player is in radiation zone
        /// p_Diff : Execution Time
        void UpdateRadiationZone(uint32 const p_Diff);
        /// Update Repairing
        /// @p_Diff : Execution Time
        void UpdateRepairing(uint32 const p_Diff);
        /// Update Cooldowns
        /// @p_Diff : Execution Time
        void UpdateCooldowns(uint32 const p_Diff);
        /// Send Update Cooldown
        /// @p_CooldownType : Cooldown Type
        void SendUpdateCooldown(CooldownType const p_CooldownType);
    public:
        /// Update Player
        /// @p_Diff : Execution Time
        bool Update(uint32 p_Diff) override;

        ///////////////////////////////////////////
        //              LOG BOOK
        ///////////////////////////////////////////
    public:
        /// Update Log Book (used for website)
        /// @p_Log : Log text
        /// @p_LogBookType : Type of Log Book
        void UpdateLogBook(std::string p_Log, LogBookType const p_LogBookType = LogBookType::LOG_BOOK_TYPE_DETAILED);

        ///////////////////////////////////////////
        //             QUESTS
        ///////////////////////////////////////////
        void SendQuests();
        void SaveQuestsToDB();
        void CheckQuestCondition(ConditionType const p_ConditionType, std::initializer_list<std::pair<AttributeKeys, std::variant<int32, std::string>>> p_List);
        void SendUpdateQuestCondition(uint32 const p_QuestId, uint32 const p_ConditionId, std::variant<int32, std::string> const p_Value);
        void FinishQuest(uint32 const p_QuestId);
        void FailQuest(uint32 const p_QuestId);
        void CancelQuest(uint32 const p_QuestId);
        void UpdateQuests(uint32 const p_Diff);

        ///////////////////////////////////////////
        //             BOOSTERS
        ///////////////////////////////////////////
    public:
        /// Update the Booster Times
        /// @p_Diff : Execution Time
        void UpdateBoosters(uint32 const p_Diff);
        /// Check to see if the player has a booster
        /// @p_BoosterType : Booster Type
        bool HasBooster(BoosterTypes const p_BoosterType) const;
        /// Get Booster Value
        /// @p_BoosterType : Booster Type
        int32 GetBoosterValue(BoosterTypes const p_BoosterType) const;
        /// Get Booster Name
        /// @p_BoosterType : Booster Type
        std::string GetBoosterName(BoosterTypes const p_BoosterType) const;
        /// Get Booster Duration
        /// @p_BoosterType : Booster Type
        int32 GetBoosterDuration(BoosterTypes const p_BoosterType) const;
        /// Send the Boosters Packet
        /// @p_ShowLastOne: Show the last booster status message
        void SendBoosters(bool const p_ShowLastOne = false);
        /// Remove Booster
        /// @p_BoosterId : Booster Id
        void RemoveBooster(uint32 const p_BoosterId);

        ///////////////////////////////////////////
        //             LAB UPGRADES
        ///////////////////////////////////////////

        /// Update Lab Upgrades
        /// @p_Diff : Execution Time
        void UpdateLabUpgrades(uint32 const p_Diff);
        /// Remove Lab Upgrade
        /// @p_Id : Id of Lab Upgrade
        void RemoveLabUpgrade(uint32 const p_Id);
        /// Get Lab Upgrade
        /// @p_LabUpgradeType : Lab Upgrade Type
        LabUpgrade* GetLabUpgrade(LabUpgradeType const p_LabUpgradeType);
        /// Save Lab Upgrades
        void SaveLabUpgrades();

        ///////////////////////////////////////////
        //        SURROUNDING SYSTEM
        ///////////////////////////////////////////
    public:
        /// Add Object to surrounding
        /// @p_Object : Object being added
        void AddToSurrounding(Object* p_Object);
        /// Remove Object from surrounding
        /// @p_Object : Object being removed
        void RemoveFromSurrounding(Object* p_Object);
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
        /// Send Clear Rocket Cooldown
        void SendClearRocketCooldown();
        /// Change Configuration
        void ChangeConfiguration(const uint16 p_Config);
        /// Send Drone Info
        void SendDrones();
        /// Send Cloak
        void SendCloak();
        /// Build Drones Packet
        std::string BuildDronesString() const;
        /// Update Cargo Max Space
        /// Note this updates the cargo bay on the client
        void UpdateMaxCargoSpace();
        /// Update Orders
        void UpdateOres();
        /// Set Ore Prices
        void SetOrePrices();
        /// On Trade Ore
        /// @p_ResourceId : Resource Id which we are trading
        /// @p_OreAmount : The Ore amount we are trading
        void OnTradeOre(const uint32 p_ResourceId, const uint32 p_OreAmount);
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
        /// Teleport Player to Map
        /// @p_MapId : Map Id to teleport to
        /// @p_PositionX : Position X to teleport to
        /// @p_PositionY : Position Y to teleport to
        void Teleport(const uint32 p_MapId, const int32 p_PositionX = 0, const int32 p_PositionY = 0);
        /// Update Items
        /// This updates the cloak, jump cpu, etc
        void UpdateExtrasInfo();
        /// Send the packet to update players health and shield
        void SendHealthAndShield();

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
        bool CanAutoChangeAmmo()   const     { return m_AutoChangeAmmo;   }
        bool IsRepairing()         const     { return m_Repairing;        }
        bool IsUpdateEvent()       const     { return m_UpdateEvent;      }
        bool IsCloaked()           const     { return m_IsCloaked;        }
        uint32 GetMaxCargoSpace()  const     { return m_MaxCargoSpace;    }
        uint32 GetMaxBattery()     const     { return m_MaxBattery;       }
        uint32 GetMaxRockets()     const     { return m_MaxRockets;       }
        uint8 GetPreset()          const     { return m_Preset;           }
        EventType GetEvent()       const     { return m_Event;            } 
        bool HasLoggedOut() const            { return m_LoggingOut;       }
        bool HasDrones()           const     { return !m_Drones.empty();  }
        bool CanRepair();
        Ammo* const GetAmmo()                { return &m_Ammo;            }
        Inventory* GetInventory()            { return &m_Inventory;       }
        std::shared_ptr<Server::GameSocket> ToSocket() { return m_Socket; }
        uint32 const GetSelectedBatteryAmmo()
        {
            switch (m_LaserType)
            {
                case BatteryType::BATTERY_TYPE_LCB10:
                    return m_Ammo.m_BatteryLCB10;
                break;
                case BatteryType::BATTERY_TYPE_MCB25:
                    return m_Ammo.m_BatteryMCB25;
                case BatteryType::BATTERY_TYPE_MCB50:
                    return m_Ammo.m_BatteryMCB50;
                case BatteryType::BATTERY_TYPE_SAB50:
                    return m_Ammo.m_BatterySAB50;
                case BatteryType::BATTERY_TYPE_UCB100:
                    return m_Ammo.m_BatteryUCB100;
                default:
                    LOG_ASSERT(false, "Player", "Could not find ammo for type %0", m_LaserType);
                break;
            }
        }
        uint32 const GetSelectedRocketAmmo()
        {
            switch (m_RocketType)
            {
                case RocketType::ROCKET_TYPE_R310:
                    return m_Ammo.m_RocketR310;
                case RocketType::ROCKET_TYPE_PLT_2021:
                    return m_Ammo.m_RocketPLT2021;
                case RocketType::ROCKET_TYPE_PLT_2026:
                    return m_Ammo.m_RocketPLT2026;
                default:
                    LOG_ASSERT(false, "Unit", "Could not find rocket for type %0", m_RocketType);
                break;
                        
            }
        }

        uint32 GetCargoSpace() const
        {
            return CalculateTotalCargoSpace();
        }

        /// Calculate total cargo space
        uint32 CalculateTotalCargoSpace() const
        {
            uint32 totalCargoSpace = 0;
            totalCargoSpace += m_Resources[Resource::RESOURCE_PROMETIUM];
            totalCargoSpace += m_Resources[Resource::RESOURCE_ENDURIUM];
            totalCargoSpace += m_Resources[Resource::RESOURCE_TERBIUM];
            totalCargoSpace += m_Resources[Resource::RESOURCE_XENOMIT];
            return totalCargoSpace;
        }

        /// Setters Function
        void SetEventType(EventType const p_EventType)      { m_Event = p_EventType;         }
        void SetIsJumping(bool const p_Jumping)             { m_Jumping = p_Jumping;         }
        void SetLogout(bool const p_LoggedOut)              { m_LoggingOut = p_LoggedOut;    }
        void SetIsRepairing(bool const p_Repairing)         { m_Repairing = p_Repairing;     }
        void SetUpdateEvent(bool const p_UpdateEvent)       { m_UpdateEvent = p_UpdateEvent; }
        void UpdateCredits(int32 const p_Credits)
        {
            if (p_Credits < 0 && m_Credits < abs(p_Credits))
				m_Credits = 0;
			else
				m_Credits += p_Credits;   
        }
        void UpdateUridium(int32 const p_Uridium)
        {
            if (p_Uridium < 0 && m_Uridium < abs(p_Uridium))
                m_Uridium = 0;
            else
                m_Uridium += p_Uridium;
        }
        void UpdateExperience(uint32 p_Experience);
        void UpdateDroneExperience(Entity::Object* p_Object);
        void SetBatteryAmmo(BatteryType const p_BatteryType, const int32 p_Amount);
        void SetRocketAmmo(RocketType const p_RocketType, const int32 p_Amount);
        void SetMine(const int32 p_Amount);
        void UpdateHonor(uint32 p_Honour);
        void Repair(bool p_Repair);
        void SetCloaked(bool const p_Cloaked) { m_IsCloaked = p_Cloaked; }

        uint32 CalculateSmartBombs() const;
        uint32 CalculateInstantShield() const;

        /// Reward Credits
        /// This will also send the reward to the client
        void RewardCredits(int32 p_Credits);
        /// Reward Uridium
        /// This will also send the reward to the client
        void RewardUridium(int32 p_Uridium);
        /// Reward Honor
        /// This will also send the reward to the client
        void RewardHonor(int32 p_Honor);
        /// Reward Experience
        /// This will also send the reward to the client and also the level up packet if the player levels up
        void RewardExperience(int32 p_Experience);

        void ToggleCloak();

        /// Timers
        Core::Diagnostic::IntervalTimer IntervalLogout;
        Core::Diagnostic::IntervalTimer ConfigTimer;

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
        uint16 m_Preset;
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
        bool m_Repairing;
        bool m_UpdateEvent;
        bool m_IsCloaked;

        bool m_LoggedIn;
        bool m_Jumping;
        bool m_LoggingOut;
        EventType m_Event;
        Ammo m_Ammo;
        Inventory m_Inventory;
        std::vector<Booster> m_Boosters;
        Core::Diagnostic::IntervalTimer m_RepairBotTimer;
        Core::Diagnostic::IntervalTimer m_IntervalNextSave;              ///< Save to database
        Core::Diagnostic::IntervalTimer m_IntervalRadiation;             ///< Save to database
        Core::Diagnostic::IntervalTimer m_IntervalQuestUpdate;           ///< Update Quests

        std::vector<LabUpgrade> m_LabUpgrades;                           ///< Lab Upgrades
        std::vector<Drone> m_Drones;                                     ///< Ship Drones
        QuestMap m_Quests;                                               ///< Quests
        SurroundingMap m_Surroundings;                                   ///< Objects surrounding player
        std::shared_ptr<Server::GameSocket> m_Socket;                    ///< Socket
        Core::Database::OperatorProcessor m_OperatorProcessor;           ///< Process Asynchronous Queries
        Core::Utils::LockedQueue<Server::ClientPacket*> m_RecievedQueue; ///< Packets
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone