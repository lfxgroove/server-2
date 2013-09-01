#include "DungeonFinder.h"

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
    
    Finder::Init()
    {
        uint32 numDungeons = sLFGDungeonStore.GetNumRows();
        const LFGDungeonEntry* dungeon;
    
        BarGoLink bar(numDungeons);
    
        for (uint32 i = 0; i < numDungeons; ++i)
        {
            bar.step();
        
            dungeonDBC = sLFGDungeonStore.LookupEntry(i);
            if (!dungeonDBC)
                continue;
        
            QueryResult* result = WorldDatabase.Query(
                "SELECT item_id, how_many, unk "
                "FROM dungeon_rewards "
                "WHERE dungeon_id = %d",
                dungeonDBC->Id);
        
            if (!result)
            {
                sLog.outError("Couldn't find any rewards for dungeon %d", dungeonDBC->Id);
                continue;
            }
        
            Dungeon* dungeon = new Dungeon(dungeonDBC->Id, dungeonDBC);
        
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
        
            m_AllDungeons.push_back(dungeon);
        }
    
        sLog.outString();
        sLog.outString(">> Loaded " SIZEFMTD " dungeons and their rewards", numDungeons);
    }

    void Finder::AddToQueue(PlayerInfo* pInfo)
    {
        if (pInfo->CanTank())
            m_tankers.push_back(pInfo);

        if (pInfo->CanHeal())
            m_healers.push_back(pInfo);

        if (pInfo->CanDps())
            m_dpsers.push_back(pInfo);
    
    }
    
    void Finder::Update()
    {
        time_t currTime = time();

        for (GroupProposalList::iterator it = m_groupProposals.begin();
             it != m_groupProposals.end();
             ++it)
        {
            GroupProposal* pProp = *it;
            if (pProp->CanCreateFullGroup())
            {
                //Off we go more to do here
                m_groupProposals.erase(it);
                delete pProp;
            }
        }
        
        // for (PlayerInfoList::iterator it = m_queue.begin();
        //      it != m_queue.end();
        //      ++it)
        // {
        //     PlayerInfo* pInfo = *it;
        //     GroupProposal* pProp = pInfo->proposal;
        //     if (!pProp)
        //         pProp = new GroupProposal;
            
        //     if (!pProp->CanCreateFullGroup())
        //         pProp->AddPlayer(pInfo);
        // }
    
    }
    
    // void Finder::FindRole(DungeonFinderRoles role, GroupProposal* proposal)
    // {
    //     PlayerListInfo::iterator it, end;
    //     switch (role)
    //     {
    //     case ROLE_DPS:
    //         it = m_dpsers.begin();
    //         end = m_dpsers.end();
    //         break;
    //     case ROLE_HEAL:
    //         it = m_healers.begin();
    //         end = m_healers.end();
    //         break;
    //     case ROLE_TANK:
    //         it = m_tanks.begin();
    //         end = m_tanks.end();
    //         break;
    //     }
    //     for (it; it != end; ++it)
    //     {
    //         PlayerInfo* pInfo = *it;
    //         if (!pInfo->HasProposal() && pInfo->Fits(proposal))
    //         {
    //             pInfo->pGroupProposal = proposal;
    //             proposal->AddPlayer(role, pInfo);
    //         }
    //     }
    // }
    
    bool Finder::AddPlayerToQueue(PlayerInfo* pInfo)
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
        m_groupProposals.push_back(pInfo);
        return true;
    }
}
