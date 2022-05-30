#ifndef __AMP_RTOS_H__
#define __AMP_RTOS_H__

#define core_0 0
#define core_1 1

//task_priority 0 to (configMAX_PRIORITIES - 1)
#define main_task_priority				5
#define UIUX_task_priority				5
#define servo_task_priority				5

//stack size in word
#define main_task_stack					2048
#define UIUX_task_stack					2048
#define servo_task_stack				2048

//Task Handle
TaskHandle_t main_task_handle;
TaskHandle_t UIUX_task_handle;
TaskHandle_t servo_task_handle;

//Queue Handle
QueueHandle_t queue_main;
QueueHandle_t queue_servo;
QueueHandle_t queue_UIUX;

#define queue_mode_num 256
uint8_t queue_mode[queue_mode_num];  //mode 0 to 255

#define main_mode(mode)		xQueueSend(queue_main, queue_mode + (mode), 0)
#define servo_mode(mode)	xQueueSend(queue_servo, queue_mode + (mode), 0)
#define UIUX_mode(mode)		xQueueSend(queue_UIUX, queue_mode + (mode), 0)

inline void queue_init(void)
{
	queue_main = xQueueCreate(1, 1);       //1 items, 1 byte each
	queue_servo = xQueueCreate(1, 1);       //1 items, 1 byte each
	queue_UIUX = xQueueCreate(1, 1);       //1 items, 1 byte each

	for (uint16_t i = 0; i < queue_mode_num; i++)
	{
		queue_mode[i] = i;
	}
}

enum _main_mode
{
	main_mode_boost			= 0xFF,
	main_mode_general		= 0x00,
};

enum _servo_mode
{
	servo_mode_boost		= 0xFF,
	servo_mode_general		= 0x00,
	servo_mode_amp_on,
	servo_mode_amp_off,
	servo_mode_amp_toggle,
};

enum _UIUX_mode
{
	UIUX_mode_boost		= 0xFF,
	UIUX_mode_general	= 0x00,
	UIUX_mode_amp_switch,
	UIUX_mode_volume_up,
	UIUX_mode_volume_down,
	UIUX_mode_save_cfg,
	UIUX_mode_reset_cfg,
	
	UIUX_mode_servo_on_angle,
	UIUX_mode_sub_servo_on_angle,

	UIUX_mode_servo_off_angle,
	UIUX_mode_sub_servo_off_angle
};


#endif // __AMP_RTOS_H__