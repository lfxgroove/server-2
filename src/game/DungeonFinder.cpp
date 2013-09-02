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
    
        sLog.outString();
        sLog.outString(">> Loaded " SIZEFMTD " dungeons and their rewards", numDungeons);
    }

    // void Finder::AddToQueue(PlayerInfo* pInfo)
    // {
    //     if (pInfo->CanTank())
    //         m_tankers.push_back(pInfo);

    //     if (pInfo->CanHeal())
    //         m_healers.push_back(pInfo);

    //     if (pInfo->CanDps())
    //         m_dpsers.push_back(pInfo);
        
    // }
    
    void Finder::Update()
    {
        for (GroupProposalList::iterator it = m_groupProposals.begin();
             it != m_groupProposals.end();
             ++it)
        {
            GroupProposal* pProp = *it;
            if (pProp->CanCreateFullGroup())
            {
                //Off we go more to do here
                //CreateGroup(); etcetc
                m_groupProposals.erase(it);
                delete pProp;
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
}
