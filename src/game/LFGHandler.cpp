/*
 * This code is part of MaNGOS. Contributor & Copyright details are in AUTHORS/THANKS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "WorldSession.h"
#include "Log.h"
#include "Player.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "World.h"

#include "DungeonFinder.h"
#include "DungeonShared.h"
#include "Dungeon.h"

void WorldSession::HandleLfgProposalResponse(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_PROPOSAL RECEIVED");
    recv_data.hexlike();    
}

void WorldSession::HandleLfgSetRoles(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_SET_ROLES RECEIVED");
    recv_data.hexlike();
    uint8 rolesFlags;
    recv_data >> rolesFlags;

    Dungeon::PlayerInfo* pInfo = sDungeonFinder.GetPlayerInfo(GetPlayer());
    if (pInfo)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "Updating roles to %d for someone",
                         rolesFlags);
        pInfo->SetRoles(Dungeon::DungeonFinderRoles(rolesFlags));
    }
}

void WorldSession::HandleLfgSetNeeds(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_SET_NEEDS RECEIVED");
    recv_data.hexlike();    
}

void WorldSession::HandleLfgBootPlayerVote(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_BOOT_PLAYER_VOTE RECEIVED");
    recv_data.hexlike();   
}

void WorldSession::HandleLfgBootPlayer(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_BOOT_PLAYER RECEIVED");
    recv_data.hexlike();
}

void WorldSession::HandleLfgTeleport(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_TELEPORT RECEIVED");
    recv_data.hexlike();
}

void WorldSession::HandleLfgGetStatus(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_GET_STATUS RECEIVED, HEXT DATA:");
    recv_data.hexlike();
}

void WorldSession::HandleLfgGetPlayerInfo(WorldPacket& recv_data)
{
    using namespace Dungeon;
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_GET_PLAYER_INFO RECEIVED, HEX DATA:");
    recv_data.hexlike();
    
    // DungeonList dungeons = sDungeonFinder.GetAvailableDungeonsForPlayer(GetPlayer());
    
    int numDungeons = 0;
    for (int i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        const LFGDungeonEntry* dungeon = sLFGDungeonStore.LookupEntry(i);
        if (dungeon)
            ++numDungeons;
    }
    
    WorldPacket packet(SMSG_LFG_PLAYER_INFO, numDungeons * 36 + 1 + 4 + 0);//(numDungeons / 2) * 4);
    
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "Sending %d instances with rewards", numDungeons);
    packet << uint8(numDungeons);
    for (int i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        const LFGDungeonEntry* dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon || i % 2 == 1)
            continue;
        packet << uint32(dungeon->Entry());
        packet << uint8(0); //doneToday
        packet << uint32(10000);
        packet << uint32(100000);
        packet << uint32(30000);
        packet << uint32(2000);
        packet << uint8(1); //1 reward item coming
        packet << uint32(54583); //itemId - Cloak of Burning Dusk
        packet << uint32(0); //unk?
        packet << uint32(1); //one of that item we will get
    }
    
    // packet << uint32(0);
    packet << uint32(numDungeons/2);
    for (int i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        const LFGDungeonEntry* dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon || i % 2 == 0)
            continue;
        packet << uint32(dungeon->Entry());
        packet << uint32(4 % i);
    }
    
    SendPacket(&packet);
}

void WorldSession::HandleLfgGetPartyInfo(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_GET_PARTY_INFO RECEIVED");
    recv_data.hexlike();
    
    WorldPacket packet(SMSG_LFG_PARTY_INFO, 13);
    packet << uint8(0);
    // packet << uint8(1); // ourselves as a player are coming
    // packet << GetPlayer()->GetObjectGuid();
    // packet << uint32(0); //no locked dungeons

    SendPacket(&packet);
}

void WorldSession::HandleLfgJoinOpcode(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "CMSG_LFG_JOIN, data hexlike:");
    recv_data.hexlike();
    
    uint8 dungeonsCount, counter2;
    uint32 rolesFlags;
    std::string comment;
    std::vector<uint32> dungeons;
    
    recv_data >> rolesFlags;                                // roles, except leader
    recv_data >> Unused<uint8>();                           // lua: GetLFGInfoLocal
    recv_data >> Unused<uint8>();                           // lua: GetLFGInfoLocal
    
    recv_data >> dungeonsCount;
    dungeons.resize(dungeonsCount);
    for (uint8 i = 0; i < dungeonsCount; ++i)
        recv_data >> dungeons[i];                           // dungeons entry = id & (type << 24)
    
    recv_data >> counter2;                                  // const count = 3, lua: GetLFGInfoLocal
    for (uint8 i = 0; i < counter2; ++i)
        recv_data >> Unused<uint8>();                       // lua: GetLFGInfoLocal
    
    recv_data >> comment;                                   // lfg comment

    DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON, "Got this data: num %d, first dungeon id: %d comment %s",
                     dungeonsCount, dungeons[0] & 0xFFFFFF, comment.c_str());

    Player* pPlayer = GetPlayer();
    if (!pPlayer)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_DUNGEON,
                         "Critical: This session doesn't have a player, this won't do!");
        return;
    }

    //Check the group proposal here aswell, if we already have one it might need to be updated
    Dungeon::PlayerInfo* pInfo = sDungeonFinder.GetPlayerInfo(pPlayer);
    if (!pInfo)
        pInfo = sDungeonFinder.CreatePlayerInfo(pPlayer);
    pInfo->SetRoles(Dungeon::DungeonFinderRoles(rolesFlags));
    pInfo->SetComment(comment);
    // pInfo->canQueueFor = ;
    // pInfo->wishesToQueueFor = ;
    // // pInfo->isQueuedFor = {};
    
    
    
    WorldPacket response(SMSG_LFG_JOIN_RESULT);
    response << uint32(0x0); //players info coming
    //One or more in group not meeting requirements = 0x6
    response << uint32(0);
    response << uint8(0); //no players currently
    SendPacket(&response);
    
    WorldPacket response2(SMSG_LFG_UPDATE_SEARCH);
    response2 << uint8(0);
    SendPacket(&response2);
    
    WorldPacket response4(SMSG_LFG_UPDATE_PLAYER);
    response4 << uint8(0); //statusCode
    response4 << uint8(1); //dataComing
    response4 << uint8(1); //queued
    response4 << uint8(0); //joined
    response4 << uint8(0); //unk??
    response4 << uint8(1); //numInstancesComing
    response4 << uint32(dungeons[0]); //dungeon entry for now, perhaps should be id?
    response4 << "abcd"; //comment
    SendPacket(&response4);
    
    WorldPacket response3(SMSG_LFG_QUEUE_STATUS);
    response3 << uint32(dungeons[0]);
    response3 << uint32(0); //myWait
    response3 << uint32(0); //avg wait
    response3 << uint32(0); //tankWait
    response3 << uint32(0); //healerWait
    response3 << uint32(0); //dpsWait
    response3 << uint8(1); //tankNeeds
    response3 << uint8(1); //healNeeds
    response3 << uint8(2); //dpsNeeds
    response3 << uint32(0); //queuedTime
    SendPacket(&response3);

    
    // SendLfgJoinResult(ERR_LFG_OK);
    // SendLfgUpdate(false, LFG_UPDATE_JOIN, dungeons[0]);
}

void WorldSession::HandleLfgLeaveOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LFG_LEAVE");
    recv_data.hexlike();
    
    // WorldPacket data(SMSG_LFG_UPDATE_PLAYER);
    // data << uint8(7); //removed from queue
    // data << uint8(0);
    // SendPacket(&data);
    
    // SendLfgUpdate(false, LFG_UPDATE_LEAVE, 0);
}

void WorldSession::HandleSearchLfgJoinOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LFG_SEARCH_JOIN");

    uint32 temp, entry;
    recv_data >> temp;

    entry = (temp & 0x00FFFFFF);
    // LfgType type = LfgType((temp >> 24) & 0x000000FF);

    // SendLfgSearchResults(type, entry);
}

void WorldSession::HandleSearchLfgLeaveOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LFG_SEARCH_LEAVE");
    recv_data.hexlike();

    recv_data >> Unused<uint32>();                          // join id?
}

void WorldSession::HandleSetLfgCommentOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_SET_LFG_COMMENT");
    recv_data.hexlike();

    std::string comment;
    recv_data >> comment;
    DEBUG_LOG("LFG comment \"%s\"", comment.c_str());
}

void WorldSession::SendLfgSearchResults(LfgType type, uint32 entry)
{
    WorldPacket data(SMSG_LFG_SEARCH_RESULTS);
    data << uint32(type);                                   // type
    data << uint32(entry);                                  // entry from LFGDungeons.dbc

    uint8 isGuidsPresent = 0;
    data << uint8(isGuidsPresent);
    if (isGuidsPresent)
    {
        uint32 guids_count = 0;
        data << uint32(guids_count);
        for (uint32 i = 0; i < guids_count; ++i)
        {
            data << uint64(0);                              // player/group guid
        }
    }

    uint32 groups_count = 1;
    data << uint32(groups_count);                           // groups count
    data << uint32(groups_count);                           // groups count (total?)

    for (uint32 i = 0; i < groups_count; ++i)
    {
        data << uint64(1);                                  // group guid

        uint32 flags = 0x92;
        data << uint32(flags);                              // flags

        if (flags & 0x2)
        {
            data << uint8(0);                               // comment string, max len 256
        }

        if (flags & 0x10)
        {
            for (uint32 j = 0; j < 3; ++j)
                data << uint8(0);                           // roles
        }

        if (flags & 0x80)
        {
            data << uint64(0);                              // instance guid
            data << uint32(0);                              // completed encounters
        }
    }

    // TODO: Guard Player map
    HashMapHolder<Player>::MapType const& players = sObjectAccessor.GetPlayers();
    uint32 playersSize = players.size();
    data << uint32(playersSize);                            // players count
    data << uint32(playersSize);                            // players count (total?)

    for (HashMapHolder<Player>::MapType::const_iterator iter = players.begin(); iter != players.end(); ++iter)
    {
        Player* plr = iter->second;

        if (!plr || plr->GetTeam() != _player->GetTeam())
            continue;

        if (!plr->IsInWorld())
            continue;

        data << plr->GetObjectGuid();                       // guid

        uint32 flags = 0xFF;
        data << uint32(flags);                              // flags

        if (flags & 0x1)
        {
            data << uint8(plr->getLevel());
            data << uint8(plr->getClass());
            data << uint8(plr->getRace());

            for (uint32 i = 0; i < 3; ++i)
                data << uint8(0);                           // talent spec x/x/x

            data << uint32(0);                              // armor
            data << uint32(0);                              // spd/heal
            data << uint32(0);                              // spd/heal
            data << uint32(0);                              // HasteMelee
            data << uint32(0);                              // HasteRanged
            data << uint32(0);                              // HasteSpell
            data << float(0);                               // MP5
            data << float(0);                               // MP5 Combat
            data << uint32(0);                              // AttackPower
            data << uint32(0);                              // Agility
            data << uint32(0);                              // Health
            data << uint32(0);                              // Mana
            data << uint32(0);                              // Unk1
            data << float(0);                               // Unk2
            data << uint32(0);                              // Defence
            data << uint32(0);                              // Dodge
            data << uint32(0);                              // Block
            data << uint32(0);                              // Parry
            data << uint32(0);                              // Crit
            data << uint32(0);                              // Expertise
        }

        if (flags & 0x2)
            data << "";                                     // comment

        if (flags & 0x4)
            data << uint8(0);                               // group leader

        if (flags & 0x8)
            data << uint64(1);                              // group guid

        if (flags & 0x10)
            data << uint8(0);                               // roles

        if (flags & 0x20)
            data << uint32(plr->GetZoneId());               // areaid

        if (flags & 0x40)
            data << uint8(0);                               // status

        if (flags & 0x80)
        {
            data << uint64(0);                              // instance guid
            data << uint32(0);                              // completed encounters
        }
    }

    SendPacket(&data);
}

void WorldSession::SendLfgJoinResult(LfgJoinResult result)
{
    WorldPacket data(SMSG_LFG_JOIN_RESULT, 0);
    data << uint32(result);
    data << uint32(0); // ERR_LFG_ROLE_CHECK_FAILED_TIMEOUT = 3, ERR_LFG_ROLE_CHECK_FAILED_NOT_VIABLE = (value - 3 == result)
    
    // if (result == ERR_LFG_NO_SLOTS_PARTY)
    // {
    //     uint8 count1 = 0;
    //     data << uint8(count1);                              // players count?
    //     for (uint32 i = 0; i < count1; ++i)
    //     {
    //         data << uint64(0);                              // player guid?
    //         uint32 count2 = 0;
    //         for (uint32 j = 0; j < count2; ++j)
    //         {
    //             data << uint32(0);                          // dungeon id/type
    //             data << uint32(0);                          // lock status?
    //         }
    //     }
    // }
    
    SendPacket(&data);
}

void WorldSession::SendLfgUpdate(bool isGroup, LfgUpdateType updateType, uint32 id)
{
    WorldPacket data(isGroup ? SMSG_LFG_UPDATE_PARTY : SMSG_LFG_UPDATE_PLAYER, 0);
    data << uint8(updateType);
    
    uint8 extra = updateType == LFG_UPDATE_JOIN ? 1 : 0;
    data << uint8(extra);
    
    if (extra)
    {
        data << uint8(0);
        data << uint8(0);
        data << uint8(0);

        if (isGroup)
        {
            data << uint8(0);
            for (uint32 i = 0; i < 3; ++i)
                data << uint8(0);
        }

        uint8 count = 1;
        data << uint8(count);
        for (uint32 i = 0; i < count; ++i)
            data << uint32(id);
        data << "";
    }
    SendPacket(&data);
}

void WorldSession::HandleSetDungeonDifficultyOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode MSG_SET_DUNGEON_DIFFICULTY");

    uint32 mode;
    recv_data >> mode;

    if (mode >= MAX_DUNGEON_DIFFICULTY)
    {
        sLog.outError("WorldSession::HandleSetDungeonDifficultyOpcode: player %d sent an invalid instance mode %d!", _player->GetGUIDLow(), mode);
        return;
    }

    if (Difficulty(mode) == _player->GetDungeonDifficulty())
        return;

    // cannot reset while in an instance
    Map* map = _player->GetMap();
    if (map && map->IsDungeon())
    {
        sLog.outError("WorldSession::HandleSetDungeonDifficultyOpcode: player %d tried to reset the instance while inside!", _player->GetGUIDLow());
        return;
    }

    // Exception to set mode to normal for low-level players
    if (_player->getLevel() < LEVELREQUIREMENT_HEROIC && mode > REGULAR_DIFFICULTY)
        return;

    if (Group* pGroup = _player->GetGroup())
    {
        if (pGroup->IsLeader(_player->GetObjectGuid()))
        {
            // the difficulty is set even if the instances can't be reset
            //_player->SendDungeonDifficulty(true);
            pGroup->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false, _player);
            pGroup->SetDungeonDifficulty(Difficulty(mode));
        }
    }
    else
    {
        _player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false);
        _player->SetDungeonDifficulty(Difficulty(mode));
    }
}
