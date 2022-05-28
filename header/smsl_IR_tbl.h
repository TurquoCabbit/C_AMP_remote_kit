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


typedef struct
{
    uint8_t PWR;
    uint8_t MUTE;
    uint8_t ENTER;
    uint8_t UP;
    uint8_t DOWN;
    uint8_t LEFT;
    uint8_t RIGHT;
    uint8_t SRC;
    uint8_t FN;
} _smsl_butt_cnt;


void smsl_remote_print(IRData * data)
{
    uint8_t mode;

    switch (data->address) {
        default:
            amp_printf("ERROR: addr: %d, cmd: %x\n", data->address, data->command);
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
            amp_printf("ERROR: mode: %c, cmd: %x\n", mode, data->command);
            return;
            break;
        case smsl_IR_butt_PWR:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "POWER");
            break;
        case smsl_IR_butt_MUTE:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "MUTE");
            break;
        case smsl_IR_butt_ENTER:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "ENTER");
            break;
        case smsl_IR_butt_UP:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "UP");
            break;
        case smsl_IR_butt_DOWN:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "DOWN");
            break;
        case smsl_IR_butt_LEFT:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "LEFT");
            break;
        case smsl_IR_butt_RIGHT:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "RIGHT");
            break;
        case smsl_IR_butt_SRC:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "SRC");
            break;
        case smsl_IR_butt_FN:
            amp_printf("SMSL REMOTE: mode: %c, butt: %s\n", mode, "FN");
            break;
    }
}

inline uint8_t smsl_remote_check(IRData * data, uint8_t * status, _smsl_IR_addr mode, _smsl_IR_butt butt)
{
    return ((*status)--) && (data->address == mode) && (data->command == butt);
}