/**
 ******************************************************************************
 * @file    HTS221.h
 * @author  AST
 * @version V1.0.0
 * @date    7 September 2017
 * @brief   Abstract class of a HTS221 Humidity and Temperature sensor.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2023 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
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
 *
 ******************************************************************************
 */

/* Prevent recursive inclusion -----------------------------------------------*/

#ifndef __HTS221_H__
#define __HTS221_H__


/* Includes ------------------------------------------------------------------*/

#include <cstring>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <system_error>

#include "HTS221_Driver.h"
#include "FileDescriptor.h"


/* Typedefs ------------------------------------------------------------------*/
typedef enum
{
	HTS221_STATUS_OK = 0,
	HTS221_STATUS_ERROR,
	HTS221_STATUS_TIMEOUT,
	HTS221_STATUS_NOT_IMPLEMENTED
} HTS221StatusTypeDef;

/* Class Declaration ---------------------------------------------------------*/

/**
 * Abstract class of an HTS221 Humidity and Temperature sensor.
 */
class HTS221Sensor
{
	public:
	HTS221Sensor						(const std::string& device);
	HTS221StatusTypeDef begin			(void);
	HTS221StatusTypeDef end				(void);
	HTS221StatusTypeDef Enable			(void);
	HTS221StatusTypeDef Disable			(void);
	HTS221StatusTypeDef ReadID			(uint8_t *ht_id);
	HTS221StatusTypeDef Reset			(void);
	HTS221StatusTypeDef GetHumidity		(float *pfData);
	HTS221StatusTypeDef GetTemperature	(float *pfData);
	HTS221StatusTypeDef GetODR			(float *odr);
	HTS221StatusTypeDef SetODR			(float odr);
	HTS221StatusTypeDef ReadReg			(uint8_t reg, uint8_t *data);
	HTS221StatusTypeDef WriteReg		(uint8_t reg, uint8_t data);
	/**
	 * @brief Utility function to read data.
	 * @param  pBuffer: pointer to data to be read.
	 * @param  RegisterAddr: specifies internal address register to be read.
	 * @param  NumByteToRead: number of bytes to be read.
	 * @retval 0 if ok, an error code otherwise.
	 */
	uint8_t IO_Read(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
	{
		if (::write(dev_i2c.fd(), &RegisterAddr, 1) == -1)
		{
			std::string what( "write " __FILE__ "(" + std::to_string(__LINE__) + ")" );
			throw std::system_error(errno, std::system_category(), what);
		}
		if (::read(dev_i2c.fd(), pBuffer, NumByteToRead) == -1)
		{
			std::string what( "read " __FILE__ "(" + std::to_string(__LINE__) + ")" );
			throw std::system_error(errno, std::system_category(), what);
		}
		return 0;
	}

	/**
	 * @brief Utility function to write data.
	 * @param  pBuffer: pointer to data to be written.
	 * @param  RegisterAddr: specifies internal address register to be written.
	 * @param  NumByteToWrite: number of bytes to write.
	 * @retval 0 if ok, an error code otherwise.
	 */
	uint8_t IO_Write(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
	{
		uint8_t *buftmp = (uint8_t*)malloc(NumByteToWrite + 1);

		buftmp[0] = RegisterAddr;
		memcpy(&buftmp[1], pBuffer, NumByteToWrite);

		if (::write(dev_i2c.fd(), buftmp, NumByteToWrite + 1) == -1)
		{
			std::string what( "write " __FILE__ "(" + std::to_string(__LINE__) + ")" );
			throw std::system_error(errno, std::system_category(), what);
		}
		free(buftmp);
		return 0;
	}

	private:
	/* Helper classes. */
	SSD1306::FileDescriptor dev_i2c;
	/* Configuration */
	uint8_t address;
};

#ifdef __cplusplus
extern "C" {
#endif
uint8_t HTS221_IO_Write(void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite);
uint8_t HTS221_IO_Read(void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead);
#ifdef __cplusplus
}
#endif

#endif