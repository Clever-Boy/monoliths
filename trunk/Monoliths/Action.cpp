#include "stdafx.h"
#include "Action.h"

Action* const Action::DEFAULT = new Action();
Action* const Action::ROBOT_IDLE = new Idle("Idle");
Action* const Action::ROBOT_WALK = new Movement(250, "Walk");