#include "sound_player.h"

#include <cstdint>

#include <Windows.h>
#include <mmeapi.h>

#include "clib.h"
#include "dyn_array.h"
#include "error.h"
#include "padding.h"

namespace
{

static constexpr auto g_sample_rate = 44100u;

DynArray generate_sin_wave(float frequency, float duration)
{
    PADDING_LINE;
    // const auto samples = static_cast<std::uint32_t>(g_sample_rate * duration);
    auto waves = DynArray{sizeof(std::uint16_t)};

    // for (auto i = 0u; i < samples; ++i)
    // {
    //     auto wave = static_cast<std::uint16_t>(sin(2 * M_PI * frequency * i / g_sample_rate) * 300);
    //     waves.push_back(&wave);
    // }

    return waves;
}

}

SoundPlayer::SoundPlayer()
{
    PADDING_LINE;
    wave_format_ = ::WAVEFORMATEX{
        .wFormatTag = WAVE_FORMAT_PCM,
        .nChannels = 1,
        .nSamplesPerSec = g_sample_rate,
        .nBlockAlign = 1,
        .wBitsPerSample = 8,
        .cbSize = 0};

    wave_format_.nBlockAlign = wave_format_.nChannels * wave_format_.wBitsPerSample / 8;
    wave_format_.nAvgBytesPerSec = wave_format_.nSamplesPerSec * wave_format_.nBlockAlign;

    ensure(
        ::waveOutOpen(&wave_out_, WAVE_MAPPER, &wave_format_, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR,
        ErrorCode::FAILED_TO_OPEN_WAVE_OUTPUT_DEVICE);
}

void SoundPlayer::play(Note note)
{
    PADDING_LINE;
    const auto waves = generate_sin_wave(note.frequency, note.duration);

    auto header = ::WAVEHDR{
        .lpData = static_cast<::LPSTR>(waves.begin()),
        .dwBufferLength = waves.size() * waves.element_size(),
        .dwFlags = 0,
        .dwLoops = 0,
        .lpNext = nullptr,
        .reserved = 0};

    ensure(
        ::waveOutPrepareHeader(wave_out_, &header, sizeof(header)) == MMSYSERR_NOERROR,
        ErrorCode::FAILED_TO_PREPARE_WAVE_HEADER);

    ensure(
        ::waveOutWrite(wave_out_, &header, sizeof(header)) == MMSYSERR_NOERROR, ErrorCode::FAILED_TO_WRITE_WAVE_OUTPUT);

    while (!(header.dwFlags & WHDR_DONE))
    {
        ::Sleep(100);
    }

    ensure(
        ::waveOutUnprepareHeader(wave_out_, &header, sizeof(header)) == MMSYSERR_NOERROR,
        ErrorCode::FAILED_TO_UNPREPARE_WAVE_HEADER);

    free(header.lpData);
}

void SoundPlayer::play(Note *notes, std::uint32_t count)
{
    PADDING_LINE;
    for (auto i = 0u; i < count; ++i)
    {
        play(notes[i]);
    }
}
