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
     * The different reasons for a instance being locked. Sent as the lockReason in the packets
     * \ref Opcodes::SMSG_LFG_PARTY_INFO and \ref Opcodes::SMSG_LFG_PLAYER_INFO.
     * \todo Document itemelevel and how we calculate the average, don't know that yet
     */
    enum DungeonFinderLockReasons
    {
        LOCKED_EXPANSION_TOO_LOW = 1, ///< You don't have the right expansion for this instance
        LOCKED_LEVEL_TOO_LOW = 2, ///< Your level is too low to do this instance
        LOCKED_LEVEL_TOO_HIGH = 3, ///< Your level is too high to do this instance
        LOCKED_GEAR_TOO_LOW = 4,///< You have too bad gear to do this instance
        LOCKED_GEAR_TOO_HIGH = 5, ///< You have too good gear to do this instance
        LOCKED_RAID = 6 ///< What's this? That we're locked to another raid instance already?
    };

    /**
     * Different error codes that can be sent when a \ref Player is trying to queue for a dungeon finder
     * The names are taken correspondingly with the names from the client except for 1 or 2. This data
     * is sent as a \ref uint32 first in a packet of the type \ref Opcodes::SMSG_LFG_JOIN_RESULT
     *
     * Todo: This should be put as comments for each status:
     * For the strings see FrameXML/GlobalStrings.lua
     * 0x0 = all ok?
     * 0x1 = ERR_LFG_ROLE_CHECK_FAILED - The role check has failed - duplicate?
     * 0x2 = ERR_LFG_GROUP_FULL - Your group is already full
     * 0x3 = This isn't present in the client and doesn't show anything when sent
     * 0x4 = ERR_LFG_NO_LFG_OBJECT - Internal LFG error
     * 0x5 = ERR_LFG_NO_SLOTS_PLAYER - You do not meet the requirements for the chosen dungeons.
     * 0x6 = ERR_LFG_NO_SLOTS_PARTY - One or more in group not meeting requirements
     * 0x7 = ERR_LFG_MISMATCHED_SLOTS - You cannot mix dungeons, raids and random when picking dungeons
     * 0x8 = ERR_LFG_PARTY_PLAYERS_FROM_DIFFERENT_REALMS - The dungeon you've chosen does not support players from multiple realms
     * 0x9 = ERR_LFG_MEMBERS_NOT_PRESENT - One or more party members are pending invites or disconnected
     * 0xA = ERR_LFG_GET_INFO_TIMEOUT - Could not retrieve information about some party members
     * 0xB = ERR_LFG_INVALID_SLOT - One or more dungeons was not valid
     * 0xC = ERR_LFG_DESERTER_PLAYER - You cannot queue for dungeons until your deserter debuff wears off
     * 0xD = ERR_LFG_DESERTER_PARTY - One or more party members has a deserter debuff
     * 0xE = ERR_LFG_RANDOM_COOLDOWN_PLAYER - You cannot queue for random dungeons while on random dungeon cooldown
     * 0xF = ERR_LFG_RANDOM_COOLDOWN_PARTY - One or more party members are on a random dungeon cooldown
     * 0x10= ERR_LFG_TOO_MANY_MEMBERS - You cannot enter dungeons with more than 5 party members
     * 0x11= ERR_LFG_CANT_USE_DUNGEONS - You cannot queue for a dungeon while using battleground or arenas
     * 0x12= ERR_LFG_ROLE_CHECK_FAILED - The role check has failed - duplicate?
     */
    enum DungeonFinderJoinErrors
    {
        JOIN_ERROR_ALL_OK = 0,
        JOIN_ERROR_LFG_ROLE_CHECK_FAILED = 1,
        JOIN_ERROR_LFG_GROUP_FULL = 2,
        //theres no 3 :/
        JOIN_ERROR_NO_LFG_OBJECT = 4,
        JOIN_ERROR_NO_SLOTS_PLAYER = 5,
        JOIN_ERROR_NO_SLOTS_PARTY = 6,
        JOIN_ERROR_MISMATCHED_SLOTS = 7,
        JOIN_ERROR_NO_DIFFERENT_REALMS = 8,
        JOIN_ERROR_MEMBERS_NOT_PRESENT = 9,
        JOIN_ERROR_GET_INFO_TIMEOUT = 10,
        JOIN_ERROR_INVALID_SLOT = 11,
        JOIN_ERROR_DESERTER_PLAYER = 12,
        JOIN_ERROR_DESERTER_PARTY = 13,
        JOIN_ERROR_RANDOM_CD_PLAYER = 14,
        JOIN_ERROR_RANDOM_CD_PARTY = 15,
        JOIN_ERROR_TOO_MANY_MEMBERS = 16,
        JOIN_ERROR_NOT_IN_BG_ARENA = 17,
        JOIN_ERROR_ROLE_CHECK_FAILED = 18 //duplicate?
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

    /// Information about why a certain dungeon is locked.
    struct DungeonLock
    {
        const Dungeon* dungeon;
        DungeonFinderLockReasons lockReason;

        bool operator<(const DungeonLock& rhs) const
        {
            return lockReason < rhs.lockReason;
        }
    };
    
    typedef std::list<Reward*> RewardList;
    typedef std::list<Dungeon*> DungeonList;
    typedef std::map<DungeonId, Dungeon*> DungeonMap;
    typedef std::list<Player*> PlayerList;
    typedef std::vector<DungeonId> DungeonIdVector;
    typedef std::vector<DungeonEntry> DungeonEntryVector;
    typedef std::set<DungeonLock> DungeonLockSet;
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
        inline const DungeonLockSet& GetLockedDungeons() const { return lockedDungeons; };
        inline const DungeonEntryVector& GetCanQueueFor() const { return canQueueFor; };
        inline const DungeonEntryVector& GetWishQueueFor() const { return wishToQueueFor; };
        inline const DungeonEntryVector& GetIsQueuedFor() const { return isQueuedFor; };
        inline int GetLastUpdatedLevelForLock() const { return lastChangedLevel; };
        
        inline void SetCanQueueFor(const DungeonEntryVector& canQueue) { canQueueFor = canQueue; };
        inline void SetLastUpdatedLevelForLock(int newLevel) { lastChangedLevel = newLevel; };
        inline void SetLockedDungeons(const DungeonLockSet& locked) { lockedDungeons = locked; };
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
        uint32 lastChangedLevel; ///< Last level that lockedDungeons was changed
        
        DungeonLockSet lockedDungeons; ///< Dungeons that this \ref PlayerInfo can't queue for
        //Turn these into sets aswell?
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
