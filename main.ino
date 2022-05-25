#include "header/amp_gpio.h"
#include "header/amp_RTOS.h"

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
}

void loop() {
	//core 1
	vTaskDelete(NULL); //Kill Loop()
}

void main_task(void * parameter)
{
	uint8_t mode = 0xFF;

	for (;;)
	{
		if (xQueueReceive(queue_main, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
				case main_mode_boost:
					servo_mode(servo_mode_general);	
					main_mode(main_mode_general);				
					break;

				case main_mode_general:
					button_scan(&But_A);
					button_scan(&But_B);

					if (Get_But(&But_A)) {
						servo_mode(servo_mode_amp_on);	
					}

					if (Get_But(&But_B)) {
						servo_mode(servo_mode_amp_off);	
					}
					
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
				case servo_mode_general:
					break;
			}
		}
	}
}