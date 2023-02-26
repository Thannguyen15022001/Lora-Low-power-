/*
 * rfm9.c
 *
 *  Created on: Feb 19, 2023
 *      Author: Nguyễn Viết Thân
 *       Gmail: thannguyen1502@gmail.com
 */

#include"rfm95.h"

bool RFM95_Init(RFM95_HandleTypedef *RFM_Init)
{
	/*Reset Module*/
//	RFM95_reset(RFM_Init);   Reset by RST

	/*Into Sleep Mode*/
	if (RFM95_setMode(RFM_Init, SLEEP_MODE | LORA_MODE) == false)
		return false;

	/*Enable ExplicitHeaderMode */
	if (RFM95_setExplicitHeaderMode(RFM_Init) == false)
		return false;

	/*Configure Frequency*/
	if (RFM95_setFrequency(RFM_Init) != RFM95_OK)
		return false;

	/*Reset base addr */
	if (RFM95_resetBaseAddr(RFM_Init) == false)
		return false;

	/*Set LNA*/
	if (RFM95_setLNA(RFM_Init) != RFM95_OK)
		return false;

	/*Set power for Tx*/
	if (RFM95_setPower(RFM_Init, PW_PABOOST) != RFM95_OK)
		return false;

	/*Set BandWidth*/
	if (RFM95_setBandWidth(RFM_Init) != RFM95_OK)
		return false;

	/*Set SpredingFactor*/
	if (RFM95_setSpredingFactor(RFM_Init) != RFM95_OK)
		return false;

	/*Set CodingRate*/
	if (RFM95_setCodingRate(RFM_Init) != RFM95_OK)
		return false;

	/*Set reamble*/
	if (RFM95_setPreamble(RFM_Init) != RFM95_OK)
		return false;

	/*Set timeout*/
	if (RFM95_setTimeout(RFM_Init) != RFM95_OK)
		return false;

	RFM95_mappingDIO0(RFM_Init);   //rx done

	/*Set Stanby Mode*/
	if (RFM95_setMode(RFM_Init, STDBY_MODE | LORA_MODE) == false)
		return false;



	return true;
}

//uint8_t RFM95_getStatusDIO(RFM95_HandleTypedef *RFM_Init)
//{
//	return HAL_GPIO_ReadPin(RFM_Init->Module.DIO0.Port,
//			RFM_Init->Module.DIO0.Pin);
//}

//void RFM95_reset(RFM95_HandleTypedef *RFM_Init)
//{
//	HAL_GPIO_WritePin(RFM_Init->Module.Reset.Port, RFM_Init->Module.Reset.Pin,
//			GPIO_PIN_RESET);
//
//	HAL_Delay(10);
//
//	HAL_GPIO_WritePin(RFM_Init->Module.Reset.Port, RFM_Init->Module.Reset.Pin,
//			GPIO_PIN_SET);
//
//	HAL_Delay(10);
//}

void RFM95_enableReceive(RFM95_HandleTypedef *RFM_Init){
	RFM95_setMode(RFM_Init, RXCONTINUOUS_MODE| LORA_MODE);
}
void RFM95_mappingDIO0(RFM95_HandleTypedef *RFM_Init){
	RFM95_writeByteRes(RFM_Init, LR_REG_DIOMAPPING1, (uint8_t)0x3F);
}
bool RFM95_setMode(RFM95_HandleTypedef *RFM_Init, uint8_t Mode)
{
	uint8_t buffer = 0;
	uint8_t dataWrite = 0;

	if (RFM95_readByteRes(RFM_Init, LR_REG_OPMODE, &buffer) != RFM95_OK)
	{
		return false;
	}
	else
	{
		dataWrite = (uint8_t) ((Mode) | (buffer & RESET_MODE));

		if (RFM95_writeByteRes(RFM_Init, LR_REG_OPMODE, dataWrite) != RFM95_OK)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

}

bool RFM95_setExplicitHeaderMode(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t dataWrite = 0;
	uint8_t buffer = 0;

	if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG1, &buffer) != RFM95_OK)
	{
		return false;
	}
	else
	{
		dataWrite = (buffer & EN_EXPLICITHEADERMODE);

		if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG1, dataWrite)
				!= RFM95_OK)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

RFM95_StatusTypedef RFM95_setFrequency(RFM95_HandleTypedef *RFM_Init)
{
	uint32_t result = (RFM_Init->Frequency << 19) >> 5;

	uint8_t freq[3] = { 0 };

	freq[0] = (uint8_t) (result >> 16);

	freq[1] = (uint8_t) (result >> 8);

	freq[2] = (uint8_t) (result >> 0);

	if (RFM95_writeBurstRes(RFM_Init, LR_REG_FRFMSB, freq, sizeof(freq))
			!= RFM95_OK)
	{
		return RFM95_ERROR;
	}
	else
	{
		return RFM95_OK;
	}
}

bool RFM95_resetBaseAddr(RFM95_HandleTypedef *RFM_Init)
{
	if (RFM95_writeByteRes(RFM_Init, LR_REG_FIFOTXBASEADDR,
			(uint8_t) ADD_RESET_TX) != RFM95_OK)
	{
		return false;
	}
	else if (RFM95_writeByteRes(RFM_Init, LR_REG_FIFORXBASEADDR,
			(uint8_t) ADD_RESET_RX) != RFM95_OK)
	{
		return false;
	}
	else
	{
		return true;
	}
}

RFM95_StatusTypedef RFM95_setLNA(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t dataWrite = (uint8_t) ((RFM_Init->LNA << 5) | BOOST_ON);

	if (RFM95_writeByteRes(RFM_Init, LR_REG_LNA, dataWrite) != RFM95_OK)
	{
		return RFM95_ERROR;
	}
	else
	{
		return RFM95_OK;
	}
}

RFM95_StatusTypedef RFM95_setPower(RFM95_HandleTypedef *RFM_Init,
		uint8_t powerMode)
{
	uint8_t dataWrite = 0;

	if (powerMode == PW_PABOOST)
	{
		/*PA_BOOST*/
		if (RFM_Init->Power > 17)
		{
			RFM_Init->Power = 17;
		}
		else if (RFM_Init->Power < 2)
		{
			RFM_Init->Power = 2;
		}
		dataWrite = (PW_PABOOST | RFM_Init->Power);
		if (RFM95_writeByteRes(RFM_Init, LR_REG_PACONFIG, dataWrite)
				!= RFM95_OK)
		{
			return RFM95_ERROR;
		}
		else
		{
			return RFM95_OK;
		}
	}
	else
	{
		/*RFO*/
		if (RFM_Init->Power > 14)
		{
			RFM_Init->Power = 14;
		}
		else if (RFM_Init->Power < 0)
		{
			RFM_Init->Power = 0;
		}
		dataWrite = (PW_RFO | RFM_Init->Power);
		if (RFM95_writeByteRes(RFM_Init, LR_REG_PACONFIG, dataWrite)
				!= RFM95_OK)
		{
			return RFM95_ERROR;
		}
		else
		{
			return RFM95_OK;
		}
	}
}

RFM95_StatusTypedef RFM95_setBandWidth(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t buffer = 0;
	uint8_t dataWrite = 0;
	if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG1, &buffer) != RFM95_OK)
	{
		return RFM95_ERROR;
	}
	else
	{
		dataWrite = (uint8_t) (((RFM_Init->BandWidth) << 4)
				| (buffer & RESET_BANDWIDTH));
		if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG1, dataWrite)
				!= RFM95_OK)
		{
			return RFM95_ERROR;
		}
		else
		{
			return RFM95_OK;
		}
	}
}

RFM95_StatusTypedef RFM95_setSpredingFactor(RFM95_HandleTypedef *RFM_Init)
{
	/*The spreading factor controls the chirp rate, and thus controls the speed of data transmission.
	 *Lower spreading factors mean faster chirps and therefore a higher data transmission rate.
	 *Lower For every increase in spreading factor, the chirp sweep rate is halved, and so the data transmission rate is halved.*/
	uint8_t buffer = 0;
	uint8_t dataWrite = 0;
	if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG2, &buffer) != RFM95_OK)
	{
		return RFM95_ERROR;
	}
	else
	{
		if (RFM_Init->SpredingFactor == SF6)
		{

			/* Set SpreadingFactor = 6 in RegModemConfig2
			 * The header must be set to Implicit mode
			 * Write bits 2-0 of register address 0x31 to value "0b101"
			 * Write register address 0x37 to value 0x0C */
			dataWrite = (uint8_t) ((RFM_Init->SpredingFactor << 4)
					| (buffer & RESET_SF));
			if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG2, dataWrite)
					!= RFM95_OK)
			{
				return RFM95_ERROR;
			}
			else
			{
				if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG1, &buffer)
						!= RFM95_OK)
				{
					return RFM95_ERROR;
				}
				else
				{
					if (RFM95_setImplicitHeaderMode(RFM_Init) == false)
					{
						return RFM95_ERROR;
					}
					else
					{
						if (RFM95_readByteRes(RFM_Init, LR_REG_DETECTOPTIMIZE,
								&buffer) != RFM95_OK)
						{
							return RFM95_ERROR;
						}
						else
						{
							dataWrite = (uint8_t) (0x05 | (buffer & 0xF8));
							if (RFM95_writeByteRes(RFM_Init,
							LR_REG_DETECTOPTIMIZE, dataWrite) != RFM95_OK)
							{
								return RFM95_ERROR;
							}
							else
							{
								if (RFM95_writeByteRes(RFM_Init,
								LR_REG_DETECTIONTHRESHOLD, (uint8_t) 0x0C)
										!= RFM95_OK)
								{
									return RFM95_ERROR;
								}
								else
								{
									return RFM95_OK;
								}
							}
						}
					}
				}
			}

		}
		else
		{
			dataWrite = (uint8_t) (RFM_Init->SpredingFactor << 4
					| (buffer & RESET_SF));
			if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG2, dataWrite)
					!= RFM95_OK)
			{
				return RFM95_ERROR;
			}
			else
			{
				return RFM95_OK;
			}
		}
	}
}

RFM95_StatusTypedef RFM95_setCodingRate(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t buffer = 0;
	uint8_t dataWrite = 0;
	if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG1, &buffer) != RFM95_OK)
	{
		return RFM95_ERROR;
	}
	else
	{
		dataWrite =
				(uint8_t) ((RFM_Init->CodingRate << 1) | (buffer & RESET_CR));
		if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG1, dataWrite)
				!= RFM95_OK)
		{
			return RFM95_ERROR;
		}
		else
		{
			return RFM95_OK;
		}
	}
}

RFM95_StatusTypedef RFM95_setPreamble(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t Prel[2] = { 0 };
	Prel[0] = (uint8_t) (RFM_Init->Preamble >> 8);
	Prel[1] = (uint8_t) (RFM_Init->Preamble >> 0);
	if (RFM95_writeBurstRes(RFM_Init, LR_REG_PREAMBLEMSB, Prel, sizeof(Prel))
			!= RFM95_OK)
	{
		return RFM95_ERROR;
	}
	else
	{
		return RFM95_OK;
	}
}

bool RFM95_setImplicitHeaderMode(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t dataWrite = 0;
	uint8_t buffer = 0;
	if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG1, &buffer) != RFM95_OK)
	{
		return false;
	}
	else
	{
		dataWrite = (buffer | EN_IMPLICITHEADERMODE);
		if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG1, dataWrite)
				!= RFM95_OK)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool RFM95_Transmit(RFM95_HandleTypedef *RFM_Init, uint8_t *buffer,
		uint8_t size, uint16_t timeout)
{
	uint8_t pAddr = 0;
	uint8_t statusIrq = 0;
	uint8_t currentMode = 0;

	if(RFM95_readByteRes(RFM_Init, LR_REG_OPMODE, &currentMode)!= RFM95_OK)
	{
		return false;
	}

	if (RFM95_setMode(RFM_Init, STDBY_MODE | LORA_MODE) == false)
	{
		return false;
	}
	else if (RFM_Init->SpredingFactor == SF6)
	{
		if (RFM95_setImplicitHeaderMode(RFM_Init) == false)
		{
			return false;
		}
	}
	else if (RFM95_readByteRes(RFM_Init, LR_REG_FIFOTXBASEADDR, &pAddr)
			!= RFM95_OK)
	{
		return false;
	}
	else if (RFM95_writeByteRes(RFM_Init, LR_REG_FIFOADDRPTR, pAddr)
			!= RFM95_OK)
	{
		return false;

	}
	else if (RFM95_writeByteRes(RFM_Init, LR_REG_PAYLOADLENGTH, 0x00)
			!= RFM95_OK)
	{
		return false;
	}
	else if (RFM95_writeByteRes(RFM_Init, LR_REG_PAYLOADLENGTH, size)
			!= RFM95_OK)
	{
		return false;
	}
	for (uint8_t i = 0; i < size; i++)
	{
		if (RFM95_writeByteRes(RFM_Init, LR_REG_FIFO, *(buffer++)) != RFM95_OK)
		{
			return false;
		}
	}

	if (RFM95_setMode(RFM_Init, TX_MODE | LORA_MODE) == false)
	{
		return false;
	}
	else
	{
		while (1)
		{
			if (RFM95_readByteRes(RFM_Init, LR_REG_IRQFLAGS, &statusIrq)
					!= RFM95_OK)
			{
				return false;
			}
			else
			{
				if ((statusIrq & TX_FLAGS) != 0)
				{
					if (RFM95_writeByteRes(RFM_Init, LR_REG_IRQFLAGS, TX_FLAGS)
							!= RFM95_OK)
						return false;
					if(RFM95_writeByteRes(RFM_Init, LR_REG_OPMODE, currentMode)!=RFM95_OK)
					{
						return false;
					}
					return true;
				}
				else
				{
					if ((timeout--) <= 0)
					{
						RFM95_writeByteRes(RFM_Init, LR_REG_OPMODE, currentMode);
						return false;
					}
				}
			}
			HAL_Delay(1);
		}
	}
}

bool RFM95_Receive(RFM95_HandleTypedef *RFM_Init, uint8_t *buffer, uint8_t size)
{
	uint8_t statusIrq = 0;
	uint8_t sizeMin = 0;
	uint8_t rxBuffer = 0;

	if (RFM95_setMode(RFM_Init, RXCONTINUOUS_MODE | LORA_MODE) == false)
	{
		return false;
	}

	if (RFM95_readByteRes(RFM_Init, LR_REG_FIFORXBASEADDR, &rxBuffer)
			!= RFM95_OK)
	{
		return false;
	}
	if (RFM95_writeByteRes(RFM_Init, LR_REG_FIFOADDRPTR, rxBuffer) != RFM95_OK)
	{
		return false;
	}

	if (RFM95_readByteRes(RFM_Init, LR_REG_IRQFLAGS, &statusIrq) != RFM95_OK)
	{
		return false;
	}
	if ((statusIrq & RX_FLAGS) != 0)
	{
		if (RFM95_writeByteRes(RFM_Init, LR_REG_IRQFLAGS, RX_FLAGS) != RFM95_OK)
		{
			return false;
		}
		if (RFM95_readByteRes(RFM_Init, LR_REG_RXNBBNYTES, &sizeMin)
				!= RFM95_OK)
		{
			return false;
		}
		if (RFM95_readByteRes(RFM_Init, LR_REG_FIFORXCURRENTADDR, &rxBuffer)
				!= RFM95_OK)
		{
			return false;
		}
		if (RFM95_writeByteRes(RFM_Init, LR_REG_FIFOADDRPTR, rxBuffer)
				!= RFM95_OK)
		{
			return false;
		}
		if (size >= sizeMin)
		{
			size = sizeMin;
		}
		else
		{
			size = size;
		}
		for (uint8_t i = 0; i < size; i++)
		{
			RFM95_readByteRes(RFM_Init, LR_REG_FIFO, buffer++);

		}
		return true;
	}
	return false;

}

bool RFM95_enableCRC(RFM95_HandleTypedef *RFM_Init)
{
	/* The Coding Rate (CR) value need to be adjusted according to conditions of the channel used for data transmission.
	 * If there are too many interference in the channel, then it’s recommended to increase the value of CR.
	 * However, the rise in CR value will also increase the duration for the transmission*/
	uint8_t buffer = 0;
	uint8_t dataWrite = 0;
	if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG2, &buffer) != RFM95_OK)
	{
		return false;
	}
	else
	{
		dataWrite = (uint8_t) (EN_BITCRC | (buffer & RESET_BITCRC));
		if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG2, dataWrite)
				!= RFM95_OK)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool RFM95_accessLoraRegister(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t buffer = 0;
	uint8_t dataWrite = 0;
	if (RFM95_readByteRes(RFM_Init, LR_REG_OPMODE, &buffer) != RFM95_OK)
	{
		return false;
	}
	else
	{
		dataWrite = (uint8_t) (0xBF & buffer);
		if (RFM95_writeByteRes(RFM_Init, LR_REG_OPMODE, dataWrite) != RFM95_OK)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

RFM95_StatusTypedef RFM95_setTimeout(RFM95_HandleTypedef *RFM_Init)
{
	if (RFM_Init->Timeout <= 255)
	{
		if (RFM95_writeByteRes(RFM_Init, LR_REG_SYMBTIMEOUTLSB,
				(uint8_t) (RFM_Init->Timeout)) != RFM95_OK)
		{
			return RFM95_ERROR;
		}
		else
		{
			return RFM95_OK;
		}
	}
	else
	{
		uint8_t dataWrite = 0;
		uint8_t buffer = 0;
		if (RFM95_readByteRes(RFM_Init, LR_REG_MODEMCONFIG2, &buffer)
				!= RFM95_OK)
		{
			return RFM95_ERROR;
		}
		dataWrite = (uint8_t) ((RFM_Init->Timeout >> 8) | buffer);
		if (RFM95_writeByteRes(RFM_Init, LR_REG_MODEMCONFIG2, dataWrite)
				!= RFM95_OK)
		{
			return RFM95_ERROR;
		}
		dataWrite = (uint8_t) (RFM_Init->Timeout >> 0);
		if (RFM95_writeByteRes(RFM_Init, LR_REG_SYMBTIMEOUTLSB, dataWrite)
				!= RFM95_OK)
		{
			return RFM95_ERROR;
		}
		return RFM95_OK;
	}
}

int16_t RFM95_getRSSI(RFM95_HandleTypedef *RFM_Init)
{
	uint8_t buffer = 0;
	int16_t valueRSSI = 0;
	RFM95_readByteRes(RFM_Init, LR_REG_PKTRSSIVALUE, &buffer);
	if (RFM_Init->Frequency <= RSSI_ULF)
	{
		valueRSSI = buffer - RSSI_OFFSETLF;
	}
	else if (RFM_Init->Frequency >= RSSI_UHF)
	{
		valueRSSI = buffer - RSSI_OFFSETHF;
	}

	return valueRSSI;
}

void RFM95_setNSS(RFM95_HandleTypedef *RFM_Init, GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(RFM_Init->Module.NSS.Port, RFM_Init->Module.NSS.Pin,
			PinState);
}

RFM95_StatusTypedef RFM95_readByteRes(RFM95_HandleTypedef *RFM_Init,
		uint8_t addr, uint8_t *pBuffer)
{
	//RFM95_setNSS(RFM_Init, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(RFM_Init->Module.SPI, &addr, 1, TIMEOUT) != HAL_OK)
	{
		RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
		return RFM95_ERROR;
	}
	if (HAL_SPI_Receive(RFM_Init->Module.SPI, pBuffer, 1, TIMEOUT) != HAL_OK)
	{
		RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
		return RFM95_ERROR;
	}

	RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
	return RFM95_OK;

}

RFM95_StatusTypedef RFM95_readBurstRes(RFM95_HandleTypedef *RFM_Init,
		uint8_t addr, uint8_t *pBuffer, uint8_t size)
{
	if (size > 0 && size <= 1)
	{
		return RFM95_readByteRes(RFM_Init, addr, pBuffer);
	}
	else
	{
		//RFM95_setNSS(RFM_Init, GPIO_PIN_RESET);
		if (HAL_SPI_Transmit(RFM_Init->Module.SPI, &addr, 1, TIMEOUT) != HAL_OK)
		{
			RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
			return RFM95_ERROR;
		}
		for (uint8_t i = 0; i < size; i++)
		{
			if (HAL_SPI_Receive(RFM_Init->Module.SPI, (pBuffer++), 1, TIMEOUT)
					!= HAL_OK)
			{
				RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
				return RFM95_ERROR;
			}
		}
		RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
		return RFM95_OK;
	}
}

RFM95_StatusTypedef RFM95_writeBurstRes(RFM95_HandleTypedef *RFM_Init,
		uint8_t addr, uint8_t *pData, uint8_t size)
{
	addr = (addr | BIT_WRITE);
	if (size > 0 && size <= 1)
	{
		return RFM95_writeByteRes(RFM_Init, addr, *pData);
	}
	else
	{
		//RFM95_setNSS(RFM_Init, GPIO_PIN_RESET);

		if (HAL_SPI_Transmit(RFM_Init->Module.SPI, &addr, 1, TIMEOUT) != HAL_OK)
		{
			RFM95_setNSS(RFM_Init->Module.SPI, GPIO_PIN_SET);
			return RFM95_ERROR;
		}
		for (uint8_t i = 0; i < size; i++)
		{
			if (HAL_SPI_Transmit(RFM_Init->Module.SPI, (pData++), 1, TIMEOUT)
					!= HAL_OK)
			{
				RFM95_setNSS(RFM_Init->Module.SPI, GPIO_PIN_SET);
				return RFM95_ERROR;
			}
		}
		RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
		return RFM95_OK;
	}

}

RFM95_StatusTypedef RFM95_writeByteRes(RFM95_HandleTypedef *RFM_Init,
		uint8_t addr, uint8_t data)
{
	uint8_t dataWrite = (addr | 0x80);
	//RFM95_setNSS(RFM_Init, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(RFM_Init->Module.SPI, &dataWrite, 1, TIMEOUT)
			!= HAL_OK)
	{
		RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
		return RFM95_ERROR;
	}
	if (HAL_SPI_Transmit(RFM_Init->Module.SPI, &data, 1, TIMEOUT) != HAL_OK)
	{
		RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
		return RFM95_ERROR;
	}
	RFM95_setNSS(RFM_Init, GPIO_PIN_SET);
	return RFM95_OK;

}


