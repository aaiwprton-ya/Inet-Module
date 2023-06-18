#ifndef INET_PROCESSOR_H
#define INET_PROCESSOR_H

#include "std_afx.h"
#include "inetutils.h"

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
	
typedef std::function<std::string(UnitBridge& bridge)> UnitType;

typedef std::pair<void*, size_t> RecvPoolStaticEntityType;
#define RecvPoolStaticEntity(elementA, elementB) std::pair<void*, size_t>((elementA), (elementB))

typedef void* RecvPoolMutableEntityType; // static_cast<std::vector<uint8_t>*>(void*);
#define RecvPoolMutableEntityCast(poolEntity) static_cast<std::vector<uint8_t>*>(poolEntity)

typedef std::function<RecvPoolMutableEntityType(RecvPoolMutableEntityType targetBuffer, const void* anySources, size_t sourcesSize)> DataGeneratorType;
typedef std::pair<RecvPoolMutableEntityType, DataGeneratorType> RecvPoolGenerativeEntityType;
#define RecvPoolGenerativeEntity(elementA, elementB) std::pair<RecvPoolMutableEntityType, DataGeneratorType>((elementA), (elementB))

typedef std::map<std::string, RecvPoolStaticEntityType> RecvPoolStaticType;
typedef std::map<std::string, RecvPoolMutableEntityType> RecvPoolMutableType;
typedef std::map<std::string, RecvPoolGenerativeEntityType> RecvPoolGenerativeType;

class Processor
{
public:
	enum UnitTemplate {
		UT_STATE_TO_GET, 
		UT_GET_TO_AGREEMENT_STATIC, 
		UT_GET_TO_AGREEMENT_MUTABLE, 
		UT_GET_TO_AGREEMENT_GENERATIVE, 
		UT_GET_TO_AGREEMENT_UNIVERSAL, 
		UT_ACCEPT_TO_READY, 
		UT_READY_TO_SEND, 
		UT_RECEIVE, 
		UT_ERROR};
	
	class RecvBufferTypeBase
	{
	public:
		virtual ~RecvBufferTypeBase(){}
	public:
		virtual size_t getSize() const = 0;
		virtual void setSize(size_t value) = 0;
		virtual void* getBuffer() = 0;
		virtual void fillBuffer(const void* data) = 0;
	};
	
	template<class T>
	class RecvBufferType : public RecvBufferTypeBase
	{
	private:
		size_t size = 0;
		T* recvBuffer = nullptr;
	public:
		RecvBufferType(){}
		RecvBufferType(size_t size, T* recvBuffer)
			: size(size), recvBuffer(recvBuffer)
		{
			recvBuffer = static_cast<T*>(malloc(size));
		}
		~RecvBufferType()
		{
			if (this->recvBuffer != nullptr)
			{
				free(this->recvBuffer);
				this->recvBuffer = nullptr;
			}
		}
	public:
		size_t getSize() const
		{
			return this->size;
		}
		void setSize(size_t value)
		{
			if (this->recvBuffer != nullptr)
			{
				free(this->recvBuffer);
				this->recvBuffer = nullptr;
			}
			this->size = value;
			this->recvBuffer = static_cast<T*>(malloc(size));
		}
		void* getBuffer()
		{
			return (void*)this->recvBuffer;
		}
		void fillBuffer(const void* data)
		{
			if (this->recvBuffer != nullptr && data != nullptr)
			{
				memcpy(this->recvBuffer, data, this->size);
			}
		}
	};
	
	struct UnitTemplateType
	{
		UnitTemplate unitTemplate;
		std::string nextUnit;
		std::string backUnit;
		std::string errorUnit;
		std::string getValue;
		RecvBufferTypeBase* recvBuffer = nullptr;
		RecvPoolStaticType recvPoolStatic;
		RecvPoolMutableType recvPoolMutable;
		RecvPoolGenerativeType recvPoolGenerative;
		const void* generativeSources;
		size_t generativeSourcesSize;
	};
private:
	InetUtils* utils;
	std::map<std::string, UnitType> procUnits;
	std::string state;
	std::string savedState;
	std::string start;
	std::string exit;
	std::string error;
public:
	Processor(InetUtils* utils);
	~Processor();
public:
	void addUnit(const std::string& key, UnitType procUnit);
	//void addAsincUnit(const std::string& key, RequestResponceLambda procUnit); TODO
	void makeUnit(const std::string& key, const UnitTemplateType& unitTemplate);
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

