#define DATA_H
#if !defined(DATA_H)

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

static bool ParseDataTable(DataTable** table, const char* filename)
{
    u32 dataSize = 0;
    void* fileData = LoadFileToMemory(filename, "rb", &dataSize);

    char* dataString = (char*)fileData;

    u32 parameterCount = 0;
    u32 tokenBufferIndex = 0;
    // #TODO (Juan): Currently there is a hard cap for a token length, maybe fix this later?
    char tokenBuffer[128];
    bool onComment = false;
    bool parsingString = false;
    bool parsingKey = true; 
	for (i32 i = 0; i < dataSize; ++i) {
        char currentChar = dataString[i];
        
        if(!parsingString && currentChar == '#') {
            onComment = true;
        }

        // #NOTE #PERFORMANCE (Juan): This does a linear scan of the current data to check the ammount of parameters
        if(parameterCount == 0 && !parsingKey) {
            i32 savedIndex = i;
            while(parsingString || (currentChar != ';' && currentChar != '#')) {
                currentChar = dataString[i];
				if (!parsingString && dataString[i - 1] <= ' ' && currentChar > ' ') {
					parameterCount++;
				}
				if(currentChar == '"') {
                    parsingString = !parsingString;
                }

                i++;
            }
            console.AddLog("Parameters: %d", parameterCount);
            i = savedIndex;
            currentChar = dataString[i];
        }

		if (!onComment && currentChar > ' ') {
            if(currentChar == '"') {
                parsingString = !parsingString;
            }

            if(parsingKey && currentChar == ':') {
                parsingKey = false;
                tokenBuffer[tokenBufferIndex] = '\0';
                char* tokenPointer = (char*)PushArray(&permanentState->arena, tokenBufferIndex, char);
                strcpy(tokenPointer, tokenBuffer);
                console.AddLog(tokenBuffer);
                tokenBufferIndex = 0;
            }
            else {
                tokenBuffer[tokenBufferIndex] = currentChar;
                tokenBufferIndex++;
            
                if(tokenBufferIndex == ArrayCount(tokenBuffer)) {
                    console.AddLog("%s parsing error, max token count reached %d", filename, ArrayCount(tokenBuffer));
                    return false;
                }
            }
		}        
        // #NOTE (Juan): End of line stop one line comments
		if (!parsingString && (currentChar == ';' || (onComment && currentChar == '\n'))) {
            onComment = false;
            parsingKey = true;
            tokenBufferIndex = 0;
            parameterCount = 0;
		}
	}

    return true;
}

#endif