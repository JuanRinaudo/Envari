#ifndef EXTERNS_H
#define EXTERNS_H

#define PushStruct(arena, type) (type *)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type *)PushSize_(arena, ((count)*sizeof(type)))
#define PushSize(arena, size) PushSize_(arena, size)
extern void *PushSize_(MemoryArena *arena, size_t size);
extern void *PushSize_(TemporaryMemory *memory, size_t size);
extern char *PushString(MemoryArena *arena, const char *string);

// #NOTE(Juan): Engine
extern Data *gameState;
extern PermanentData *permanentState;
extern SceneData *sceneState;
extern TemporalData *temporalState;
extern TemporaryMemory renderTemporaryMemory;

extern SerializableTable* configSave;
extern SerializableTable* saveData;
extern SerializableTable* editorSave;

// #NOTE (Juan): Bindings
extern void SaveData();

extern void LoadLUAScriptFile(const char* filePath);
#ifdef SOL_HPP
extern void LoadLUALibrary(sol::lib library);
#endif

extern void SetCustomCursor(TextureAsset texture);
extern void DisableCustomCursor();

extern v2 V2(f32 x, f32 y);

extern bool MouseOverRectangle(rectangle2 rectangle);
extern bool ClickOverRectangle(rectangle2 rectangle, i32 button);
extern bool ClickedOverRectangle(rectangle2 rectangle, i32 button);

extern m44 PerspectiveProjection(f32 fovY, f32 aspect, f32 nearPlane, f32 farPlane);
extern m44 OrtographicProjection(f32 left, f32 right, f32 top, f32 bottom, f32 nearPlane, f32 farPlane);
extern m44 OrtographicProjection(f32 size, f32 aspect, f32 nearPlane, f32 farPlane);

extern f32 Perlin2D(f32 x, f32 y);
extern f32 Perlin2D(i32 x, i32 y);
extern f32 Perlin2DOctaves(f32 x, f32 y, u32 octaves, f32 frecuency);

extern u32 colorLocation;
extern u32 mvpLocation;

extern u32 bufferSizeLocation;
extern u32 scaledBufferSizeLocation;
extern u32 textureSizeLocation;
extern u32 dimensionsLocation;
extern u32 borderLocation;

extern u32 timeLocation;

extern void DrawClear(f32 red, f32 green, f32 blue, f32 alpha);
extern void DrawSetStyle(const char* filepath, const char* filepathHovered, const char* filepathDown, f32 slice);
extern void DrawColor(f32 red, f32 green, f32 blue, f32 alpha);
extern void DrawTransparent();
extern void DrawTransparent(u32 modeRGB, u32 modeAlpha, u32 srcRGB, u32 dstRGB, u32 srcAlpha, u32 dstAlpha);
extern void DrawTransparentDisable();
extern void DrawSetLayer(u32 targetLayer, bool transparent);
extern void DrawSetTransform(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle);
extern void DrawPushTransform(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle);
extern void DrawPopTransform();
extern void DrawLineWidth(f32 width);
extern void DrawLine(f32 startX, f32 startY, f32 endX, f32 endY);
extern void DrawTriangle(f32 p1X, f32 p1Y, f32 p2X, f32 p2Y, f32 p3X, f32 p3Y);
extern void DrawRectangle(f32 posX, f32 posY, f32 sizeX, f32 sizeY);
extern void DrawCircle(f32 posX, f32 posY, f32 radius, u32 segments);
extern void DrawInstancedCircles(u32 instanceCount, std::vector<f32> positions, f32 radius, u32 segments);
extern void DrawTextureParameters(u32 wrapS, u32 wrapT, u32 minFilter, u32 magFilter);
extern void DrawTexture(f32 posX, f32 posY, f32 sizeX, f32 sizeY, u32 textureID);
extern void DrawImage(f32 posX, f32 posY, const char* filepath, u32 renderFlags);
extern void DrawImageUV(f32 posX, f32 posY, f32 uvX, f32 uvY, f32 uvEndX, f32 uvEndY, const char* filepath);
extern void DrawImage9Slice(f32 posX, f32 posY, f32 endX, f32 endY, f32 slice, const char* filepath);
extern void DrawAtlasSprite(f32 posX, f32 posY, const char* filepath, const char* atlasName, const char* key);
extern void DrawSetFont(i32 fontID);
extern void DrawChar(f32 posX, f32 posY, const char singleChar);
extern void DrawString(f32 posX, f32 posY, const char* string, u32 renderFlags);
extern void DrawStyledString(f32 posX, f32 posY, f32 endX, f32 endY, const char* string, u32 renderFlags);
extern void ClearInputBuffer();
extern bool DrawStringInput(f32 posX, f32 posY, f32 endX, f32 endY, const char* baseText, u32 maxSize);
extern bool DrawButton(f32 posX, f32 posY, f32 endX, f32 endY, const char* label);
extern i32 DrawMultibutton(f32 posX, f32 posY, f32 endX, f32 height, f32 yPadding, const char* options);
extern void DrawSetUniform(u32 locationID, UniformType type);
extern void DrawOverrideProgram(u32 programID);
extern void DrawOverrideVertices(f32* vertices, u32 count);
extern void DrawOverrideIndices(u32* indices, u32 count);
extern v4 ColorHexRGBA(u32 hex);
extern v4 ColorHexRGB(u32 hex);

extern const char* GetSceneFilepath();
extern void LoadLUAScene(const char* luaFilepath);

extern u32 defaultFontID;

extern void BindTextureID(u32 textureID, f32 width, f32 height);
extern TextureAsset LoadTextureFile(const char *texturePath, bool permanentAsset);
extern v2 TextureSize(const char* texturePath);
extern v2 TextureSize(u32 textureID);
extern u32 GenerateFont(const char *filepath, f32 fontSize, u32 width, u32 height);
extern u32 GenerateBitmapFontStrip(const char *filepath, const char* glyphs, u32 glyphWidth, u32 glyphHeight);
extern u32 CompileProgram(const char *vertexShaderSource, const char *fragmentShaderSource);
extern u32 CompileProgramPlatform(const char *vertexShaderPlatform, const char *fragmentShaderPlatform);

extern SoundInstance* PlaySound(const char* filepath, f32 volume, bool loop, bool unique);
extern void StopSound(SoundInstance* instance);
extern void SetMasterVolume(float value);
extern float dbToVolume(float db);
extern float volumeToDB(float volume);

extern v2 V2(f32 x, f32 y);
extern v3 V3(f32 x, f32 y, f32 z);
extern v4 V4(f32 x, f32 y, f32 z, f32 w);

extern rectangle2 Rectangle2(f32 x, f32 y, f32 width, f32 height);

extern m22 M22(
    f32 _00, f32 _01,
    f32 _10, f32 _11);
extern m22 IdM22();

extern m33 M33(
    f32 _00, f32 _01, f32 _02,
    f32 _10, f32 _11, f32 _12,
    f32 _20, f32 _21, f32 _22);
extern m33 IdM33();

extern m44 M44(
    f32 _00, f32 _01, f32 _02, f32 _03,
    f32 _10, f32 _11, f32 _12, f32 _13,
    f32 _20, f32 _21, f32 _22, f32 _23,
    f32 _30, f32 _31, f32 _32, f32 _33);
extern m44 M44(m33 a);
extern m44 IdM44();

extern f32 Lerp(f32 a, f32 b, f32 t);

extern transform2D Transform2D(f32 posX, f32 posY, f32 scaleX, f32 scaleY, f32 angle);

extern f32 Length(v2 a);

extern void RuntimeQuit();

extern void ChangeLogFlag_(u32 newFlag);

extern void SerializeTable(SerializableTable** table, const char* filepath);

#if defined(PLATFORM_EDITOR) && defined(SOL_HPP)
extern ShaderDebuggerWindow editorShaderDebugger;
#endif

GenerateTableGetExtern(String, char*)
GenerateTableSetExtern(String, const char*, SerializableType_STRING)
GenerateTableGetExtern(Bool, bool)
GenerateTableSetExtern(Bool, bool, SerializableType_BOOL)
GenerateTableGetExtern(I32, i32)
GenerateTableSetExtern(I32, i32, SerializableType_I32)
GenerateTableGetExtern(F32, f32)
GenerateTableSetExtern(F32, f32, SerializableType_F32)
GenerateTableGetExtern(V2, v2)
GenerateTableSetExtern(V2, v2, SerializableType_V2)

#endif