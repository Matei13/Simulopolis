/* Simulopolis
 * Copyright (C) 2018 Pierre Vigier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#pragma once

#include <SFML/Graphics.hpp>

class Animation
{
public:
    struct Frame
    {
        sf::IntRect rect;
        float duration;
    };

    Animation();
    Animation(std::vector<Frame> frames);

    void addFrame(Frame frame);
    const Frame& getFrame(std::size_t iFrame) const;

    std::size_t getNbFrames() const;
    float getTotalDuration() const;

private:
    std::vector<Frame> mFrames;
    float mTotalDuration;
};

