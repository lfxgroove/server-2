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

#ifndef _MANGOS_WEIGHTED_AVERAGE
#define _MANGOS_WEIGHTED_AVERAGE

#include "Common.h"

namespace MaNGOS
{
    /**
     * This class is used to calculate a weighted average of a few given values added by
     * \ref WeightedAverage::AddNewValue. The weight for each element is calculated as follows:
     * \verbatim
     * element * (currElementIndex / (numElements * 2))
     * \endverbatim
     * which means that the first element (the one that's been there the longest) will get the highest
     * value. We can set how many items there should be in the list at any time, by calling
     * \ref WeightedAverage::SetMax which means the when we call \ref WeightedAverage::AddNewValue
     * and we have more than the set max elements the oldest one will be removed first. If we want
     * to increase the value of the current value we can use \ref WeightedAverage::IncreaseCurrBy.
     */
    class WeightedAverage
    {
    public:
        typedef std::list<uint32> UIntQueue; //Used as a queue
        WeightedAverage();
        virtual ~WeightedAverage();
        
        /** 
         * @return The current weighted average of the values in here
         */
        uint32 GetCurrentAverage() const;
        /** 
         * Increase the last added value by the given value, ie:
         * currVal += val
         * @param val the value to inrease by
         */
        void IncreaseCurrBy(uint32 val);
        /** 
         * Adds a new value to the weighted average list, if we reach the max with this one we
         * will pop the one that is the oldest.
         * @param val 
         */
        void AddNewValue(uint32 val);
        /** 
         * Sets the maximum number of values we will use before starting to remove old values
         * when new ones are being added by \ref WeightedAverage::AddNewValue
         * @param val the new maximum amount of numbers to keep
         */
        void SetMax(uint32 val);
    private:
        UIntQueue m_numQueue;
        uint32 m_max;
    };
};

#endif
