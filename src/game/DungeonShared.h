#ifndef MANGOS_DUNGEON_SHARED_H
#define MANGOS_DUNGEON_SHARED_H

#include "SharedDefines.h"
#include "Common.h"
#include "Player.h"

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
     * (read will be) enforced on server side aswell.
     */
    enum DungeonFinderRoles
    {
        ROLE_LEAD = 0x1,
        ROLE_TANK = 0x2,
        ROLE_HEAL = 0x4,
        ROLE_DPS  = 0x8,
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
    struct PlayerInfo;
    struct Reward;
    class GroupProposal;
    class Dungeon;
    
    typedef uint32 DungeonEntry;
    typedef std::list<Reward*> RewardList;
    typedef std::list<Dungeon*> DungeonList;
    typedef std::list<Player*> PlayerList;
    typedef std::vector<DungeonId> DungeonIdVector;
    typedef std::list<PlayerInfo*> PlayerInfoList;
    typedef std::list<GroupProposal*> GroupProposalList;
    
    struct PlayerInfo
    {
        DungeonFinderRoles roles;
        Player* pPlayer;
        // Group* pGroup;
        GroupProposal* pGroupProposal;
        
        DungeonIdVector canQueueFor;
        DungeonIdVector wishToQueueFor;
        DungeonIdVector isQueuedFor;
        
        bool CanTank() const { return roles & ROLE_TANK; };
        bool CanHeal() const { return roles & ROLE_HEAL; };
        bool CanDps() const { return roles & ROLE_DPS; };
        bool CanLead() const { return roles & ROLE_LEAD; };
    };
    
    struct Reward
    {
        // DungeonId dungeonId; ///< id of the dungeon this reward is associated with
        uint32 itemId; ///< Id of the reward item, see item_template database table
        uint32 howMany; ///< How many of this reward we will receive upon completion
        uint32 unk; ///< This isn't used yet, but will be sent, perhaps is modelId?
    };
};

#endif
