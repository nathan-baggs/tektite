#pragma once

#include <cstdint>

#include <Windows.h>
#include <mmeapi.h>

struct Note
{
    float frequency;
    float duration;
};

class SoundPlayer
{
  public:
    SoundPlayer();

    void play(Note note);
    void play(Note *notes, std::uint32_t count);

  private:
    ::HWAVEOUT wave_out_;
    ::WAVEFORMATEX wave_format_;
};
