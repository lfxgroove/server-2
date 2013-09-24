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

#include "Dungeon.h"

namespace Dungeon
{
    Dungeon::Dungeon(const LFGDungeonEntry* dungeonDBC)
        : m_id(dungeonDBC->ID),
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
    
    uint32 Dungeon::GetMaxLevel() const
    {
        return m_dungeonEntry->maxlevel;
    }
    
    uint32 Dungeon::GetMinLevel() const
    {
        return m_dungeonEntry->minlevel;
    }
    
    Expansions Dungeon::GetRequiredExpansion() const
    {
        return Expansions(m_dungeonEntry->expansion);
    }

    bool Dungeon::IsHeroic() const
    {
        //Is this correct?
        return m_dungeonEntry->difficulty == DIFFICULTY_HEROIC;
    }
    
    Reward* Dungeon::CreateAndAddReward()
    {
        //(m_dungeonEntry->ID); for now we don't associate the reward back to a certain dungeon,
        //the dungeons will keep track of that themselves
        Reward* reward = new Reward();
        m_rewards.push_back(reward);
        return reward;
    }
    
    RewardList& Dungeon::GetRewards()
    {
        return m_rewards;
    }
    
    const RewardList& Dungeon::GetConstRewards() const
    {
        return m_rewards;
    }
};
