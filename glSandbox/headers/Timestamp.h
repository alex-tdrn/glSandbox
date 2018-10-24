#pragma once
#include <chrono>

class Timestamp
{
private:
	using chrono = std::chrono::steady_clock;
	chrono::time_point timestamp = chrono::now();

protected:
	Timestamp() = default;
	Timestamp(Timestamp const&) = default;
	Timestamp(Timestamp&&) = default;
	Timestamp& operator=(Timestamp const&) = default;
	Timestamp& operator=(Timestamp&&) = default;
	~Timestamp() = default;

protected:
	void updateTimestamp()
	{
		timestamp = chrono::now();
	}

public:
	chrono::time_point const& getTimestamp() const
	{
		return timestamp;
	}

};