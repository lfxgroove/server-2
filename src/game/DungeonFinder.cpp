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

#include "DungeonFinder.h"
#include "DBCStores.h"
#include "ProgressBar.h"

namespace Dungeon
{

    Finder::Finder()
    {}

    Finder::~Finder()
    {
        for (DungeonList::iterator it = m_allDungeons.begin();
             it != m_allDungeons.end();
             ++it)
        {
            delete *it;
        }

        for (GroupProposalList::iterator it = m_groupProposals.begin();
             it != m_groupProposals.end();
             ++it)
        {
            delete *it;
        }

        for (PlayerInfoMap::iterator it = m_playerInfoMap.begin();
             it != m_playerInfoMap.end();
             ++it)
        {
            delete it->second;
        }
    }
    
    void Finder::Init()
    {
        uint32 numDungeons = sLFGDungeonStore.GetNumRows();
        const LFGDungeonEntry* dungeonDBC;
        
        BarGoLink bar(numDungeons);
        
        for (uint32 i = 0; i < numDungeons; ++i)
        {
            bar.step();
        
            dungeonDBC = sLFGDungeonStore.LookupEntry(i);
            if (!dungeonDBC)
                continue;
        
            QueryResult* result = WorldDatabase.PQuery(
                "SELECT item_id, how_many, unk "
                "FROM dungeon_rewards "
                "WHERE dungeon_id = %d",
                dungeonDBC->ID);
        
            if (!result)
            {
                sLog.outError("Couldn't find any rewards for dungeon %d", dungeonDBC->ID);
                continue;
            }
        
            Dungeon* dungeon = new Dungeon(dungeonDBC);
        
            do
            {
                Field* fields = result->Fetch();
                Reward* reward = dungeon->CreateAndAddReward();
                reward->itemId = fields[0].GetUInt32();
                reward->howMany = fields[1].GetUInt32();
                reward->unk = fields[2].GetUInt32();
            }
            while(result->NextRow());
            delete result; //Is this needed or not?
        
            m_allDungeons.push_back(dungeon);
        }

        m_allDungeons.sort();
        
        sLog.outString();
        sLog.outString(">> Loaded " SIZEFMTD " dungeons and their rewards", numDungeons);
    }

    void Finder::ProposalAccepted(Player* pPlayer)
    {
        PlayerInfoMap::iterator it = m_playerInfoMap.find(pPlayer);
        if (it == m_playerInfoMap.end())
        {
            DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "Can't find player %u that accepted proposal",
                             pPlayer->GetObjectGuid());
            return;
        }
        
        PlayerInfo* pInfo = *it;
        pInfo->acceptedProposal = true;
        DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "Player %u just accepted the proposal to join the"
                         " proposal %x", pPlayer->GetObjectGuid(), pInfo->pGroupProposal);
        
        pInfo->pGroupProposal->IncreaseAcceptedCount();
        if (pInfo->pGroupProposal->GetAcceptedCount() == 5)
        {
            //actually create the group and remove the proposal
        }
    }

    void Finder::ProposalRejected(Player* pPlayer)
    {
        PlayerInfoMap::iterator it = m_playerInfoMap.find(pPlayer);
        if (it == m_playerInfoMap.end())
        {
            DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "Can't find player %u that rejected proposal",
                             pPlayer->GetObjectGuid());
            return;
        }
        
        PlayerInfo* pInfo = *it;
        //should this one be removed from queue in this case?
        pInfo->acceptedProposal = false;
        DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "Player %u just rejected the proposal to join the"
                         " proposal %x", pPlayer->GetObjectGuid(), pInfo->pGroupProposal);

        //More things should be done here, try to find a replacement from some of the other
        //current proposals and such.
    }
    
    void Finder::Update(time_t diff)
    {
        //We should update average wait times here aswell.
        
        for (GroupProposalList::iterator it = m_groupProposals.begin();
             it != m_groupProposals.end();
             ++it)
        {
            GroupProposal* pProp = *it;
            if (pProp->CanCreateFullGroup())
            {
                //Off we go more to do here
                //if the proposal goes well we will actually create the group
                SendProposalToGroup(pProp);
                m_groupProposals.erase(it); //Does this work or do i have to keep a iterator forwards aswell?
                delete pProp;
            }
            else
            {
                //Sends a few packets related to lfg 
                SendUpdates(pProp);
            }
        }
    }
    
    bool Finder::AddToQueue(PlayerInfo* pInfo)
    {
        //Add checks for dps classes not being able to queue as healer etc.
        if (pInfo->CanHeal() && pInfo->CanDps() && pInfo->CanTank())
            return false;
        
        m_queue.push_back(pInfo);
        for (GroupProposalList::iterator it = m_groupProposals.begin();
             it != m_groupProposals.end();
             ++it)
        {
            GroupProposal* pProp = *it;
            if (pProp->Fits(pInfo))
            {
                pProp->AddPlayer(pInfo);
                return true;
            }
        }
        
        //Since we don't have anyone in this proposal just yet this AddPlayer() will be okay
        GroupProposal* pProp = new GroupProposal();
        pProp->AddPlayer(pInfo);
        m_groupProposals.push_back(pProp);
        return true;
    }
    
    void Finder::SendProposalToGroup(const GroupProposal* pProp)
    {
        PlayerInfoList players = pProp->GetUniquePlayers();
        for (PlayerInfoList::const_iterator it = players.begin();
             it != players.end();
             ++it)
        {
            //TODO: Send SMSG_LFG_PROPOSAL_UPDATE
        }
    }
    
    void Finder::SendUpdates(const GroupProposal* pProp)
    {
        PlayerInfoList players = pProp->GetUniquePlayers();
        DungeonList queuedFor = pProp->GetQueuedDungeons();
        for (PlayerInfoList::const_iterator it = players.begin();
             it != players.end();
             ++it)
        {
            WorldSession* session = (*it)->pPlayer->GetSession();
            
            WorldPacket searchUpdate(SMSG_LFG_UPDATE_SEARCH);
            searchUpdate << uint8(0); //no data for now, this should be changed
            session->SendPacket(&searchUpdate);

            WorldPacket playerUpdate(SMSG_LFG_UPDATE_PLAYER);
            playerUpdate << uint8(0); //statusCode, TODO
            playerUpdate << uint8(1); //dataComing, TODO
            playerUpdate << uint8(1); //isQueued, always true in here
            playerUpdate << uint8(0); //hasJoined, always false in here
            playerUpdate << uint8(0); //unk, TODO
            //This part i'm still unsure about, what instances are actually sent here?
            playerUpdate << uint8(queuedFor.size()); //number of instances coming
            for (DungeonList::const_iterator itr = queuedFor.begin();
                 itr != queuedFor.end();
                 ++itr)
            {
                playerUpdate << uint32((*itr)->Entry());
            }
            playerUpdate << (*it)->comment;
            session->SendPacket(&playerUpdate);

            WorldPacket partyUpdate(SMSG_LFG_UPDATE_PARTY);
            //TODO
            
            WorldPacket queueStats(SMSG_LFG_QUEUE_STATUS);
            //commented out for compilation atm
            //queueStats << uint32(dungeonId); //which dungeon is this the wait time for (?)
            queueStats << uint32((*it)->myWaitTime); //myWait
            queueStats << uint32(GetAvgWaitTime()); //avg wait
            queueStats << uint32(GetTankWaitTime()); //tankWait
            queueStats << uint32(GetHealerWaitTime()); //healerWait
            queueStats << uint32(GetDpsWaitTime()); //dpsWait
            queueStats << uint8(pProp->GetNeededTanks()); //tankNeeds
            queueStats << uint8(pProp->GetNeededHealers()); //healNeeds
            queueStats << uint8(pProp->GetNeededDpsers()); //dpsNeeds
            queueStats << uint32(0); //queuedTime - used for what? isn't it equal to myWaitTime?
            session->SendPacket(&queueStats);
        }
    }
    
    PlayerInfo* Finder::CreatePlayerInfo(Player* pPlayer)
    {
        if (!pPlayer)
            return NULL;
        
        PlayerInfo* pPlayerInfo = new PlayerInfo();
        pPlayerInfo->pPlayer = pPlayer;
        std::pair<PlayerInfoMap::iterator, bool> insertion;
        insertion = m_playerInfoMap.insert(std::make_pair(pPlayer, pPlayerInfo));
        if (insertion.second)
            return pPlayerInfo;
        else
        {
            delete pPlayerInfo;
            return NULL;
        }
    }
    
    PlayerInfo* Finder::GetPlayerInfo(Player* pPlayer)
    {
        if (!pPlayer)
            return NULL;
        
        PlayerInfoMap::iterator it = m_playerInfoMap.find(pPlayer);
        if (it != m_playerInfoMap.end())
            return it->second;
        else
            return NULL;
    }

    uint32 Finder::GetAvgWaitTime() const
    {
        return m_avgWaitTime;
    }
    
    uint32 Finder::GetDpsWaitTime() const
    {
        return m_dpsWaitTime;
    }
    
    uint32 Finder::GetTankWaitTime() const
    {
        return m_tankWaitTime;
    }

    uint32 Finder::GetHealerWaitTime() const
    {
        return m_healerWaitTime;
    }
}
