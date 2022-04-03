#include "IdleCondition.h"

IdleCondition::IdleCondition(const bool wants_to_idle)
{
	m_name = "Close Combat Condition";
	setWantsToIdle(wants_to_idle);
}

IdleCondition::~IdleCondition()
= default;

void IdleCondition::setWantsToIdle(const bool state)
{
	m_wantsToIdle = state;
}

bool IdleCondition::Condition()
{
	return m_wantsToIdle;
}