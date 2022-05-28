#ifndef __AMP_LOG_H_
#define __AMP_LOG_H_

#include <stdarg.h>0

//LOG
#define log_max_lenght	255

int amp_printf(const char * format,...)
{
    char InputBuff[log_max_lenght];

    va_list    args;
    va_start(args, format);
    vsprintf(InputBuff, format, args);
    va_end(args);

    Serial.print(InputBuff);
    return 0;
}

#endif // __AMP_LOG_H_