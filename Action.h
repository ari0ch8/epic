#pragma once

class Action
{
public:
	Action(void);
public:
	~Action(void);


	enum ActionType
	{
		None,
		Talk,
		Attack
	};

	ActionType mActionType;
};
