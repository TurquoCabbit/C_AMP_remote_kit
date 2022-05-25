#ifndef __AMP_GPIO_H__
#define __AMP_GPIO_H__

//Pin define
#define GPIO_SERVO_MOTOR_PIN		02
#define GPIO_GEAR_MOTOR_PIN			15
#define GPIO_LED_PIN				21
#define GPIO_LCD_BKLGHT				04

#define SERVER_MOTOR_channel		0

#define SERVER_MOTOR_freq			50

#define LED_channel					2
#define LED_freq					1000

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

	pinMode(GPIO_LED_PIN, OUTPUT);
	digitalWrite(GPIO_LED_PIN, 0);

	ledcSetup(SERVER_MOTOR_channel, SERVER_MOTOR_freq, 10);
	ledcAttachPin(GPIO_SERVO_MOTOR_PIN, SERVER_MOTOR_channel);
	ledcWrite(SERVER_MOTOR_channel, 511);

	ledcSetup(LED_channel, LED_freq, 10);
	ledcAttachPin(GPIO_LED_PIN, LED_channel);
	ledcWrite(LED_channel, 0);

	pinMode(GPIO_BUTT_A_PIN, INPUT_PULLUP);
	pinMode(GPIO_BUTT_B_PIN, INPUT_PULLUP);
	Button_struct_init(&But_A, GPIO_BUTT_A_PIN);
	Button_struct_init(&But_B, GPIO_BUTT_B_PIN);
}

void amp_gpio_set_pwm(uint8_t duty, uint8_t channel)
{
	ledcWrite(channel, duty * 1023 / 100);
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

#endif // __AMP_GPIO_H__