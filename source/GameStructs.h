#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

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
    u32 fontTextureID;
    f32 fontSize;
    f32 lineHeight;
    i32 tabSize;
    u32 width;
    u32 height;
    stbtt_bakedchar charData[FONT_CHAR_SIZE];
};

struct GLFontReference {
    u32 key;
    FontAtlas value;
};

#ifdef GAME_EDITOR
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

enum ConsoleLogType
{
    ConsoleLogType_NORMAL,
    ConsoleLogType_COMMAND,
    ConsoleLogType_ERROR,
};

enum RenderType
{
    RenderType_RenderTempData,
    RenderType_RenderClear,
    RenderType_RenderColor,
    RenderType_RenderLayer,
    RenderType_RenderLineWidth,
    RenderType_RenderTransparent,
    RenderType_RenderLine,
    RenderType_RenderTriangle,
    RenderType_RenderRectangle,
    RenderType_RenderCircle,
    RenderType_RenderTextureParameters,
    RenderType_RenderTexture,
    RenderType_RenderImage,
    RenderType_RenderImageUV,
    RenderType_RenderImage9Slice,
    RenderType_RenderAtlasSprite,
    RenderType_RenderFont,
    RenderType_RenderChar,
    RenderType_RenderText,
    RenderType_RenderStyledText,
    RenderType_RenderButton,
    RenderType_RenderSetUniform,
    RenderType_RenderOverrideProgram,
    RenderType_RenderOverrideVertices,
    RenderType_RenderOverrideIndices,
};

enum UniformType {
    UniformType_Float,
    UniformType_Vector2,
};

enum ImageRenderFlag {
    ImageRenderFlag_Fit = 0x1,
    ImageRenderFlag_KeepRatioX = 0x2,
    ImageRenderFlag_KeepRatioY = 0x4,
};

enum TextRenderFlag {
    TextRenderFlag_Center = 0x1,
    TextRenderFlag_LetterWrap = 0x2,
    TextRenderFlag_WordWrap = 0x4,
};

struct RenderHeader
{
    i32 id;
    RenderType type;
    u32 renderFlags;
    u32 size;
};

struct RenderTempData
{
    RenderHeader header;
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

struct RenderLayer
{
    RenderHeader header;
    u32 layer;
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
    v2 uvMin;
    v2 uvMax;
    char* filepath;
    u32 filepathSize;
};

struct RenderImage9Slice
{
    RenderHeader header;
    v2 position;
    v2 endPosition;
    f32 slice;
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
    i32 fontID;
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
    char* string;
    u32 stringSize;
};

struct RenderStyledText
{
    RenderHeader header;
    v2 position;
    v2 endPosition;
    char* string;
    u32 stringSize;
};

struct RenderSetUniform {
    RenderHeader header;
    u32 location;
    UniformType type;
};

struct RenderOverrideProgram
{
    RenderHeader header;
    u32 programID;
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
    u32 usedLayers;
    u32 transparentLayers;
    u32 wrapS;
    u32 wrapT;
    u32 minFilter;
    u32 magFilter;
    u32 currentProgram;
    u32 overrideProgram;
    bool overridingVertices;
    bool overridingIndices;
};

// #NOTE (Juan): Game
struct Game {
    bool running;
    bool updateRunning;

    i32 version;
};

struct Render {
    bool framebufferEnabled;
    u32 frameBuffer;
    u32 renderBuffer;
    u32 depthrenderbuffer;
    i32 refreshRate;
    v2 size;
    v2 bufferSize;
    v2 windowPosition;
    v2 windowSize;
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
    f32 startTime;
    f32 gameTime;
    f32 deltaTime;
    i64 gameFrames;
    i64 frames;
};

struct Memory {
    size_t permanentStorageSize;
    void *permanentStorage;
    size_t sceneStorageSize;
    void *sceneStorage;
#ifdef GAME_EDITOR
    size_t editorStorageSize;
    void *editorStorage;
#endif
    size_t temporalStorageSize;
    void *temporalStorage;
};

struct Input
{
    u32 mouseTextureID;
    v2 mouseTextureSize;
    v2 mousePosition;
    v2 mouseScreenPosition;
    i32 mouseWheel;
    char textInputBuffer[TEXT_INPUT_BUFFER_COUNT];
    i32 textInputIndex;
    u8 mouseState[MOUSE_COUNT];
    u8 keyState[KEY_COUNT];
};

struct Data {
    Game game;
    Camera camera;
    Render render;
    Time time;
    Input input;
    Memory memory;
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

#ifdef GAME_EDITOR
struct EditorData {
    b32 initialized;
    MemoryArena arena;
};
#endif

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
    SerializableType_STRING,
    SerializableType_BOOL,
    SerializableType_I32,
    SerializableType_F32,
    SerializableType_V2,
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