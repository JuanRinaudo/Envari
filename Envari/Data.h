#if !defined(DATA_H)
#define DATA_H

#include <cstdlib>

// #TODO (Juan): For now this works only with the ASCII and extended ASCII codes, add more support later

// #NOTE #PERFORMANCE (Juan): This could be changed for a function that returns the correct type and checks the string only once 
static bool TokenIsInt(const char* string)
{
    i32 i = 0;
    while(string[i] != '\0') {
        i32 normalizedChar = (i32)string[i] - 48;
        if(normalizedChar < 0 || normalizedChar > 9) {
            return false;
        }
        ++i;
    }
    return true;
}

static bool TokenIsFloat(const char* string)
{    
    i32 i = 0;
    bool hasDot = false;
    while(string[i] != '\0') {
        i32 normalizedChar = (i32)string[i] - 48;
        if(string[i] == '.') {
            if(hasDot) {
                return false;
            }
            else {
                hasDot = true;
            }
        } else if(normalizedChar < 0 || normalizedChar > 9) {
            return false;
        }
        ++i;
    }
    return true;
}

// #NOTE (Juan): This does a linear scan of the current data to check the ammount of parameters, dataString must point to the start of the line
static i32 GetDataLineParameters(char* dataString, i32 index) {
    char currentChar = dataString[index];
    bool parsingString = false;
    bool parsingKey = false;
    i32 parameterCount = 0;
    while(parsingString || (currentChar != ';' && currentChar != '#')) {
        currentChar = dataString[index];
        if (!parsingString && dataString[index - 1] <= ' ' && currentChar > ' ') {
            parameterCount++;
        }
        if(currentChar == '"') {
            parsingString = !parsingString;
        }

        index++;
    }

    currentChar = dataString[index];
}

static bool ParseDataTable(DataTable** table, const char* filename)
{
    u32 dataSize = 0;
    void* fileData = LoadFileToMemory(filename, "rb", &dataSize);

    char* dataString = (char*)fileData;

    u32 tokenBufferIndex = 0;
    // #TODO (Juan): Currently there is a hard cap for a token length, maybe fix this later?
    char tokenBuffer[128];
    bool onComment = false;
    bool parsingString = false;
    bool parsingKey = true; 

    i32 index = 0;
    while (index < dataSize) {
        char currentChar = dataString[index];        
        
        if(!parsingString && currentChar == '#') {
            onComment = true;
        }

        if (parsingString || (!onComment && currentChar > ' ')) {
            if(currentChar == '"') {
                parsingString = !parsingString;
            }
            
            // #NOTE (Juan): End of line stop one line comments
            if (!parsingString && currentChar == ';') {
                // NOTE (Juan): Double 0 character represents and end of the parameters in the data
                PushChar(&permanentState->arena, '\0');
                parsingKey = true;
                tokenBufferIndex = 0;
            }
            else {
                if(parsingKey && currentChar == ':') {
                    parsingKey = false;
                    tokenBuffer[tokenBufferIndex] = '\0';
                    PushString(&permanentState->arena, tokenBuffer, tokenBufferIndex);
                    tokenBufferIndex = 0;

                    console.AddLog(tokenBuffer);
                }
                else {
                    tokenBuffer[tokenBufferIndex] = currentChar;
                    tokenBufferIndex++;
                    
                    if(tokenBufferIndex == ArrayCount(tokenBuffer)) {
                        console.AddLog("%s parsing error, max token count reached %d", filename, ArrayCount(tokenBuffer));
                        return false;
                    }

                    if(!parsingKey && !parsingString) {
                        char nextChar = dataString[index + 1];
                        if(nextChar == ';' || nextChar == ' ') {
                            tokenBuffer[tokenBufferIndex] = '\0';
                            PushString(&permanentState->arena, tokenBuffer, tokenBufferIndex);
                            tokenBufferIndex = 0;

                            console.AddLog(tokenBuffer);
                        }
                    }
                }
            }
        }

        if(onComment && currentChar == '\n') {
            onComment = false;
        }

        ++index;
    }

    return true;
}

#endif