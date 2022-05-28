
#include <IRremote.h>
#include "header/amp_gpio.h"
#include "header/amp_RTOS.h"
#include "header/smsl_IR_tbl.h"

void setup() {
	
	Serial.begin(115200);
	Serial.print("======program start======\n");

	amp_gpio_init();

	queue_init();

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

					if (Get_But(&But_A)) {
						Serial.print("Butt_A\n");
						servo_mode(servo_mode_amp_on);	
					}

					if (Get_But(&But_B)) {
						Serial.print("Butt_B\n");
						servo_mode(servo_mode_amp_off);	
					}

					if (IrReceiver.decode()) {
						smsl_ir = &IrReceiver.decodedIRData;
						
						if (smsl_remote_check(smsl_ir, smsl_IR_addr_C, smsl_IR_butt_PWR)) {

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

	for (;;)
	{
		if (xQueueReceive(queue_servo, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
				case servo_mode_boost:
					break;
				case servo_mode_general:
					break;

				case servo_mode_amp_on:
					Serial.print("servo_mode_amp_on\n");
					amp_servo_set_angle(180);
					servo_mode(servo_mode_general);	
					break;

				case servo_mode_amp_off:
					Serial.print("servo_mode_amp_off\n");
					amp_servo_set_angle(0);
					servo_mode(servo_mode_general);	
					break;

				case servo_mode_amp_toggle:
					break;
			}
		}
	}
}