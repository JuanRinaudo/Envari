#ifndef RUNTIME_H
#define RUNTIME_H

void Log_(ConsoleLogType type, const char* fmt, ...)
{
    char buffer[1024];
    i32 bufferSize = ArrayCount(buffer);

    va_list args;
    va_start(args, fmt);
    i32 size = vsnprintf(buffer, bufferSize - 2, fmt, args);
    va_end(args);

#if defined(PLATFORM_WASM) || defined(PLATFORM_WINDOWS)
    buffer[size] = '\n';
    
    buffer[size + 1] = 0;
#endif
    buffer[bufferSize-1] = 0;
    printf("%s", buffer);
}

#define Log(fmt, ...) Log_(ConsoleLogType_NORMAL, fmt, ##__VA_ARGS__)
#define LogError(fmt, ...) Log_(ConsoleLogType_ERROR, fmt, ##__VA_ARGS__)
#define LogCommand(fmt, ...) Log_(ConsoleLogType_COMMAND, fmt, ##__VA_ARGS__)
#define LogWarning(fmt, ...) Log_(ConsoleLogType_WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

void ChangeLogFlag_(u32 newFlag) {

};
#define ChangeLogFlag(newFlag) ChangeLogFlag_(newFlag)

#endif