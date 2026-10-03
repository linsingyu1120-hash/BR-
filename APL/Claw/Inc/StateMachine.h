
#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief 状态机事件类型
     */
    typedef enum
    {
        SM_EVENT_NONE = 0, // 无事件
        SM_EVENT_ENTRY, // 进入状态事件
        SM_EVENT_EXIT, // 退出状态事件
        SM_EVENT_REMOTE, // IDLE事件
        SM_EVENT_GETSKY, //去传递区天空块事件
        SM_EVENT_PUTSKY, //建塔放天空块事件
        SM_EVENT_SPINSKY, //翻转对方的天空块事件

    } SM_EventType;

    typedef struct SM_State SM_State;

    /**
     * @brief 状态机事件结构体
     * @details 用于传递事件信息
     */
    typedef struct
    {
        SM_EventType type; // 事件类型
    } SM_Event;

    /**
     * @brief 状态机结构体
     */
    typedef struct SM_StateMachine
    {
        SM_State *currentState;       /* 当前状态 */
        SM_State *previousState;      /* 前一状态 */
        SM_State *initialState;       /* 初始状态 */
        uint32_t lastStateChangeTime; /* 上次状态改变时间 */
        bool isInitialized;           /* 初始化标志 */
    } SM_StateMachine;

    /**
     * @brief 状态处理函数类型
     * @param stateMachine 状态机指针
     * @param event 事件指针
     * @return 下一状态指针，NULL表示保持当前状态
     */
    typedef SM_State *(*SM_StateHandler)(SM_StateMachine *stateMachine, const SM_Event *event);

    /**
     * @brief 状态结构体
     */
    struct SM_State
    {
        SM_StateHandler handler; /* 状态处理函数 */
        SM_State *parentState;   /* 父状态指针，用于层级状态机 */
        void *data;              /* 状态数据指针，用于保存状态机内部数据 */
    };

    /**
     * @brief 状态机转换结构体
     */
    typedef struct
    {
        SM_State *fromState;                                        /* 源状态 */
        SM_EventType eventType;                                     /* 触发事件类型 */
        SM_State *toState;                                          /* 目标状态 */
        bool (*guard)(SM_StateMachine *sm, const SM_Event *event);  /* 转换条件函数 */
        void (*action)(SM_StateMachine *sm, const SM_Event *event); /* 转换动作函数 */
    } SM_Transition;

    /**
     * @brief 初始化状态机
     * @param stateMachine 状态机指针
     * @param initialState 初始状态
     * @return 初始化结果，0表示成功，非0表示失败
     */
    int SM_Init(SM_StateMachine *stateMachine, SM_State *initialState);

    /**
     * @brief 处理事件
     * @param stateMachine 状态机指针
     * @param event 事件指针
     * @return 处理结果，0表示成功，非0表示失败
     */
    int SM_ProcessEvent(SM_StateMachine *stateMachine, const SM_Event *event);

    /**
     * @brief 切换状态
     * @param stateMachine 状态机指针
     * @param newState 新状态指针
     * @return 切换结果，0表示成功，非0表示失败
     */
    int SM_TransitionTo(SM_StateMachine *stateMachine, SM_State *newState);

    /**
     * @brief 重置状态机到初始状态
     * @param stateMachine 状态机指针
     * @return 重置结果，0表示成功，非0表示失败
     */
    int SM_Reset(SM_StateMachine *stateMachine);

    /**
     * @brief 获取当前状态
     * @param stateMachine 状态机指针
     * @return 当前状态指针
     */
    SM_State *SM_GetCurrentState(const SM_StateMachine *stateMachine);

    /**
     * @brief 获取前一状态
     * @param stateMachine 状态机指针
     * @return 前一状态指针
     */
    SM_State *SM_GetPreviousState(const SM_StateMachine *stateMachine);

    /**
     * @brief 获取状态机是否处于指定状态
     * @param stateMachine 状态机指针
     * @param state 要检查的状态指针
     * @return true表示处于指定状态，false表示不处于
     */
    bool SM_IsInState(const SM_StateMachine *stateMachine, const SM_State *state);

    /**
     * @brief 添加状态转换
     * @param transitions 转换数组指针
     * @param maxTransitions 转换数组最大容量
     * @param fromState 源状态
     * @param eventType 触发事件类型
     * @param toState 目标状态
     * @param guard 转换条件函数指针，可为NULL
     * @param action 转换动作函数指针，可为NULL
     * @return 添加结果，0表示成功，非0表示失败
     */
    int SM_AddTransition(SM_Transition *transitions, int maxTransitions, SM_State *fromState,
                         SM_EventType eventType, SM_State *toState,
                         bool (*guard)(SM_StateMachine *, const SM_Event *),
                         void (*action)(SM_StateMachine *, const SM_Event *));

    /**
     * @brief 处理状态转换
     * @param stateMachine 状态机指针
     * @param transitions 转换数组指针
     * @param numTransitions 转换数组实际大小
     * @param event 事件指针
     * @return 处理结果，0表示成功，非0表示失败
     */
    int SM_ProcessTransitions(SM_StateMachine *stateMachine, const SM_Transition *transitions,
                              int numTransitions, const SM_Event *event);

    /**
     * @brief 创建事件
     * @param eventType 事件类型
     * @param data 事件需要传入的控制参数指针，可为NULL
     * @param dataSize 事件参数数据大小
     * @return 创建的事件
     */
    SM_Event SM_CreateEvent(SM_EventType eventType /*, void *data, uint16_t dataSize*/);

    /**
     * @brief 检查状态机是否已初始化
     * @param stateMachine 状态机指针
     * @return true表示已初始化，false表示未初始化
     */
    bool SM_IsInitialized(const SM_StateMachine *stateMachine);

#ifdef __cplusplus
}
#endif

#endif /* __STATEMACHINE_H__ */