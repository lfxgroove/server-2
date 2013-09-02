#include "DungeonGroupProposal.h"

namespace Dungeon
{
    bool GroupProposal::Fits(const PlayerInfo* pInfo) const
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

        //Check if we want to queue for the same dungeons at all etc.
        if (m_wishedDungeons.size() > 0)
        {
            //Sort these 2 lists?
            // for (DungeonIdVector::const_iterator i = m_wishedDungeons.begin();
            //      i != m_wishedDungeons.end();
            //      ++i)
            // {
            //     for (DungeonIdVector::const_iterator j = pInfo->wishToQueueFor.begin();
            //          j != pInfo->wishToQueueFor.end();
            //          ++j)
            //     {
            //         if (*j == *i)
            //             break;
            //     }
            // }
        }
        
        //Very basic, not finished at all
        PlayerInfo* pInfoCheck = *(m_players.begin());
        if (pInfo->pPlayer->getLevel() - pInfoCheck->pPlayer->getLevel() <= 10)
            return true;
        else
            return false;
    }
    
    bool GroupProposal::CanCreateFullGroup() const
    {
        return m_players.size() == 5;
    }
    
    void GroupProposal::AddPlayer(PlayerInfo* pInfo) //add const to the pointer?
    {
        if (pInfo->CanDps())
            m_dpsers.push_back(pInfo);
        if (pInfo->CanHeal())
            m_healers.push_back(pInfo);
        if (pInfo->CanTank())
            m_tanks.push_back(pInfo);
        m_players.push_back(pInfo);
    }
    
    GroupProposal::GroupProposal()
    {}
    
    GroupProposal::~GroupProposal()
    {}
}
