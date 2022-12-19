#ifndef UTF8_H
#define UTF8_H

static u32 GetUTF8SizeBackwards(char* string, char* stringStart)
{    
    u32 utfSize = 1;
    if((*(string - utfSize) & 0xC0) == 0x80) { //#NOTE (Juan): Is UTF-8
        while(string - utfSize > stringStart && (*(string - utfSize) & 0xC0) == 0x80) {
            utfSize++;
        }
    }

    return utfSize;
}

static u32 GetUTF8Char(char *string, u32* utfSize)
{
    *utfSize = 0;
    if((string[0] & 0x80) > 0) { //#NOTE (Juan): Is UTF-8
        u32 firstChar = string[0] & 0xF0;
        if(firstChar == 0xC0) { //#NOTE (Juan): 2 byte UTF-8
            *utfSize = 2;
            return (string[0] & 0x1F) << 6 | (string[1] & 0x3F);
        }
        else if(firstChar == 0xE0) { //#NOTE (Juan): 3 byte UTF-8
            *utfSize = 3;
            return (string[0] & 0x7) << 12 | (string[1] & 0x3F) << 6 | (string[2] & 0x3F);
        }
        else if(firstChar == 0xF0) { //#NOTE (Juan): 4 byte UTF-8
            *utfSize = 4;
            return (string[0] & 0x7) << 18 | (string[1] & 0x3F) << 12 | (string[2] & 0x3F) << 6 | (string[3] & 0x3F);
        }
        else { //#NOTE (Juan): Malformed
            return 0;
        }
    }
    else {
        *utfSize = 1;
        return string[0];
    }
}

#endif