#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"

#include "dht11_lib.h"

#include "gpiolib.h"

#define GPIO_DHT11    2

#define USE_US_TIMER


#define user_procTaskPrio        0
#define user_procTaskQueueLen    10
os_event_t    user_procTaskQueue[user_procTaskQueueLen];


static void loop(os_event_t *events);
static ETSTimer read_dht11_timer;

#define SIGNAL_START                   0
#define SIGNAL_DHT11_END_READ          1


//-------------------------------------------------------------------------------------------------
// interrupt handler
// this function will be executed on any edge of GPIO0
LOCAL void  general_gpio_intr_handler(int * dummy)
{
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

// if the interrupt was by GPIO_DHT11 -> Execute dht11 handler
    if (gpio_status & GPIO_Pin(GPIO_DHT11))
    {
        dht11_gpio_intr_handler(gpio_status);
    }
}

//-------------------------------------------------------------------------------------------------
static void  read_dht11_timer_handler(void *dummy)
{
    system_os_post(user_procTaskPrio, SIGNAL_START, 0 );
}

//-------------------------------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR  loop(os_event_t *events)
{
    static uint32_t lasttim=0;
    switch(events->sig)
    {
        case SIGNAL_START:
            dht11_start_read();
        break;
        case SIGNAL_DHT11_END_READ:
            if(!dht11_error())
            {
                uint32_t T=dht11_get_temperature();
                uint32_t RH=dht11_get_rh();
                os_printf("Temperature=%d.%d C.  Relative Humidity=%d.%d%%\n",
                          T/1000,T%1000,
                          RH/1000,RH%1000);
            }
            else
                os_printf("----------- ERROR ------------\n");
        break;
    }
}
//-------------------------------------------------------------------------------------------------
//Init function
void ICACHE_FLASH_ATTR  user_init()
{
    system_timer_reinit();

// Initialize UART0 to use as debug
    uart_div_modify(0, UART_CLK_FREQ / 9600);


    os_printf(  "DHT11 Test\r\n");
    os_printf("---------------------------\r\n");

// Initialize the GPIO subsystem.
   gpio_init();

// Initialize dht11 communication
   dht11_init(GPIO_DHT11,user_procTaskPrio,SIGNAL_DHT11_END_READ);

// =================================================
// Activate gpio interrupt for gpio2
// =================================================

// Disable interrupts by GPIO
   ETS_GPIO_INTR_DISABLE();

   ETS_GPIO_INTR_ATTACH(general_gpio_intr_handler, NULL);

   ETS_GPIO_INTR_ENABLE();

// Initialize read dht11 timer
   os_timer_disarm(&read_dht11_timer);
   os_timer_setfn(&read_dht11_timer,read_dht11_timer_handler,NULL);

//Start os task
    system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);

    os_timer_arm(&read_dht11_timer,5000,1); // Start read each 5 seconds
}

