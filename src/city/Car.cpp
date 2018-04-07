#include "city/Car.h"

TextureManager* Car::sTextureManager = nullptr;

Car::Car() : mKinematic(1.0f, 150.0f, 600.0f), mSteering(mKinematic)
{
    mSprite.setTexture(sTextureManager->getTexture("car"));
    mSprite.setTextureRect(sf::IntRect(0, 0, WIDTH, HEIGHT));

    mSteering.setSeekDistance(20.0f);
    mSteering.setArriveDistance(100.0f);
}

void Car::setTextureManager(TextureManager* textureManager)
{
    sTextureManager = textureManager;
}

void Car::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mSprite);
}

void Car::update(float dt)
{
    Vector2f steeringForce = mSteering.compute(dt);
    mKinematic.addForce(steeringForce);
    mKinematic.update(dt);

    mSprite.setPosition(mKinematic.getPosition());
    float angle = toDegrees(mKinematic.getDirection().angle());
    if (angle >= 0.0f)
        angle = 360.0f - angle;
    else
        angle = -angle;
    int i = static_cast<int>((angle + 22.5f) / 45.0f) % 8;
    mSprite.setTextureRect(sf::IntRect(i * WIDTH, 0, WIDTH, HEIGHT));
    std::cout << angle << std::endl;
    std::cout << i << std::endl;
    //mShape.setRotation();
}


Kinematic& Car::getKinematic()
{
    return mKinematic;
}

SteeringBehaviors& Car::getSteering()
{
    return mSteering;
}
