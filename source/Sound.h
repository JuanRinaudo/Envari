#ifndef SOUND_H
#define SOUND_H

#ifndef SOUND_MIX_SIZE
#define SOUND_MIX_SIZE       32
#endif
#ifndef SOUND_FORMAT
#define SOUND_FORMAT         ma_format_f32
#endif
#ifndef SOUND_CHANNELS
#define SOUND_CHANNELS       2
#endif
#ifndef SOUND_SAMPLE_RATE
#define SOUND_SAMPLE_RATE    48000
#endif

#define STB_VORBIS_HEADER_ONLY
#include "STB/stb_vorbis.c"

#define MA_NO_MP3
#define MA_NO_FLAC
#define MINIAUDIO_IMPLEMENTATION
#include "Miniaudio/miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "STB/stb_vorbis.c"

ma_device soundDevice;

struct MASoundCache {
    char* key;
    ma_decoder* value;
};
MASoundCache* soundCache = NULL;

struct SoundInstance {
    ma_decoder* decoder;
    f32 volumeModifier;
    bool loop;
};

i32 soundMixIndex = -1;
SoundInstance soundMix[SOUND_MIX_SIZE];

f32 soundRangeMin;
f32 soundRangeMax;

#if GAME_EDITOR
#define BUFFER_CHANNEL_TO_SHOW_SIZE SOUND_SAMPLE_RATE * 1
#define BUFFER_TO_SHOW_SIZE BUFFER_CHANNEL_TO_SHOW_SIZE * SOUND_CHANNELS
f32* bufferToShow;
f32 bufferToShowMin[SOUND_CHANNELS];
f32 bufferToShowMax[SOUND_CHANNELS];
#endif

static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    f32* pOutputFormatted = (f32*)pOutput;

#if GAME_EDITOR
    for(i32 channelIndex = 0; channelIndex < SOUND_CHANNELS; ++channelIndex) {
        f32* channelBufferToShow = bufferToShow + channelIndex * BUFFER_CHANNEL_TO_SHOW_SIZE;
        for(i32 i = BUFFER_CHANNEL_TO_SHOW_SIZE; i > frameCount; --i) {
            channelBufferToShow[i] = channelBufferToShow[i - frameCount];
        }
    }
#endif

    for(i32 i = 0; i <= soundMixIndex; ++i) {
        f32 buffer[4096];
        u32 bufferCapInFrames = ma_countof(buffer) / SOUND_CHANNELS;
        u64 totalFramesRead = 0;
        bool fileEnded = false;

        while(totalFramesRead < frameCount) {
            u64 framesRead = ma_decoder_read_pcm_frames(soundMix[i].decoder, buffer, frameCount);
            
            if(framesRead == 0) {
                fileEnded = true;
                break; /* Reached EOF. */
            }
            else {
                for(u32 sampleIndex = 0; sampleIndex < framesRead * SOUND_CHANNELS; ++sampleIndex) {
                    pOutputFormatted[totalFramesRead * SOUND_CHANNELS + sampleIndex] += buffer[sampleIndex] * soundMix[i].volumeModifier;
                }
                totalFramesRead += framesRead;

                if(framesRead > bufferCapInFrames) {
                    break;
                }
            }
        }

        if(fileEnded) {
            if(soundMix[i].loop) {
                ma_decoder_seek_to_pcm_frame(soundMix[i].decoder, 0);
            }
            else {
                soundMix[i] = soundMix[soundMixIndex];
                soundMix[soundMixIndex].decoder = 0;
                soundMixIndex--;
            }
        }
    }

#if GAME_EDITOR
    for(i32 channelIndex = 0; channelIndex < SOUND_CHANNELS; ++channelIndex) {
        f32* channelBufferToShow = bufferToShow + channelIndex * BUFFER_CHANNEL_TO_SHOW_SIZE;
        for(i32 i = 0; i < frameCount; ++i) {
            channelBufferToShow[i] = pOutputFormatted[i * SOUND_CHANNELS + channelIndex];
            bufferToShowMin[channelIndex] = MIN(bufferToShowMin[channelIndex], pOutputFormatted[i * SOUND_CHANNELS + channelIndex]);
            bufferToShowMax[channelIndex] = MAX(bufferToShowMax[channelIndex], pOutputFormatted[i * SOUND_CHANNELS + channelIndex]);
        }
    }
#endif
}

static void SoundInit()
{
    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = SOUND_FORMAT;
    deviceConfig.playback.channels = SOUND_CHANNELS;
    deviceConfig.sampleRate        = SOUND_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;

#if GAME_EDITOR
    bufferToShow = (f32*)malloc(sizeof(f32) * BUFFER_TO_SHOW_SIZE);
#endif

    if (ma_device_init(NULL, &deviceConfig, &soundDevice) != MA_SUCCESS) {
        Log("Failed to open playback device.\n");
        return;
    }

    if (ma_device_start(&soundDevice) != MA_SUCCESS) {
        Log("Failed to start playback device.\n");
        ma_device_uninit(&soundDevice);
        return;
    }

    switch(SOUND_FORMAT) {
        case ma_format_u8: {
            soundRangeMin = 0.0f;
            soundRangeMax = 255.0f;
        } break;
        case ma_format_s16: {
            soundRangeMin = -32768.0f;
            soundRangeMax =  32767.0f;
        } break;
        case ma_format_s24: {
            soundRangeMin = -8388608.0f;
            soundRangeMax =  8388607.0f;
        } break;
        case ma_format_s32: {
            soundRangeMin = -2147483648.0f;
            soundRangeMax =  2147483647.0f;
        } break;
        case ma_format_f32: {
            soundRangeMin = -1.0f;
            soundRangeMax =  1.0f;
        } break;
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
        ma_decoder_config config = ma_decoder_config_init(SOUND_FORMAT, 2, 48000);
        result = ma_decoder_init_file(soundKey, &config, decoder);
        if (result != MA_SUCCESS) {
            Log("Failed to load sound.\n");
        }
        
        shput(soundCache, soundKey, decoder);

        return decoder;
    }
}

void SetMasterVolume(float value)
{    
    ma_device_set_master_volume(&soundDevice, value);
}

void SoundPlay(const char* filepath, f32 volume, bool loop = false)
{
    if(soundMixIndex < SOUND_MIX_SIZE) {
        ma_decoder* decoder = SoundLoad(filepath);
        i32 soundIndex = -1;

        ma_decoder_seek_to_pcm_frame(decoder, 0);

        for(i32 i = 0; i <= soundMixIndex; ++i) {
            if(decoder == soundMix[i].decoder) {
                soundIndex = i;
                break;
            }
        }

        if(soundIndex == -1) {
            soundMixIndex++;
            soundMix[soundMixIndex].decoder = decoder;
        }
        soundMix[soundMixIndex].volumeModifier = volume;
        soundMix[soundMixIndex].loop = loop;
    }
    else {
        // #TODO (Juan): What to do if sound cant be played
    }
}

void SoundStop(const char* filepath)
{
    i32 soundIndex = -1;
    
    ma_decoder* decoder = SoundLoad(filepath);

    for(i32 i = 0; i <= soundMixIndex; ++i) {
        if(decoder == soundMix[i].decoder) {
            soundIndex = i;
            break;
        }
    }

    if(soundIndex != -1) {
        soundMix[soundIndex] = soundMix[soundMixIndex];
        soundMix[soundMixIndex].decoder = 0;
        soundMixIndex--;
    }
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