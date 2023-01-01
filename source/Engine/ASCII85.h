#ifndef ASCII85_H
#define ASCII85_H

// Taken from dear imgui decoding/encoding
// binary_to_compressed_c.cpp
// imgui_draw.cpp

static unsigned int ASCII85DecodeByte(char c) 
{
    return c >= '\\' ? c-36 : c-35;
}

static void ASCII85Decode(char* src, u8* dst)
{
    while (*src)
    {
        u32 tmp = ASCII85DecodeByte(src[0]) + 85 * (ASCII85DecodeByte(src[1]) + 85 * (ASCII85DecodeByte(src[2]) + 85 * (ASCII85DecodeByte(src[3]) + 85 * ASCII85DecodeByte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}

static char Encode85Byte(u32 x)
{
    x = (x % 85) + 35;
    return (char)((x >= '\\') ? x + 1 : x);
}

static u32 GetEncoded85Size(u32 sourceSize)
{
    return ((u32)((sourceSize + 3) / 4)) * 5;
}

static char* Encode85(char* src)
{
    u32 sourceSize = strlen(src);
    // u32 encodedSize = GetEncoded85Size(sourceSize);
    char* encoded = (char*)malloc(sourceSize);
    
    char prev_c = 0;
    u32 dst_i = 0;
    for (u32 src_i = 0; src_i < sourceSize; src_i += 4)
    {
        // This is made a little more complicated by the fact that ??X sequences are interpreted as trigraphs by old C/C++ compilers. So we need to escape pairs of ??.
        u32 d = *(u32*)(src + src_i);
        for (u32 n5 = 0; n5 < 5; n5++, d /= 85)
        {
            char c = Encode85Byte(d);

            if((c == '?' && prev_c == '?') ? "\\%c" : "%c") {
                encoded[dst_i] = '\\';
                dst_i++;
            }
            encoded[dst_i] = c;
            dst_i++;

            prev_c = c;
        }
    }

    return encoded;    
}

#endif