#pragma once

#include "Building.h"
#include "Good.h"

class Work;

class Business : public Building
{
public:
    Business(const std::string& name, Type type, unsigned int nbStairs, Good good, std::size_t nbEmployees);
    virtual ~Business();

    virtual std::unique_ptr<Tile> clone() const override;

    Good getGood() const;
    float getPrice() const;

protected:
    Good mGood;
    unsigned int mQuantity;
    float mPrice;
    std::size_t mNbEmployees;
    Work* mManager;
    std::vector<Work*> mEmployees;
};
