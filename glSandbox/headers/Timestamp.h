#pragma once
#include <chrono>

class Timestamp
{
private:
	using chrono = std::chrono::steady_clock;
	chrono::time_point timestamp = chrono::now();

public:
	Timestamp() = default;
	Timestamp(Timestamp const&) = default;
	Timestamp(Timestamp&&) = default;
	Timestamp& operator=(Timestamp const&) = default;
	Timestamp& operator=(Timestamp&&) = default;
	~Timestamp() = default;

public:
	void update()
	{
		timestamp = chrono::now();
	}
	
	void reset()
	{
		timestamp = chrono::time_point::min();
	}

	bool isNewerThan(Timestamp const& other) const
	{
		return this->timestamp > other.timestamp;
	}

	bool isOlderThan(Timestamp const& other) const
	{
		return this->timestamp < other.timestamp;
	}
};
