#include "DungeonGroupProposal.h"

namespace Dungeon
{
    bool GroupProposal::Fits(const PlayerInfo* pInfo)
    {
        if (m_players.size() == 0)
            return true;
        
        //3 basic checks to not overfill on some role
        if (m_dpsers.size() >= 3
            && !pInfo->CanTank() && !pInfo->CanHeal())
                return false;
        
        if (m_healers.size() >= 1
            && !pInfo->CanTank() && !pInfo->CanDps())
            return false;
        
        if (m_tanks.size() >= 1
            && !pInfo->CanHeal() && !pInfo->CanDps())
            return false;

        //If we haven't queud for anything at all when calling this one something is off
        MANGOS_ASSERT(m_wishedDungeons.size() > 0);
        //Check if we want to queue for the same dungeons at all etc.
        //Should these vectors be sorted to allow earlier termination?
        DungeonEntryVector newWish;
        for (DungeonEntryVector::const_iterator i = m_wishedDungeons.begin();
             i != m_wishedDungeons.end();
             ++i)
        {
            for (DungeonEntryVector::const_iterator j = pInfo->GetCanQueueFor().begin();
                 j != pInfo->GetCanQueueFor().end();
                 ++j)
            {
                if (*i == *j)
                    newWish.push_back(*i);
            }
        }
        if (newWish.size() == 0)
            return false;
        
        m_wishedDungeons = newWish; //Perhaps do a swap() instead?
        return true;
        //Very basic, not finished at all
        // PlayerInfo* pInfoCheck = *(m_players.begin());
        // if (pInfo->GetPlayer()->getLevel() - pInfoCheck->GetPlayer()->getLevel() <= 10)
        //     return true;
        // else
        //     return false;
    }
    
    bool GroupProposal::CanCreateFullGroup() const
    {
        return m_players.size() == 5;
    }

    uint8 GroupProposal::NumPlayers() const
    {
        return m_players.size();
    }
    
    void GroupProposal::AddPlayer(PlayerInfo* pInfo) //add const to the pointer?
    {
        //Add the player to all appropriate slots
        if (pInfo->CanDps())
            m_dpsers.push_back(pInfo);
        if (pInfo->CanHeal())
            m_healers.push_back(pInfo);
        if (pInfo->CanTank())
            m_tanks.push_back(pInfo);
        m_players.push_back(pInfo);
        
        FindMatchingDungeons(pInfo->GetWishQueueFor());
    }
    
    const DungeonList& GroupProposal::GetQueuedDungeons() const
    {
        //todo: implement
    }
    
    void GroupProposal::FindMatchingDungeons(const DungeonIdVector& matchWith)
    {
        
    }
    
    uint8 GroupProposal::GetNeededTanks() const
    {
        return GetNeededByMax(MAX_TANKS, m_tanks.size());
    }
    
    uint8 GroupProposal::GetNeededHealers() const
    {
        return GetNeededByMax(MAX_HEALERS, m_healers.size());
    }
    
    uint8 GroupProposal::GetNeededDpsers() const
    {
        return GetNeededByMax(MAX_DPS, m_dpsers.size());
    }

    uint8 GroupProposal::GetNeededByMax(uint8 max, uint8 currSize) const
    {
        if (currSize > max)
            return 0;
        else
            return max - currSize;
    }
    
    GroupProposal::GroupProposal()
        : m_acceptedCount(0)
    {}
    
    GroupProposal::~GroupProposal()
    {}
}
