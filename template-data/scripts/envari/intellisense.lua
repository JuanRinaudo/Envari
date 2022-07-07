-- #NOTE (Juan): Platform

PLATFORM_EDITOR = 0;
PLATFORM_WINDOWS = 0;
PLATFORM_WASM = 0;

-- #NOTE (Juan): Lua

---@param filepath string
function LoadScriptFile(filepath) end

---@param library LuaLibrary
function LoadLibrary(library) end

---@class LuaLibrary

---@type LuaLibrary
SOL_LIBRARY_BASE = 0;
---@type LuaLibrary
SOL_LIBRARY_PACKAGE = 0;
---@type LuaLibrary
SOL_LIBRARY_COROUTINE = 0;
---@type LuaLibrary
SOL_LIBRARY_STRING = 0;
---@type LuaLibrary
SOL_LIBRARY_OS = 0;
---@type LuaLibrary
SOL_LIBRARY_MATH = 0;
---@type LuaLibrary
SOL_LIBRARY_TABLE = 0;
---@type LuaLibrary
SOL_LIBRARY_DEBUG = 0;
---@type LuaLibrary
SOL_LIBRARY_BIT32 = 0;
---@type LuaLibrary
SOL_LIBRARY_IO = 0;
---@type LuaLibrary
SOL_LIBRARY_BIT32 = 0;
---@type LuaLibrary
SOL_LIBRARY_FFI = 0;
---@type LuaLibrary
SOL_LIBRARY_JIT = 0;
---@type LuaLibrary
SOL_LIBRARY_UTF8 = 0;

-- #NOTE (Juan): Memory

---@class DynamicString
DynamicString = {
    value = "",
}

-- #NOTE (Juan): C/C++

---@return integer
---@param singleChar string
function CharToInt(singleChar) end

---@return string
---@param value integer
function IntToChar(value) end

-- #NOTE (Juan): Data

---@class Game
game = {
    running = false,
    updateRunning = false,
    hasFocus = false,
    version = 0,
    saveSlotID = 0,
}

---@class Render
render = {
    vsync = false,
    framebufferEnabled = false,
    frameBuffer = 0,
    renderBuffer = 0,
    depthrenderbuffer = 0,
    refreshRate = 0,
    ---@type v2
    size = nil,
    ---@type v2
    bufferSize = nil,
    renderScale = 0.0,
    ---@type v2
    scaledBufferSize = nil,
    ---@type v2
    windowPosition = nil,
    ---@type v2
    windowSize = nil,
    defaultFontID = 0,

    ---@type TextureAdjustStyle
    fitStyle = 0,
}

---@class Camera
camera = {
    size = 0.0,
    ratio = 0.0,
    nearPlane = 0.0,
    farPlane = 0.0,
    ---@type m44
    view = nil,
    ---@type m44
    projection = nil,
}

---@class Time
time = {
    realLastFrameGameTime = 0.0,
    lastFrameGameTime = 0.0,
    gameTime = 0.0,
    deltaTime = 0.0,
    gameFrames = 0,
    frames = 0,

    fpsLimit = 0,
    fpsFixed = 0,
    fpsDelta = 0.0,
}

---@class Sound
sound = {
    bindingsEnabled = false,
}

---@class Input
input = {
    mouseTextureID = 0,
    ---@type v2
    mousePosition = nil,
    ---@type v2
    mouseDeltaPosition = nil,
    ---@type v2
    mouseScreenPosition = nil,
    ---@type v2
    mouseScreenDeltaPosition = nil,
    mouseWheel = 0,
    ---@type string
    textInputBuffer = nil,
    ---@type KeyState[]
    keyState = nil,
    ---@type KeyState
    anyReasonableKeyState = nil,
    ---@type KeyState
    anyKeyState = nil,
    ---@type KeyState[]
    mouseState = nil,
    ---@type KeyState
    anyMouseState = nil,
}

---@param texture TextureAsset
function SetCustomCursor(texture) end

function DisableCustomCursor() end

KEY_COUNT = 0;
MOUSE_COUNT = 0;
TEXT_INPUT_BUFFER_COUNT = 0;

---@class KeyState

---@type KeyState
KEY_UP = 0
---@type KeyState
KEY_RELEASED = 0
---@type KeyState
KEY_PRESSED = 0
---@type KeyState
KEY_DOWN = 0

-- #NOTE (Juan): Temporal memory

---@return number[]
---@param value number
function RenderTemporaryPushFloat(value) end

---@return v2[]
---@param value v2
function RenderTemporaryPushVector2(value) end

-- #NOTE (Juan): Input

---@return boolean
---@param rectangle rectangle2
function MouseOverRectangle(rectangle) end

---@return boolean
---@param rectangle rectangle2
---@param button integer
function ClickOverRectangle(rectangle, button) end

---@return boolean
---@param rectangle rectangle2
---@param button integer
function ClickedOverRectangle(rectangle, button) end

---@return string
function GetClipboardText() end

---@return integer
---@param text string
function SetClipboardText(text) end

-- #NOTE (Juan): Render

---@class TextureAdjustStyle

---@type TextureAdjustStyle
TextureAdjustStyle_Stretch = 0
---@type TextureAdjustStyle
TextureAdjustStyle_FitRatio = 0
---@type TextureAdjustStyle
TextureAdjustStyle_KeepRatioX = 0
---@type TextureAdjustStyle
TextureAdjustStyle_KeepRatioY = 0
    
colorLocation = 0;
mvpLocation = 0;

bufferSizeLocation = 0;
scaledBufferSizeLocation = 0;
textureSizeLocation = 0;
dimensionsLocation = 0;
borderLocation = 0;

timeLocation = 0;

---@param red number
---@param green number
---@param blue number
---@param alpha number
function DrawClear(red, green, blue, alpha) end

---@param filepath string
---@param filepathHovered string
---@param filepathDown string
---@param slice number
function DrawSetStyle(filepath, filepathHovered, filepathDown, slice) end

---@param red number
---@param green number
---@param blue number
---@param alpha number
function DrawColor(red, green, blue, alpha) end

function DrawDefaultTransparent() end

---@param modeRGB integer
---@param modeAlpha integer
---@param srcRGB integer
---@param dstRGB integer
---@param srcAlpha integer
---@param dstAlpha integer
function DrawTransparent(modeRGB, modeAlpha, srcRGB, dstRGB, srcAlpha, dstAlpha) end

function DrawTransparentDisable() end

---@param targetLayer integer
---@param transparent boolean
function DrawSetLayer(targetLayer, transparent) end

---@param posX number
---@param posY number
---@param scaleX number
---@param scaleY number
---@param angle number
function DrawSetTransform(posX, posY, scaleX, scaleY, angle) end

---@param posX number
---@param posY number
---@param scaleX number
---@param scaleY number
---@param angle number
function DrawPushTransform(posX, posY, scaleX, scaleY, angle) end

function DrawPopTransform() end

function DrawDefaultTransform() end

---@param width number
function DrawLineWidth(width) end

---@param startX number
---@param startY number
---@param endX number
---@param endY number
function DrawLine(startX, startY, endX, endY) end

---@param p1X number
---@param p1Y number
---@param p2X number
---@param p2Y number
---@param p3X number
---@param p3Y number
function DrawTriangle(p1X, p1Y, p2X, p2Y, p3X, p3Y) end

---@param posX number
---@param posY number
---@param sizeX number
---@param sizeY number
function DrawRectangle(posX, posY, sizeX, sizeY) end

---@param posX number
---@param posY number
---@param radius number
---@param segments integer
function DrawCircle(posX, posY, radius, segments) end

---@param instanceCount integer
---@param positions number[]
---@param radius number
---@param segments integer
function DrawInstancedCircles(instanceCount, positions, radius, segments) end

---@param wrapS integer
---@param wrapT integer
---@param minFilter integer
---@param magFilter integer
function DrawTextureParameters(wrapS, wrapT, minFilter, magFilter) end

---@param posX number
---@param posY number
---@param sizeX number
---@param sizeY number
---@param textureID integer
function DrawTexture(posX, posY, sizeX, sizeY, textureID) end

---@param posX number
---@param posY number
---@param filepath string
---@param renderFlags integer
function DrawImage(posX, posY, filepath, renderFlags) end

---@param posX number
---@param posY number
---@param uvX number
---@param uvY number
---@param uvEndX number
---@param uvEndY number
---@param filepath string
function DrawImageUV(posX, posY, uvX, uvY, uvEndX, uvEndY, filepath) end

---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param slice number
---@param filepath string
function DrawImage9Slice(posX, posY, endX, endY, slice, filepath) end

---@param posX number
---@param posY number
---@param filepath string
---@param atlasName string
---@param key string
function DrawAtlasSprite(posX, posY, filepath, atlasName, key) end

---@param fontID integer
function DrawSetFont(fontID) end

---@param posX number
---@param posY number
---@param singleChar string
function DrawChar(posX, posY, singleChar) end

---@param posX number
---@param posY number
---@param string string
---@param renderFlags integer
function DrawString(posX, posY, string, renderFlags) end

---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param string string
---@param renderFlags integer
function DrawStyledString(posX, posY, endX, endY, string, renderFlags) end

function ClearInputBuffer() end

---@return boolean
---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param baseText string
---@param maxSize integer
function DrawStringInput(posX, posY, endX, endY, baseText, maxSize) end

---@return boolean
---@param posX number
---@param posY number
---@param endX number
---@param endY number
---@param label string
function DrawButton(posX, posY, endX, endY, label) end

---@return integer
---@param posX number
---@param posY number
---@param endX number
---@param height number
---@param yPadding number
---@param options string
function DrawMultibutton(posX, posY, endX, height, yPadding, options) end

---@param locationID integer
---@param type integer
function DrawSetUniform(locationID, type) end

---@param programID integer
function DrawOverrideProgram(programID) end

function DrawDisableOverrideProgram() end

---@param vertices number[]
---@param count integer
function DrawOverrideVertices(vertices, count) end

function DrawDisableOverrideVertices() end

---@param indices integer[]
---@param count integer
function DrawOverrideIndices(indices, count) end

function DrawDisableOverrideIndices() end

---@return v4
---@param hex integer
function ColorHexRGBA(hex) end

---@return v4
---@param hex integer
function ColorHexRGB(hex) end
    
function GetSceneFilepath() end

---@param luaFilepath string
function LoadLUAScene(luaFilepath) end

---@class ImageRenderFlag

---@type ImageRenderFlag
ImageRenderFlag_Fit = 0;
---@type ImageRenderFlag
ImageRenderFlag_KeepRatioX = 0;
---@type ImageRenderFlag
ImageRenderFlag_KeepRatioY = 0;
---@type ImageRenderFlag
ImageRenderFlag_NoMipMaps = 0;

---@class TextRenderFlag

---@type TextRenderFlag
TextRenderFlag_Left = 0;
---@type TextRenderFlag
TextRenderFlag_Center = 0;
---@type TextRenderFlag
TextRenderFlag_Right = 0;
---@type TextRenderFlag
TextRenderFlag_LetterWrap = 0;
---@type TextRenderFlag
TextRenderFlag_WordWrap = 0;

---@return m44
---@param fovY number
---@param aspect number
---@param nearPlane number
---@param farPlane number
function PerspectiveProjection(fovY, aspect, nearPlane, farPlane) end

---@return m44
---@param size number
---@param aspect number
---@param nearPlane number
---@param farPlane number
function OrtographicProjection(size, aspect, nearPlane, farPlane) end

---@return number
---@param x number
---@param y number
function Perlin2D(x, y) end

---@return number
---@param x number
---@param y number
function Perlin2DInt(x, y) end

---@return number
---@param x number
---@param y number
---@param octaves integer
---@param frecuency number
function Perlin2DOctaves(x, y, octaves, frecuency) end

-- #NOTE (Juan): GLRender

---@class TextureAsset
TextureAsset = {
    textureID = 0,
    width = 0,
    height = 0,
    channels = 0,
}

---@param textureID integer
---@param width number
---@param height number
function LoadTextureID(textureID, width, height) end

---@return TextureAsset
---@param texturePath string
function LoadSceneTexture(texturePath) end

---@return TextureAsset
---@param texturePath string
function LoadPermanentTexture(texturePath) end

---@return integer, integer
---@param texturePath string
function TextureSize(texturePath) end

-- function TextureSizeID() end

---@return integer
---@param filepath string
---@param fontSize string
---@param width number
---@param height number
function GenerateFont(filepath, fontSize, width, height) end

DEFAULT_FONT_ATLAS_WIDTH = 0;
DEFAULT_FONT_ATLAS_HEIGHT = 0;

---@return integer
---@param filepath string
---@param glyphs string
---@param glyphWidth number
---@param glyphHeight number
function GenerateBitmapFontStrip(filepath, glyphs, glyphWidth, glyphHeight) end

---@return integer
---@param vertexShaderPath string
---@param fragmentShaderPath string
function CompileProgram(vertexShaderPath, fragmentShaderPath) end

---@return integer
---@param vertexShaderPath string
---@param fragmentShaderPath string
function CompileProgramPlatform(vertexShaderPath, fragmentShaderPath) end

---@return integer
---@param program integer
---@param name string
function GetUniformLocation(program, name) end

---@param programID integer
---@param locationID integer
---@param v0 number
function SetUniform1F(programID, locationID, v0) end

---@param programID integer
---@param locationID integer
---@param v0 number
---@param v1 number
function SetUniform2F(programID, locationID, v0, v1) end

---@param programID integer
---@param locationID integer
---@param v0 number
---@param v1 number
---@param v2 number
function SetUniform3F(programID, locationID, v0, v1, v2) end

---@param programID integer
---@param locationID integer
---@param v0 number
---@param v1 number
---@param v2 number
---@param v3 number
function SetUniform4F(programID, locationID, v0, v1, v2, v3) end

---@class UniformType

---@type UniformType
UniformType_Float = 0;
---@type UniformType
UniformType_Vector2 = 0;

-- #NOTE (Juan): OpenGL

GL_ZERO = 0;
GL_ONE = 0;
GL_FUNC_ADD = 0;
GL_FUNC_SUBTRACT = 0;
GL_FUNC_REVERSE_SUBTRACT = 0;
GL_MIN = 0;
GL_MAX = 0;
GL_SRC_COLOR = 0;
GL_ONE_MINUS_SRC_COLOR = 0;
GL_SRC_ALPHA = 0;
GL_ONE_MINUS_SRC_ALPHA = 0;
GL_DST_ALPHA = 0;
GL_ONE_MINUS_DST_ALPHA = 0;
GL_DST_COLOR = 0;
GL_ONE_MINUS_DST_COLOR = 0;
GL_CONSTANT_COLOR = 0;
GL_ONE_MINUS_CONSTANT_COLOR = 0;
GL_CONSTANT_ALPHA = 0;
GL_ONE_MINUS_CONSTANT_ALPHA = 0;
GL_SRC_ALPHA_SATURATE = 0;

GL_TEXTURE_2D = 0;
GL_TEXTURE_2D_ARRAY = 0;
GL_TEXTURE_3D = 0;
GL_TEXTURE_WRAP_S = 0;
GL_TEXTURE_WRAP_T = 0;
GL_TEXTURE_WRAP_R = 0;
GL_CLAMP_TO_EDGE = 0;;
GL_MIRRORED_REPEAT = 0;
GL_REPEAT = 0;

GL_TEXTURE_MIN_FILTER = 0;
GL_TEXTURE_MAG_FILTER = 0;
GL_NEAREST = 0;
GL_LINEAR = 0;
GL_NEAREST_MIPMAP_NEAREST = 0;
GL_LINEAR_MIPMAP_NEAREST = 0;
GL_NEAREST_MIPMAP_LINEAR = 0;
GL_LINEAR_MIPMAP_LINEAR = 0;
GL_TEXTURE_BASE_LEVEL = 0;
GL_TEXTURE_COMPARE_FUNC = 0;
GL_TEXTURE_COMPARE_MODE = 0;
GL_TEXTURE_MIN_LOD = 0;
GL_TEXTURE_MAX_LOD = 0;
GL_TEXTURE_MAX_LEVEL = 0;
GL_TEXTURE_SWIZZLE_R = 0;
GL_TEXTURE_SWIZZLE_G = 0;
GL_TEXTURE_SWIZZLE_B = 0;
GL_TEXTURE_SWIZZLE_A = 0;

GL_SRC1_COLOR = 0;
GL_ONE_MINUS_SRC1_COLOR = 0;
GL_SRC1_ALPHA = 0;
GL_ONE_MINUS_SRC1_ALPHA = 0;
GL_TEXTURE_1D = 0;
GL_TEXTURE_1D_ARRAY = 0;
GL_TEXTURE_2D_MULTISAMPLE = 0;
GL_TEXTURE_2D_MULTISAMPLE_ARRAY = 0;
GL_TEXTURE_CUBE_MAP = 0;
GL_TEXTURE_CUBE_MAP_ARRAY = 0;
GL_TEXTURE_RECTANGLE = 0;
GL_CLAMP_TO_BORDER = 0;
GL_MIRROR_CLAMP_TO_EDGE = 0;

GL_DEPTH_STENCIL_TEXTURE_MODE = 0;
GL_TEXTURE_LOD_BIAS = 0;

-- #NOTE (Juan): Sound

---@class SoundInstance

---@return SoundInstance
---@param filepath string
---@param volume number
---@param unique boolean
function PlaySound(filepath, volume, unique) end

---@return SoundInstance
---@param filepath string
---@param volume number
---@param loop boolean
---@param unique boolean
function PlaySoundLoop(filepath, volume, loop, unique) end

---@param instance SoundInstance
function StopSound(instance) end

---@param value number
function SetMasterVolume(value) end

---@return number
---@param db number
function dbToVolume(db) end

---@return number
---@param volume number
function volumeToDB(volume) end

-- #NOTE (Juan): Console

---@param log string
function LogConsole(log) end

---@param log string
function LogConsoleError(log) end

---@param log string
function LogConsoleCommand(log) end

-- #NOTE (Juan): Serialization
function SaveData() end

---@return string
---@params key string
---@params defaultValue string
function SaveGetString(key, defaultValue) end

---@params key string
---@params value string
function SaveSetString(key, value) end

---@return boolean
---@params key string
---@params defaultValue boolean
function SaveGetBool(key, defaultValue) end

---@params key string
---@params value boolean
function SaveSetBool(key, value) end

---@return integer
---@params key string
---@params defaultValue integer
function SaveGetI32(key, defaultValue) end

---@params key string
---@params value integer
function SaveSetI32(key, value) end

---@return number
---@params key string
---@params defaultValue number
function SaveGetF32(key, defaultValue) end

---@params key string
---@params value number
function SaveSetF32(key, value) end

---@return v2
---@params key string
---@params defaultValue v2
function SaveGetV2(key, defaultValue) end

---@params key string
---@params value v2
function SaveSetV2(key, value) end

---@return string
---@params key string
---@params defaultValue string
function EditorSaveGetString(key, defaultValue) end

---@params key string
---@params value string
function EditorSaveSetString(key, value) end

---@return boolean
---@params key string
---@params defaultValue boolean
function EditorSaveGetBool(key, defaultValue) end

---@params key string
---@params value boolean
function EditorSaveSetBool(key, value) end

---@return integer
---@params key string
---@params defaultValue integer
function EditorSaveGetI32(key, defaultValue) end

---@params key string
---@params value integer
function EditorSaveSetI32(key, value) end

---@return number
---@params key string
---@params defaultValue number
function EditorSaveGetF32(key, defaultValue) end

---@params key string
---@params value number
function EditorSaveSetF32(key, value) end

---@return v2
---@params key string
---@params defaultValue v2
function EditorSaveGetV2(key, defaultValue) end

---@params key string
---@params value v2
function EditorSaveSetV2(key, value) end

-- #NOTE (Juan): Runtime
function RuntimeQuit() end

-- #NOTE (Juan): Editor
---@param flag integer
function ChangeLogFlag(flag) end

---@class ShaderDebuggerWindow
editorShaderDebugger = {
    programIndexChanged = false,
    programIndex = 0,
}

LogFlag_PERFORMANCE = 0;
LogFlag_RENDER = 0;
LogFlag_MEMORY = 0;
LogFlag_TEXTURE = 0;
LogFlag_SOUND = 0;
LogFlag_INPUT = 0;
LogFlag_TIME = 0;
LogFlag_LUA = 0;

LogFlag_GAME = 0;
LogFlag_SCRIPTING = 0;

ImGuiCond_FirstUseEver = 0;

---@class ImDrawList

---@param width number
---@param height number
---@param cond integer
function ImGuiSetNextWindowSize(width, height, cond) end

---@param minX number
---@param minY number
---@param maxX number
---@param maxY number
function ImGuiSetNextWindowSizeConstraints(minX, minY, maxX, maxY) end

---@return boolean, boolean
---@param name string
---@param open boolean
---@param flags integer
function ImGuiBegin(name, open, flags) end

function ImGuiEnd() end

---@param offset_from_start_x number
---@param spacing_w number
function ImGuiSameLine(offset_from_start_x, spacing_w) end

function ImGuiSpacing() end

function ImGuiSeparator() end

---@param width number
---@param height number
function ImGuiDummy(width, height) end

---@return boolean, integer
---@param label string
---@param value integer
---@param step integer
---@param fastStep integer
---@param flags integer
function ImGuiInputInt(label, value, step, fastStep, flags) end

---@return boolean, number
---@param label string
---@param value number
---@param step number
---@param fastStep number
---@param flags integer
function ImGuiInputFloat(label, value, step, fastStep, flags) end

---@return boolean
---@param label string
function ImGuiSmallButton(label) end

---@return boolean
---@param label string
---@param width number
---@param height number
function ImGuiButton(label, width, height) end

---@param id integer
---@param width number
---@param height number
function ImGuiImage(id, width, height) end

---@return boolean
---@param id integer
---@param width number
---@param height number
function ImGuiImageButton(id, width, height) end

---@param styleColor integer
---@param r number
---@param g number
---@param b number
---@param a number
function ImGuiPushStyleColor(styleColor, r, g, b, a) end

ImGuiCol_Button = 0;

---@param count integer
function ImGuiPopStyleColor(count) end

---@param item_width number
function ImGuiPushItemWidth(item_width) end

function ImGuiPopItemWidth() end

---@param str_id string
function ImGuiPushID(str_id) end

function ImGuiPopID() end

---@return boolean
---@param label string
---@param enabled boolean
function ImGuiBeginMenu(label, enabled) end

function ImGuiEndMenu() end

---@return boolean, boolean
---@param label string
---@param value boolean
function ImGuiCheckbox(label, value) end

---@param text string
---@param text_end string
function ImGuiTextUnformatted(text, text_end) end

---@return boolean, string
---@param label string
function ImGuiInputText(label) end

---@return ImDrawList[]
function ImGuiGetWindowDrawList() end

---@return number, number
function ImGuiGetCursorScreenPos() end

---@param drawList ImDrawList[]
---@param id integer
---@param x number
---@param y number
---@param width number
---@param height number
function ImGuiAddImage(drawList, id, x, y, width, height) end

-- #NOTE (Juan): Math usertypes

---@class v2
v2 = {
    x = 0.0,
    y = 0.0,
    ---@type number[]
    e = nil,
}

---@return v2
---@param x number
---@param y number
function V2(x, y) end

---@class v3
v3 = {
    x = 0.0,
    y = 0.0,
    z = 0.0,
    ---@type number[]
    e = nil,
}

---@return v3
---@param x number
---@param y number
---@param z number
function V3(x, y, z) end

---@class v4
v4 = {
    x = 0.0,
    y = 0.0,
    z = 0.0,
    w = 0.0,
    ---@type number[]
    e = nil,
}

---@return v4
---@param x number
---@param y number
---@param z number
---@param w number
function V4(x, y, z, w) end

---@param _00 number
---@param _01 number
---@param _10 number
---@param _11 number
function M22(_00, _01, _10, _11) end
function IdM22() end

---@class m33
m33 = {
    _00 = 0.0,
    _10 = 0.0,
    _20 = 0.0,
    _01 = 0.0,
    _11 = 0.0,
    _21 = 0.0,
    _02 = 0.0,
    _12 = 0.0,
    _22 = 0.0,
    ---@type number[]
    e = nil,
}

---@param _00 number
---@param _01 number
---@param _02 number
---@param _10 number
---@param _11 number
---@param _12 number
---@param _20 number
---@param _21 number
---@param _22 number
function M33(_00, _01, _02, _10, _11, _12, _20, _21, _22) end
function IdM33() end

---@class m44
m44 = {
    _00 = 0.0,
    _10 = 0.0,
    _20 = 0.0,
    _30 = 0.0,
    _01 = 0.0,
    _11 = 0.0,
    _21 = 0.0,
    _31 = 0.0,
    _02 = 0.0,
    _12 = 0.0,
    _22 = 0.0,
    _32 = 0.0,
    _03 = 0.0,
    _13 = 0.0,
    _23 = 0.0,
    _33 = 0.0,
    ---@type number[]
    e = nil,
}

---@param _00 number
---@param _01 number
---@param _02 number
---@param _03 number
---@param _10 number
---@param _11 number
---@param _12 number
---@param _13 number
---@param _20 number
---@param _21 number
---@param _22 number
---@param _23 number
---@param _30 number
---@param _31 number
---@param _32 number
---@param _33 number
function M44(_00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33) end
---@param m m33
function M44fromM33(m) end
function IdM44() end

---@class rectangle2
rectangle2 = {
    x = 0.0,
    y = 0.0,
    width = 0.0,
    height = 0.0,
}

---@return rectangle2
---@param x number
---@param y number
---@param width number
---@param height number
function Rectangle2(x, y, width, height) end

---@class transform2D
transform2D = {
    ---@type v2
    position = nil,
    ---@type v2
    scale = nil,
    angle = 0.0,
}

---@return transform2D
---@param posX number
---@param posY number
---@param scaleX number
---@param scaleY number
---@param angle number
function Transform2D(posX, posY, scaleX, scaleY, angle) end

--     // lua["LengthV2"] = sol::resolve<f32(v2)>(Length);
--     // lua["NormalizeV2"] = sol::resolve<v2(v2)>(Normalize);
--     // lua["LengthSqV2"] = sol::resolve<f32(v2)>(LengthSq);

--     // #NOTE (Juan): Math custom functionality
--     // lua["math"]["sign"] = Sign;
--     // lua["math"]["round"] = RoundToInt;
--     // lua["math"]["rotLeft"] = RotateLeft;
--     // lua["math"]["rotRight"] = RotateRight;
--     // lua["math"]["sqr"] = Square;
--     // lua["math"]["floorV2"] = sol::resolve<v2(v2)>(Floor);
--     lua["math"]["lerp"] = Lerp;

---@return number
---@param a number
---@param b number
---@param t number
function math.lerp(a, b, t) end