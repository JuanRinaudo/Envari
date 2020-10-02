#ifndef SOUND_H
#define SOUND_H

#ifndef SOUND_FORMAT
#define SOUND_FORMAT         ma_format_s16
#endif
#ifndef SOUND_CHANNELS
#define SOUND_CHANNELS       2
#endif
#ifndef SOUND_SAMPLE_RATE
#define SOUND_SAMPLE_RATE    48000
#endif

#define MINIAUDIO_IMPLEMENTATION
#include "Miniaudio/miniaudio.h"

ma_device soundDevice;

struct MASoundCache {
    char* key;
    ma_decoder* value;
};
MASoundCache* soundCache = NULL;

ma_decoder* targetDecoder;
static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    if (targetDecoder == NULL) {
        return;
    }
    
    ma_decoder_read_pcm_frames(targetDecoder, pOutput, frameCount);
}

static void SoundInit()
{
    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = SOUND_FORMAT;
    deviceConfig.playback.channels = SOUND_CHANNELS;
    deviceConfig.sampleRate        = SOUND_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;

    if (ma_device_init(NULL, &deviceConfig, &soundDevice) != MA_SUCCESS) {
        Log(&editorConsole, "Failed to open playback device.\n");
    }

    if (ma_device_start(&soundDevice) != MA_SUCCESS) {
        Log(&editorConsole, "Failed to start playback device.\n");
        ma_device_uninit(&soundDevice);
    }
}

ma_decoder* SoundLoad(const char* soundKey)
{
    i32 index = (i32)shgeti(soundCache, soundKey);
    if(index > -1) {
        return shget(soundCache, soundKey);
    } else {
        ma_result result;
        ma_decoder* decoder = (ma_decoder*)malloc(sizeof(ma_decoder));
        result = ma_decoder_init_file_wav(soundKey, NULL, decoder);
        if (result != MA_SUCCESS) {
            Log(&editorConsole, "Failed to load sound.\n");
        }
        
        shput(soundCache, soundKey, decoder);

        return decoder;
    }
}

void SoundPlay(const char* filepath)
{
    targetDecoder = SoundLoad(filepath);
    ma_decoder_seek_to_pcm_frame(targetDecoder, 0);
}

float dbToVolume(float db)
{
    return powf(10.0f, 0.05f * db);    
}

float volumeToDB(float volume)
{
    return 20.0f * log10f(volume);
}

#endif