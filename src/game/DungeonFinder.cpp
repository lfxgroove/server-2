#include "DungeonFinder.h"

Finder::Finder()
{}

Finder::~Finder()
{}

Finder::Init()
{
    uint32 numDungeons = sLFGDungeonStore.GetNumRows();
    const LFGDungeonEntry* dungeon;
    
    BarGoLink bar(numDungeons);
    
    for (uint32 i = 0; i < numDungeons; ++i)
    {
        bar.step();
        
        dungeonDBC = sLFGDungeonStore.LookupEntry(i);
        if (!dungeonDBC)
            continue;
        
        QueryResult* result = WorldDatabase.Query(
            "SELECT item_id, how_many, unk "
            "FROM dungeon_rewards "
            "WHERE dungeon_id = %d",
            dungeonDBC->Id);
        
        if (!result)
        {
            sLog.outError("Couldn't find any rewards for dungeon %d", dungeonDBC->Id);
            continue;
        }
        
        Dungeon dungeon = Dungeon(dungeonDBC->Id, dungeonDBC);
        
        do
        {
            Field* fields = result->Fetch();
            Reward reward;
            reward.dungeonId = dungeonDBC->Id;
            reward.itemId = fields[0].GetUInt32();
            reward.howMany = fields[1].GetUInt32();
            reward.unk = fields[2].GetUInt32();
            dungeon.AddReward(reward);
        }
        while(result->NextRow());
        delete result; //Is this needed or not?
    }
    
    sLog.outString();
    sLog.outString(">> Loaded " SIZEFMTD " dungeons and their rewards", numDungeons);
}
