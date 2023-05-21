#include "processor.h"

Processor::Processor()
{}

Processor::~Processor()
{}

void Processor::addUnit(const std::string& key, UnitType procUnit)
{
	this->procUnits[key] = procUnit;
}

void Processor::operator()(UnitBridge& bridge)
{
	do
	{
		this->state = this->procUnits[this->state](bridge);
		if (this->procUnits.count(this->state) == 0)
		{
			this->setupExitState();
			break;
		}
	} while (this->state.compare(this->error) == 0);
}

void Processor::setStartState(const std::string& key) noexcept
{
	this->start = key;
}

void Processor::setExitState(const std::string& key) noexcept
{
	this->exit = key;
}

void Processor::setErrorState(const std::string& key) noexcept
{
	this->error = key;
}

void Processor::setupStartState() noexcept
{
	this->savedState = this->state;
	this->state = this->start;
}

void Processor::setupExitState() noexcept
{
	this->savedState = this->state;
	this->state = this->exit;
}

void Processor::setupErrorState() noexcept
{
	this->savedState = this->state;
	this->state = this->error;
}

void Processor::setupSavedState() noexcept
{
	this->state = this->savedState;
}

bool Processor::isExit() const noexcept
{
	if (this->state.compare(this->exit) == 0)
	{
		return true;
	}
	return false;
}

