/*
 *  Some stolen code from
 *  Copyright (C) 2014 -2016  Espressif System
 *
 *  To complement gpio.h from sdk.
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <eagle_soc.h>
#include <gpio.h>
#include "gpio_register.h"

#define GPIO_Pin(i)             (BIT(i))

#define GPIO_Pin_0 (BIT(0)) /* Pin 0 selected */
#define GPIO_Pin_1 (BIT(1)) /* Pin 1 selected */
#define GPIO_Pin_2 (BIT(2)) /* Pin 2 selected */
#define GPIO_Pin_3 (BIT(3)) /* Pin 3 selected */
#define GPIO_Pin_4 (BIT(4)) /* Pin 4 selected */
#define GPIO_Pin_5 (BIT(5)) /* Pin 5 selected */
#define GPIO_Pin_6 (BIT(6)) /* Pin 6 selected */
#define GPIO_Pin_7 (BIT(7)) /* Pin 7 selected */
#define GPIO_Pin_8 (BIT(8)) /* Pin 8 selected */
#define GPIO_Pin_9 (BIT(9)) /* Pin 9 selected */
#define GPIO_Pin_10 (BIT(10)) /* Pin 10 selected */
#define GPIO_Pin_11 (BIT(11)) /* Pin 11 selected */
#define GPIO_Pin_12 (BIT(12)) /* Pin 12 selected */
#define GPIO_Pin_13 (BIT(13)) /* Pin 13 selected */
#define GPIO_Pin_14 (BIT(14)) /* Pin 14 selected */
#define GPIO_Pin_15 (BIT(15)) /* Pin 15 selected */
#define GPIO_Pin_All (0xFFFF) /* All pins selected */

#define GPIO_PIN_REG_0          PERIPHS_IO_MUX_GPIO0_U
#define GPIO_PIN_REG_1          PERIPHS_IO_MUX_U0TXD_U
#define GPIO_PIN_REG_2          PERIPHS_IO_MUX_GPIO2_U
#define GPIO_PIN_REG_3          PERIPHS_IO_MUX_U0RXD_U
#define GPIO_PIN_REG_4          PERIPHS_IO_MUX_GPIO4_U
#define GPIO_PIN_REG_5          PERIPHS_IO_MUX_GPIO5_U
#define GPIO_PIN_REG_6          PERIPHS_IO_MUX_SD_CLK_U
#define GPIO_PIN_REG_7          PERIPHS_IO_MUX_SD_DATA0_U
#define GPIO_PIN_REG_8          PERIPHS_IO_MUX_SD_DATA1_U
#define GPIO_PIN_REG_9          PERIPHS_IO_MUX_SD_DATA2_U
#define GPIO_PIN_REG_10         PERIPHS_IO_MUX_SD_DATA3_U
#define GPIO_PIN_REG_11         PERIPHS_IO_MUX_SD_CMD_U
#define GPIO_PIN_REG_12         PERIPHS_IO_MUX_MTDI_U
#define GPIO_PIN_REG_13         PERIPHS_IO_MUX_MTCK_U
#define GPIO_PIN_REG_14         PERIPHS_IO_MUX_MTMS_U
#define GPIO_PIN_REG_15         PERIPHS_IO_MUX_MTDO_U

#define GPIO_PIN_REG(i) \
    (i==0) ? GPIO_PIN_REG_0:  \
    (i==1) ? GPIO_PIN_REG_1:  \
    (i==2) ? GPIO_PIN_REG_2:  \
    (i==3) ? GPIO_PIN_REG_3:  \
    (i==4) ? GPIO_PIN_REG_4:  \
    (i==5) ? GPIO_PIN_REG_5:  \
    (i==6) ? GPIO_PIN_REG_6:  \
    (i==7) ? GPIO_PIN_REG_7:  \
    (i==8) ? GPIO_PIN_REG_8:  \
    (i==9) ? GPIO_PIN_REG_9:  \
    (i==10)? GPIO_PIN_REG_10: \
    (i==11)? GPIO_PIN_REG_11: \
    (i==12)? GPIO_PIN_REG_12: \
    (i==13)? GPIO_PIN_REG_13: \
    (i==14)? GPIO_PIN_REG_14: \
    GPIO_PIN_REG_15



typedef enum {
    GPIO_Mode_Input = 0x0,
    GPIO_Mode_Out_OD,
    GPIO_Mode_Output ,
    GPIO_Mode_Sigma_Delta ,
} GPIO_Mode;

typedef enum {
    GPIO_Pull_DIS = 0x0,
    GPIO_Pullup_EN  = 0x1,
    GPIO_Pulldown_EN  = 0x2,
} GPIO_Pull;

#define GPIO_AS_INPUT(gpio_no)      gpio_output_set(0, 0, 1<<gpio_no,0)
#define GPIO_AS_OUTPUT(gpio_no)     gpio_output_set(0, 0, 0         , 1<<gpio_no)

void gpio_config(int gpio_num,GPIO_Mode mode, GPIO_Pull pull, GPIO_INT_TYPE intMode  );
void gpio_pin_wakeup_enable(uint32_t i, GPIO_INT_TYPE intr_state);
void gpio_pin_wakeup_disable();

#endif
