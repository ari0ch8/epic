#pragma once

#include "event.h"

#include <list>

#define MAXNUMBEROFEVENTSINLOG		1024

class EventManager
{
public:
	EventManager(void);
public:
	~EventManager(void);

	void AddEventToQueue(Event& theEvent);
	void AddEventToLog(Event& theEvent);


	std::list<Event> mEventLog;
	std::list<Event> mEventQueue;
};
