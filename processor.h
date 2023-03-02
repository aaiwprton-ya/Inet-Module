#ifndef INET_PROCESSOR_H
#define INET_PROCESSOR_H

#include "std_afx.h"

typedef std::function<std::string(
	const void* const request, 
	size_t requestSize, 
	const void** response, 
	size_t* responseSize)> RequestResponceLambda;

class Processor
{
private:
	std::map<std::string, RequestResponceLambda> procUnits;
	std::string state;
	std::string savedState;
	std::string start;
	std::string exit;
	std::string error;
public:
	Processor();
	~Processor();
public:
	void addUnit(const std::string& key, RequestResponceLambda procUnit);
	//void addAsincUnit(const std::string& key, RequestResponceLambda procUnit); TODO
	void operator()(
		const void* const request, 
		const size_t requestSize, 
		const void** response, 
		size_t* responseSize);
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

