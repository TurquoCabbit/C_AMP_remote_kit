#include <IRremote.h>
#include <EEPROM.h>

#include "header/amp_log.h"
#include "header/amp_system_cfg.h"
#include "header/amp_gpio.h"
#include "header/amp_RTOS.h"
#include "header/smsl_IR_tbl.h"


void setup() {
	char * Version = "1.10";
	char * Date = "2024/10/21";
	// esp_reset_reason_t reset_reason = esp_reset_reason();

	Serial.begin(115200);
	amp_printf("\nVer:%s\t%s\n\n", Version, Date);

	amp_gpio_init();

	queue_init();

	amp_system_cfg_init();
	
	// Serial.print("Reset reason: ");
  	// Serial.println(reset_reason);

	xTaskCreatePinnedToCore(
		main_task, /* Function to implement the task */
		"main_task", /* Name of the task */
		main_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		main_task_priority,  /* Priority of the task */
		&main_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		servo_task, /* Function to implement the task */
		"servo_task", /* Name of the task */
		servo_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		servo_task_priority,  /* Priority of the task */
		&servo_task_handle,  /* Task handle. */
		core_1);

	xTaskCreatePinnedToCore(
		UIUX_task, /* Function to implement the task */
		"UIUX_task", /* Name of the task */
		UIUX_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		UIUX_task_priority,  /* Priority of the task */
		&UIUX_task_handle,  /* Task handle. */
		core_1);

	main_mode(main_mode_boost);	
}

void loop() {
	//core 1
	vTaskDelete(NULL); //Kill Loop()
}

void main_task(void * parameter)
{
	_main_mode mode;
	IRData *smsl_ir;
	uint32_t light_sleep_cnt;

	_smsl_butt_cnt smsl_butt = {IR_debounce_cnt_init};

	TickType_t tick = xTaskGetTickCount();

	light_sleep_cnt = 0;

	for (;;)
	{
		if (xQueueReceive(queue_main, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
				case main_mode_boost:
					IrReceiver.begin(GPIO_IR_RX_PIN, DISABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);

					servo_mode(servo_mode_boost);
					UIUX_mode(UIUX_mode_general);

					main_mode(main_mode_general);
					break;

				case main_mode_general:
					button_scan(&But_A);
					button_scan(&But_B);

					if (Get_But(&But_A, debouncing_times)) {
						light_sleep_cnt = 0;
						UIUX_mode(UIUX_mode_amp_on);	
					}

					if (Get_But(&But_B, debouncing_times)) {
						light_sleep_cnt = 0;
						UIUX_mode(UIUX_mode_amp_off);	
					}

					if (IrReceiver.decode()) {
						light_sleep_cnt = 0;
						smsl_ir = &IrReceiver.decodedIRData;
						
						// mode C
						if (smsl_remote_check(smsl_ir, &smsl_butt.PWR, smsl_IR_addr_C, smsl_IR_butt_PWR))
						{
							smsl_butt.PWR = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_amp_switch);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.UP, smsl_IR_addr_C, smsl_IR_butt_UP))
						{
							smsl_butt.UP = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_volume_up);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.DOWN, smsl_IR_addr_C, smsl_IR_butt_DOWN))
						{
							smsl_butt.DOWN = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_volume_down);
						}
						
						// mode B
						if (smsl_remote_check(smsl_ir, &smsl_butt.UP, smsl_IR_addr_B, smsl_IR_butt_UP))
						{
							smsl_butt.UP = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_sub_config_inc);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.DOWN, smsl_IR_addr_B, smsl_IR_butt_DOWN))
						{
							smsl_butt.DOWN = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_sub_config_dec);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.ENTER, smsl_IR_addr_B, smsl_IR_butt_ENTER))
						{
							smsl_butt.ENTER = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_save_cfg);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.MUTE, smsl_IR_addr_B, smsl_IR_butt_MUTE))
						{
							smsl_butt.MUTE = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_reset_cfg);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.SRC, smsl_IR_addr_B, smsl_IR_butt_SRC))
						{
							smsl_butt.SRC = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_servo_on_angle);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.FN, smsl_IR_addr_B, smsl_IR_butt_FN))
						{
							smsl_butt.FN = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_servo_off_angle);
						}
						
						if (smsl_remote_check(smsl_ir, &smsl_butt.PWR, smsl_IR_addr_B, smsl_IR_butt_PWR))
						{
							smsl_butt.PWR = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_gear_roll_time);
						}
						
						if (smsl_remote_check(smsl_ir, &smsl_butt.LEFT, smsl_IR_addr_B, smsl_IR_butt_LEFT))
						{
							smsl_butt.LEFT = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_gear_roll_dir);
						}


						//mode A
						if (smsl_remote_check(smsl_ir, &smsl_butt.UP, smsl_IR_addr_A, smsl_IR_butt_UP))
						{
							smsl_butt.UP = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_sub_config_inc);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.DOWN, smsl_IR_addr_A, smsl_IR_butt_DOWN))
						{
							smsl_butt.DOWN = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_sub_config_dec);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.SRC, smsl_IR_addr_A, smsl_IR_butt_SRC))
						{
							smsl_butt.SRC = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_servo_angle_res);
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.FN, smsl_IR_addr_A, smsl_IR_butt_FN))
						{
							smsl_butt.FN = IR_debounce_cnt_init;
							UIUX_mode(UIUX_mode_gear_act_level);
						}

						IrReceiver.resume(); // Enable receiving of the next value
					}

					light_sleep_cnt++;
					if (light_sleep_cnt > 10000) {
						light_sleep_cnt = 0;
						amp_printf("Enter light sleep mode, wake up when any IR signal received\n");
						esp_light_sleep_start();
						amp_printf("Wake up from light sleep mode\n");
					}

					vTaskDelayUntil(&tick, 1 / portTICK_PERIOD_MS);
					main_mode(main_mode_general);
					break;
			}
		}
	}
}

void UIUX_task(void * parameter)
{
	
	_UIUX_mode mode;

	_UIUX_mode config_setting_mode = UIUX_mode_general;

	TickType_t tick = xTaskGetTickCount();

	for (;;)
	{
		if (xQueueReceive(queue_UIUX, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
				case UIUX_mode_boost:
					amp_gear_stop();
					vTaskDelayUntil(&tick, 100 / portTICK_PERIOD_MS);
					UIUX_mode(UIUX_mode_general);
					break;
					
				case UIUX_mode_general:
					amp_gear_stop();
					break;
					
				case UIUX_mode_amp_on:
					amp_printf("Amp servo on\n");
					
					servo_mode(servo_mode_amp_on);
					UIUX_mode(UIUX_mode_general);
					break;
				case UIUX_mode_amp_off:
					amp_printf("Amp servo off\n");
				
					servo_mode(servo_mode_amp_off);
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_amp_switch:
					amp_printf("Amp power switch\n");
					
					servo_mode(servo_mode_amp_toggle);
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_volume_up:
					amp_printf("Volume up\n");
					amp_gear_set(amp_cfg.gear_roll_dir);
					vTaskDelayUntil(&tick, amp_cfg.gear_roll_time_ms / portTICK_PERIOD_MS);
					amp_gear_stop();
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_volume_down:
					amp_printf("Volume down\n");
					amp_gear_set(!amp_cfg.gear_roll_dir);
					vTaskDelayUntil(&tick, amp_cfg.gear_roll_time_ms / portTICK_PERIOD_MS);
					amp_gear_stop();
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_save_cfg:
					amp_printf("SAVE config\n");

					amp_save_config(&amp_cfg);
					vTaskDelayUntil(&tick, 10 / portTICK_PERIOD_MS);
					ESP.restart();
					break;

				case UIUX_mode_reset_cfg:
					amp_printf("Reset to default\n");

					amp_clear_config();
					vTaskDelayUntil(&tick, 10 / portTICK_PERIOD_MS);
					ESP.restart();
					break;
				
				case UIUX_mode_servo_on_angle:
					if (!config_setting_mode)
					{
						config_setting_mode = UIUX_mode_servo_on_angle;
						amp_printf("Enter servo ON angle setting mode\n");
					}
					else if (config_setting_mode == UIUX_mode_servo_on_angle)
					{
						config_setting_mode = UIUX_mode_general;
						amp_printf("Leave servo ON angle setting mode\n");
					}
					else
					{
						amp_printf("Already in other setting mode\n");
					}
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_servo_off_angle:
					if (!config_setting_mode)
					{
						config_setting_mode = UIUX_mode_servo_off_angle;
						amp_printf("Enter servo OFF angle setting mode\n");
					}
					else if (config_setting_mode == UIUX_mode_servo_off_angle)
					{
						config_setting_mode = UIUX_mode_general;
						amp_printf("Leave servo OFF angle setting mode\n");
					}
					else
					{
						amp_printf("Already in other setting mode\n");
					}
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_servo_angle_res:
					if (!config_setting_mode)
					{
						config_setting_mode = UIUX_mode_servo_angle_res;
						amp_printf("Enter angle resolution setting mode\n");
					}
					else if (config_setting_mode == UIUX_mode_servo_angle_res)
					{
						config_setting_mode = UIUX_mode_general;
						amp_printf("Leave angle resolution setting mode\n");
					}
					else
					{
						amp_printf("Already in other setting mode\n");
					}
					UIUX_mode(UIUX_mode_general);
					break;
				
				case UIUX_mode_gear_act_level:
					if (!config_setting_mode)
					{
						config_setting_mode = UIUX_mode_gear_act_level;
						amp_printf("Enter gear motor relay active level setting mode\n");
					}
					else if (config_setting_mode == UIUX_mode_gear_act_level)
					{
						config_setting_mode = UIUX_mode_general;
						amp_printf("Leave gear motor relay active level setting mode\n");
					}
					else
					{
						amp_printf("Already in other setting mode\n");
					}
					UIUX_mode(UIUX_mode_general);
					break;

					
				case UIUX_mode_gear_roll_time:
					if (!config_setting_mode)
					{
						config_setting_mode = UIUX_mode_gear_roll_time;
						amp_printf("Enter gear motor rolling time setting mode\n");
					}
					else if (config_setting_mode == UIUX_mode_gear_roll_time)
					{
						config_setting_mode = UIUX_mode_general;
						amp_printf("Leave gear motor rolling time setting mode\n");
					}
					else
					{
						amp_printf("Already in other setting mode\n");
					}
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_gear_roll_dir:
					if (!config_setting_mode)
					{
						config_setting_mode = UIUX_mode_gear_roll_dir;
						amp_printf("Enter gear motor rolling direction setting mode\n");
					}
					else if (config_setting_mode == UIUX_mode_gear_roll_dir)
					{
						config_setting_mode = UIUX_mode_general;
						amp_printf("Leave gear motor rolling direction setting mode\n");
					}
					else
					{
						amp_printf("Already in other setting mode\n");
					}
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_sub_config_inc:
					switch (config_setting_mode)
					{
						case UIUX_mode_servo_on_angle:
							amp_cfg.servo_on_ang += amp_cfg.servo_ang_setting_resol;
							if (amp_cfg.servo_on_ang > 180)
								amp_cfg.servo_on_ang = 180;
							
							servo_mode(servo_mode_amp_on);
							break;
						case UIUX_mode_servo_off_angle:
							amp_cfg.servo_off_ang += amp_cfg.servo_ang_setting_resol;
							if (amp_cfg.servo_off_ang > 180)
								amp_cfg.servo_off_ang = 180;
							
							servo_mode(servo_mode_amp_off);
							break;
						
						case UIUX_mode_servo_angle_res:
							amp_cfg.servo_ang_setting_resol += 0.5;
							if (amp_cfg.servo_ang_setting_resol > 30)
								amp_cfg.servo_ang_setting_resol = 30;
							amp_printf("Servo angle resolution: %f\n", amp_cfg.servo_ang_setting_resol);
							break;
						
						case UIUX_mode_gear_act_level:
							amp_cfg.gear_active_level = 1;
							amp_printf("Gear relay active level: %s\n", amp_cfg.gear_active_level ? "HIGH" : "LOW");
							break;

						case UIUX_mode_gear_roll_time:
							amp_cfg.gear_roll_time_ms += 10;
							if (amp_cfg.gear_roll_time_ms > 5000)
								amp_cfg.gear_roll_time_ms = 5000;
							amp_printf("Gear motor roll time: %d ms\n", amp_cfg.gear_roll_time_ms);
							break;

						case UIUX_mode_gear_roll_dir:
							amp_cfg.gear_roll_dir = 1;
							amp_printf("Gear motor turn clockwise when volume up\n");
							break;
					}
					UIUX_mode(UIUX_mode_general);
					break;

				case UIUX_mode_sub_config_dec:
					switch (config_setting_mode)
					{
						case UIUX_mode_servo_on_angle:
							amp_cfg.servo_on_ang -= amp_cfg.servo_ang_setting_resol;
							if (amp_cfg.servo_on_ang < 0)
								amp_cfg.servo_on_ang = 0;
							
							servo_mode(servo_mode_amp_on);
							break;
						case UIUX_mode_servo_off_angle:
							amp_cfg.servo_off_ang -= amp_cfg.servo_ang_setting_resol;
							if (amp_cfg.servo_off_ang < 0)
								amp_cfg.servo_off_ang = 0;
							
							servo_mode(servo_mode_amp_off);
							break;

						case UIUX_mode_servo_angle_res:
							amp_cfg.servo_ang_setting_resol -= 0.5;
							if (amp_cfg.servo_ang_setting_resol < 0)
								amp_cfg.servo_ang_setting_resol = 0;
							amp_printf("Servo angle resolution: %f\n", amp_cfg.servo_ang_setting_resol);
							break;
													
						case UIUX_mode_gear_act_level:
							amp_cfg.gear_active_level = 0;
							amp_printf("Gear relay active level: %s\n", amp_cfg.gear_active_level ? "HIGH" : "LOW");
							break;

						case UIUX_mode_gear_roll_time:
							amp_cfg.gear_roll_time_ms -= 10;
							if (amp_cfg.gear_roll_time_ms < 0)
								amp_cfg.gear_roll_time_ms = 0;
							amp_printf("Gear motor roll time: %d ms\n", amp_cfg.gear_roll_time_ms);
							break;

						case UIUX_mode_gear_roll_dir:
							amp_cfg.gear_roll_dir = 0;
							amp_printf("Gear motor turn counterclockwise when volume up\n");
							break;
					}
					UIUX_mode(UIUX_mode_general);
					break;
			}
		}
	}
}

void servo_task(void * parameter)
{
	_servo_mode mode;
	uint8_t amp_switch_now = 0;

	TickType_t tick = xTaskGetTickCount();

	for (;;)
	{
		if (xQueueReceive(queue_servo, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
				case servo_mode_boost:
					vTaskDelayUntil(&tick, 100 / portTICK_PERIOD_MS);
					servo_mode(servo_mode_amp_off);
					break;
				case servo_mode_general:
					break;

				case servo_mode_amp_on:
					amp_printf("amp_switch_on: angle: %.2f\n", amp_cfg.servo_on_ang);

					if (amp_servo_set_angle(amp_cfg.servo_on_ang) == SERVO_OK)
						amp_switch_now = 1;

					servo_mode(servo_mode_general);
					break;

				case servo_mode_amp_off:
					amp_printf("amp_switch_off: angle: %.2f\n", amp_cfg.servo_off_ang);

					if (amp_servo_set_angle(amp_cfg.servo_off_ang) == SERVO_OK)
						amp_switch_now = 0;

					servo_mode(servo_mode_general);
					break;

				case servo_mode_amp_toggle:
					servo_mode(amp_switch_now ? servo_mode_amp_off : servo_mode_amp_on);
					break;
			}
		}
	}
}