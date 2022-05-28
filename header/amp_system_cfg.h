#ifndef __AMP_AYATEM_CFG_H__
#define __AMP_AYATEM_CFG_H__

//EEPROM
#define EEPROM_offset					0x00

#define EEPROM_flag_length				1
#define EEPROM_flag_offset				EEPROM_offset
	#define EEPROM_saved				0x69

#define EEPROM_config_offset			EEPROM_flag_offset + EEPROM_flag_length

//SERVO
#define SERVO_on_ang_init				0
#define SERVO_off_ang_init				180
#define SERVO_switch_timw_init			2000

typedef struct _system_config
{
	// power switch config
	float servo_off_ang;
	float servo_on_ang;
	uint16_t servo_switch_time_ms;
};

static _system_config amp_cfg =
{
	.servo_off_ang			= SERVO_off_ang_init,
	.servo_on_ang			= SERVO_on_ang_init,
	.servo_switch_time_ms	= SERVO_switch_timw_init
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
	amp_printf("servo_off_ang: %.2f\n", amp_cfg.servo_off_ang);
	amp_printf("servo_on_ang: %.2f\n", amp_cfg.servo_on_ang);
	amp_printf("servo_switch_time_ms: %d\n", amp_cfg.servo_switch_time_ms);
	amp_printf("}\n");
	#endif
}

inline void amp_clear_config(void)
{
	EEPROM.write(EEPROM_flag_offset, 0);
	EEPROM.commit();
}

#endif // __AMP_AYATEM_CFG_H__