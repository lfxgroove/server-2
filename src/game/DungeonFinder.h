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
#include "DungeonShared.h"
#include "Dungeon.h"
#include "DungeonGroupProposal.h"

/**
 * \addtogroup dungeonfinder
 * @{
 * \file
 */
namespace Dungeon
{
    /**
     * The base class that's used for finding people a group of other people to be able to do a
     * dungeon together. To understand the idea for this class and the classes related to this
     * one please read a little bit about the dungeon finding system that was introduced in WotLK.
     * 
     * The idea with this class and the ones related to it is that the methods that have a Create
     * somewhere in their name and give back a pointer to a object will generally take care of
     * deallocating the memory allocated, if not it will be noted in the functions documentation.
     */
    class Finder : public MaNGOS::Singleton<Finder>
    {
    public:
        Finder();
        virtual ~Finder();

        /** 
         * Get's the current \ref Dungeon::PlayerInfo for a \ref Player
         * @param pPlayer the \ref Player we want to find the association for
         * @return a pointer to the current \ref Dungeon::PlayerInfo associated to the \ref Player,
         * if there's no association null is returned
         */
        PlayerInfo* GetPlayerInfo(Player* pPlayer);
        /** 
         * Tries to add a \ref Player to the dungeon finder queue, if it fails for some
         * reason we currently dont get to know that reason, one of them may be that they
         * tried to queue as dps, tank and healer at the same time which is not possible?
         * @param pPlayer the player to try to add to the queue
         * @return false if the player wasn't added to the queue, true otherwise
         */
        bool AddToQueue(PlayerInfo* pPlayer);
        
        void ProposalAccepted(Player* pPlayer);
        void ProposalRejected(Player* pPlayer);
        
        void StartVoteToBoot(PlayerInfo* pPlayer);
        void BootPlayer(PlayerInfo* pPlayer);
        
        //All times in ms. ADD DOCUMENTATION
        uint32 GetAvgWaitTime() const;
        uint32 GetDpsWaitTime() const;
        uint32 GetHealerWaitTime() const;
        uint32 GetTankWaitTime() const;
        
        DungeonList GetAvailableDungeonsForPlayer(Player* pPlayer) const;
        
        /** 
         * This will go through all \ref Player s currently in the dungeon finder queue and
         * send them updates about waiting time etc, if they've found a matching group and such.
         * @param diff the time difference since last call to this method in ms
         */
        void Update(time_t diff);
        
        /** 
         * Initializes the \ref Finder for dungeons, fills the dungeon list with some data from
         * the database and such.
         */
        void Init();
        
        //Perhaps add a reloadDb()?
    private:
        /** 
         * Creates a struct with information needed when trying to find a group, if the
         * sent in player is null nothing is created.
         *
         * Do note that the given pointer is managed by \ref Dungeon::Finder and hence doesn't
         * need to be deleted by you.
         * @param pPlayer the player you want to associate with the newly created info
         * @return a pointer to the object if it was successfully created, null otherwise
         */
        PlayerInfo* CreatePlayerInfo(Player* pPlayer);
        
        /** 
         * Sends update packets to the players in the given \ref Dungeon::GroupProposal, these
         * include: Opcodes::SMSG_LFG_UPDATE_SEARCH, \ref Opcodes::SMSG_LFG_UPDATE_PLAYER,
         * \ref Opcodes::SMSG_LFG_UPDATE_GROUP, \ref Opcodes::SMSG_LFG_QUEUE_STATUS
         * \todo Research which opcodes needs to be sent this often and which ones don't.
         * @param pProp the proposal we would like to send updates about and too
         */
        void SendUpdates(const GroupProposal* pProp);
        void SendProposalToGroup(const GroupProposal* pProp);
        
        Group* CreateGroup();
        void DisbandGroup(Group* pGroup);

        void ReadyCheck(Group* pGroup);
        
        /** 
         * Calculates a \ref Player s average item level and locks the given lock if it's too
         * low or too high (not used yet) for the given \ref Dungeon.
         * @param pPlayer the \ref Player you want to check the item level for
         * @param dungeon the \ref Dungeon we want to check if we're locked against
         * @param lock the \ref Dungeon::DungeonLock that should be locked with either:
         * - \ref LockReasons::LOCKED_GEAR_TOO_LOW
         * - \ref LockReasons::LOCKED_GEAR_TOO_HIGH
         * \todo This needs some more info, we don't know what limits there were for the itemlevels
         */
        void CalcAvgItemLevelAndLock(Player* pPlayer, const Dungeon* dungeon, DungeonLock& lock) const;
        /** 
         * Populates the \ref Dungeon::PlayerInfo::GetCanQueueFor with the dungeons that are locked
         * for the given \ref PlayerInfo
         * @param pInfo the \ref PlayerInfo to use when populating the locked dungeons
         * \see DungeonFinderLockReasons
         */
        void PopulateLockedDungeons(PlayerInfo* pInfo) const;
        /** 
         * Intersects the \ref Dungeon::PlayerInfo::GetWishQueueFor and
         * \ref Dungeon::PlayerInfo::GetLockedDungeons to create a new vector which holds all the
         * instances that this \ref Dungeon::PlayerInfo can actually queue for
         * @param pInfo the \ref Dungeon::PlayerInfo to use when intersecting
         * @return a vector of the dungeons that this \ref Player can actually queue for
         */
        const DungeonEntryVector& FindDungeonsCanQueueFor(PlayerInfo* pInfo) const;
        
        /** 
         * Checks that the given \ref Player only has queued as something that his class can queue
         * as, ie: a shaman can only heal or dps, not tank. This function makes sure that even if
         * he tried to queue as that we'll just remove that flag when queueing him for real.
         *
         * \todo This might lead to stray people in the queue making the memory come to an end if
         * it goes on forever, ie: a shaman tries to queue as tank only and that role is removed ->
         * possibly problem since he won't match any \ref GroupProposal at all :(
         * @param pInfo info about the \ref Player that's queued
         */
        void CheckAndChangeRoles(PlayerInfo* pInfo);
        
        PlayerInfoList m_queue;
        // PlayerInfoList m_dpsers;
        // PlayerInfoList m_healers;
        // PlayerInfoList m_tankers;
        PlayerInfoList m_grouped;
        GroupProposalList m_groupProposals;
        DungeonMap m_allDungeons; ///< These are loaded on server start
        /**
         * This one is filled with data from the db when we start mangosd, after that it's continuosly
         * updated to keep track of what instances each player has locked for some reason. And also
         * saved back to db when it happens.
         * \todo Add the database things for this
         * \todo This is probably already implemented for raids, merge with that instead!
         */
        LockedDungeonMap m_lockedDungeons; //This will be removed
        PlayerInfoMap m_playerInfoMap;
        
        uint32 m_avgWaitTime; ///< Average wait time of the three, dps, tank and healer, in ms.
        uint32 m_dpsWaitTime; ///< Average wait time for dpsers, in ms.
        uint32 m_tankWaitTime; ///< Average wait time for tanks, in ms.
        uint32 m_healerWaitTime; ///< Average wait time for healers, in ms.

        /**
         * The idea of grouped here means that we went from queueing to forming an actual group,
         * measured in ms.
         */
        uint32 m_groupedSinceUpdate; 
        uint32 m_groupedDpsSinceUpdate;
        uint32 m_groupedHealersSinceUpdate;
        uint32 m_groupedTanksSinceUpdate;
        time_t m_lastUpdate;
    };
};

/// Convenience access to the instance of the \ref Dungeon::Finder
#define sDungeonFinder MaNGOS::Singleton<Dungeon::Finder>::Instance()

/** @} */

#endif
