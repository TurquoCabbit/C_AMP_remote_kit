#ifndef __AMP_GPIO_H__
#define __AMP_GPIO_H__

//Pin define
#define GPIO_SERVO_MOTOR_PIN		02
#define GPIO_GEAR_MOTOR_PIN			15
#define GPIO_IR_RX_PIN				13
#define GPIO_LCD_BKLGHT				04

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
	pinMode(GPIO_LCD_BKLGHT, OUTPUT);
	digitalWrite(GPIO_LCD_BKLGHT, 0); // Turn off LCD back light

	pinMode(GPIO_SERVO_MOTOR_PIN, OUTPUT);
	digitalWrite(GPIO_SERVO_MOTOR_PIN, 0);

	pinMode(GPIO_GEAR_MOTOR_PIN, OUTPUT);
	digitalWrite(GPIO_GEAR_MOTOR_PIN, 0);

	pinMode(GPIO_IR_RX_PIN, INPUT);

	ledcSetup(SERVO_MOTOR_channel, SERVO_MOTOR_freq, PWM_res_bit_num);
	ledcAttachPin(GPIO_SERVO_MOTOR_PIN, SERVO_MOTOR_channel);
	ledcWrite(SERVO_MOTOR_channel, 0);

	pinMode(GPIO_BUTT_A_PIN, INPUT_PULLUP);
	pinMode(GPIO_BUTT_B_PIN, INPUT_PULLUP);
	Button_struct_init(&But_A, GPIO_BUTT_A_PIN);
	Button_struct_init(&But_B, GPIO_BUTT_B_PIN);
}

void amp_gpio_set_pwm(float duty, uint8_t channel)
{	
	if (duty > 100 || duty < 0)
		duty = 0;

	uint32_t res_max = (1 << PWM_res_bit_num) - 1;
	
	amp_printf("Duty: %.2f, PWM: %d\n", duty, (uint32_t)(duty * res_max / 100));
	
	ledcWrite(channel, (uint32_t)(duty * res_max / 100));
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

#define SREVO_scale		10	// %	(20  ms)
#define SERVO_offset	2.5 // %	(0.5 ms)

void amp_servo_set_angle(float ang)
{
	if (ang > 180 || ang < 0)
		return;
	float duty = SREVO_scale * ang / 180 + SERVO_offset;

	amp_gpio_set_pwm(duty, SERVO_MOTOR_channel);
}

void amp_servo_360_set(float diversion)
{
	float duty;

	if (diversion > 0)
		duty = SREVO_scale + SERVO_offset;
	else if (diversion < 0)
		duty = SERVO_offset;
	else
		duty = SREVO_scale * 0.5 + SERVO_offset;

	amp_gpio_set_pwm(duty, SERVO_MOTOR_channel);
}


#endif // __AMP_GPIO_H__