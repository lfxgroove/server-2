#ifndef MANGOS_DUNGEON_SHARED_H
#define MANGOS_DUNGEON_SHARED_H

#include "SharedDefines.h"
#include "Common.h"
#include "Player.h"

/**
 * \addtogroup dungeonfinder
 * @{
 * \file
 */

namespace Dungeon
{
    /**
     * \todo More documentation here!
     * 
     * There's either a dungeon group, ie: the normal dungeon finder you always think of and that
     * is represented by the word queue, then there's also a raid finder, which is represented by the
     * word list. This is how it was in the client. So, if we're removed from the dungeon finder queue
     * the following update code is used:
     * \code{.cpp}
     * DungeonFinderUpdateCodes::DUNGEON_FINDER_UPDATE_REMOVED_FROM_QUEUE
     * \endcode
     * But if we're removed from the raid finders list of people it would instead be:
     * \code{.cpp}
     * DungeonFinderUpdateCodes::DUNGEON_FINDER_UPDATE_REMOVED_FROM_LIST
     * \endcode
     */
    enum DungeonFinderUpdateCodes
    {
        // DUNGEON_FINDER_UPDATE_NOTHING = 0,
        DUNGEON_FINDER_UPDATE_REMOVED_FROM_QUEUE = 7,
        DUNGEON_FINDER_UPDATE_REMOVED_FROM_LIST = 15,
    };
    
    /**
     * These are the different roles you can apply for when joining the dungeon finder
     * queue, the client makes some checks so that a mage can't join as dps etc. But this is
     * (read will be) enforced on server side aswell. This is used as a bitflag, so to queue
     * as a possible leader/dungeon guide and tank you would use:
     * \code{.cpp}
     * DungeonFinderRoles tankAndLeader = ROLE_LEAD | ROLE_TANK;
     * \endcode
     */
    enum DungeonFinderRoles
    {
        /**
         * Indicates that the \ref Player has some experience with this dungeon and can be
         * the dungeon leader. */
        ROLE_LEAD = 0x1,
        ROLE_TANK = 0x2, ///< Indicates that the \ref Player can tank the instance
        ROLE_HEAL = 0x4, ///< Indicates that the \ref Player can be a healer in the instance
        ROLE_DPS  = 0x8, ///< Indicates that the \ref Player can be a damage dealer in the instance
    };

    /**
     * This decides what state a \ref Dungeon::GroupProposal has, when we've reached the last one
     * (\ref Dungeon::GroupProposal::WAITING_FOR_CREATE) we just want to create a group and remove
     * the \ref Dungeon::GroupProposal. See \ref Dungeon::GroupProposal for more info.
     */
    enum ProposalState
    {
        WAITING_FOR_PLAYERS, ///< We still don't have a possibility to create a full group
        WAITING_FOR_ACCEPT, ///< We've proposed the group to all \ref Player, waiting for them to accept
        WAITING_FOR_CREATE ///< Waiting to be created so we can remove this \ref Dungeon::GroupProposal
    };
    
    /**
     * A DungeonId is only the dungeon's id without any type information. See \ref DungeonEntry
     */
    typedef uint32 DungeonId;
    /**
     * An DungeonEntry is the same as the dungeon's id and it's type in the same int, ie:
     * \code{.cpp}
     * //To create the entry
     * DungeonEntry entry = dungeonId & (dungeonType << 24);
     * //To get an id:
     * DungeonId id = entry & 0xFFFFFF; //Lowest 3 bytes
     * \endcode
     * \see LFGDungeonEntry::TypeFromEntry
     * \see LFGDungeonEntry::IdFromEntry
     */
    typedef uint32 DungeonEntry;
    
    struct PlayerInfo;
    struct Reward;
    class GroupProposal;
    class Dungeon;
    
    typedef std::list<Reward*> RewardList;
    typedef std::list<Dungeon*> DungeonList;
    typedef std::map<DungeonId, Dungeon*> DungeonMap;
    typedef std::list<Player*> PlayerList;
    typedef std::vector<DungeonId> DungeonIdVector;
    typedef std::vector<DungeonEntry> DungeonEntryVector;
    typedef std::list<PlayerInfo*> PlayerInfoList;
    typedef std::list<GroupProposal*> GroupProposalList;
    typedef std::map<Player*, DungeonList> LockedDungeonMap;
    typedef std::map<Player*, PlayerInfo*> PlayerInfoMap;
    
    /**
     * Keeps information related to finding a group and what the associated \ref Player can do
     * in terms of dps, heal or tank.
     */
    struct PlayerInfo
    {
        inline DungeonFinderRoles GetRoles() const { return roles; };
        inline Player* GetPlayer() const { return pPlayer; };
        inline GroupProposal* GetProposal() const { return pGroupProposal; };
        inline uint32 GetWaitTime() const { return myWaitTime; };
        inline const std::string& GetComment() const { return comment; };
        inline bool HasAcceptedProposal() const { return acceptedProposal; };
        inline const DungeonEntryVector& GetCanQueueFor() const { return canQueueFor; };
        inline const DungeonEntryVector& GetWishQueueFor() const { return wishToQueueFor; };
        inline const DungeonEntryVector& GetIsQueuedFor() const { return isQueuedFor; };
        
        inline void SetRoles(DungeonFinderRoles newRoles) { roles = newRoles; };
        inline void SetComment(const std::string& newComment) { comment = newComment; };
        inline void SetAcceptedProposal(bool accepted) { acceptedProposal = accepted; };
        inline void SetPlayer(Player* player) { pPlayer = player; };
        inline void SetWishesToQueueFor(DungeonEntryVector& dungeons) { wishToQueueFor = dungeons; };
        
        bool CanTank() const { return roles & ROLE_TANK; };
        bool CanHeal() const { return roles & ROLE_HEAL; };
        bool CanDps() const { return roles & ROLE_DPS; };
        bool CanLead() const { return roles & ROLE_LEAD; };
        
    private:
        DungeonFinderRoles roles; ///< The roles that this \ref Player / \ref PlayerInfo can undertake.
        Player* pPlayer; ///< The \ref Player that this \ref PlayerInfo is associated with
        // Group* pGroup;
        GroupProposal* pGroupProposal; ///< The current proposal for this \ref PlayerInfo
        std::string comment; ///< Can be sent by the client, doesn't seem to be used there though
        uint32 myWaitTime; ///< Wait time that this \ref Player has waited in ms.
        bool acceptedProposal; ///< Whether or not we have accepted an ongoing proposal for a group
        
        DungeonEntryVector canQueueFor; ///< Dungeons that this \ref PlayerInfo can queue for
        DungeonEntryVector wishToQueueFor; ///< Dungeons that this \ref PlayerInfo wishes to queue for
        DungeonEntryVector isQueuedFor; ///< Dungeons that this \ref PlayerInfo is currently queued for
    };
    
    struct Reward
    {
        // DungeonId dungeonId; ///< id of the dungeon this reward is associated with
        uint32 itemId; ///< Id of the reward item, see item_template database table
        uint32 howMany; ///< How many of this reward we will receive upon completion
        uint32 unk; ///< This isn't used yet, but will be sent, perhaps is modelId?
    };
};

/** @} */

#endif
