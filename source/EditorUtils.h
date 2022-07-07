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

const char* GetShaderTypeName(u32 vertexType)
{
    switch (vertexType)
    {
    case GL_VERTEX_SHADER:
        return "Vertex";
    case GL_GEOMETRY_SHADER:
        return "Geometry";
    case GL_FRAGMENT_SHADER:
        return "Fragment";
    default:
        return "Unknown";
    }
}

const char* GetErrorSourceName(u32 sourceType)
{
    switch (sourceType)
    {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "Window system";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "Shader compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "Third party";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "Application";
        case GL_DEBUG_SOURCE_OTHER:
            return "Other";
        default:
            return "Unknown";
    }
}

const char* GetErrorTypeName(u32 errorType)
{
    switch (errorType)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "Deprecated behavior";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "Undefined behavior";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "Performance";
        case GL_DEBUG_TYPE_MARKER:
            return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "Push group";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "Pop group";
        case GL_DEBUG_TYPE_OTHER:
            return "Other";
        default:
            return "Unknown";
    }
}

const char* GetErrorSeverityName(u32 severityType)
{
    switch (severityType)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            return "High";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "Medium";
        case GL_DEBUG_SEVERITY_LOW:
            return "Low";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "Notification";
        default:
            return "Unknown";
    }
}

#endif