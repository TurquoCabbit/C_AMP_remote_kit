#ifndef __AMP_GPIO_H__
#define __AMP_GPIO_H__

#define SERVO_OK		0
#define SERVO_ERROR		1

//Pin define
#define GPIO_SERVO_MOTOR_PIN				02
#define GPIO_RELAY_CLOCKWISE_PIN			15
#define GPIO_RELAY_COUNTERCLOCKWISE_PIN		13
#define GPIO_IR_RX_PIN						12
#define GPIO_LCD_BKLGHT						04

#define WAKEUP_PIN 							GPIO_NUM_12  // Define with gpio_num_t type

#define PWM_res_bit_num				16

#define SERVO_MOTOR_channel			0

#define SERVO_MOTOR_freq			50

#define GPIO_BUTT_A_PIN				00
#define GPIO_BUTT_B_PIN				35

#define debouncing_times	        5  //in ms
#define But_press			        1
#define But_release			        2
#define But_CD				        3

typedef struct
{
	uint8_t flag;
	uint32_t cnt;
	uint8_t But;
}_Button;

_Button But_A, But_B;

bool Encoder_up = 0;
bool Encoder_down = 0;

static void Button_struct_init(_Button *pButton, uint8_t But)
{
	pButton->flag = 0;
	pButton->cnt = 0;
	pButton->But = But;
}

void amp_gpio_init(void)
{
	pinMode(GPIO_RELAY_CLOCKWISE_PIN, OUTPUT);
	digitalWrite(GPIO_RELAY_CLOCKWISE_PIN, !amp_cfg.gear_active_level);

	pinMode(GPIO_RELAY_COUNTERCLOCKWISE_PIN, OUTPUT);
	digitalWrite(GPIO_RELAY_COUNTERCLOCKWISE_PIN, !amp_cfg.gear_active_level);

	pinMode(GPIO_LCD_BKLGHT, OUTPUT);
	digitalWrite(GPIO_LCD_BKLGHT, LOW); // Turn off LCD back light

	pinMode(GPIO_IR_RX_PIN, INPUT);

	esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 0);  // Wake up when GPIO goes LOW

	ledcSetup(SERVO_MOTOR_channel, SERVO_MOTOR_freq, PWM_res_bit_num);
	ledcAttachPin(GPIO_SERVO_MOTOR_PIN, SERVO_MOTOR_channel);
	ledcWrite(SERVO_MOTOR_channel, LOW);

	pinMode(GPIO_BUTT_A_PIN, INPUT_PULLUP);
	pinMode(GPIO_BUTT_B_PIN, INPUT_PULLUP);
	Button_struct_init(&But_A, GPIO_BUTT_A_PIN);
	Button_struct_init(&But_B, GPIO_BUTT_B_PIN);
}

uint8_t amp_gpio_set_pwm(float duty, uint8_t channel)
{	
	uint32_t res_max = (1 << PWM_res_bit_num) - 1;
	uint32_t duty_raw;
	char str[32];
	
	if (duty > 100 || duty < 0)
		duty = 0;

	duty_raw = (uint32_t)(duty * res_max / 100);
	sprintf(str, "Duty: %.2f, PWM: %d\n", duty, duty_raw);
	Serial.print(str);
	
	if (ledcRead(channel) != duty_raw)
		ledcWrite(channel, duty_raw);

	return SERVO_OK;
}

void button_scan(_Button *pButton)
{
	if (!digitalRead(pButton->But))  //first press
	{
		pButton->cnt++;
		if (pButton->cnt > debouncing_times && !pButton->flag)	//pass debouncing time
		{
			pButton->flag = But_press;
		}
	}
	else if (pButton->flag == But_CD)	//task done wait here
	{
		pButton->cnt = 0;
		pButton->flag = 0;
	}
	else if (pButton->cnt > debouncing_times)	//release
	{
		pButton->cnt = 0;
		pButton->flag = But_release;
	}
}

bool Get_But(_Button *pButton, uint32_t hold_time = 0)
{
	if (hold_time)
	{
		if (pButton->cnt > hold_time && pButton->flag == But_press)
		{
			pButton->flag = But_CD;
			return 1;
		}
		else if (pButton->flag == But_release)
		{
			pButton->flag = But_CD;
		}
	}
	else if (pButton->flag == But_release)
	{
		pButton->flag = 0;
		return 1;
	}
	return 0;
}

bool Get_But_press(_Button *pButton)
{
	if (pButton->flag == But_press)
	{
		return 1;
	}
	else if(pButton->flag == But_release)
	{
		pButton->flag = 0;
	}
	return 0;
}


uint8_t amp_servo_set_angle(float ang)
{
	float duty;

	if (ang > 180 || ang < 0)
		return SERVO_ERROR;

	duty = SREVO_scale * ang / 180 + SERVO_offset;

	if (amp_gpio_set_pwm(duty, SERVO_MOTOR_channel) == SERVO_OK)
		return SERVO_OK;
	else
		return SERVO_ERROR;
}

uint8_t amp_servo_stop(void)
{	
	if (amp_gpio_set_pwm(0, SERVO_MOTOR_channel) == SERVO_OK)
		return SERVO_OK;
	else
		return SERVO_ERROR;
}

void amp_gear_stop(void)
{
	digitalWrite(GPIO_RELAY_CLOCKWISE_PIN, !amp_cfg.gear_active_level);
	digitalWrite(GPIO_RELAY_COUNTERCLOCKWISE_PIN, !amp_cfg.gear_active_level);
}

void amp_gear_set(uint8_t clockwise)
{
	amp_gear_stop();

	digitalWrite(clockwise ? GPIO_RELAY_CLOCKWISE_PIN : GPIO_RELAY_COUNTERCLOCKWISE_PIN, amp_cfg.gear_active_level);
}

#endif // __AMP_GPIO_H__