#pragma once
#include "Timestamp.h"
#include "ResourceWrapper.h"

template <typename OutputResource>
class Producer
{
protected:
	mutable ResourceWrapper<OutputResource> output;

protected:
	 mutable Timestamp settingsTimestamp;

public:
	Producer() 
	{
		output.timestamp.reset();
	};
	virtual ~Producer() = default;

protected:
	virtual OutputResource const* getResourceHandle() const = 0;
	virtual void update() const = 0; 
	void push() const
	{
		update();
		output.handle = getResourceHandle();
		output.timestamp.update();
	}

public:
	virtual ResourceWrapper<OutputResource> getOutput() const
	{
		if(settingsTimestamp.isNewerThan(output.timestamp))
			push();

		return output;
	}

};