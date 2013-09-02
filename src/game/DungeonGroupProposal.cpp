#include "DungeonGroupProposal.h"

namespace Dungeon
{
    bool GroupProposal::Fits(const PlayerInfo* pInfo) const
    {
        if (m_players.size() == 0)
            return true;
        
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
