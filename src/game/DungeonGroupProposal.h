#ifndef MANGOS_DUNGEON_PROPOSAL_H
#define MANGOS_DUNGEON_PROPOSAL_H

#include "DungeonShared.h"

namespace Dungeon
{
    class GroupProposal
    {
    public:
        GroupProposal();
        virtual ~GroupProposal();
        
        /** 
         * Checks if we can add the given \ref Player to this proposition, ie the levels
         * match, they have unlocked/locked the same dungeons and such. The exact requirements
         * for them to be able to be together are unknown atm.
         * \todo Add documentation on what needs to match for it to be a fit
         * @param pInfo The possible addition to the group
         * @return true if we can add this person to the proposition, false otherwise
         */
        bool Fits(const PlayerInfo* pInfo) const;

        /** 
         * Checks if we can create a group with the current members, if we can we should do that
         * and remove this \ref GroupProposal
         * @return true if we have 5 unique members.
         */
        bool CanCreateFullGroup() const;
        
        const inline PlayerInfoList& GetPossibleHealers() const { return m_healers; };
        const inline PlayerInfoList& GetPossibleTanks() const { return m_tanks; };
        const inline PlayerInfoList& GetPossibleDpsers() const { return m_dpsers; };
        
        void AddPlayer(PlayerInfo* pInfo);
    private:
        /**
         * Unique players, no duplicates as the m_tanks,m_healers and m_dpsers may have.
         */
        PlayerInfoList m_players;
        PlayerInfoList m_tanks; ///< All possible tanks, may have duplicates in m_healers and m_dpsers
        PlayerInfoList m_healers; ///< All possible healers, may have duplicates in m_tanks and m_dpsers
        PlayerInfoList m_dpsers; ///< All possible dpsers, may have duplicates in m_tanks and m_healers
    };
}    

#endif // MANGOS_DUNGEON_PROPOSAL_H
