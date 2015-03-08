#include "ets_sys.h"
#include "osapi.h"
#include "gpiolib.h"

#include "dht11_lib.h"

static int dht11_gpio;

typedef enum {
    dht11_standby=0,
    dht11_mark,
    dht11_connecting,
    dht11_mark_connecting,
    dht11_waiting_bit,
    dht11_mark_bit,
    dht11_read_bit
} dht11_status;
static volatile dht11_status  sStatus;

static volatile uint8_t sRead[5];
static volatile uint32_t last_timer;
static ETSTimer dht11_timer;

static uint32_t dht11_user_task;
static uint32_t dht11_user_task_signal;

//-----------------------------------------------------------------------------------------
static bool dht11_set_read_nok()
{
    int i;
    for(i=0;i<4;i++)
        sRead[i]=0;
    sRead[4] = 0xFF;
}

//-----------------------------------------------------------------------------------------
static void dht11_set_standby()
{
    os_timer_disarm(&dht11_timer);
    sStatus = dht11_standby;
    os_timer_disarm(&dht11_timer);
// Disable interrupt
    gpio_pin_intr_state_set(GPIO_ID_PIN(dht11_gpio), GPIO_PIN_INTR_DISABLE);
// GPIO as Output to high level by default.
    GPIO_OUTPUT_SET(dht11_gpio,1);
// Read is finish. Signal to aplication
    system_os_post(dht11_user_task, dht11_user_task_signal, 0 );
}

//-----------------------------------------------------------------------------------------
void static dht11_protocol(uint32 gpio_status,int cause)
{
    static int actual_bit;

    switch(cause) // 0 = gpio interrupt, 1=timer
    {
        case 0: // gpio edge
        {
// disable interrupt for GPIO
            gpio_pin_intr_state_set(GPIO_ID_PIN(dht11_gpio), GPIO_PIN_INTR_DISABLE);
// clear interrupt status for GPIO
            GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & GPIO_Pin(dht11_gpio));
// Reactivate interrupts for GPIO0
            gpio_pin_intr_state_set(GPIO_ID_PIN(dht11_gpio), GPIO_PIN_INTR_ANYEGDE);

            switch(sStatus)
            {
                case dht11_connecting:
                    if(GPIO_INPUT_GET(dht11_gpio))
                    {
// Rising edge ?? Error.
                        dht11_set_read_nok();
                        dht11_set_standby();
                    }
                    else
                    {
                        sStatus = dht11_mark_connecting;
                    }
                break;
                case dht11_mark_connecting:
                    if(!GPIO_INPUT_GET(dht11_gpio))
                    {
// Falling edge ?? Error.
                        dht11_set_read_nok();
                        dht11_set_standby();
                    }
                    else
                    {
                        sStatus = dht11_waiting_bit;
                    }
                break;
                case dht11_waiting_bit:
                    if(GPIO_INPUT_GET(dht11_gpio))
                    {
// Rising edge ?? Error.
                        dht11_set_read_nok();
                        dht11_set_standby();
                    }
                    else
                    {
                        sStatus = dht11_mark_bit;
                        actual_bit=0;
                    }
                break;
                case dht11_mark_bit:
                    if(! GPIO_INPUT_GET(dht11_gpio))
                    {
// Falling edge ?? Error.
                        dht11_set_read_nok();
                        dht11_set_standby();
                    }
                    else
                    {
                        if(actual_bit >= 40)
                        {
                            dht11_set_standby();     // finish OK
                        }
                        else
                        {
                            last_timer = system_get_time();
                            sStatus = dht11_read_bit;
                        }
                    }
                break;
                case dht11_read_bit:
                    if(GPIO_INPUT_GET(dht11_gpio))
                    {
// Rising edge ?? Error.
                        dht11_set_read_nok();
                        dht11_set_standby();
                    }
                    else
                    {
// 26-28 uS means 0.   70 uS means 1
                        int bit_data = ((system_get_time()-last_timer) > 40) ? 1:0;
                        int actual_byte = actual_bit / 8;
                        sRead[actual_byte] <<= 1;
                        sRead[actual_byte] |= bit_data;
                        actual_bit++;
                        sStatus = dht11_mark_bit;
                    }
                break;
                case dht11_standby:
                case dht11_mark:
                default:
                    dht11_set_standby();
                break;
            }
        }
        break;
        case 1: //timer
            switch(sStatus)
            {
                case dht11_mark: // end of mark
                    sStatus = dht11_connecting;
                    // GPIO as Output to high level by default.
                    GPIO_OUTPUT_SET(dht11_gpio,1);
                    GPIO_AS_INPUT(dht11_gpio);

                    ETS_GPIO_INTR_DISABLE();

                    gpio_register_set(GPIO_PIN_ADDR(dht11_gpio),
                                       GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)  |
                                       GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE) |
                                       GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));

                    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(dht11_gpio));

                    gpio_pin_intr_state_set(GPIO_ID_PIN(dht11_gpio), GPIO_PIN_INTR_ANYEGDE);

                    ETS_GPIO_INTR_ENABLE();

                    os_timer_disarm(&dht11_timer);
                    os_timer_arm(&dht11_timer,6,0); // maximun frame time 4.8 ms
                break;
                case dht11_connecting:
                case dht11_mark_connecting:
                case dht11_waiting_bit:
                case dht11_mark_bit:
                case dht11_read_bit:
                default:
                    dht11_set_read_nok();
                    dht11_set_standby();
                break;
            }

        default:
        break;
    }
}

//-----------------------------------------------------------------------------------------
static void  dht11_timer_handler(void *dummy)
{
    dht11_protocol(0,1);
}

//-----------------------------------------------------------------------------------------
void  dht11_gpio_intr_handler(uint32 gpio_status)
{
    dht11_protocol(gpio_status,0);
}

//-----------------------------------------------------------------------------------------
void dht11_init(int gpio_id,uint32_t user_task,uint32_t user_task_signal)
{
    dht11_gpio = gpio_id;
    dht11_user_task = user_task;
    dht11_user_task_signal = user_task_signal;

    gpio_config(dht11_gpio,GPIO_Mode_Out_OD,GPIO_Pull_DIS,GPIO_PIN_INTR_DISABLE);

// GPIO as Output to high level by default.
    GPIO_OUTPUT_SET(dht11_gpio,1);

// Set gpio status. we don't want interrupts now
   gpio_pin_intr_state_set(GPIO_ID_PIN(dht11_gpio), GPIO_PIN_INTR_DISABLE);

// disarm and setup timer
    os_timer_disarm(&dht11_timer);
    os_timer_setfn(&dht11_timer,dht11_timer_handler,NULL);
}

//-----------------------------------------------------------------------------------------
void dht11_start_read()
{

// set gpio to  0 for 18 mS minimun
    GPIO_OUTPUT_SET(dht11_gpio, 0);

    sStatus = dht11_mark;

    os_timer_disarm(&dht11_timer);
    os_timer_arm(&dht11_timer,20,0);
}

//-----------------------------------------------------------------------------------------
bool dht11_error()
{
    int i;
    uint8_t Result=0;
    for(i=0;i<4;i++)
        Result += sRead[i];
    return(Result != sRead[4]);
}

//-----------------------------------------------------------------------------------------
int dht11_get_temperature()
{
    return(sRead[2]*1000+sRead[3]);
}

//-----------------------------------------------------------------------------------------
int dht11_get_rh()
{
    return(sRead[0]*1000+sRead[1]);
}
