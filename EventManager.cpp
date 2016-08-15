#include "EventManager.h"

EventManager::EventManager(void)
{
}

EventManager::~EventManager(void)
{
}

void EventManager::AddEventToLog(Event& theEvent)
{
	mEventLog.push_front(theEvent);

	while( mEventLog.size() > MAXNUMBEROFEVENTSINLOG )
	{
		mEventLog.pop_back();
	}
}

void EventManager::AddEventToQueue(Event &theEvent)
{
	// check time and insert into correct turn
	mEventQueue.push_front(theEvent);

	// todo: temp: auto send to log
	AddEventToLog(theEvent);
	mEventQueue.pop_front();
}
