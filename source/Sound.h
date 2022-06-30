#ifndef SOUND_H
#define SOUND_H

#define STB_VORBIS_HEADER_ONLY
#include "STB/stb_vorbis.c"

#define MA_NO_MP3
#define MA_NO_FLAC
#define MINIAUDIO_IMPLEMENTATION
#include "Miniaudio/miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "STB/stb_vorbis.c"

ma_device soundDevice;

i32 soundMixIndex = -1;
SoundInstance soundMix[SOUND_MIX_SIZE];

static SoundInstance* soundDecoderCache = NULL;

f32 masterVolumeModifier;
bool soundMuted;

f32 soundRangeMin;
f32 soundRangeMax;

#if PLATFORM_EDITOR
SoundInstance previewInstance;
#endif

void CleanSoundCache()
{
    
    shfree(soundDecoderCache);
}

void StopSound(SoundInstance* sound)
{    
    SoundInstance* instance = &soundMix[sound->index];
    if(instance->decoder != 0) {
        i32 index = instance->index;
        
        ma_decoder_uninit(instance->decoder);
        
        instance->playing = false;
        instance->index = -1;
        free(instance->filepath);
        instance->filepath = 0;
        free(instance->decoder);
        instance->decoder = 0;

        soundMix[index] = soundMix[soundMixIndex];
        soundMix[index].index = index;
        
        soundMixIndex--;
    }
}

#undef PlaySound
SoundInstance* PlaySound(const char* filepath, f32 volume, bool loop = false, bool unique = false)
{
    if(soundMixIndex >= SOUND_MIX_SIZE) {
        Log("Too many sounds being played at the same time.\n");
        return NULL;
    }
    
    if(unique) {
        for(i32 i = 0; i <= soundMixIndex; ++i) {
            if(soundMix[i].playing && strcmp(soundMix[i].filepath, filepath) == 0) {
                return NULL;
            }
        }
    }

    ma_decoder* decoder = (ma_decoder*)malloc(sizeof(ma_decoder));
    ma_decoder_config config = ma_decoder_config_init(SOUND_FORMAT, SOUND_CHANNELS, SOUND_SAMPLE_RATE);
    ma_result result = ma_decoder_init_file(filepath, &config, decoder);
    if (result != MA_SUCCESS) {
        Log("Failed to load sound.\n");
    }
    
    soundMixIndex++;
    SoundInstance* instance = &soundMix[soundMixIndex];
    instance->index = soundMixIndex;
    instance->decoder = decoder;
    instance->filepath = Strdup(filepath);
    instance->volumeModifier = volume;
    instance->loop = loop;
    instance->playing = true;

    ma_decoder_seek_to_pcm_frame(decoder, 0);

    return instance;
}

#if PLATFORM_EDITOR
void EditorStopPreviewSound()
{
    if(previewInstance.decoder != 0) {
        ma_decoder_uninit(previewInstance.decoder);
        free(previewInstance.filepath);
        previewInstance.decoder = 0;
    }
}

SoundInstance* EditorPlayPreviewSound(const char* filepath, f32 volume)
{
    EditorStopPreviewSound();

    previewInstance.decoder = (ma_decoder*)malloc(sizeof(ma_decoder));
    ma_decoder_config config = ma_decoder_config_init(SOUND_FORMAT, SOUND_CHANNELS, SOUND_SAMPLE_RATE);
    ma_result result = ma_decoder_init_file(filepath, &config, previewInstance.decoder);
    if (result != MA_SUCCESS) {
        Log("Failed to load sound.\n");
    }
    
    previewInstance.index = -1;
    previewInstance.filepath = Strdup(filepath);
    previewInstance.volumeModifier = volume;
    previewInstance.loop = false;
    previewInstance.playing = true;

    ma_decoder_seek_to_pcm_frame(previewInstance.decoder, 0);

    return &previewInstance;
}
#endif

static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    f32* pOutputFormatted = (f32*)pOutput;

    f32 buffer[8192];
    u32 bufferCapInFrames = ma_countof(buffer) / SOUND_CHANNELS;
    u64 totalFramesRead = 0;
    u64 framesRead;
    bool fileEnded = false;

    for(i32 i = 0; i <= soundMixIndex; ++i) {
        framesRead = 0;
        totalFramesRead = 0;
        fileEnded = false;

        while(totalFramesRead < frameCount) {
            ma_result result = ma_decoder_read_pcm_frames(soundMix[i].decoder, buffer, frameCount, &framesRead);
            
            if(framesRead == 0) {
                fileEnded = true;
                break; /* Reached EOF. */
            }
            else {
                if(!soundMuted) {
                    for(u32 sampleIndex = 0; sampleIndex < framesRead * SOUND_CHANNELS; ++sampleIndex) {
                        pOutputFormatted[totalFramesRead * SOUND_CHANNELS + sampleIndex] += buffer[sampleIndex] * soundMix[i].volumeModifier * masterVolumeModifier;
                    }
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
                StopSound(&soundMix[i]);
            }
        }
    }

#if PLATFORM_EDITOR
    if(previewInstance.decoder != 0) {
        framesRead = 0;
        totalFramesRead = 0;
        fileEnded = false;

        while(totalFramesRead < frameCount) {
            ma_result result = ma_decoder_read_pcm_frames(previewInstance.decoder, buffer, frameCount, &framesRead);
            
            if(framesRead == 0) {
                fileEnded = true;
                break; /* Reached EOF. */
            }
            else {
                for(u32 sampleIndex = 0; sampleIndex < framesRead * SOUND_CHANNELS; ++sampleIndex) {
                    pOutputFormatted[totalFramesRead * SOUND_CHANNELS + sampleIndex] += buffer[sampleIndex] * previewInstance.volumeModifier;
                }
                totalFramesRead += framesRead;

                if(framesRead > bufferCapInFrames) {
                    break;
                }
            }
        }

        if(fileEnded) {
            if(previewInstance.loop) {
                ma_decoder_seek_to_pcm_frame(previewInstance.decoder, 0);
            }
            else {
                EditorStopPreviewSound();
            }
        }
    }

    for(i32 channelIndex = 0; channelIndex < SOUND_CHANNELS; ++channelIndex) {
        f32* channelBufferToShow = editorSoundDebugger.bufferToShow + channelIndex * BUFFER_CHANNEL_TO_SHOW_SIZE;
        for(i32 i = 0; i < frameCount; ++i) {
            channelBufferToShow[editorSoundDebugger.bufferOffset + i] = pOutputFormatted[i * SOUND_CHANNELS + channelIndex];
            editorSoundDebugger.bufferToShowMin[channelIndex] = MIN(editorSoundDebugger.bufferToShowMin[channelIndex], pOutputFormatted[i * SOUND_CHANNELS + channelIndex]);
            editorSoundDebugger.bufferToShowMax[channelIndex] = MAX(editorSoundDebugger.bufferToShowMax[channelIndex], pOutputFormatted[i * SOUND_CHANNELS + channelIndex]);
        }
    }
    editorSoundDebugger.bufferOffset = (editorSoundDebugger.bufferOffset + frameCount) % BUFFER_CHANNEL_TO_SHOW_SIZE;
#endif
}

static void SoundInit()
{
    masterVolumeModifier = 1;
    
    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = SOUND_FORMAT;
    deviceConfig.playback.channels = SOUND_CHANNELS;
    deviceConfig.sampleRate        = SOUND_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;

#if PLATFORM_EDITOR
    editorSoundDebugger.bufferOffset = 0;
    editorSoundDebugger.bufferToShow = (f32*)malloc(sizeof(f32) * BUFFER_TO_SHOW_SIZE);
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
    
    for(i32 i = 0; i <= SOUND_MIX_SIZE; ++i) {
        soundMix[soundMixIndex].index = -1;
    }
}

void SetMasterVolume(float value)
{
    ma_device_set_master_volume(&soundDevice, value);
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