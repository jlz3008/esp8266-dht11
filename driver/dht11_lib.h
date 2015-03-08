#ifndef DHT11_LIB_H
#define DHT11_LIB_H

// DHT11 is a low cost Temperature and Relative Humidity sensor.
// It use a one wire protocol to send data to MCU (ESP8266 in this case)
//
// Your application must:
//   - Indicate with dht11_init()
//        - What gpio use to communicate with
//        - What task get signaled when read will be finished
//        - What signal send
//   - Setup a gpio interrupt handler.
//     When a interrupt is receive from gpio selected, the gpio interrupt
//     handler must execute, dht11_gpio_interrupt_handler() function.
//   - Start a read executing dht11_start_read() function.
//   - Wait to end read is signaled by dht11 library
//   - Verify read was correct using dht11_error() function.
//   - Read temperature and RH with dht11_get_temperature() and
//     dht11_get_rh().
//


// dht11_init :
// Initialize communication protocol with dht11
// Params:
//   gpio_id: GPIO ID to communicate with DHT11
//   user_task: user_task priority to signal read end
//   user_task_signal : The signal will be sent to user_task when read is finish
void dht11_init(int gpio_id,uint32_t user_task,uint32_t user_task_signal);


// dhtll_start_read()
// Start a DHT11 Read
//
void dht11_start_read();

// dht11_gpio_intr_handler:
// General GPIO interrupt handler must be execute this function when a
// interrupt from DHT11 gpio selected arrive.
//
void  dht11_gpio_intr_handler(uint32 gpio_status);

//
// dht11_error :
// Return true if was a error on last comunication with dht11
//
bool dht11_error();

//
// dht11_get_temperature :
// Get last readed temperature (milli ºC)
// The result of this funcion is undefined if dht11_error() return true
//
int dht11_get_temperature();

//
// dht11_get_rh :
// Get last readed relative humidity (in %% (per thousand)
// The result of this funcion is undefined if dht11_error() return true
//
int dht11_get_rh();


#endif // DHT11_LIB_H
