/*
 * This code is part of MaNGOS. Contributor & Copyright details are in AUTHORS/THANKS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MANGOS_DUNGEON_FINDER_H
#define MANGOS_DUNGEON_FINDER_H

#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"

namespace Dungeon
{
    /**
     * \todo More documentation here!
     * 
     * There's either a dungeon group, ie: the normal dungeon finder you always think of and that
     * is represented by the word queue, then there's also a raid finder, which is represented by the
     * word list. This is how it was in the client. So, if we're removed from the dungeon finder queue
     * the following update code is used:
     * \code{.cpp}
     * DungeonFinderUpdateCodes::DUNGEON_FINDER_UPDATE_REMOVED_FROM_QUEUE
     * \endcode
     * But if we're removed from the raid finders list of people it would instead be:
     * \code{.cpp}
     * DungeonFinderUpdateCodes::DUNGEON_FINDER_UPDATE_REMOVED_FROM_LIST
     * \endcode
     */
    enum DungeonFinderUpdateCodes
    {
        // DUNGEON_FINDER_UPDATE_NOTHING = 0,
        DUNGEON_FINDER_UPDATE_REMOVED_FROM_QUEUE = 7,
        DUNGEON_FINDER_UPDATE_REMOVED_FROM_LIST = 15,
    };
    
    /**
     * These are the different roles you can apply for when joining the dungeon finder
     * queue, the client makes some checks so that a mage can't join as dps etc. But this is
     * (read will be) enforced on server side aswell.
     */
    enum DungeonFinderRoles
    {
        ROLE_LEAD = 0x1,
        ROLE_TANK = 0x2,
        ROLE_HEAL = 0x4,
        ROLE_DPS  = 0x8,
    };

    /**
     * A DungeonId is only the dungeon's id without any type information. See \ref DungeonEntry
     */
    typedef DungeonId uint32;
    /**
     * An DungeonEntry is the same as the dungeon's id and it's type in the same int, ie:
     * \code{.cpp}
     * //To create the entry
     * DungeonEntry entry = dungeonId & (dungeonType << 24);
     * //To get an id:
     * DungeonId id = entry & 0xFFFFFF; //Lowest 3 bytes
     * \endcode
     * \see LFGDungeonEntry::TypeFromEntry
     * \see LFGDungeonEntry::IdFromEntry
     */
    typedef DungeonEntry uint32;
    typedef std::list<Reward> RewardList;
    typedef std::list<Dungeon*> DungeonList;
    typedef std::list<Player*> PlayerList;
    
    struct PlayerInfo
    {
        DungeonFinderRoles roles;
        Player* pPlayer;
        PlayerList proposedPlayers;
        // Group* pGroup;
        
        std::vector<DungeonId> canQueueFor;
        std::vector<DungeonId> wishToQueueFor;
        std::vector<DungeonId> isQueuedFor;
        
        bool CanTank() const { return roles & ROLE_TANK; };
        bool CanHeal() const { return roles & ROLE_HEAL; };
        bool CanDps() const { return roles & ROLE_DPS; };
        bool CanLead() const { return roles & ROLE_LEAD; };
    };
    
    struct Reward
    {
        DungeonId dungeonId; ///< id of the dungeon this reward is associated with
        uint32 itemId; ///< Id of the reward item, see item_template database table
        uint32 howMany; ///< How many of this reward we will receive upon completion
        uint32 unk; ///< This isn't used yet, but will be sent, perhaps is modelId?
    };
    
    class Dungeon
    {
    public:
        Dungeon(DungeonId id, const LFGDungeonEntry* dungeonDBC);
        virtual ~Dungeon();
        
        RewardList GetRewards() const;
        uint32 GetMoneyRewardConstant() const;
        uint32 GetMoneyRewardVariable() const;
        uint32 GetExpRewardConstant() const;
        uint32 GetExpRewardVariable() const;

        void AddReward(const Reward& rew);
        
        DungeonEntry Entry() const { return dungeonEntry->Entry(); };
    private:
        const LFGDungeonEntry* dungeonEntry;
        RewardList m_Rewards; ///< Rewards we will receive upon completing the dungeon
    };
    
    class Finder : public MaNGOS::Singleton<DungeonFinder>
    {
    public:
        Finder();
        virtual ~Finder();
        
        PlayerInfo* CreatePlayerInfo();
        const PlayerInfo* GetPlayerInfo(Player* pPlayer) const;
        void AddPlayerToQueue(PlayerInfo* pPlayer);
        
        void StartVoteToBoot(PlayerInfo* pPlayer);
        void BootPlayer(PlayerInfo* pPlayer);
        
        //All times in ms. ADD DOCUMENTATION
        uint32 GetAvgWaitTime() const;
        uint32 GetDpsWaitTime() const;
        uint32 GetHealWaitTime() const;
        uint32 GetTankWaitTime() const;
        
        DungeonList GetAvailableDungeonsForPlayer(Player* pPlayer) const;
        
        /** 
         * This will go through all \ref Player s currently in the dungeon finder queue and
         * send them updates about waiting time etc, if they've found a matching group and such.
         */
        void Update();
        
        /** 
         * Initializes the \ref Finder for dungeons, fills the dungeon list with some data from
         * the database and such.
         */
        void Init();
        
        //Perhaps add a reloadDb()?
    private:
        Group* CreateGroup();
        void DisbandGroup(Group* pGroup);
        
        void ReadyCheck(Group* pGroup);
        
        std::list<PlayerInfo> m_Queue;
        DungeonList m_AllDungeons; ///< These are loaded on server start
    };
};

#define sDungeonFinder MaNGOS::Singleton<DungeonFinder>::Instance()

#endif
