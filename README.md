# esp8266-dht11

DHT11 is a low cost Temperature and Relative Humidity sensor.
It use a one wire protocol to send data to MCU (ESP8266 in this case)

This communication protocol is implemented as a driver library. This
library use a (my) gpiolib to configure gpio.

# Example application

In *user* directory there is a example aplication using dht11 and gpio libraries

As can been see in the example, the application must:
   - Indicate with dht11_init()
		- What gpio use to communicate with
		- What task get signaled when read will be finished
		- What signal send
   - Setup a gpio interrupt handler.
	 When a interrupt is receive from gpio selected, the gpio interrupt
	 handler must execute, dht11_gpio_interrupt_handler() function.
   - Start a read executing dht11_start_read() function.
   - Wait to end read is signaled by dht11 library
   - Verify read was correct using dht11_error() function.
   - Read temperature and RH with dht11_get_temperature() and
	 dht11_get_rh().

# Acknowledgements

Thanks to https://github.com/esp8266/source-code-examples for their Makefile


