#ifndef RUNTIME_H
#define RUNTIME_H

void Log_(ConsoleLogType type, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf(fmt, args);
    va_end(args);
}

#define Log(fmt, ...) Log_(ConsoleLogType_NORMAL, fmt, ##__VA_ARGS__)
#define LogError(fmt, ...) Log_(ConsoleLogType_ERROR, fmt, ##__VA_ARGS__)
#define LogCommand(fmt, ...) Log_(ConsoleLogType_COMMAND, fmt, ##__VA_ARGS__)

#endif