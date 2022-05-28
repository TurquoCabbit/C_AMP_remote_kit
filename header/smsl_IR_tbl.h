#define smsl_IR_decoed_error    1
#define smsl_IR_decode_ok       0

enum _smsl_IR_addr
{
    smsl_IR_addr_A = 0x3412,
    smsl_IR_addr_B = 0x3512,
    smsl_IR_addr_C = 0x3612,
};

enum _smsl_IR_butt
{
    smsl_IR_butt_PWR = 0x01,
    smsl_IR_butt_UP,
    smsl_IR_butt_LEFT,
    smsl_IR_butt_ENTER,
    smsl_IR_butt_RIGHT,
    smsl_IR_butt_DOWN,
    smsl_IR_butt_SRC,
    smsl_IR_butt_FN,
    smsl_IR_butt_MUTE,
};

void smsl_remote_print(IRData * data)
{
    uint8_t mode;
    char str[40] = {0};

    switch (data->address) {
        default:
            sprintf(str, "ERROR: addr: %d, cmd: %x", data->address, data->command);
            Serial.println(str);
            return;
            break;
        case smsl_IR_addr_A:
            mode = 'A';
            break;
        case smsl_IR_addr_B:
            mode = 'B';
            break;
        case smsl_IR_addr_C:
            mode = 'C';
            break;
    }

    switch (data->command)
    {
        default:
            sprintf(str, "ERROR: mode: %c, cmd: %x", mode, data->command);
            Serial.println(str);
            return;
            break;
        case smsl_IR_butt_PWR:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "POWER");
            break;
        case smsl_IR_butt_MUTE:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "MUTE");
            break;
        case smsl_IR_butt_ENTER:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "ENTER");
            break;
        case smsl_IR_butt_UP:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "UP");
            break;
        case smsl_IR_butt_DOWN:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "DOWN");
            break;
        case smsl_IR_butt_LEFT:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "LEFT");
            break;
        case smsl_IR_butt_RIGHT:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "RIGHT");
            break;
        case smsl_IR_butt_SRC:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "SRC");
            break;
        case smsl_IR_butt_FN:
            sprintf(str, "SMSL REMOTE: mode: %c, butt: %s", mode, "FN");
            break;
    }

    Serial.println(str);
}

inline uint8_t smsl_remote_check(IRData * data, _smsl_IR_addr mode, _smsl_IR_butt butt)
{
    return (data->address == mode) && (data->command == butt);
}