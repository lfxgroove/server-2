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

namespace Dungeon
{
    class Finder : public MaNGOS::Singleton<Finder>
    {
    public:
        Finder();
        virtual ~Finder();
        
        PlayerInfo* CreatePlayerInfo();
        const PlayerInfo* GetPlayerInfo(Player* pPlayer) const;
        /** 
         * Tries to add a \ref Player to the dungeon finder queue, if it fails for some
         * reason we currently dont get to know that reason, one of them may be that they
         * tried to queue as dps, tank and healer at the same time which is not possible?
         * @param pPlayer the player to try to add to the queue
         * @return false if the player wasn't added to the queue, true otherwise
         */
        bool AddToQueue(PlayerInfo* pPlayer);
        
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
        
        PlayerInfoList m_queue;
        // PlayerInfoList m_dpsers;
        // PlayerInfoList m_healers;
        // PlayerInfoList m_tankers;
        PlayerInfoList m_grouped;
        GroupProposalList m_groupProposals;
        DungeonList m_allDungeons; ///< These are loaded on server start
        
        time_t m_lastUpdate;
    };
};

#define sDungeonFinder MaNGOS::Singleton<DungeonFinder>::Instance()

#endif
