#ifndef MANGOS_DUNGEON_PROPOSAL_H
#define MANGOS_DUNGEON_PROPOSAL_H

#include "DungeonShared.h"

/**
 * \addtogroup dungeonfinder
 * @{
 * \file
 */

namespace Dungeon
{
    /**
     * This class is used to keep track of which \ref Player s match which. This class is supposed
     * to take care of matching people to other people based on level, what they want to do etc via
     * the \ref Dungeon::GroupProposal::Fits method. And when a full group can be created this
     * class is no longer of any use. It also keeps tracks of how many healers/dps/tanks we have
     * that would be available for this certain group.
     *
     * What decides if we can create a group or not is the
     * \ref Dungeon::GroupProposal::CanCreateFullGroup function which just checks the unique player list.
     *
     * Some ideas in this class: we have three lists for the separate roles you can undertake, the leader
     * one isn't specially interesting as we'll have to assign someone to it, it isn't necessary
     * to create a group. Between these three lists we can have duplicates and such, ie:
     * - tanks: player 1, player 3
     * - healers: player 1, player 2
     * - dps: player 2, player 3, player 4, player 5
     * And we keep a list of all unique \ref Player s, ie:
     * - unique: player 1, player 2, player 3, player 4, player 5
     * When the unique list is filled we should have filled enough spots to be able to create a group,
     * this in turn is enforced by the \ref Dungeon::GroupProposal::Fits only adding people which
     * increase the possibility of creating a group, ie: if we have 3 dps we'll only add healers/tanks,
     * perhaps they've signed up as dps/healer and in that case we will have 4 dps but also a healer so
     * it works out.
     */
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
        
        ProposalState m_state; ///< The current state of this group, see \ref ProposalState
        
        DungeonIdVector m_wishedDungeons; ///< The dungeons that everyone in the group would like to do
    };
}    

/** @} */

#endif // MANGOS_DUNGEON_PROPOSAL_H
