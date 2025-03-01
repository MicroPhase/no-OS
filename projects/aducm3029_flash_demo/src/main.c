/***************************************************************************//**
 *   @file   aducm3029_flash_demo/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <sys/platform.h>

#include "adi_initialize.h"
#include "flash.h"
#include "irq.h"
#include "platform_init.h"
#include "uart.h"
#include "uart_extra.h"
#include "uart_stdio.h"

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(int argc, char *argv[])
{
	/**
	 * Initialize managed drivers and/or services that have been added to
	 * the project.
	 * @return zero on success
	 */
	adi_initComponents();

	int32_t ret;
	uint32_t flash_val = 0;
	struct flash_dev *flash_dut;
	struct flash_init_param flash_init_par = {
		.id = 0
	};
	struct uart_desc *uart_dut;
	struct aducm_uart_init_param aducm_uart_init = {
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS
	};
	struct uart_init_param uart_init_par = {
		.baud_rate = BD_115200,
		.device_id = 0,
		.parity = UART_PAR_NO,
		.size = UART_CS_8,
		.stop = UART_STOP_1,
		.extra = &aducm_uart_init
	};
	struct irq_ctrl_desc *irq_dut;
	int32_t platform_irq_init_par = 0;
	struct irq_init_param irq_init = {
		.irq_ctrl_id = 0,
		.extra = &platform_irq_init_par
	};

	ret = platform_init();
	if(ret < 0)
		return ret;

	ret = irq_ctrl_init(&irq_dut, &irq_init);
	if(ret < 0)
		return ret;

	ret = irq_global_enable(irq_dut);
	if (ret < 0)
		return ret;

	ret = uart_init(&uart_dut, &uart_init_par);
	if (ret < 0)
		return ret;
	init_uart_stdio(uart_dut);

	ret = flash_init(&flash_dut, &flash_init_par);
	if (ret < 0)
		return ret;

	printf("UART online.\n");

	flash_read(flash_dut, 0x3E000, &flash_val, 4);
	printf("Address 0x3E000: %lX\n", flash_val);
	flash_read(flash_dut, 0x3E004, &flash_val, 4);
	printf("Address 0x3E004: %lX\n", flash_val);

	flash_val = 0xAABBCCDD;

	printf("Write 0xAABBCCDD to 0x3E000\n");
	ret = flash_write(flash_dut, 0x3E000, &flash_val, 1);
	if (ret < 0)
		return ret;

	flash_read(flash_dut, 0x3E000, &flash_val, 1);
	printf("Address 0x3E000: %lX\n", flash_val);
	flash_read(flash_dut, 0x3E004, &flash_val, 1);
	printf("Address 0x3E004: %lX\n", flash_val);

	flash_val = 0xCCDDAABB;

	printf("Write 0xCCDDAABB to 0x3E004\n");
	ret = flash_write(flash_dut, 0x3E004, &flash_val, 1);
	if (ret < 0)
		return ret;

	flash_read(flash_dut, 0x3E000, &flash_val, 1);
	printf("Address 0x3E000: %lX\n", flash_val);
	flash_read(flash_dut, 0x3E004, &flash_val, 1);
	printf("Address 0x3E004: %lX\n", flash_val);

	ret = irq_global_disable(irq_dut);
	if (ret < 0)
		return ret;
	ret = irq_ctrl_remove(irq_dut);
	if (ret < 0)
		return ret;
	ret = uart_remove(uart_dut);
	if (ret < 0)
		return ret;
	ret = flash_remove(flash_dut);
	if (ret < 0)
		return ret;

	return 0;
}

