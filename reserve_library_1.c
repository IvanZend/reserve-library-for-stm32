void UART_message_check(UART_HandleTypeDef *huart_pointer)
{
	init_array_by_zero(sizeof(UART_rx_buffer), UART_rx_buffer);
	if ((HAL_UART_Receive(huart_pointer, UART_rx_buffer, (uint16_t)UART_MESSAGE_SIZE, UART_TIMEOUT)) != HAL_TIMEOUT)
	{
		dip_switch_emulate_decode(UART_rx_buffer);
	}
}

uint32_t unite_digits_sequence(uint8_t number_of_values, uint8_t *byte_array_pointer)
{
	uint32_t summary_value = 0;
	for (int i = number_of_values - 1; i >= 0; i--)
	{
		summary_value = summary_value | (byte_array_pointer[i] << ((number_of_values - 1 - i)*8));
	}
	return summary_value;
}

void distrbute_digits_to_bytes(uint64_t value_to_distribute, uint8_t number_of_values, uint8_t *byte_array_pointer)
{
	for (int i = 0; i < number_of_values; i++)
	{
		*(byte_array_pointer + number_of_values - i) = value_to_distribute >> i;
	}
}


void FLASH_erase_write(uint64_t value_to_write)
{
	uint32_t FirstPage = 0, NbOfPages = 0;
	uint32_t Address = 0, PageError = 0;

	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	FirstPage = GetPage(FLASH_USER_START_ADDR);
	NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = NbOfPages;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	Address = FLASH_USER_START_ADDR;
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, value_to_write);
	HAL_FLASH_Lock();

	uint64_t value_from_memory = *(__IO uint64_t *)Address;
	if (value_from_memory)
	{

	}
}

uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;;
}

void dip_switch_emulate_decode(uint8_t* array_pointer)
{
	switch (UART_rx_buffer[UART_COMMAND_BYTE])
	{
	case 0x01:
	{
		flash_vrite_page(DIP_SWITCH_STATE_FLASH, UART_rx_buffer[UART_DATA_BYTE]);
		break;
	}
	}
}

void flash_vrite_page(uint32_t Addr, uint64_t value_to_write)
{
	uint32_t FirstPage = 0;
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	FirstPage = GetPage(Addr);
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = 1;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Addr, value_to_write);
	HAL_FLASH_Lock();

	uint64_t value_from_memory = *(__IO uint64_t *)Addr;
	if (value_from_memory)
	{

	}
}



_Bool compare_int_and_char_arrays(uint8_t* int_array_pointer, char* char_array_pointer)
{
	_Bool arrays_match = 1;
	int i = 1;
	while (int_array_pointer[i] != CHAR_CODE_UART_MESSAGE_END)
	{
		if(int_array_pointer[i] != char_array_pointer[i-1])
		{
			arrays_match = 0;
		}
		i++;
	}

	return arrays_match;
}


// добавляем сообщение в очередь на отправку
void add_char_message_to_TX_queue_buffer(uint16_t message_to_transmit_size, char* message_to_transmit_pointer)
{
	for (int i = 0; i <= message_to_transmit_size; i++)								// записываем содержимое отправляемого сообщения
	{
		TX_queue_buffer[TX_queue_buffer_write_counter][i] = message_to_transmit_pointer[i];		// посимвольно записываем пришедшее сообщение в очередь на отправку
	}
	TX_string_lenght_buffer[TX_queue_buffer_write_counter] = message_to_transmit_size;
	TX_queue_buffer_write_counter++;									// инкрементируем счётчик записи элемента в очереди на отправку
	if (TX_queue_buffer_write_counter > TX_QUEUE_BUFFER_SIZE)			// если превышен максимальный размер в очереди на отправку
	{
		UART_error_handler(TX_QUEUE_OVERFLOW);							// ошибка: очередь на отправку переполнена
		TX_queue_buffer_write_counter = 0;
	}
}


// обработчик прерывания для отправки сообщения по UART
void transmit_messages_IT_handler(void)
{
	if (TX_string_lenght_buffer[0] != 0)			// если в очереди на отправку содержится хотя бы одно сообщение
	{
		for (int i = 0; i < TX_queue_buffer_write_counter; i++)			// до тех пор пока мы не достигнем количества записанных сообщений в очередь
		{
			for (int ii = 0; ii < (TX_string_lenght_buffer[i]); ii++)
			{
				HAL_UART_Transmit(&huart2, &TX_queue_buffer[i][ii], UART_TX_MESSAGE_SIZE, UART_TIMEOUT);
			}
		}
		TX_queue_buffer_write_counter = 0;								// когда отправили все сообщения из очереди, обнуляем счётчик сообщений
	}
}