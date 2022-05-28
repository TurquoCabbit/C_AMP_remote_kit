#include <IRremote.h>
#include <EEPROM.h>

#include "header/amp_log.h"
#include "header/amp_gpio.h"
#include "header/amp_RTOS.h"
#include "header/smsl_IR_tbl.h"
#include "header/amp_system_cfg.h"


void setup() {
	
	Serial.begin(115200);
	amp_printf("======program start======\n");

	amp_gpio_init();

	queue_init();

	amp_system_cfg_init();

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

	main_mode(main_mode_boost);	
}

void loop() {
	//core 1
	vTaskDelete(NULL); //Kill Loop()
}

void main_task(void * parameter)
{
	uint8_t mode = 0xFF;
	IRData *smsl_ir;
	
	_smsl_butt_cnt smsl_butt = {1};

	TickType_t tick = xTaskGetTickCount();

	for (;;)
	{
		if (xQueueReceive(queue_main, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
				case main_mode_boost:
					IrReceiver.begin(GPIO_IR_RX_PIN, DISABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);

					servo_mode(servo_mode_boost);
					main_mode(main_mode_general);
					break;

				case main_mode_general:
					button_scan(&But_A);
					button_scan(&But_B);

					if (Get_But(&But_A, 10)) {
						amp_printf("Butt_A\n");
						servo_mode(servo_mode_amp_on);	
					}

					if (Get_But(&But_B, 10)) {
						amp_printf("Butt_B\n");
						servo_mode(servo_mode_amp_off);	
					}

					if (IrReceiver.decode()) {
						smsl_ir = &IrReceiver.decodedIRData;
						
						if (smsl_remote_check(smsl_ir, &smsl_butt.PWR, smsl_IR_addr_C, smsl_IR_butt_PWR)) {
							smsl_butt.PWR = 1;
							amp_printf("mode C, PWR : amp power switch\n");
							servo_mode(servo_mode_amp_toggle);

						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.MUTE, smsl_IR_addr_B, smsl_IR_butt_MUTE)) {
							smsl_butt.MUTE = 1;
							amp_printf("mode B, MUTE : reset to default\n");

							amp_clear_config();
							ESP.restart();
						}

						if (smsl_remote_check(smsl_ir, &smsl_butt.ENTER, smsl_IR_addr_B, smsl_IR_butt_ENTER)) {
							smsl_butt.ENTER = 1;
							amp_printf("mode B, ENTER : SAVE config\n");

							amp_save_config(&amp_cfg);
						}


						IrReceiver.resume(); // Enable receiving of the next value
					}

					vTaskDelayUntil(&tick, 1 / portTICK_PERIOD_MS);
					main_mode(main_mode_general);				
					break;
			}
		}
	}
}

void servo_task(void * parameter)
{
	uint8_t mode = 0xFF;
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