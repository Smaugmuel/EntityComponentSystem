#pragma once
#include <chrono>
#include <string>
#include <vector>

namespace Timer
{
	using Storage = std::vector<std::pair<std::string, long long>>;
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	static Storage& getStorage()
	{
		static Storage s_instance;
		return s_instance;
	}

	struct Scoped final
	{
		explicit Scoped(const std::string& str) : label(std::move(str)), start(Clock::now()) { }
		~Scoped() { getStorage().push_back({ label, (Clock::now() - start).count() }); }

	public:
		std::string label;
		TimePoint start;
	};

	struct Stopped final
	{
		explicit Stopped(const std::string& str) : label(std::move(str)), start(Clock::now()) { }
		void stop() { getStorage().push_back({ label, (Clock::now() - start).count() }); };

	public:
		std::string label;
		TimePoint start;
	};
};