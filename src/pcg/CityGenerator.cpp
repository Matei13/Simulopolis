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

#include "CityGenerator.h"
#include <fstream>

std::vector<std::string> CityGenerator::sNames;

void CityGenerator::setUp()
{
    std::string line;
    std::ifstream file;

    // List of patterns
    file.open("media/cities/names.txt");
    if (file.is_open())
    {
        while (std::getline(file, line))
            sNames.push_back(line);
        file.close();
    }
}

std::string CityGenerator::generate(uint64_t seed)
{
    return sNames[seed % sNames.size()];
}
