#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

// #NOTE (Juan): Math
union v2
{
    struct
    {
        f32 x, y;
    };
    f32 e[2];
};

union v3
{
    struct
    {
        f32 x, y, z;
    };
    struct
    {
        f32 r, g, b;
    };
    struct
    {
        v2 xy;
        f32 IgnoredZ_;
    };
    f32 e[3];
};

union v4
{
    struct
    {
        f32 x, y, z, w;
    };
    struct
    {
        f32 r, g, b, a;
    };
    f32 e[4];
};

union m22
{
    struct
    {
        f32 _00, _01,
            _10, _11;
    };
    f32 e[4];
};

union m33
{
    struct
    {
        f32 _00, _01, _02,
            _10, _11, _12,
            _20, _21, _22;
    };
    f32 e[9];
};

union m44
{
    struct
    {
        f32 _00, _01, _02, _03,
            _10, _11, _12, _13,
            _20, _21, _22, _23,
            _30, _31, _32, _33;
    };
    f32 e[16];
};

struct rectangle2
{
    v2 min;
    v2 max;
};

struct rectangle3
{
    v3 min;
    v3 max;
};

struct transform2D
{
    v2 position;
    v2 scale;
};

// #NOTE (Juan): Memory
struct MemoryArena {
    size_t size;
    u8 *base;
    size_t used;

    u32 tempCount;
};

struct TemporaryMemory
{
    MemoryArena *arena;
    size_t used;
};

// #NOTE (Juan): Render
struct GLRenderBuffer {
    u32 vertexArray;
    u32 vertexBuffer;
    u32 indexBuffer;
};

struct GLTexture {
    u32 textureID;
    u32 width;
    u32 height;
    u32 channels;
};
struct GLTextureCache {
    char* key;
    GLTexture value;
};

struct AtlasSprite {
    char* key;
    rectangle2 value;
};

struct TextureAtlas {
    AtlasSprite* sprites;
};

struct GLTextureAtlasReference {
    char* key;
    TextureAtlas value;
};

struct FontAtlas {
    char* fontFilepath;
    u32 fontFilepathSize;
    f32 fontSize;
    u32 width;
    u32 height;
    stbtt_bakedchar charData[FONT_CHAR_SIZE];
};

struct GLFontReference {
    char* key;
    FontAtlas value;
};

#ifdef GAME_INTERNAL
struct WatchedProgram {
    u32 vertexShader;
    u32 fragmentShader;
    u32 shaderProgram;
    char vertexFilepath[100];
    char fragmentFilepath[100];
    std::filesystem::file_time_type vertexTime;
    std::filesystem::file_time_type fragmentTime;
};
#endif

enum RenderType
{
    type_RenderClear,
    type_RenderColor,
    type_RenderLineWidth,
    type_RenderTransparent,
    type_RenderLine,
    type_RenderTriangle,
    type_RenderRectangle,
    type_RenderCircle,
    type_RenderTextureParameters,
    type_RenderTexture,
    type_RenderImage,
    type_RenderImageUV,
    type_RenderAtlasSprite,
    type_RenderFont,
    type_RenderChar,
    type_RenderText,
    type_RenderOverrideVertices,
    type_RenderOverrideIndices,
};

struct RenderHeader
{
    i32 id;
    RenderType type;
    u32 renderFlags;
};

struct RenderClear
{
    RenderHeader header;
    v4 color;
};

struct RenderColor
{
    RenderHeader header;
    v4 color;
};

struct RenderLineWidth
{
    RenderHeader header;
    float width;
};

struct RenderTransparent
{
    RenderHeader header;
    bool enabled;
    u32 modeRGB;
    u32 modeAlpha;
    u32 srcRGB;
    u32 dstRGB;
    u32 srcAlpha;
    u32 dstAlpha;
};

struct RenderLine
{
    RenderHeader header;
    v2 start;
    v2 end;
};

struct RenderTriangle
{
    RenderHeader header;
    v2 point1;
    v2 point2;
    v2 point3;
};

struct RenderRectangle
{
    RenderHeader header;
    v2 position;
    v2 scale;
};

struct RenderCircle
{
    RenderHeader header;
    v2 position;
    f32 radius;
    u32 segments;
};

struct RenderTextureParameters
{
    RenderHeader header;
    u32 wrapS;
    u32 wrapT;
    u32 minFilter;
    u32 magFilter;
};

struct RenderTexture
{
    RenderHeader header;
    v2 position;
    v2 scale;
    u32 textureID;
};

struct RenderImage
{
    RenderHeader header;
    v2 position;
    v2 scale;
    char* filepath;
    u32 filepathSize;
};

struct RenderImageUV
{
    RenderHeader header;
    v2 position;
    v2 scale;
    rectangle2 uv;
    char* filepath;
    u32 filepathSize;
};

struct RenderAtlasSprite
{
    RenderHeader header;
    v2 position;
    v2 scale;
    char* filepath;
    u32 filepathSize;
    char* atlasName;
    u32 atlasNameSize;
    char* spriteKey;
    u32 spriteKeySize;
};

struct RenderFont
{
    RenderHeader header;
    char* filepath;
    u32 filepathSize;
    f32 fontSize;
    u32 width;
    u32 height;
};

struct RenderChar
{
    RenderHeader header;
    v2 position;
    v2 scale;
    char singleChar;
};

struct RenderText
{
    RenderHeader header;
    v2 position;
    v2 scale;
    char* string;
    u32 stringSize;
};

struct RenderOverrideVertices
{
    RenderHeader header;
    f32* vertices;
    u32 size;
};

struct RenderOverrideIndices
{
    RenderHeader header;
    u32* indices;
    u32 size;
};

struct RenderState {
    i32 lastRenderID;
    v4 renderColor;
    u32 wrapS;
    u32 wrapT;
    u32 minFilter;
    u32 magFilter;
    bool overridingVertices;
    bool overridingIndices;
};

// #NOTE (Juan): Game
struct Game {
    bool running;

};

struct Render {
    bool framebufferEnabled;
    u32 frameBuffer;
    u32 renderBuffer;
    u32 depthrenderbuffer;
    i32 refreshRate;
    i32 width;
    i32 height;
    i32 bufferWidth;
    i32 bufferHeight;
};

struct Camera {
    f32 size;
    f32 ratio;
    f32 nearPlane;
    f32 farPlane;
    m44 view;
    m44 projection;
};

struct Time {
    f32 lastFrameGameTime;
    f32 gameTime;
    f32 deltaTime;
    i64 frames;
};

struct Memory {
    size_t permanentStorageSize;
    void *permanentStorage;
    size_t sceneStorageSize;
    void *sceneStorage;
    size_t temporalStorageSize;
    void *temporalStorage;
};

struct Input
{
    v2 mousePosition;
    v2 mouseScreenPosition;
    i32 mouseWheel;
    u8 mouseState[MOUSE_COUNT];
    u8 keyState[KEY_COUNT];
};

struct Data {
    Game game;
    Camera camera;
    Render render;
    Time time;
    Memory memory;
    Input input;
};

struct PermanentData {
    b32 initialized;
    MemoryArena arena;
};

struct SceneData {
    b32 initialized;
    MemoryArena arena;
};

struct TemporalData {
    b32 initialized;
    MemoryArena arena;
};

enum DataType {
    data_Int,
    data_Float,
    data_String,
    data_V2,
};

struct DataTable {
    char* key;
    char* value;
};

// #NOTE (Juan): Changing this enum order will break save games
enum SerializableType {
    SerializableType_CHAR,
    SerializableType_I32,
    SerializableType_F32
};

struct SerializableValue {
    void* value;
    SerializableType type;
    u32 count;
};

struct SerializableTable {
    char* key;
    SerializableValue* value;
};

#endif