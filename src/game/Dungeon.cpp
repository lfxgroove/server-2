#include "Dungeon.h"

Dungeon::Dungeon(DungeonId id, const LFGDungeonEntry* dungeonDBC)
    : m_id(id),
      m_dungeonEntry(dungeonDBC)
{}

Dungeon::~Dungeon()
{
    for (RewardList::iterator it = m_rewards.begin();
         it != m_rewards.end();
         ++it)
    {
        delete *it;
    }
}

Reward* Dungeon::CreateAndAddReward()
{
    Reward* reward = new Reward(m_dungeonEntry->Id);
    m_rewards.push_back(reward);
    return reward;
}

RewardList& Dungeon::GetRewards()
{
    return m_rewards;
}

const RewardList& Dungeon::GetRewards() const
{
    return m_rewards;
}
