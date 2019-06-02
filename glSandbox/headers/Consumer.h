#pragma once
#include "Producer.h"

template <typename InputResource>
class Consumer
{
protected:
	Producer<InputResource> const* connectedProducer;
	mutable ResourceWrapper<InputResource> input;

public:
	Consumer()
	{
		input.timestamp.reset();
	};
	virtual ~Consumer() = default;

protected:
	virtual void setResourceHandle(InputResource const*) = 0;
	void pull() const
	{
		auto newInput = connectedProducer->getOutput();
		if(newInput.timestamp >= input.timestamp)
		{
			input = newInput;
			setResourceHandle(input.handle);
		}
	}

};