#include "StateMachine.h"
#include "main.h"
#include <string.h>
#include <stdio.h>


#define MAX_DEPTH 3

/*初始化状态机*/
int SM_Init(SM_StateMachine *stateMachine, SM_State *initialState)
{
    if (stateMachine == NULL || initialState == NULL)
    {
        return -1;
    }
    stateMachine->currentState = NULL;
    stateMachine->previousState = NULL;
    stateMachine->initialState = initialState;
    stateMachine->lastStateChangeTime = 0;
    stateMachine->isInitialized = false;

    // 切换到初始状态
    int result = SM_TransitionTo(stateMachine, initialState);
    if (result == 0)
    {
        stateMachine->isInitialized = true;
    }

    return result;
}

/*处理事件*/
int SM_ProcessEvent(SM_StateMachine *stateMachine, const SM_Event *event)
{
    if (stateMachine == NULL || !stateMachine->isInitialized ||
        stateMachine->currentState == NULL || stateMachine->currentState->handler == NULL)
    {
        return -1;
    }

    SM_Event noneEvent;
    if (event == NULL)
    {
        noneEvent = SM_CreateEvent(SM_EVENT_NONE);
        event = &noneEvent;
    }

    SM_State *nextState = stateMachine->currentState->handler(stateMachine, event);
    if (nextState != NULL && nextState != stateMachine->currentState)
    {
        return SM_TransitionTo(stateMachine, nextState);
    }

    return 0;
}

/*切换状态*/
int SM_TransitionTo(SM_StateMachine *stateMachine, SM_State *newState)
{
    if (stateMachine == NULL || newState == NULL || newState->handler == NULL)
    {
        return -1;
    }
    SM_State *oldState = stateMachine->currentState;

    // 在退出旧状态前检查目标层级，防止祖先数组越界。
    SM_State *ancestors[MAX_DEPTH];
    int depth = 0;
    SM_State *temp = newState;
    while (temp != NULL)
    {
        if (depth >= MAX_DEPTH || temp->handler == NULL)
        {
            return -1;
        }
        ancestors[depth++] = temp;
        temp = temp->parentState;
    }

    // 退出当前状态及其所有祖先状态
    SM_Event exitEvent = SM_CreateEvent(SM_EVENT_EXIT /*, NULL, 0*/);
    temp = oldState;
    while (temp != NULL)
    {
        if (temp->handler != NULL)
        {
            temp->handler(stateMachine, &exitEvent);
        }
        temp = temp->parentState;
    }

    // 进入新状态及其所有祖先状态
    SM_Event entryEvent = SM_CreateEvent(SM_EVENT_ENTRY /*, NULL, 0*/);
    // 从顶层祖先开始进入
    for (int i = depth - 1; i >= 0; i--)
    {
        if (ancestors[i]->handler != NULL)
        {
            ancestors[i]->handler(stateMachine, &entryEvent);
        }
    }
    stateMachine->previousState = oldState;
    stateMachine->currentState = newState;
    stateMachine->lastStateChangeTime = HAL_GetTick();

    return 0;
}

/*重置状态机到初始状态*/
int SM_Reset(SM_StateMachine *stateMachine)
{
    if (stateMachine == NULL || stateMachine->initialState == NULL)
    {
        return -1;
    }

    return SM_TransitionTo(stateMachine, stateMachine->initialState);
}

/*获取当前状态*/
SM_State *SM_GetCurrentState(const SM_StateMachine *stateMachine)
{
    if (stateMachine == NULL)
    {
        return NULL;
    }

    return stateMachine->currentState;
}

/*获取前一状态*/
SM_State *SM_GetPreviousState(const SM_StateMachine *stateMachine)
{
    if (stateMachine == NULL)
    {
        return NULL;
    }

    return stateMachine->previousState;
}

/*获取状态机是否处于指定状态*/
bool SM_IsInState(const SM_StateMachine *stateMachine, const SM_State *state)
{
    if (stateMachine == NULL || state == NULL)
    {
        return false;
    }

    const SM_State *currentState = stateMachine->currentState;

    // 检查当前状态是否为指定状态
    while (currentState != NULL)
    {
        if (currentState == state)
        {
            return true;
        }

        // 检查父状态
        currentState = currentState->parentState;
    }

    return false;
}

/*添加状态转换*/
int SM_AddTransition(SM_Transition *transitions, int maxTransitions, SM_State *fromState,
                     SM_EventType eventType, SM_State *toState,
                     bool (*guard)(SM_StateMachine *, const SM_Event *),
                     void (*action)(SM_StateMachine *, const SM_Event *))
{
    if (transitions == NULL || maxTransitions <= 0 || fromState == NULL || toState == NULL)
    {
        return -1;
    }
    for (int i = 0; i < maxTransitions; i++)
    {
        if (transitions[i].fromState == NULL)
        {
            transitions[i].fromState = fromState;
            transitions[i].eventType = eventType;
            transitions[i].toState = toState;
            transitions[i].guard = guard;
            transitions[i].action = action;
            return 0;
        }
    }
    return -2;
}

/*处理状态转换*/
int SM_ProcessTransitions(SM_StateMachine *stateMachine, const SM_Transition *transitions,
                          int numTransitions, const SM_Event *event)
{
    if (stateMachine == NULL || !stateMachine->isInitialized || transitions == NULL || numTransitions <= 0)
    {
        return -1;
    }

    SM_Event noneEvent;
    if (event == NULL)
    {
        noneEvent = SM_CreateEvent(SM_EVENT_NONE);
        event = &noneEvent;
    }

    for (int i = 0; i < numTransitions; i++)
    {
        if (transitions[i].fromState != NULL &&
            transitions[i].fromState == stateMachine->currentState &&
            transitions[i].eventType == event->type)
        {
            if (transitions[i].guard != NULL && !transitions[i].guard(stateMachine, event))
            {
                continue;
            }
            if (transitions[i].action != NULL)
            {
                transitions[i].action(stateMachine, event);
            }
            return SM_TransitionTo(stateMachine, transitions[i].toState);
        }
    }
    return SM_ProcessEvent(stateMachine, event);
}

/*创建事件*/
SM_Event SM_CreateEvent(SM_EventType eventType /*, void *data, uint16_t dataSize*/)
{
    SM_Event event;
    event.type = eventType;
    // event.data = data;
    // event.dataSize = dataSize;
    return event;
}

/*检查状态机是否已经初始化*/
bool SM_IsInitialized(const SM_StateMachine *stateMachine)
{
    if (stateMachine == NULL)
    {
        return false;
    }

    return stateMachine->isInitialized;
}