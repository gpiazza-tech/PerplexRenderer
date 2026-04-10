#pragma once

class Example
{
public:
	Example() {}
	virtual ~Example() {}

	virtual const char* Name() = 0;

	virtual void Enter() = 0;
	virtual void Update(float ts) = 0;
	virtual void Exit() = 0;
};