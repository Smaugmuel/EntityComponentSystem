#pragma once
#include "SparseSet.hpp"
#include <string>
#include <chrono>
#include <Psapi.h>

template<typename T>
class SparseSetBenchmark final
{
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using SparseSetIndex = typename SparseSet<T>::IndexType;

	struct Results
	{
		size_t creationTime = { 0 };
		size_t removalTime = { 0 };
		size_t recreationTime = { 0 };
		size_t halfRemovalTime = { 0 };
		size_t halfCreationTime = { 0 };
		size_t checkTime = { 0 };
		size_t getTime = { 0 };

		size_t initialSize = { 0 };
		size_t creationSize = { 0 };
		size_t removalSize = { 0 };
		size_t recreationSize = { 0 };
		size_t halfRemovalSize = { 0 };
		size_t halfCreationSize = { 0 };
		size_t checkSize = { 0 };
		size_t getSize = { 0 };

		size_t initialWindowsSize = { 0 };
		size_t totalWindowsSize = { 0 };
	};

public:

	SparseSetBenchmark() = default;
	~SparseSetBenchmark() = default;

	void runBenchMark()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		constexpr size_t N_ELEMS = 100'000'000ui64;

		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		m_results.initialWindowsSize = pmc.WorkingSetSize;

		m_results.initialSize = m_sparseSet.getByteSize();

		m_results.creationTime = create(N_ELEMS);
		m_results.creationSize = m_sparseSet.getByteSize();

		m_results.removalTime = destroyFirst(N_ELEMS);
		m_results.removalSize = m_sparseSet.getByteSize();

		m_results.recreationTime = create(N_ELEMS);
		m_results.recreationSize = m_sparseSet.getByteSize();

		m_results.halfRemovalTime = destroyFirst(N_ELEMS / 2);
		m_results.halfRemovalSize = m_sparseSet.getByteSize();

		m_results.halfCreationTime = create(N_ELEMS / 2);
		m_results.halfCreationSize = m_sparseSet.getByteSize();

		m_results.checkTime = check(N_ELEMS);
		m_results.checkSize = m_sparseSet.getByteSize();

		m_results.getTime = get(N_ELEMS);
		m_results.getSize = m_sparseSet.getByteSize();

		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		m_results.totalWindowsSize = pmc.WorkingSetSize;

		constructString(N_ELEMS);
	}

	const std::string& getResultString() const
	{
		return m_resultString;
	}

	size_t getBenchmarkByteSize() const
	{
		size_t size = 0;
		size += sizeof(Results);
		size += sizeof(std::string);
		size += sizeof(unsigned char) * m_resultString.capacity();
		return size;
	}
	size_t getSparseSetByteSize() const
	{
		return m_sparseSet.getByteSize();
	}

private:
	template<typename Func>
	size_t measureTime(Func f)
	{
		TimePoint t1 = Clock::now();
		f();
		TimePoint t2 = Clock::now();
		return (t2 - t1).count();
	}

	size_t create(const size_t N)
	{
		return measureTime([N, this]() { for (size_t i = 0; i < N; i++) m_sparseSet.add((SparseSetIndex)i); });
	}
	size_t destroyFirst(const size_t N)
	{
		return measureTime([N, this]() { for (size_t i = 0; i < N; i++) m_sparseSet.remove((SparseSetIndex)i); });
	}
	size_t check(const size_t N)
	{
		return measureTime([N, this]() { for (size_t i = 0; i < N; i++) m_sparseSet.has((SparseSetIndex)i); });
	}
	size_t get(const size_t N)
	{
		return measureTime([N, this]() { for (size_t i = 0; i < N; i++) m_sparseSet.get((SparseSetIndex)i); });
	}
	
	void constructString(const size_t N)
	{
		auto toMs = [](const size_t ns) { return ns / 1'000'000; };
		auto str = [](const size_t n) { return std::to_string(n); };
		auto toMsStr = [str, toMs](const size_t ns) { return str(toMs(ns)); };
		auto alignRight = [](const std::string& _str, const size_t totalLength)
		{
			const size_t nToAdd = totalLength - _str.size();
			std::string alignedStr = "";
			alignedStr.append(nToAdd, ' ');
			alignedStr.append(_str);
			return alignedStr;
		};

		const std::string actionHeader = "     Action ";
		const std::string amountHeader = "  Amount ";
		const std::string timeHeader = " Time (microS) ";
		const std::string memoryHeader = " Memory (B)";

		auto alignAction = [=](const std::string& action) { return alignRight(action, actionHeader.size()); };
		auto alignAmount = [=](const size_t n) { return alignRight(str(n), amountHeader.size()); };
		auto alignTime = [=](const size_t ns) { return alignRight(toMsStr(ns), timeHeader.size()); };
		auto alignMemory = [=](const size_t n) { return alignRight(str(n), memoryHeader.size()); };

		auto lineH = [=](const std::string& _str) { return std::string(_str.size(), '-'); };
		auto spacing = [=](const std::string& _str) { return std::string(_str.size(), ' '); };

		const std::string newStr =
			std::string(actionHeader              + "|" + amountHeader          + "|" + timeHeader                            + "|" + memoryHeader)                            + "\n" +
			std::string(lineH(actionHeader)       + "|" + lineH(amountHeader)   + "|" + lineH(timeHeader)                     + "|" + lineH(memoryHeader))                     + "\n" +
			std::string(alignAction("Initial")    + "|" + spacing(amountHeader) + "|" + spacing(timeHeader)                   + "|" + alignMemory(m_results.initialSize))      + "\n" +
			std::string(alignAction("Creating")   + "|" + alignAmount(N)        + "|" + alignTime(m_results.creationTime)     + "|" + alignMemory(m_results.creationSize))     + "\n" +
			std::string(alignAction("Removing")   + "|" + alignAmount(N)        + "|" + alignTime(m_results.removalTime)      + "|" + alignMemory(m_results.removalSize))      + "\n" +
			std::string(alignAction("Recreating") + "|" + alignAmount(N)        + "|" + alignTime(m_results.recreationTime)   + "|" + alignMemory(m_results.recreationSize))   + "\n" +
			std::string(alignAction("Removing")   + "|" + alignAmount(N/2)      + "|" + alignTime(m_results.halfRemovalTime)  + "|" + alignMemory(m_results.halfRemovalSize))  + "\n" +
			std::string(alignAction("Recreating") + "|" + alignAmount(N/2)      + "|" + alignTime(m_results.halfCreationTime) + "|" + alignMemory(m_results.halfCreationSize)) + "\n" +
			std::string(alignAction("Checking")   + "|" + alignAmount(N)        + "|" + alignTime(m_results.checkTime)        + "|" + alignMemory(m_results.checkSize))        + "\n" +
			std::string(alignAction("Retrieving") + "|" + alignAmount(N)        + "|" + alignTime(m_results.getTime)          + "|" + alignMemory(m_results.getSize))          + "\n" +
			lineH(actionHeader + "|" + amountHeader + "|" + timeHeader + "|" + memoryHeader)                                                                                   + "\n" +
			std::string(str(N) + " " + std::string(typeid(T).name()) + "s should use " + str(sizeof(T) * m_sparseSet.size()) + " B")                                             + "\n" +
			std::string("Windows memory before creation: " + str(m_results.initialWindowsSize) + " B")                                                                         + "\n" +
			std::string("Windows memory after creation:  " + str(m_results.totalWindowsSize) + " B")                                                                           + "\n" +
			std::string("Windows memory - data:          " + str(m_results.totalWindowsSize - m_results.initialWindowsSize) + " B")                                            + "\n";

		m_resultString = newStr;
	}

private:
	SparseSet<T> m_sparseSet;
	Results m_results;
	std::string m_resultString;
};