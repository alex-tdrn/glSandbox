#pragma once
#include "Producer.h"
#include "Consumer.h"

template <typename InputResource, typename OutputResource>
class Processor : public Producer<OutputResource>, public Consumer<InputResource>
{
public:
	ResourceWrapper<OutputResource> getOutput() const final override
	{
		pullInput();

		if(input.timestamp.isNewerThan(output.timestamp))
		{
			push();
			return output
		}

		return Producer<OutputResource>::getOutput();
	}
};