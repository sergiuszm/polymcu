/*
 * Copyright (c) 2015-2016, Lab A Part
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this
 * o list of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "internal.h"
#include "Driver_USART.h"

extern const ARM_DRIVER_USART Driver_UART_DEBUG;

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

#define LED_COUNT  2

static const int g_led_gpio_port_num[LED_COUNT] = { 0,   0 };
static const int g_led_gpio_bit_num[LED_COUNT]  = { 11, 12 };

void __libc_init_array(void) {
	// Do nothing. NXP LPCOpen already initialized the .data and .bss segments
}

void hardware_init_hook(void) {
	int i;

	// Setup pin muxings
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, IOCON_MODE_PULLUP | IOCON_FUNC1); /* P0.11: GPIO/LED1 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, IOCON_MODE_PULLUP | IOCON_FUNC1); /* P0.12: GPIO/LED2 */

	// Initialize Board LEDs
	for (i = 0; i < LED_COUNT; i++) {
		Chip_GPIO_WriteDirBit(LPC_GPIO, g_led_gpio_port_num[i], g_led_gpio_bit_num[i], true);
	}

#ifdef SUPPORT_DEVICE_USB
	//Setup USB-related pins
	//USB_VBUS input with pull-down
	LPC_IOCON->PIO0[3] = 0x00000009;

	//!USB_CONNECT output
	LPC_IOCON->PIO0[6] = 0x00000001;

	//Make sure device is disconnected
	LPC_USB->DEVCMDSTAT = 0;

	// Initialize USBD ROM
	usbd_rom_init(usb_interface_inits, ep_count);
#endif

#ifndef SUPPORT_DEBUG_UART_NONE
	// Initialize UART
	Driver_UART_DEBUG.Initialize(NULL);
#endif

	// Ensure SystemCoreClock is set
	SystemCoreClockUpdate();
}

void set_led(int led, int value) {
	if(led < LED_COUNT) {
		Chip_GPIO_WritePortBit(LPC_GPIO, g_led_gpio_port_num[led], g_led_gpio_bit_num[led], value);
	}
}
