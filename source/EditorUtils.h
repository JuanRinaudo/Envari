#ifndef EDITOR_UTILS_H
#define EDITOR_UTILS_H

const char* GetWrapTypeName(u32 wrapType)
{
    switch (wrapType)
    {
    case GL_CLAMP_TO_EDGE:
        return "Clamp to edge";
    case GL_MIRRORED_REPEAT:
        return "Mirrored Repeat";
    case GL_REPEAT:
        return "Repeat";
    default:
        return "Unknown";
    }
}

const char* GetFilteringTypeName(u32 wrapType)
{
    switch (wrapType)
    {
    case GL_NEAREST:
        return "Nearest";
    case GL_LINEAR:
        return "Linear";
    case GL_NEAREST_MIPMAP_NEAREST:
        return "Nearest mipmap mearest";
    case GL_LINEAR_MIPMAP_NEAREST:
        return "Linear mipmap nearest";
    case GL_NEAREST_MIPMAP_LINEAR:
        return "Nearest mipmap linear";
    case GL_LINEAR_MIPMAP_LINEAR:
        return "Linear mipmap linear";
    default:
        return "Unknown";
    }
}

#endif