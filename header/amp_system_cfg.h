#ifndef __AMP_AYATEM_CFG_H__
#define __AMP_AYATEM_CFG_H__

//EEPROM
#define EEPROM_offset					0x00

#define EEPROM_flag_length				1
#define EEPROM_flag_offset				EEPROM_offset
	#define EEPROM_saved				0x69

#define EEPROM_config_offset			EEPROM_flag_offset + EEPROM_flag_length

//SERVO
#define SREVO_scale						10	// %	(20  ms)
#define SERVO_offset					2.5 // %	(0.5 ms)

#define SERVO_on_ang_init				30
#define SERVO_off_ang_init				150
#define SERVO_angle_resol_init			5	//degree

//GEAR
#define GEAR_MOTOR_ACT_TIME_ms			250
#define GEAR_MOTOR_RELAY_ACT			LOW
#define GEAR_MOTOR_ROLL_DIR				1	//1:clockwise volume up, 0: counterclockwise volume down

//IR 
#define IR_debounce_cnt_init			1

typedef struct _system_config
{
	// power switch config
	float servo_on_ang;
	float servo_off_ang;
	float servo_ang_setting_resol;

	uint8_t gear_active_level;
	int16_t gear_roll_time_ms;
	uint8_t gear_roll_dir;
};

static _system_config amp_cfg =
{
	.servo_on_ang				= SERVO_on_ang_init,
	.servo_off_ang				= SERVO_off_ang_init,
	.servo_ang_setting_resol	= SERVO_angle_resol_init,

	.gear_active_level			= GEAR_MOTOR_RELAY_ACT,
	.gear_roll_time_ms			= GEAR_MOTOR_ACT_TIME_ms,
	.gear_roll_dir				= GEAR_MOTOR_ROLL_DIR,
};

inline void amp_save_config(_system_config * pdata)
{
	uint8_t buff[sizeof(_system_config)];

	memcpy(buff, pdata, sizeof(_system_config));


	for (int i = 0; i < sizeof(_system_config); i++) {
		EEPROM.write(EEPROM_config_offset + i, buff[i]);
	}
	
	EEPROM.write(EEPROM_flag_offset, EEPROM_saved);

	EEPROM.commit();
}

inline void amp_load_config(_system_config * pdata)
{
	uint8_t buff[sizeof(_system_config)];

	for (int i = 0; i < sizeof(_system_config); i++) {
		buff[i] = EEPROM.read(EEPROM_config_offset + i);
	}

	memcpy(pdata, buff, sizeof(_system_config));
}

inline void amp_system_cfg_init()
{
	EEPROM.begin(EEPROM_config_offset + sizeof(_system_config));
	if (EEPROM.read(EEPROM_flag_offset) == EEPROM_saved)
	{
		amp_load_config(&amp_cfg);
		amp_printf("Load config from EEPROM\n");
	}

	#if 1
	amp_printf("amp_cfg {\n");
	amp_printf("\tServo off angle: %.2f\n", amp_cfg.servo_off_ang);
	amp_printf("\tServo on angle: %.2f\n", amp_cfg.servo_on_ang);
	amp_printf("\tServo_angle config resolution: %.2f\n", amp_cfg.servo_ang_setting_resol);
	amp_printf("\tGear motor active level: %S\n", amp_cfg.gear_active_level ? "HIGH" : "LOW");
	amp_printf("\tGear rolling times: %d ms\n", amp_cfg.gear_roll_time_ms);
	amp_printf("\tGear rolling direction at volume up: %s\n", amp_cfg.gear_roll_dir ? "Clockwise" : "counterClockwise");
	amp_printf("}\n\n");
	#endif
}

inline void amp_clear_config(void)
{
	if (EEPROM.read(EEPROM_flag_offset) == EEPROM_saved)
	{
		EEPROM.write(EEPROM_flag_offset, 0);
		EEPROM.commit();
	}
}

#endif // __AMP_AYATEM_CFG_H__