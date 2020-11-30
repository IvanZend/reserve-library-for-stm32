
#include "universal_extern_lib_1.h"

/*
 * Считывание и запись состояния входного пина
 */
void check_input_signal_state(InSignalAttributes_StructTypeDef* signal_to_check)
{
	GPIO_PinState current_logic_state = HAL_GPIO_ReadPin(signal_to_check->signal_pin.GPIO_port_pointer, signal_to_check->signal_pin.pin_number);

	switch (current_logic_state)
	{
	case GPIO_PIN_SET:
	{
		signal_to_check->signal_logic_level = LOGIC_LEVEL_HIGH;
		break;
	}
	case GPIO_PIN_RESET:
	{
		signal_to_check->signal_logic_level = LOGIC_LEVEL_LOW;
		break;
	}
	}
}

/*
 * Проверяем состояние кнопки
 */
void check_and_update_button_state(ButtonAttributes_TypeDef* button_to_check)
{
	if (button_to_check->button_released_default_signal_level == LOGIC_LEVEL_LOW)						// если при отпущенной кнопке логическое состояние пина "0"
	{
		if (button_to_check->button_signal.signal_logic_level == LOGIC_LEVEL_HIGH)						// если текущий уровень сигнала на пине кнопки "1"
		{
			if (button_to_check->button_current_state != BUTTON_LONG_PRESS)								// если ещё не было зафиксировано долгое нажатие
			{
				if (button_to_check->button_pressing_duration_counter >= button_to_check->button_bounce_filter_counts)	// если нажатие длится дольше, чем нужно для фильтрации дребезга
				{
					button_to_check->button_current_state = BUTTON_SHORT_PRESS;							// выставляем флаг короткого нажатия
				}
			}
			if (button_to_check->button_pressing_duration_counter >= button_to_check->button_long_press_counts)			// если счётчик продолжительности нажатия дошёл до значения длительного нажатия
			{
				button_to_check->button_pressing_duration_counter = button_to_check->button_long_press_counts;			// удерживаем счётчик от дальнейшего увеличения
				button_to_check->button_current_state = BUTTON_LONG_PRESS;								// выставляем флаг долгого нажатия
			}
			button_to_check->button_pressing_duration_counter = button_to_check->button_pressing_duration_counter + 1;	// инкрементируем счётчик продолжительности нажатия
		}
		else																							// если текущий уровень сигнала на пине кнопки "0"
		{
			button_to_check->button_pressing_duration_counter = 0;										// обнуляем счётчик продолжительности нажатия
			button_to_check->button_current_state = BUTTON_RELEASED;									// выставляем флаг, что кнопка отпущена
		}
	}
	else																								// если при отпущенной кнопке логическое состояние пина "1"
	{
		if (button_to_check->button_signal.signal_logic_level == LOGIC_LEVEL_HIGH)						// если текущий уровень сигнала на пине кнопки "1"
		{
			button_to_check->button_pressing_duration_counter = 0;										// обнуляем счётчик продолжительности нажатия
			button_to_check->button_current_state = BUTTON_RELEASED;									// выставляем флаг, что кнопка отпущена
		}
		else																							// если текущий уровень сигнала на пине кнопки "0"
		{
			if (button_to_check->button_current_state != BUTTON_LONG_PRESS)								// если ещё не было зафиксировано долгое нажатие
			{
				if (button_to_check->button_pressing_duration_counter >= button_to_check->button_bounce_filter_counts)	// если нажатие длится дольше, чем нужно для фильтрации дребезга
				{
					button_to_check->button_current_state = BUTTON_SHORT_PRESS;							// выставляем флаг короткого нажатия
				}
			}
			if (button_to_check->button_pressing_duration_counter >= button_to_check->button_long_press_counts)			// если счётчик продолжительности нажатия дошёл до значения длительного нажатия
			{
				button_to_check->button_pressing_duration_counter = button_to_check->button_long_press_counts;			// удерживаем счётчик от дальнейшего увеличения
				button_to_check->button_current_state = BUTTON_LONG_PRESS;								// выставляем флаг долгого нажатия
			}
			button_to_check->button_pressing_duration_counter = button_to_check->button_pressing_duration_counter + 1;	// инкрементируем счётчик продолжительности нажатия
		}
	}
}

void button_init(ButtonAttributes_TypeDef* button_object)
{
	button_object->button_signal.signal_pin.GPIO_port_pointer = BUTTON_PORT_DEFAULT;
	button_object->button_signal.signal_pin.pin_number = BUTTON_PIN_DEFAULT;
	button_object->button_released_default_signal_level = BUTTON_RELEASED_SIGNAL_LEVEL_DEFAULT;
	button_object->button_bounce_filter_counts = BUTTON_BOUNCE_FILTER_COUNTS_DEFAULT;
	button_object->button_long_press_counts = BUTTON_LONG_PRESS_COUNTS_DEFAULT;

	button_object->button_current_state = BUTTON_RELEASED;
	button_object->button_pressing_duration_counter = 0;
}

void motor_init(MotorObject_StructTypeDef* motor_object)
{
	motor_object->limit_switch.limit_switch_IN_signal.signal_pin.GPIO_port_pointer = LIMIT_SWITCH_PORT_DEFAULT;
	motor_object->limit_switch.limit_switch_IN_signal.signal_pin.pin_number = LIMIT_SWTICH_PIN_DEFAULT;
	motor_object->limit_switch.limit_switch_logic_inverted = LIMIT_SWTICH_LOGIC_INVERTED_DEFAULT;
	motor_object->motor_signals.STEP_OUT_signal.signal_pin.GPIO_port_pointer = MOTOR_STEP_PORT_DEFAULT;
	motor_object->motor_signals.STEP_OUT_signal.signal_pin.pin_number = MOTOR_STEP_PIN_DEFAULT;
	motor_object->motor_signals.DIR_OUT_signal.signal_pin.GPIO_port_pointer = MOTOR_DIR_PORT_DEFAULT;
	motor_object->motor_signals.DIR_OUT_signal.signal_pin.pin_number = MOTOR_DIR_PIN_DEFAULT;
	motor_object->motor_signals.ENABLE_OUT_signal.signal_pin.GPIO_port_pointer = MOTOR_ENABLE_PORT_DEFAULT;
	motor_object->motor_signals.ENABLE_OUT_signal.signal_pin.pin_number = MOTOR_ENABLE_PIN_DEFAULT;
	motor_object->emergency_step_impulses_to_limit = EMERGENCY_STEP_IMPULSES_TO_LIMIT_DEFAULT;
	motor_object->DIR_pin_logic_level_inverted = DIR_PIN_LOGIC_LEVEL_INVERTED_DEFAULT;
	motor_object->motor_timer_tick_duration_MKS = MOTOR_TIMER_TICK_DURATION_MKS_DEFUALT;
	motor_object->motor_timer_ticks_per_sec = MOTOR_TIMER_TICKS_PER_SEC_DEFAULT;

	motor_object->step_impulses_distance_from_limit_switch = 1;
	motor_object->limit_emergency_counter = 0;
	motor_object->motor_movement_direction = MOVE_TO_COORD_END;
	motor_object->step_pin_current_phase = STEP_LOW_PHASE;
	motor_object->ticks_before_next_step_counter = 0;
	motor_object->ticks_since_start_movement_counter = 0;
	motor_object->step_impulses_since_start_movement_counter = 0;
	motor_object->speed_mks_per_step_impulse = 0;
	motor_object->ticks_for_acceleration_counter = 0;
	motor_object->step_impulses_for_acceleration_counter = 0;
	motor_object->cyclic_movement_direction = MOVE_TO_COORD_END;
}


void movement_profile_init(MotorMovementProfile_StructTypeDef* movement_profile)
{
	movement_profile->acceleration_type = ACCELERATION_TYPE_DEFAULT;
	movement_profile->short_distance_step_impulses = SHORT_DISTANCE_STEP_IMPULSES_DEFAULT;
	movement_profile->far_distance_step_impulses = FAR_DISTANCE_STEP_IMPULSES_DEFAULT;
	movement_profile->min_speed_mks_per_step_impulse = MIN_SPEED_MKS_PER_STEP_IMPULSE_DEFAULT;
	movement_profile->max_speed_mks_per_step_impulse = MAX_SPEED_MKS_PER_STEP_IMPULSE_DEFAULT;
	movement_profile->linear_acceleration_coefficient = LINEAR_ACCELERATION_COEFFICIENT_DEFAULT;
	movement_profile->quadratic_acceleration_coefficient = QUADRATIC_ACCELERATION_COEFFICIENT_DEFAULT;
	movement_profile->acceleration_time_ms = ACCELERATION_TIME_MS_DEFAULT;
}

/*
 * Вызываем эту функцию тогда же, когда запускаем прерывания для движения
 */
void motor_movement_init(MotorObject_StructTypeDef* motor_object, MotorMovementProfile_StructTypeDef* movement_profile)
{
	calculate_acceleration_coefficient(movement_profile);
	reset_movement_counters(motor_object);
}

void calculate_acceleration_coefficient(MotorMovementProfile_StructTypeDef* movement_profile)
{
	movement_profile->linear_acceleration_coefficient = ((movement_profile->max_speed_mks_per_step_impulse - movement_profile->min_speed_mks_per_step_impulse)*1000)/movement_profile->acceleration_time_ms;
	movement_profile->quadratic_acceleration_coefficient = ((movement_profile->max_speed_mks_per_step_impulse - movement_profile->min_speed_mks_per_step_impulse)*1000*1000) \
			/movement_profile->acceleration_time_ms/movement_profile->acceleration_time_ms;
}

/*
 * Циклическое движение мотора в режиме экспозиции
 */
void cyclic_movement_step(MotorObject_StructTypeDef* motor_object, MotorMovementProfile_StructTypeDef* movement_profile)
{
	if (motor_object->step_impulses_distance_from_limit_switch <= movement_profile->short_distance_step_impulses)					// если мы в крайней точке точке, ближайшей к начальному положению
	{
		motor_object->cyclic_movement_direction = MOVE_TO_COORD_END;			// выставляем флаг движения от начального положения
		motor_check_counter_and_make_step_to_direction(motor_object, movement_profile, MOVE_TO_COORD_END);											// делаем шаг в сторону от начального положения
		reset_movement_counters(motor_object);
	}
	/*
	 * если мы находимся в промежутке между крайними положениями растра (ближнее и дальнее)
	 */
	if ((motor_object->step_impulses_distance_from_limit_switch > movement_profile->short_distance_step_impulses) && \
			(motor_object->step_impulses_distance_from_limit_switch < movement_profile->far_distance_step_impulses))
	{
		if (motor_object->cyclic_movement_direction == MOVE_TO_COORD_END)		// если выставлен флаг движения от начального положения
		{
			motor_check_counter_and_make_step_to_direction(motor_object, movement_profile, MOVE_TO_COORD_END);										// делаем шаг от начального положения
		}
		else
		{
			motor_check_counter_and_make_step_to_direction(motor_object, movement_profile, MOVE_TO_COORD_ORIGIN);										// иначе делаем шаг в сторону начального положения
		}
	}
	if (motor_object->step_impulses_distance_from_limit_switch >= movement_profile->far_distance_step_impulses)						// если мы в крайней точке, дальней от начального положения
	{
		motor_object->cyclic_movement_direction = MOVE_TO_COORD_ORIGIN;					// выставляем флаг движения к начальному положению
		motor_check_counter_and_make_step_to_direction(motor_object, movement_profile, MOVE_TO_COORD_ORIGIN);											// делаем шаг в сторону начального положения
		reset_movement_counters(motor_object);
	}
}

void reset_movement_counters(MotorObject_StructTypeDef* motor_object)
{
	motor_object->ticks_before_next_step_counter = 0;
	motor_object->ticks_since_start_movement_counter = 0;
	motor_object->step_impulses_for_acceleration_counter = 0;
	motor_object->step_impulses_since_start_movement_counter = 0;
	motor_object->ticks_for_acceleration_counter = 0;
}

/*
 * Делаем шаг в заданном направлении
 */
void motor_check_counter_and_make_step_to_direction(MotorObject_StructTypeDef* motor_object, MotorMovementProfile_StructTypeDef* movement_profile, MotorMoveDirection_EnumTypeDef movement_direction)
{
	motor_object->ticks_since_start_movement_counter++;
	motor_object->ticks_before_next_step_counter--;
	if ((movement_profile->far_distance_step_impulses - motor_object->step_impulses_since_start_movement_counter) >= motor_object->step_impulses_for_acceleration_counter)
	{
		if (motor_object->speed_mks_per_step_impulse < movement_profile->max_speed_mks_per_step_impulse)
		{
			motor_object->ticks_for_acceleration_counter++;
		}
	}
	else
	{
		motor_object->ticks_for_acceleration_counter--;
	}

	if (motor_object->ticks_before_next_step_counter <= 0)
	{
		motor_object->motor_movement_direction = movement_direction;
		motor_direction_pin_set(motor_object);
		check_limit_switch_and_make_step(motor_object);
		calculate_ticks_per_next_step(motor_object, movement_profile);
	}
}

/*
 * выставляем пин направления мотора
 */
void motor_direction_pin_set(MotorObject_StructTypeDef* motor_object)
{
	switch (motor_object->motor_movement_direction)																// если направление движения
	{
	case MOVE_TO_COORD_ORIGIN:																		// если направление движения к начальному положению
	{
		if (motor_object->DIR_pin_logic_level_inverted)		// !! ifdef											// если логический уровень направления инвертирован аппаратно
		{
			HAL_GPIO_WritePin(motor_object->motor_signals.DIR_OUT_signal.signal_pin.GPIO_port_pointer, motor_object->motor_signals.DIR_OUT_signal.signal_pin.pin_number, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(motor_object->motor_signals.DIR_OUT_signal.signal_pin.GPIO_port_pointer, motor_object->motor_signals.DIR_OUT_signal.signal_pin.pin_number, GPIO_PIN_SET);
		}
		break;
	}
	case MOVE_TO_COORD_END:																			// если направление движения от начального положения
	{
		if (motor_object->DIR_pin_logic_level_inverted)															// если логический уровень направления инвертирован аппаратно
		{
			HAL_GPIO_WritePin(motor_object->motor_signals.DIR_OUT_signal.signal_pin.GPIO_port_pointer, motor_object->motor_signals.DIR_OUT_signal.signal_pin.pin_number, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(motor_object->motor_signals.DIR_OUT_signal.signal_pin.GPIO_port_pointer, motor_object->motor_signals.DIR_OUT_signal.signal_pin.pin_number, GPIO_PIN_RESET);
		}
		break;
	}
	}
}

/*
 * проверяем состояние концевика и совершаем шаг
 */
void check_limit_switch_and_make_step(MotorObject_StructTypeDef* motor_object)
{
	/*
//	 * если направление движения к начальному положению, и концевик не активен, и не пройдено аварийное количество шагов к начальному положению
	 */
	if ((motor_object->motor_movement_direction == MOVE_TO_COORD_ORIGIN) && \
		(!(limit_switch_return_state(motor_object))) && \
		(motor_object->limit_emergency_counter < motor_object->emergency_step_impulses_to_limit))
	{
		step_toggle(motor_object);																				// совершаем шаг
		motor_object->step_impulses_distance_from_limit_switch = motor_object->step_impulses_distance_from_limit_switch - 1;		// декрементируем счётчик расстояния от начального положения
		motor_object->limit_emergency_counter = motor_object->limit_emergency_counter + 1;							// инкрементируем аварийный счётчик шагов
	}
	if (motor_object->motor_movement_direction == MOVE_TO_COORD_END)											// если направлениение движения от начального положения
	{
		step_toggle(motor_object);																				// совершаем шаг
		motor_object->step_impulses_distance_from_limit_switch = motor_object->step_impulses_distance_from_limit_switch + 1;		// инкрементируем счётчик расстояния от начального положения
		motor_object->limit_emergency_counter = 0;															// обнуляем аварийный счётчик шагов
	}
}

void calculate_ticks_per_next_step(MotorObject_StructTypeDef* motor_object, MotorMovementProfile_StructTypeDef* movement_profile)
{
	switch (movement_profile->acceleration_type)
	{
	case (NO_ACCELERATION):
	{
		motor_object->ticks_before_next_step_counter = movement_profile->max_speed_mks_per_step_impulse/motor_object->motor_timer_tick_duration_MKS;
		break;
	}
	default:
	{
		// если оставшееся количество шагов больше количества шагов для ускорения/замедления, мы либо ускоряемся, либо движемся с макс. скоростью
		if ((movement_profile->far_distance_step_impulses - motor_object->step_impulses_since_start_movement_counter) > motor_object->step_impulses_for_acceleration_counter)
		{
			// если мы не достигли максимальной скорости, ускоряемся
			if (motor_object->speed_mks_per_step_impulse < movement_profile->max_speed_mks_per_step_impulse)
			{
				motor_object->speed_mks_per_step_impulse = movement_time_function(motor_object->ticks_since_start_movement_counter, motor_object, movement_profile);
				motor_object->ticks_before_next_step_counter = motor_object->speed_mks_per_step_impulse/motor_object->motor_timer_tick_duration_MKS;
				motor_object->step_impulses_for_acceleration_counter++;
			}
			// иначе движемся с максимальной скоростью
			else
			{
				motor_object->ticks_before_next_step_counter = movement_profile->max_speed_mks_per_step_impulse/motor_object->motor_timer_tick_duration_MKS;
			}
		}
		// иначе замедляемся
		else
		{
			motor_object->speed_mks_per_step_impulse = movement_time_function(motor_object->ticks_for_acceleration_counter, motor_object, movement_profile);
			motor_object->ticks_before_next_step_counter = motor_object->speed_mks_per_step_impulse/motor_object->motor_timer_tick_duration_MKS;
		}
		break;
	}
	}
	motor_object->step_impulses_since_start_movement_counter++;
}


int32_t movement_time_function(uint32_t ticks_value, MotorObject_StructTypeDef* motor_object, MotorMovementProfile_StructTypeDef* movement_profile)
{
	int32_t calculated_speed_mks_per_step_impulse = 0;
	switch (movement_profile->acceleration_type)
	{
	case (LINEAR_ACCELERATION):
	{
		calculated_speed_mks_per_step_impulse = (ticks_value * movement_profile->linear_acceleration_coefficient)/motor_object->motor_timer_ticks_per_sec + movement_profile->min_speed_mks_per_step_impulse;
		break;
	}
	case (QUADRATIC_ACCELERATION):
	{
		//calculated_speed_mks_per_step_impulse = ((time_value*time_value*quadratic_acceleration_coefficient)/MOTOR_TIMER_TICKS_PER_SEC/MOTOR_TIMER_TICKS_PER_SEC) + MIN_STEP_IMPULSES_PER_SEC_ALL_MODES;
		break;
	}
	default:
	{

		break;
	}
	}
	return calculated_speed_mks_per_step_impulse;
}


/*
 * опрашиваем и возрващаем состояние концевика
 */
_Bool limit_switch_return_state(MotorObject_StructTypeDef* motor_object)
{
	_Bool current_state;																			// флаг состояния концевика
	check_input_signal_state(&motor_object->limit_switch.limit_switch_IN_signal);					// опрашиваем состояние пина концевика
	if (motor_object->limit_switch.limit_switch_logic_inverted)										// если логический уровень концевика инвертирован аппаратно
	{
		if (motor_object->limit_switch.limit_switch_IN_signal.signal_logic_level == LOGIC_LEVEL_LOW)			// если на пине концевика "0"
		{
			current_state = 1;																		// выставляем флаг концевика в "1"
			motor_object->step_impulses_distance_from_limit_switch = 0;										// обнуляем счётчик расстояния до концевика
		}
		else
		{
			current_state = 0;																		// иначе выставляем флаг концевика в "0"
		}
	}
	else
	{
		if (motor_object->limit_switch.limit_switch_IN_signal.signal_logic_level == LOGIC_LEVEL_LOW)			// иначе если на пине концевика "0"
		{
			current_state = 0;																		// выставляем флаг концевика в "0"
		}
		else
		{
			current_state = 1;																		// иначе выставляем флаг концевика в "1"
			motor_object->step_impulses_distance_from_limit_switch = 0;												// обнуляем счётчик расстояния до концевика
		}
	}
	return current_state;																			// возвращаем флаг состояния концевика
}

/*
 * совершаем шаг
 */
void step_toggle(MotorObject_StructTypeDef* motor_object)
{
	switch (motor_object->step_pin_current_phase)															// если текущее логическое состояние на пине шага
	{
	case STEP_LOW_PHASE:																			// если текущеее логическое состояние "0"
	{
		HAL_GPIO_WritePin(motor_object->motor_signals.STEP_OUT_signal.signal_pin.GPIO_port_pointer, motor_object->motor_signals.STEP_OUT_signal.signal_pin.pin_number, GPIO_PIN_SET);
		motor_object->step_pin_current_phase = STEP_HIGH_PHASE;												// выставляем флаг, что пин шага находится в логическом состоянии "1"
		break;
	}
	case STEP_HIGH_PHASE:																			// если текущеее логическое состояние "1"
	{
		HAL_GPIO_WritePin(motor_object->motor_signals.STEP_OUT_signal.signal_pin.GPIO_port_pointer, motor_object->motor_signals.STEP_OUT_signal.signal_pin.pin_number, GPIO_PIN_RESET);
		motor_object->step_pin_current_phase = STEP_LOW_PHASE;												// выставляем флаг, что пин шага находится в логическом состоянии "0"
		break;
	}
	}
}
