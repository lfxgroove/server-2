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
        
        static const uint8 MAX_TANKS = 1; ///< Max number of tanks in a group
        static const uint8 MAX_HEALERS = 1; ///< Max number of healers in a group
        static const uint8 MAX_DPS = 3; ///< Max number of damagers in a group
        
        /** 
         * Checks if we can add the given \ref Player to this proposition, ie the levels
         * match, they have unlocked/locked the same dungeons and such. The exact requirements
         * for them to be able to be together are unknown atm.
         * \todo Add documentation on what needs to match for it to be a fit
         * @param pInfo The possible addition to the group
         * @return true if we can add this person to the proposition, false otherwise
         */
        bool Fits(const PlayerInfo* pInfo);
        
        /** 
         * Checks if we can create a group with the current members, if we can we should do that
         * and remove this \ref GroupProposal
         * @return true if we have 5 unique members.
         */
        bool CanCreateFullGroup() const;
        uint8 NumPlayers() const;
        
        const inline PlayerInfoList& GetPossibleTanks() const { return m_tanks; };
        const inline PlayerInfoList& GetPossibleHealers() const { return m_healers; };
        const inline PlayerInfoList& GetPossibleDpsers() const { return m_dpsers; };
        const inline PlayerInfoList& GetUniquePlayers() const { return m_players; };
        
        uint8 GetNeededTanks() const;
        uint8 GetNeededHealers() const;
        uint8 GetNeededDpsers() const;
        
        inline uint8 GetAcceptedCount() { return m_acceptedCount; };
        inline void IncreaseAcceptedCount() { ++m_acceptedCount; };
        
        const DungeonList& GetQueuedDungeons() const;
        
        /** 
         * Adds a \ref Player and his/her info to this proposal
         * @param pInfo the info to add
         */
        void AddPlayer(PlayerInfo* pInfo);

    private:
        /** 
         * Finds the \ref Dungeon s that match with all the others in this proposal, ie:
         * - player 1 wishes to do dungeon 1, 2, 5
         * - player 2 wishes to do dungeon 1, 3, 4, 5
         * This would update this \ref GroupProposal s internal list to have the dungeons
         * 1 and 5.
         *
         * Only use this if you've gotten a true from \ref GroupProposal::Fits otherwise
         * a empty list will be created and this proposal won't ever finish.
         * @param matchWith the dungeons we would like to match our internal list with.
         */
        void FindMatchingDungeons(const DungeonIdVector& matchWith);
        
        /** 
         * Used by \ref GroupProposal::GetNeededTanks, \ref GroupProposal::GetNeededHealers and
         * \ref GroupProposal::GetNeededDpsers as a helper function. It returns how many more of
         * a role we need given the max we can have of the role and how many of that role we currently
         * have. Example use:
         * \code{.cpp}
         * GetNeededByMax(MAX_TANKS, m_tanks.size());
         * \endcode
         * @param max the max we can have of this role
         * @param currSize the current number of this role we have
         * @return how many more of this role we need
         */
        uint8 GetNeededByMax(uint8 max, uint8 currSize) const;
        /**
         * Unique players, no duplicates as the m_tanks,m_healers and m_dpsers may have.
         */
        PlayerInfoList m_players;
        PlayerInfoList m_tanks; ///< All possible tanks, may have duplicates in m_healers and m_dpsers
        PlayerInfoList m_healers; ///< All possible healers, may have duplicates in m_tanks and m_dpsers
        PlayerInfoList m_dpsers; ///< All possible dpsers, may have duplicates in m_tanks and m_healers
        
        ProposalState m_state; ///< The current state of this group, see \ref ProposalState

        //Sort this one?
        DungeonEntryVector m_wishedDungeons; ///< Dungeons that everyone in the group would like to do
        
        uint8 m_acceptedCount; ///< How many people have accepted our proposal?
    };
}    

/** @} */

#endif // MANGOS_DUNGEON_PROPOSAL_H
