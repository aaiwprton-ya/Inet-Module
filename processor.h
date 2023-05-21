#ifndef INET_PROCESSOR_H
#define INET_PROCESSOR_H

#include "std_afx.h"

struct UnitBridge
{
	const void* request = nullptr;
	size_t requestSize = 0;
	const void* response = nullptr;
	size_t responseSize = 0;
	void** plannedData = nullptr;
	uint64_t* plannedSize = nullptr;
	uint64_t* expectedSize = nullptr;
};

/*typedef std::function<std::string(
	const void* const request, 
	size_t requestSize, 
	const void** response, 
	size_t* responseSize,
	void*& plannedData, 
	uint64_t& plannedSize, 
	uint64_t& expectedSize)> RequestResponceLambda;*/
	
typedef std::function<std::string(UnitBridge& bridge)> UnitType;

class Processor
{
private:
	std::map<std::string, UnitType> procUnits;
	std::string state;
	std::string savedState;
	std::string start;
	std::string exit;
	std::string error;
public:
	Processor();
	~Processor();
public:
	void addUnit(const std::string& key, UnitType procUnit);
	//void addAsincUnit(const std::string& key, RequestResponceLambda procUnit); TODO
	void operator()(UnitBridge& bridge);
	void setStartState(const std::string& key) noexcept;
	void setExitState(const std::string& key) noexcept;
	void setErrorState(const std::string& key) noexcept;
	void setupStartState() noexcept;
	void setupExitState() noexcept;
	void setupErrorState() noexcept;
	void setupSavedState() noexcept;
	bool isExit() const noexcept;
};

#endif // INET_PROCESSOR_H

