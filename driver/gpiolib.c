/*
 * Some stolen code from
 * Copyright (C) 2014 -2016  Espressif System
 *
 */


#include "gpiolib.h"

#include "ets_sys.h"
#include "osapi.h"


void ICACHE_FLASH_ATTR
gpio_config(int gpio_num,GPIO_Mode mode, GPIO_Pull pull, GPIO_INT_TYPE intMode  )
{
     uint16_t gpio_pin_mask = GPIO_Pin(gpio_num);
     uint32_t io_reg;
     uint8_t io_num = 0;
     uint32_t pin_reg;

     if (mode == GPIO_Mode_Input)
     {
         GPIO_AS_INPUT(gpio_num);
         os_printf("GPIO %d as input\n",gpio_num);
     }
     else if (mode == GPIO_Mode_Output)
     {
         GPIO_AS_OUTPUT(gpio_num);
         os_printf("GPIO %d as output\n",gpio_num);
     }

     io_reg = GPIO_PIN_REG(gpio_num);

     if ( gpio_pin_mask & (GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5))
     {
         PIN_FUNC_SELECT(io_reg, 0);
     }
     else
     {
         PIN_FUNC_SELECT(io_reg, 3);
     }

     PIN_PULLUP_DIS(io_reg);
     PIN_PULLDWN_DIS(io_reg);
     if (pull == GPIO_Pullup_EN)
     {
         PIN_PULLUP_EN(io_reg);
         os_printf("GPIO %d pullup enable\n",gpio_num);
     }
     else if (pull == GPIO_Pulldown_EN)
     {
         PIN_PULLDWN_EN(io_reg);
         os_printf("GPIO %d down enable\n",gpio_num);
     }

     if (mode == GPIO_Mode_Out_OD)
     {
         os_printf("GPIO %d Open Drain\n",gpio_num);

         pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(gpio_num));
         pin_reg &= (~GPIO_PIN_PAD_DRIVER_MASK);
         pin_reg |= (GPIO_PAD_DRIVER_ENABLE << GPIO_PIN_PAD_DRIVER_LSB);
         GPIO_REG_WRITE(GPIO_PIN_ADDR(gpio_num), pin_reg);

     }
     else if(mode == GPIO_Mode_Sigma_Delta)
     {

         os_printf("GPIO %d Sigma delta\n",gpio_num);
         pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(gpio_num));
         pin_reg &= (~GPIO_PIN_SOURCE_MASK);
         pin_reg |= (0x1 << GPIO_PIN_SOURCE_LSB);
         GPIO_REG_WRITE(GPIO_PIN_ADDR(gpio_num), pin_reg);
         GPIO_REG_WRITE(GPIO_SIGMA_DELTA_ADDRESS, SIGMA_DELTA_ENABLE);
     }

     gpio_pin_intr_state_set(gpio_num, intMode);
}

/*
  only highlevel and lowlevel intr can use for wakeup
*/
void ICACHE_FLASH_ATTR
gpio_pin_wakeup_enable(uint32_t i, GPIO_INT_TYPE intMode)
{
    uint32_t pin_reg;

    if ((intMode == GPIO_PIN_INTR_LOLEVEL) || (intMode == GPIO_PIN_INTR_HILEVEL))
    {
        pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));
        pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
        pin_reg |= (intMode << GPIO_PIN_INT_TYPE_LSB);
        pin_reg |= GPIO_PIN_WAKEUP_ENABLE_SET(GPIO_WAKEUP_ENABLE);
        GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);
    }
}

void ICACHE_FLASH_ATTR
gpio_pin_wakeup_disable(void)
{
    uint8_t  i;
    uint32_t pin_reg;

    for (i = 0; i < GPIO_PIN_COUNT; i++)
    {
        pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));

        if (pin_reg & GPIO_PIN_WAKEUP_ENABLE_MASK)
        {
            pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
            pin_reg |= (GPIO_PIN_INTR_DISABLE << GPIO_PIN_INT_TYPE_LSB);
            pin_reg &= ~(GPIO_PIN_WAKEUP_ENABLE_SET(GPIO_WAKEUP_ENABLE));
            GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);
        }
    }
}

