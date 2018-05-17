#include "Road.h"

Road::Road(const sf::Texture& texture, Type type) :
    Tile(texture, type)
{
    sf::IntRect rect(0, 0, 132, 101);
    if (type == Type::ROAD_SIDEWALK)
        rect.left = 132;
    else if (type == Type::ROAD_WATER)
        rect.left = 264;
    mSprite.setTextureRect(rect);
}

Road::~Road()
{
    //dtor
}

std::unique_ptr<Tile> Road::clone() const
{
    return std::unique_ptr<Tile>(new Road(*this));
}

bool Road::updateVariant(Tile* neighbors[3][3])
{
    sf::IntRect rect = mSprite.getTextureRect();

    if (neighbors[0][1]->isRoad() && neighbors[1][0]->isRoad() &&
        neighbors[2][1]->isRoad() && neighbors[1][2]->isRoad())
        rect.top = 202;
    else if (neighbors[0][1]->isRoad() && neighbors[1][0]->isRoad() && neighbors[2][1]->isRoad())
        rect.top = 707;
    else if (neighbors[0][1]->isRoad() && neighbors[1][0]->isRoad() && neighbors[1][2]->isRoad())
        rect.top = 909;
    else if (neighbors[0][1]->isRoad() && neighbors[2][1]->isRoad() && neighbors[1][2]->isRoad())
        rect.top = 808;
    else if (neighbors[1][0]->isRoad() && neighbors[2][1]->isRoad() && neighbors[1][2]->isRoad())
        rect.top = 1010;
    else if (neighbors[0][1]->isRoad() && neighbors[1][0]->isRoad())
        rect.top = 505;
    else if (neighbors[1][0]->isRoad() && neighbors[2][1]->isRoad())
        rect.top = 404;
    else if (neighbors[2][1]->isRoad() && neighbors[1][2]->isRoad())
        rect.top = 606;
    else if (neighbors[1][2]->isRoad() && neighbors[0][1]->isRoad())
        rect.top = 303;
    else if ((neighbors[1][0]->isRoad() && neighbors[1][2]->isRoad()) ||
        (neighbors[1][0]->isRoad() && !neighbors[1][2]->isRoad()) ||
        (!neighbors[1][0]->isRoad() && neighbors[1][2]->isRoad()))
        rect.top = 101;

    if (mSprite.getTextureRect() == rect)
        return false;

    mSprite.setTextureRect(rect);
    return true;
}

bool Road::isRoad() const
{
    return true;
}

bool Road::hasSidewalk() const
{
    return mType == Type::ROAD_SIDEWALK;
}
