#include "myState.h"

/*爪子任务静态变量*/




/*取传递区天空块*/
SM_State *State_GetSky(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:


    case SM_EVENT_EXIT: //退出状态时的操作

    case SM_EVENT_GETSKY:
        switch (getsky_phase)
        {
        }
    default:
        break;
    }
    return NULL;
}


/*建塔放天空块*/
SM_State *State_PutSky(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:

    case SM_EVENT_EXIT: //退出状态时的操作

    case SM_EVENT_PUTSKY:
        switch (putsky_phase)
        {
        }
    default:
        break;
    }
    return NULL;
}

/*翻转对方的天空块*/
SM_State *State_SpinSky(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:

    case SM_EVENT_EXIT: //退出状态时的操作

    case SM_EVENT_SPINSKY:
        switch (spinsky_phase)
        {
        }
    default:
        break;
    }
    return NULL;
}
