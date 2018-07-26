#include "GoalThink.h"

GoalThink::GoalThink(Person* owner) : Goal(owner)
{
    //ctor
}

GoalThink::~GoalThink()
{
    //dtor
}

void GoalThink::activate()
{
    mState = State::ACTIVE;
    // Add subgoals
    clearSubgoals();
    arbitrate();
}

Goal::State GoalThink::process()
{
    activateIfInactive();

    mState = processSubgoals();
    if (mState == State::COMPLETED || mState == State::FAILED)
        mState = State::INACTIVE;
    return mState;
}

void GoalThink::terminate()
{

}

bool GoalThink::handle(Message message)
{
    forward(message);
}

void GoalThink::arbitrate()
{
    float maxDesirability = std::numeric_limits<float>::min();
    const GoalEvaluator* bestEvaluator = nullptr;
    for (const std::unique_ptr<GoalEvaluator>& evaluator : mEvaluators)
    {
        float desirability = evaluator->computeDesirability(mOwner);
        if (desirability > maxDesirability)
        {
            maxDesirability = desirability;
            bestEvaluator = evaluator.get();
        }
    }

    if (bestEvaluator)
        bestEvaluator->setGoal(mOwner);
}
