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

#include "WeightedAverage.h"

namespace MaNGOS
{
    WeightedAverage::WeightedAverage() :
        m_numQueue(),
        m_max(10)
    {
    }
    
    WeightedAverage::~WeightedAverage()
    {
    }
    
    uint32 WeightedAverage::GetCurrAverage() const
    {
        uint32 avg = 0;
        uint32 weightNum = m_numQueue.size();
        uint32 denominator = m_numQueue.size() * 2;
        for (UIntQueue::const_iterator it = m_numQueue.begin();
             it != m_numQueue.end();
             ++it)
        {
            avg += ((float(weightNum) / denominator) * (*it));
            --weightNum;
        }
        return avg;
    }
    
    void WeightedAverage::IncreaseCurrBy(uint32 val)
    {
        m_numQueue.front() += val;
    }
    
    void WeightedAverage::AddNewValue(uint32 val)
    {
        if (m_numQueue.size() >= m_max)
            m_numQueue.pop_front();
        m_numQueue.push_back(val);
    }

    void WeightedAverage::SetMax(uint32 newMax)
    {
        m_max = newMax;
    }

    bool WeightedAverage::IsEmpty() const
    {
        return m_numQueue.empty();
    }
};
