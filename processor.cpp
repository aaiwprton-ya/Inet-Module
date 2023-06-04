#include "processor.h"

Processor::Processor(InetUtils* utils)
	: utils(utils)
{}

Processor::~Processor()
{}

void Processor::addUnit(const std::string& key, UnitType procUnit)
{
	this->procUnits[key] = procUnit;
}
		
void Processor::makeUnit(const std::string& key, const UnitTemplateType& unitTemplate)
{
	UnitType result;
	switch (unitTemplate.unitTemplate) {
	case UT_STATE_TO_GET:
		
		
		result = [this, &unitTemplate](UnitBridge& bridge) -> std::string {
			std::cout << "unit start" << std::endl;
			std::string request = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << request << std::endl;
			
			Argparser cmdPars(request);
			std::string* p_state = nullptr;
			if (cmdPars.findArg(ArgType::ARGTYPE_STRING, "state", "s"))
			{
				p_state = cmdPars.getArg("state", &p_state);
			}
			
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = 0;
			if (p_state == nullptr)
			{
				return unitTemplate.errorUnit;
			} else
			{
				if ((*p_state).compare("BAD") == 0)
				{
					return unitTemplate.backUnit;
				} else
				if ((*p_state).compare("OK") == 0)
				{
					this->utils->makeResponse({InetUtils::RT_CMD_GET_VALUE, unitTemplate.getValue}, &(bridge.response), &(bridge.responseSize));
					return unitTemplate.nextUnit;
				} else
				{
					return unitTemplate.errorUnit;
				}
			}
		};
		
		
		break;
	case UT_GET_TO_PREPARATION:
		
		
		result = [this, &unitTemplate](UnitBridge& bridge) -> std::string {
			std::cout << "unit start" << std::endl;
			
			std::string request = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << request << std::endl;
			
			Argparser cmdPars(request);
			std::string* p_value = nullptr;
			if (cmdPars.findArg(ArgType::ARGTYPE_STRING, "value", "v"))
			{
				p_value = cmdPars.getArg("value", &p_value);
			}
			
			if (p_value == nullptr)
			{
				*(bridge.plannedData) = nullptr;
				*(bridge.plannedSize) = 0;
				*(bridge.expectedSize) = 0;
				return unitTemplate.errorUnit;
			} else
			{
				std::map<std::string, std::pair<void*, size_t>>::const_iterator dataIter = unitTemplate.dataPool.find(*p_value);
				if (dataIter != unitTemplate.dataPool.end())
				{
					*(bridge.plannedData) = (*dataIter).second.first;
					*(bridge.plannedSize) = (*dataIter).second.second;
					*(bridge.expectedSize) = 0;
				} else
				{
					*(bridge.plannedData) = nullptr;
					*(bridge.plannedSize) = 0;
					*(bridge.expectedSize) = 0;
					return unitTemplate.errorUnit;
				}
			}
			this->utils->makeResponse({InetUtils::RT_CMD_ACCEPT_SIZE, std::to_string(*(bridge.plannedSize))}, &(bridge.response), &(bridge.responseSize));
			return unitTemplate.nextUnit;
		};
		
		
		break;
	case UT_ACCEPT_TO_READY:
		
		
		result = [this, &unitTemplate](UnitBridge& bridge) -> std::string {
			std::cout << "unit accept" << std::endl;
			
			std::string request = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << request << std::endl;
			
			Argparser cmdPars(request);
			cmdPars.findArg(ArgType::ARGTYPE_INT, "size", "sz");
			int* p_size = cmdPars.getArg("size", &p_size);
			if (p_size == nullptr)
			{
				*(bridge.expectedSize) = 0;
				return unitTemplate.errorUnit;
			}
			
			this->utils->makeResponse({InetUtils::RT_CMD_GIVE_OK, ""}, &(bridge.response), &(bridge.responseSize));
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = *p_size;
			return unitTemplate.nextUnit;
		};
		
		
		break;
	case UT_READY_TO_SEND:
		
		
		result = [this, &unitTemplate](UnitBridge& bridge) -> std::string {
			std::cout << "unit accepted" << std::endl;
			
			std::string request = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << request << std::endl;
			
			Argparser cmdPars(request);
			std::string* p_state = nullptr;
			if (cmdPars.findArg(ArgType::ARGTYPE_STRING, "state", "s"))
			{
				p_state = cmdPars.getArg("state", &p_state);
			}
			
			if (p_state == nullptr)
			{
				return unitTemplate.errorUnit;
			} else
			{
				if ((*p_state).compare("BAD") == 0)
				{
					return unitTemplate.backUnit;
				} else
				if ((*p_state).compare("OK") == 0)
				{
					bridge.response = *(bridge.plannedData);
					bridge.responseSize = *(bridge.plannedSize);
				} else
				{
					return unitTemplate.errorUnit;
				}
			}
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = 0;
			return unitTemplate.nextUnit;
		};
		
		
		break;
	case UT_RECEIVE:
		
		
		result = [this, &unitTemplate](UnitBridge& bridge) -> std::string {
			std::cout << "unit receive" << std::endl;
			if (unitTemplate.recvBuffer == nullptr)
			{
				return unitTemplate.errorUnit;
			}
			unitTemplate.recvBuffer->setSize(bridge.requestSize);
			unitTemplate.recvBuffer->fillBuffer(bridge.request);
			return unitTemplate.nextUnit;
		};
		
		
		break;
		
	case UT_ERROR:
		
		
		result = [this, &unitTemplate](UnitBridge& bridge) -> std::string {
			std::cout << "unit error" << std::endl;
			this->utils->makeResponse({InetUtils::RT_CMD_GIVE_BAD, ""}, &(bridge.response), &(bridge.responseSize));
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = 0;
			return unitTemplate.backUnit;
		};
		
		
		break;
	}
	this->addUnit(key, result);
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

