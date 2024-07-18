/**
 * @file Sample app to demonstrate PWM-based LED fade
 * and UART asynchronous read.
 */

#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/pwm.h>
// gpio
#include <zephyr/drivers/gpio.h>
// uart
#include <zephyr/drivers/uart.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

// Define the size of the receive buffer
#define RECEIVE_BUFF_SIZE 10

// Define the receiving timeout period
#define RECEIVE_TIMEOUT 100

// Get the device pointers of the LEDs through gpio_dt_spec
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// Get the device pointers of the PWM LEDs through pwm_dt_spec
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwmled0));

// Get the device pointer of the UART hardware
#define UART_NODE DT_NODELABEL(usart1)
const struct device *uart = DEVICE_DT_GET(UART_NODE);

// Define the transmission buffer, which is a buffer to hold the data to be sent over UART
static uint8_t tx_buf[] = {"PWM LED and UART Async test\n\r"
													 "Press 1 on your keyboard to toggle LEDS 1:\n\r"};

// Define the receive buffer
static uint8_t rx_buf[RECEIVE_BUFF_SIZE] = {0};

#define NUM_STEPS 50U
#define SLEEP_MSEC 25U

// Define the callback function for UART
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch (evt->type)
	{

	case UART_RX_RDY:
		if ((evt->data.rx.len) == 1)
		{

			if (evt->data.rx.buf[evt->data.rx.offset] == '1')
				gpio_pin_toggle_dt(&led0);
		}
		break;
	case UART_RX_DISABLED:
		uart_rx_enable(dev, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT);
		break;

	default:
		break;
	}
}

int main(void)
{
	uint32_t pulse_width = 0U;
	uint32_t step = pwm_led0.period / NUM_STEPS;
	uint8_t dir = 1U;
	int ret;

	printk("PWM-based LED fade\n");


	// Verify that the PWM LED is ready
	if (!pwm_is_ready_dt(&pwm_led0))
	{
		printk("Error: PWM device %s is not ready\n",
					 pwm_led0.dev->name);
		return EXIT_FAILURE;
	}

	// Verify that the UART device is ready
	if (!device_is_ready(uart))
	{
		printk("UART device not ready\r\n");
		return EXIT_FAILURE;
	}

	// Verify that the LED devices are ready
	if (!device_is_ready(led0.port))
	{
		printk("GPIO device is not ready\r\n");
		return EXIT_FAILURE;
	}

	// Configure the GPIOs of the LEDs
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return EXIT_FAILURE;
	}

	// Register the UART callback function
	ret = uart_callback_set(uart, uart_cb, NULL);
	if (ret)
	{
		printk("UART callback set failed: %d\n", ret);
		return EXIT_FAILURE;
	}

	printk("UART callback set\n");

	// Send the data over UART by calling uart_tx()
	ret = uart_tx(uart, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
	if (ret)
	{
		return EXIT_FAILURE;
	}

	// Start receiving by calling uart_rx_enable() and pass it the address of the receive  buffer
	ret = uart_rx_enable(uart, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT);
	if (ret)
	{
		return EXIT_FAILURE;
	}

	while (1)
	{
		ret = pwm_set_pulse_dt(&pwm_led0, pulse_width);
		if (ret)
		{
			printk("Error %d: failed to set pulse width\n", ret);
			return 0;
		}
		printk("Using pulse width %d%%\n", 100 * pulse_width / pwm_led0.period);

		if (dir)
		{
			pulse_width += step;
			if (pulse_width >= pwm_led0.period)
			{
				pulse_width = pwm_led0.period - step;
				dir = 0U;
			}
		}
		else
		{
			if (pulse_width >= step)
			{
				pulse_width -= step;
			}
			else
			{
				pulse_width = step;
				dir = 1U;
			}
		}

		k_sleep(K_MSEC(SLEEP_MSEC));
		// k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}