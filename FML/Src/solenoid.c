/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-24 16:51:06
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-09-10 15:35:16
 * @FilePath: \f4_show\FML\Src\Solenoid.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

 /*----------------------------------- UART 控制 --------------------------------------------------------*/
#include "solenoid.h"

Solenoid_t solenoid_Channel1 = {0};
Solenoid_t solenoid_Channel2 = {0};
Solenoid_t solenoid_Channel3 = {0};

void solenoid_channel_init(Solenoid_t *solenoid, GPIO_TypeDef *gpio_port, uint16_t gpio_pin_sda, uint16_t gpio_pin_clk)
{
    solenoid->gpio_port = gpio_port;
    solenoid->gpio_pin_sda = gpio_pin_sda;
    solenoid->gpio_pin_clk = gpio_pin_clk;
    solenoid->data_prve = 0xF0;
}
// usart_channel=串口号 不需要在cube中配置 直接调用即可
void solenoid_init(uint8_t usart_channel)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    switch (usart_channel)
    {
    case 1:
        solenoid_channel_init(&solenoid_Channel1, GPIOA, GPIO_PIN_9, GPIO_PIN_10);
        __HAL_RCC_GPIOA_CLK_ENABLE();
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        break;
    case 2:
        solenoid_channel_init(&solenoid_Channel2, GPIOA, GPIO_PIN_2, GPIO_PIN_3);
        __HAL_RCC_GPIOA_CLK_ENABLE();
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_RESET);
        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        break;
    case 3:
        solenoid_channel_init(&solenoid_Channel3, GPIOC, GPIO_PIN_10, GPIO_PIN_11);
        __HAL_RCC_GPIOC_CLK_ENABLE();
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        break;
    default:
        break;
    }
    solenoid_on(usart_channel, 0);
}
void register_updata(Solenoid_t *solenoid, uint8_t *data)
{
    if (*data == solenoid->data_prve)
        return;
    solenoid->data_prve = *data;
    for (int i = 0; i < 4; i++)
    {
        if ((*data & 0x08) == 0x08)
        {
            HAL_GPIO_WritePin(solenoid->gpio_port, solenoid->gpio_pin_sda, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(solenoid->gpio_port, solenoid->gpio_pin_sda, GPIO_PIN_RESET);
        }
        *data <<= 1;
        HAL_GPIO_WritePin(solenoid->gpio_port, solenoid->gpio_pin_clk, GPIO_PIN_SET);
        HAL_GPIO_WritePin(solenoid->gpio_port, solenoid->gpio_pin_clk, GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(solenoid->gpio_port, solenoid->gpio_pin_clk, GPIO_PIN_SET);
    HAL_GPIO_WritePin(solenoid->gpio_port, solenoid->gpio_pin_clk, GPIO_PIN_RESET);
}
// usart_channel=串口号, cmd=命令(低4位控制) 0000 4321 （4321为对应通道）
void solenoid_on(uint8_t usart_channel, uint8_t cmd)
{
    uint8_t data = cmd & 0x0f;
    switch (usart_channel)
    {
    case 1:
        register_updata(&solenoid_Channel1, &data);
        break;
    case 2:
        register_updata(&solenoid_Channel2, &data);
        break;
    case 3:
        register_updata(&solenoid_Channel3, &data);
        break;
    default:
        break;
    }
}


/*----------------------------------- CAN 控制 --------------------------------------------------------*/
// #include "solenoid.h"

// /** @brief 本模块使用的 CAN 句柄(由 solenoid_init 绑定) */
// static CAN_HandleTypeDef *s_solenoid_can = NULL;

// /** @brief 最后一次成功下发的控制字节(低 4 位有效),仅用于调试读回 */
// static uint8_t s_solenoid_state = 0x00U;

// /**
//  * @brief 取本模块使用的 CAN 总线句柄。
//  */
// static CAN_HandleTypeDef *Solenoid_GetCan(void)
// {
// #if (SOLENOID_CAN_BUS == 0U)
//     return &hcan1;
// #elif (SOLENOID_CAN_BUS == 1U)
//     return &hcan2;
// #else
//     return NULL;
// #endif
// }

// /**
//  * @brief 初始化电磁阀控制模块(绑定 CAN 句柄)。
//  * @note  须在 MX_CANx_Init() 之后调用。
//  */
// void solenoid_init(void)
// {
//     s_solenoid_can = Solenoid_GetCan();
//     s_solenoid_state = 0x00U;
// }

// /**
//  * @brief 按位图设置 4 路电磁阀输出,并通过 CAN 下发。
//  * @param usart_channel 历史遗留参数(保留以兼容 Claw.c 的调用),不参与报文组装
//  * @param cmd 控制字节,仅低 4 位有效:bit0->CH1, bit1->CH2, bit2->CH3, bit3->CH4, 1=开 0=关
//  */
// void solenoid_on(uint8_t usart_channel, uint8_t cmd)
// {
//     CAN_TxHeaderTypeDef tx_header;
//     uint32_t tx_mailbox;
//     uint8_t tx_data[1];
//     uint8_t data = cmd & 0x0FU;

//     (void)usart_channel; /* 报文内容只由 data 决定,通道参数仅用于兼容旧接口 */

//     s_solenoid_state = data;

//     if (s_solenoid_can == NULL)
//         return;

//     tx_data[0] = data;

//     tx_header.StdId = SOLENOID_CAN_ID;
//     tx_header.ExtId = 0U;
//     tx_header.IDE = CAN_ID_STD;
//     tx_header.RTR = CAN_RTR_DATA;
//     tx_header.DLC = 1U;
//     tx_header.TransmitGlobalTime = DISABLE;

//     /* 每次调用都实际下发一帧:不做"值未变化则跳过"的去重,
//        避免电磁阀板漏收一帧后相同的值永远发不出去。 */
//     if (HAL_CAN_AddTxMessage(s_solenoid_can, &tx_header, tx_data, &tx_mailbox) != HAL_OK)
//     {
//         /* 发送失败(无空闲邮箱/总线异常):清空发送请求,避免占用邮箱影响后续发送 */
//         (void)HAL_CAN_AbortTxRequest(s_solenoid_can, 0x07U);
//     }
// }

// /**
//  * @brief 读回最后一次下发的控制字节(低 4 位有效)。主要用于调试。
//  */
// uint8_t solenoid_get_state(void)
// {
//     return s_solenoid_state;
// }