#pragma once

#include <cstdint>

#include <Windows.h>
#include <mmeapi.h>

struct Note
{
    float frequency;
    float duration;
};

/**
 * Class representing a sound player. Note that all the play methods are blocking.
 */
class SoundPlayer
{
  public:
    /**
     * Construct a new sound player.
     */
    SoundPlayer();

    /**
     * Block and play a note.
     *
     * @param note
     *   The note to play.
     */
    void play(Note note);

    /**
     * Block and play a sequence of notes.
     *
     * @param notes
     *   The notes to play.
     *
     * @param count
     *   The number of notes to play.
     */
    void play(Note *notes, std::uint32_t count);

  private:
    /** The wave output device. */
    ::HWAVEOUT wave_out_;

    /** The wave format. */
    ::WAVEFORMATEX wave_format_;
};
