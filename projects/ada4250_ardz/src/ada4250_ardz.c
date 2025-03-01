/***************************************************************************//**
 *   @file   ada4250_ardz/src/ada4250_ardz.c
 *   @brief  Implementation of Main Function.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#include <sys/platform.h>
#include "adi_initialize.h"
#include "error.h"
#include "iio.h"
#include "parameters.h"
#include "ada4250.h"
#include "iio_ada4250.h"
#include "spi.h"
#include "spi_extra.h"
#include "iio_app.h"

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main()
{
	int32_t ret;

	adi_initComponents();

	struct ada4250_dev *ada4250_dev;

	struct aducm_spi_init_param spi_param = {
		.continuous_mode = true,
		.dma = false,
		.half_duplex = false,
		.master_mode = MASTER
	};

	struct spi_init_param init_param = {
		.device_id = 0,
		.chip_select = 1,
		.extra = &spi_param,
		.max_speed_hz = 1000000,
		.mode = SPI_MODE_3,
		.platform_ops = &aducm_spi_ops
	};

	struct ada4250_init_param ada4250_param = {
		.device_id = ADA4250,
		.spi_init = &init_param,
		.refbuf_en = ADA4250_BUF_DISABLE,
		.bias = ADA4250_BIAS_DISABLE,
		.gain = ADA4250_GAIN_8,
		.avdd_v = 5,
		.offset_uv = 0,
	};

	ret = ada4250_init(&ada4250_dev, &ada4250_param);
	if (IS_ERR_VALUE(ret))
		return ret;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ADA4250", ada4250_dev,
			       &ada4250_iio_descriptor,
			       NULL, NULL)
	};

	return iio_app_run(devices, ARRAY_SIZE(devices));
}
