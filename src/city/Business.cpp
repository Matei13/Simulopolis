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

#include "city/Business.h"
#include "message/MessageBus.h"
#include "city/City.h"
#include "city/Company.h"
#include "city/Market.h"
#include "city/Person.h"
#include "city/Work.h"
#include "city/Good.h"

Tile::Type Business::getBusinessType(GoodType goodType)
{
    switch (goodType)
    {
        case GoodType::NECESSARY:
            return Tile::Type::GROCERY;
        case GoodType::NORMAL:
            return Tile::Type::MALL;
        case GoodType::LUXURY:
            return Tile::Type::BOUTIQUE;
        default:
            return Tile::Type::GROCERY;
    }
}

Business::Business(const std::string& name, Type type, unsigned int nbStairs, GoodType goodType, unsigned int maxSizeStock,
    double employeeProductivity, std::size_t nbEmployees, WorkType employeeType) :
    Building(name, type, nbStairs), mGoodType(goodType), mMaxSizeStock(maxSizeStock), mStock(0), mStockCost(0.0),
    mPrice(0.0), mEmployeeProductivity(employeeProductivity)
{
    mEmployees.emplace_back(std::make_unique<Work>(WorkType::MANAGER, this));
    for (std::size_t i = 0; i < nbEmployees; ++i)
        mEmployees.emplace_back(std::make_unique<Work>(employeeType, this));
}

std::unique_ptr<Tile> Business::clone() const
{
    return std::make_unique<Business>(mTextureName, mType, mNbStairs, mGoodType, mMaxSizeStock, mEmployeeProductivity, mEmployees.size() - 1, mEmployees.back()->getType());
}

void Business::update()
{
    // Read messages
    bool priceDirty = false;
    bool desiredQuantityDirty = false;
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::MARKET)
        {
            const MarketBase::EventBase& eventBase = message.getInfo<MarketBase::EventBase>();
            if (eventBase.marketType == MarketType::WORK)
            {
                const Market<Work>::Event& event = static_cast<const Market<Work>::Event&>(eventBase);
                switch (event.type)
                {
                    case Market<Work>::Event::Type::ITEM_ADDED:
                        mWorksInMarket.insert(event.itemId);
                        break;
                    case Market<Work>::Event::Type::SALE:
                        mWorksInMarket.erase(event.sale.itemId);
                        break;
                    default:
                        break;
                }
            }
            else
            {
                const Market<Good>::Event& event = static_cast<const Market<Good>::Event&>(eventBase);
                if (event.type == Market<Good>::Event::Type::PURCHASE)
                {
                    // To do : save the building to fetch the good later
                    ++mStock;
                    mStockCost += event.sale.value;
                    priceDirty = true;
                    desiredQuantityDirty = true;
                    mOwner->getMessageBus()->send(Message::create(mOwner->getCity()->getBank().getMailboxId(), MessageType::BANK, mOwner->getCity()->getBank().createTransferMoneyEvent(mOwner->getAccount(), event.sale.sellerAccount, event.sale.value)));
                }
            }
        }
        else if (message.type == MessageType::BUSINESS)
        {
            const Event& event = message.getInfo<Event>();
            if (event.type == Event::Type::RESERVATION)
            {
                if (hasPreparedGoods())
                {
                    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), message.sender, MessageType::BUSINESS, Event{Event::Type::RESERVATION_ACCEPTED, mOwner->getAccount(), mPrice}));
                    --mStock;
                    mStockCost -= mPrice;
                    mPreparedGoods -= 1.0;
                    desiredQuantityDirty = true;
                }
                else
                    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), message.sender, MessageType::BUSINESS, Event{Event::Type::RESERVATION_REFUSED, {}, {}}));
            }
        }
    }

    // Update price
    if (priceDirty)
        updatePrice();

    // Update desired quantity
    if (desiredQuantityDirty)
        updateDesiredQuantity();
}

void Business::tearDown()
{
    update();
    // Remove everything from markets
    const Market<Work>* laborMarket = static_cast<const Market<Work>*>(mOwner->getCity()->getMarket(MarketType::WORK));
    for (Id id : mWorksInMarket)
        mOwner->getMessageBus()->send(Message::create(laborMarket->getMailboxId(), MessageType::MARKET, laborMarket->createRemoveItemEvent(id)));
    // Remove bids for market
    const Market<Good>* goodsMarket = getMarket();
    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), goodsMarket->getMailboxId(), MessageType::MARKET, goodsMarket->createSetQuantityEvent(0)));
}

void Business::setOwner(Company* owner)
{
    Building::setOwner(owner);
    for (std::unique_ptr<Work>& employee : mEmployees)
        employee->setEmployer(mOwner);
}

GoodType Business::getGoodType() const
{
    return mGoodType;
}

unsigned int Business::getStock() const
{
    return mStock;
}

unsigned int Business::getMaxSizeStock() const
{
    return mMaxSizeStock;
}

bool Business::hasPreparedGoods() const
{
    return mStock > 0 && mPreparedGoods >= 1.0;
}

unsigned int Business::getPreparedGoods() const
{
    return mPreparedGoods;
}

Money Business::getPrice() const
{
    return mPrice;
}

std::unique_ptr<Work>& Business::getManager()
{
    return mEmployees[0];
}

const std::unique_ptr<Work>& Business::getManager() const
{
    return mEmployees[0];
}

std::vector<std::unique_ptr<Work>>& Business::getEmployees()
{
    return mEmployees;
}

const std::vector<std::unique_ptr<Work>>& Business::getEmployees() const
{
    return mEmployees;
}

void Business::onNewMonth()
{
    prepareGoods();
    buyGoods();
}

void Business::prepareGoods()
{
    double preparedGoods = 0.0;
    double dmonth = mOwner->getCity()->getWeeklyStandardWorkingHours() / City::NB_HOURS_PER_MONTH;
    for (std::size_t i = 1; i < mEmployees.size(); ++i)
    {
        if (mEmployees[i]->hasWorkedThisMonth())
            preparedGoods += mEmployeeProductivity * mEmployees[i]->getEmployee()->getProductivity() * dmonth;
    }
    if (getManager()->hasWorkedThisMonth())
        preparedGoods += preparedGoods * getManager()->getEmployee()->getProductivity() * dmonth;
    mPreparedGoods += preparedGoods;
}

void Business::buyGoods()
{
    updateDesiredQuantity();
    const Market<Good>* market = getMarket();
    for (const Market<Good>::Item* item : market->getItems())
    {
        if (mOwner->getCity()->getMap().isReachableFrom(this, item->good->getProductionPlace()))
            mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), market->getMailboxId(), MessageType::MARKET, market->createBidEvent(item->id, item->reservePrice)));
    }
}

const Market<Good>* Business::getMarket()
{
    switch (mGoodType)
    {
        case GoodType::NECESSARY:
            return static_cast<const Market<Good>*>(mOwner->getCity()->getMarket(MarketType::NECESSARY_GOOD));
        case GoodType::NORMAL:
            return static_cast<const Market<Good>*>(mOwner->getCity()->getMarket(MarketType::NORMAL_GOOD));
        case GoodType::LUXURY:
            return static_cast<const Market<Good>*>(mOwner->getCity()->getMarket(MarketType::LUXURY_GOOD));
        default:
            return nullptr;
    };
}

void Business::updatePrice()
{
    // Compute payroll
    Money payroll(0.0);
    for (const std::unique_ptr<Work>& employee : mEmployees)
    {
        if (employee->hasWorkedThisMonth())
            payroll += employee->getSalary();
    }
    // Update price
    mPrice = (mStockCost + payroll) / mStock * mOwner->getRetailMargin(mGoodType);
}

void Business::updateDesiredQuantity()
{
    const Market<Good>* market = getMarket();
    unsigned int desiredQuantity = std::max(0, static_cast<int>(mMaxSizeStock) - static_cast<int>(mStock));
    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), market->getMailboxId(), MessageType::MARKET, market->createSetQuantityEvent(desiredQuantity)));
}
