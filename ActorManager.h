#pragma once

#include "actor.h"

class ActorManager
{
public:
	ActorManager(void);
public:
	~ActorManager(void);

	void Init(void);

	Actor*		mActors;
};
