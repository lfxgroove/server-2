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

#ifndef MANGOS_DUNGEON_H
#define MANGOS_DUNGEON_H

#include "DungeonShared.h"

namespace DungeonFinder
{
    class Dungeon
    {
    public:
        Dungeon(DungeonId id, const LFGDungeonEntry* dungeonDBC);
        virtual ~Dungeon();
        
        RewardList GetRewards() const;
        uint32 GetMoneyRewardConstant() const;
        uint32 GetMoneyRewardVariable() const;
        uint32 GetExpRewardConstant() const;
        uint32 GetExpRewardVariable() const;
        
        /** 
         * Gives you back a pointer to a \ref Reward that you can change the way you like and it will
         * be associated with this \ref Dungeon
         * @return a \ref Reward to fill up with information that's associated to this \ref Dungeon
         */
        Reward* CreateAndAddReward();
        RewardList& GetRewards();
        const RewardList& GetRewards() const;
        
        DungeonEntry Entry() const { return dungeonEntry->Entry(); };
    private:
        const LFGDungeonEntry* m_dungeonEntry;
        RewardList m_rewards; ///< Rewards we will receive upon completing the dungeon
    };
};
#endif
