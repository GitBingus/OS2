#ifndef SOUND_H
#define SOUND_H

#include "types.h"
#include "io.h"
#include "sb16.h"
#include "kbool.h"

#define DMA_CHANNEL         1

#define DMA_ADDR            0x02
#define DMA_COUNT           0x03
#define DMA_PAGE            0x83

#define DMA_MASK            0x0A
#define DMA_MODE            0x0B
#define DMA_CLEAR_FF        0x0C

#define DMA_BUFFER_SIZE     16384
#define DMA_PAGE_COUNT      4

#define SB16_DSP_SET_OUTPUT_RATE  0x41
#define SB16_DSP_SPEAKER_ON       0xD1
#define SB16_DSP_SPEAKER_OFF      0xD3
#define SB16_DSP_DMA8_AUTO_OUT    0xC6

#define SB16_SAMPLE_RATE          22050
#define SB16_PCM_MODE             0x00

extern u32int dma_physical_address;
extern u8int *dma_buffer;

#define MIXER_IRQ       0x5

#define NUM_NOTES 8

#define NUM_OCTAVES 7
#define OCTAVE_SIZE 12

#define OCTAVE_1 0
#define OCTAVE_2 1
#define OCTAVE_3 2
#define OCTAVE_4 3
#define OCTAVE_5 4
#define OCTAVE_6 5
#define OCTAVE_7 6

#define NOTE_C      0
#define NOTE_CS     1
#define NOTE_DF     NOTE_CS
#define NOTE_D      2
#define NOTE_DS     3
#define NOTE_EF     NOTE_DS
#define NOTE_E      4
#define NOTE_F      5
#define NOTE_FS     6
#define NOTE_GF     NOTE_FS
#define NOTE_G      7
#define NOTE_GS     8
#define NOTE_AF     NOTE_GS
#define NOTE_A      9
#define NOTE_AS     10
#define NOTE_BF     NOTE_AS
#define NOTE_B      11

#define NOTE_NONE   12

#define WAVE_SIN        0
#define WAVE_SQUARE     1
#define WAVE_NOISE      2
#define WAVE_TRIANGLE   3

#define BPM 150         // beats per minute
#define BPS (BPM / 60.0) // beats per second

struct Note {
    u8int octave;
    u8int note;
    u16int duration;
};

struct NoteActive {
    struct Note note;
    double ticks;
};

#define CHORUS_MELODY_LENGTH (sizeof(CHORUS_MELODY) / sizeof(CHORUS_MELODY[0]))
static const struct Note CHORUS_MELODY[] = {
    // CHORUS MELODY
    { OCTAVE_5, NOTE_E,     16 },
    { OCTAVE_4, NOTE_B,      8 },
    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_5, NOTE_D,     16 },

    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_4, NOTE_B,      8 },
    { OCTAVE_4, NOTE_A,     16 },

    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_5, NOTE_E,     16 },

    { OCTAVE_5, NOTE_D,      8 },
    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_4, NOTE_B,     16 },

    { OCTAVE_4, NOTE_B,      8 },
    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_5, NOTE_D,     16 },
    { OCTAVE_5, NOTE_E,     16 },
    { OCTAVE_5, NOTE_C,     16 },
    { OCTAVE_4, NOTE_A,     16 },
    { OCTAVE_4, NOTE_A,     16 },
    { OCTAVE_4, NOTE_NONE,  24 },

    { OCTAVE_5, NOTE_D,     16 },
    { OCTAVE_5, NOTE_F,      8 },
    { OCTAVE_5, NOTE_A,      8 },
    { OCTAVE_5, NOTE_A,      4 },
    { OCTAVE_5, NOTE_A,      4 },
    { OCTAVE_5, NOTE_G,      8 },
    { OCTAVE_5, NOTE_F,      8 },
    { OCTAVE_5, NOTE_E,     16 },
    { OCTAVE_5, NOTE_NONE,   8 },

    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_5, NOTE_E,      8 },
    { OCTAVE_5, NOTE_E,      4 },
    { OCTAVE_5, NOTE_E,      4 },
    { OCTAVE_5, NOTE_D,      8 },
    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_4, NOTE_B,     16 },

    { OCTAVE_4, NOTE_B,      8 },
    { OCTAVE_5, NOTE_C,      8 },
    { OCTAVE_5, NOTE_D,     16 },
    { OCTAVE_5, NOTE_E,     16 },
    { OCTAVE_5, NOTE_C,     16 },
    { OCTAVE_4, NOTE_A,     16 },
    { OCTAVE_4, NOTE_A,     16 },
    { OCTAVE_4, NOTE_NONE,  16 },
};

#define BRIDGE_MELODY_LENGTH (sizeof(BRIDGE_MELODY) / sizeof(BRIDGE_MELODY[0]))
static const struct Note BRIDGE_MELODY[] = {
    // BRIDGE
    { OCTAVE_4, NOTE_E,     32 },
    { OCTAVE_4, NOTE_C,     32 },
    { OCTAVE_4, NOTE_D,     32 },
    { OCTAVE_3, NOTE_B,     32 },
    { OCTAVE_4, NOTE_C,     32 },
    { OCTAVE_3, NOTE_A,     32 },
    { OCTAVE_3, NOTE_AF,    48 },
    { OCTAVE_3, NOTE_NONE,  16 },

    { OCTAVE_4, NOTE_E,     32 },
    { OCTAVE_4, NOTE_C,     32 },
    { OCTAVE_4, NOTE_D,     32 },
    { OCTAVE_3, NOTE_B,     32 },
    { OCTAVE_3, NOTE_A,     16 },
    { OCTAVE_4, NOTE_E,     16 },
    { OCTAVE_4, NOTE_A,     32 },
    { OCTAVE_4, NOTE_AF,    48 },
    { OCTAVE_4, NOTE_NONE,  16 },
};

#define BASS_NOTE(_octave, _note)\
    { _octave,          _note,  8 },\
    { (_octave + 1),    _note,  8 }

#define BASS_HALF_MEASURE(_octave, _note)\
    BASS_NOTE(_octave, _note),\
    BASS_NOTE(_octave, _note)

#define BASS_MEASURE(_octave, _note)\
    BASS_HALF_MEASURE(_octave, _note),\
    BASS_HALF_MEASURE(_octave, _note)

#define CHORUS_BASS_LENGTH (sizeof(CHORUS_BASS) / sizeof(CHORUS_BASS[0]))
static const struct Note CHORUS_BASS[] = {
    // CHORUS BASS
    BASS_MEASURE(OCTAVE_2, NOTE_E),
    BASS_MEASURE(OCTAVE_2, NOTE_A),
    BASS_HALF_MEASURE(OCTAVE_2, NOTE_AF),
    BASS_HALF_MEASURE(OCTAVE_2, NOTE_E),
    BASS_HALF_MEASURE(OCTAVE_2, NOTE_A),
    { OCTAVE_2, NOTE_A,  8 },
    { OCTAVE_2, NOTE_A,  8 },
    { OCTAVE_2, NOTE_B,  8 },
    { OCTAVE_3, NOTE_C,  8 },

    BASS_MEASURE(OCTAVE_2, NOTE_D),
    BASS_MEASURE(OCTAVE_2, NOTE_C),
    BASS_HALF_MEASURE(OCTAVE_2, NOTE_AF),
    BASS_HALF_MEASURE(OCTAVE_2, NOTE_E),
    BASS_MEASURE(OCTAVE_2, NOTE_A),
};

#define BRIDGE_BASS_NOTE(_octave_0, _note_0, _octave_1, _note_1)\
    { _octave_0, _note_0, 8 },\
    { _octave_1, _note_1, 8 }

#define BRIDGE_BASS_HALF_MEASURE(_octave_0, _note_0, _octave_1, _note_1)\
    BRIDGE_BASS_NOTE(_octave_0, _note_0, _octave_1, _note_1),\
    BRIDGE_BASS_NOTE(_octave_0, _note_0, _octave_1, _note_1)

#define BRIDGE_BASS_MEASURE(_octave_0, _note_0, _octave_1, _note_1)\
    BRIDGE_BASS_HALF_MEASURE(_octave_0, _note_0, _octave_1, _note_1),\
    BRIDGE_BASS_HALF_MEASURE(_octave_0, _note_0, _octave_1, _note_1)

#define BRIDGE_BASS_LENGTH (sizeof(BRIDGE_BASS) / sizeof(BRIDGE_BASS[0]))
static const struct Note BRIDGE_BASS[] = {
    BRIDGE_BASS_MEASURE(OCTAVE_2, NOTE_A, OCTAVE_3, NOTE_E),
    BRIDGE_BASS_MEASURE(OCTAVE_2, NOTE_AF, OCTAVE_3, NOTE_E),

    BRIDGE_BASS_HALF_MEASURE(OCTAVE_2, NOTE_A, OCTAVE_3, NOTE_E),
    BRIDGE_BASS_HALF_MEASURE(OCTAVE_2, NOTE_E, OCTAVE_2, NOTE_A),

    BRIDGE_BASS_HALF_MEASURE(OCTAVE_2, NOTE_E, OCTAVE_2, NOTE_AF),
    BRIDGE_BASS_NOTE(OCTAVE_2, NOTE_E, OCTAVE_2, NOTE_AF),
    { OCTAVE_2, NOTE_E,      8 },
    { OCTAVE_2, NOTE_NONE,   8 },

    BRIDGE_BASS_MEASURE(OCTAVE_2, NOTE_A, OCTAVE_3, NOTE_E),
    BRIDGE_BASS_MEASURE(OCTAVE_2, NOTE_AF, OCTAVE_3, NOTE_E),

    BRIDGE_BASS_HALF_MEASURE(OCTAVE_2, NOTE_A, OCTAVE_3, NOTE_E),
    BRIDGE_BASS_HALF_MEASURE(OCTAVE_2, NOTE_E, OCTAVE_2, NOTE_A),

    BRIDGE_BASS_HALF_MEASURE(OCTAVE_2, NOTE_E, OCTAVE_2, NOTE_AF),
    BRIDGE_BASS_NOTE(OCTAVE_2, NOTE_E, OCTAVE_2, NOTE_AF),
    { OCTAVE_2, NOTE_E,      8 },
    { OCTAVE_2, NOTE_NONE,   8 }
};

#define CHORUS_HARMONY_LENGTH (sizeof(CHORUS_HARMONY) / sizeof(CHORUS_HARMONY[0]))
static const struct Note CHORUS_HARMONY[] = {
    // CHORUS HARMONY
    { OCTAVE_5, NOTE_NONE,  16 },
    { OCTAVE_4, NOTE_AF,     8 },
    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_4, NOTE_B,     16 },

    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_4, NOTE_AF,     8 },
    { OCTAVE_4, NOTE_E,     16 },

    { OCTAVE_4, NOTE_E,      8 },
    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_4, NOTE_A,     16 },

    { OCTAVE_4, NOTE_B,      8 },
    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_4, NOTE_AF,    16 },

    { OCTAVE_4, NOTE_AF,     8 },
    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_4, NOTE_B,     16 },
    { OCTAVE_5, NOTE_C,     16 },
    { OCTAVE_4, NOTE_A,     16 },
    { OCTAVE_4, NOTE_E,     16 },
    { OCTAVE_4, NOTE_E,     16 },
    { OCTAVE_4, NOTE_NONE,  24 },

    { OCTAVE_4, NOTE_F,     16 },
    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_5, NOTE_C,     16 },
    { OCTAVE_4, NOTE_B,      8 },
    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_4, NOTE_G,     16 },
    { OCTAVE_4, NOTE_NONE,   8 },

    { OCTAVE_4, NOTE_E,      8 },
    { OCTAVE_4, NOTE_G,     16 },
    { OCTAVE_4, NOTE_F,      8 },
    { OCTAVE_4, NOTE_E,      8 },
    { OCTAVE_4, NOTE_AF,    16 },

    { OCTAVE_4, NOTE_AF,     8 },
    { OCTAVE_4, NOTE_A,      8 },
    { OCTAVE_4, NOTE_B,     16 },
    { OCTAVE_5, NOTE_C,     16 },
    { OCTAVE_4, NOTE_A,     16 },
    { OCTAVE_4, NOTE_E,     16 },
    { OCTAVE_4, NOTE_E,     16 },
    { OCTAVE_4, NOTE_NONE,  16 },
};

#define BRIDGE_HARMONY_LENGTH (sizeof(BRIDGE_HARMONY) / sizeof(BRIDGE_HARMONY[0]))
static const struct Note BRIDGE_HARMONY[] = {
    { OCTAVE_4, NOTE_C,     32 },
    { OCTAVE_3, NOTE_A,     32 },
    { OCTAVE_3, NOTE_B,     32 },
    { OCTAVE_3, NOTE_AF,    32 },
    { OCTAVE_3, NOTE_A,     32 },
    { OCTAVE_3, NOTE_E,     32 },
    { OCTAVE_3, NOTE_E,     48 },
    { OCTAVE_3, NOTE_NONE,  16 },

    { OCTAVE_4, NOTE_C,     32 },
    { OCTAVE_3, NOTE_A,     32 },
    { OCTAVE_3, NOTE_B,     32 },
    { OCTAVE_3, NOTE_AF,    32 },
    { OCTAVE_3, NOTE_E,     16 },
    { OCTAVE_3, NOTE_A,     16 },
    { OCTAVE_4, NOTE_E,     32 },
    { OCTAVE_4, NOTE_E,     48 },
    { OCTAVE_4, NOTE_NONE,  16 },
};

#define SNARE_OCTAVE OCTAVE_4
#define SNARE_NOTE NOTE_E
#define SNARE_AND\
    { SNARE_OCTAVE, NOTE_NONE,  8},\
    { SNARE_OCTAVE, SNARE_NOTE, 2},\
    { SNARE_OCTAVE, NOTE_NONE,  6}
#define SNARE_EIGTH\
    { SNARE_OCTAVE, SNARE_NOTE, 2},\
    { SNARE_OCTAVE, NOTE_NONE,  6}

#define SNARE_MEASURE\
    SNARE_AND,\
    SNARE_AND,\
    SNARE_AND,\
    SNARE_AND

#define CHORUS_SNARE_LENGTH (sizeof(CHORUS_SNARE) / sizeof(CHORUS_SNARE[0]))
static const struct Note CHORUS_SNARE[] = {
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_AND,
    SNARE_AND,
    SNARE_AND,
    SNARE_EIGTH,
    SNARE_EIGTH,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_AND,
    SNARE_AND,
    SNARE_AND,
    SNARE_EIGTH,
    SNARE_EIGTH,
};

#define BRIDGE_SNARE_LENGTH (sizeof(BRIDGE_SNARE) / sizeof(BRIDGE_SNARE[0]))
static const struct Note BRIDGE_SNARE[] = {
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_MEASURE,
    SNARE_AND,
    SNARE_AND,
    SNARE_AND,
    SNARE_EIGTH,
    SNARE_EIGTH,
};

extern u8int volume_master;
extern u8int volumes[NUM_NOTES];
extern u8int notes[NUM_NOTES];
extern u8int waves[NUM_NOTES];

extern const f64 NOTES[NUM_OCTAVES * OCTAVE_SIZE];

extern u8int major;
extern u8int minor;

struct MusicTrack {
    const struct Note *sequence;
    u32int length;
    u32int position;
    u32int ticks_remaining;
    u8int channel;
};

extern volatile u32int sb16_debug_stage;
extern volatile u8int sb16_last_write_status;
extern volatile u32int sb16_write_attempts;
extern volatile u8int sb16_write_value;

bool sb16_start_playback(void); // Start playback of the Sound Blaster 16 device
void sb16_generate_test_tone(void); // Generate a test tone for the Sound Blaster 16 device
void sound_blaster_irq_handler(void); // Handle the Sound Blaster 16 interrupt
bool sb16_init(void); // Initialise the Sound Blaster 16 device
bool sb16_reset(void); // Reset the Sound Blaster 16 device
bool sb16_write(u8int value); // Write a value to the Sound Blaster 16 device
bool sb16_read(u8int *value); // Read a value from the Sound Blaster 16 device
bool sb16_get_version(u8int *major, u8int *minor); // Get the version of the Sound Blaster 16 device
bool sb16_write_command(u8int command, u8int parameter); // Write a command and its parameter to the Sound Blaster 16 device
void sb16_mixer_write(u8int reg, u8int value); // Write a value to a mixer register of the Sound Blaster 16 device
u8int sb16_mixer_read(u8int reg); // Read a value from a mixer register of the Sound Blaster 16 device
bool sb16_dma_init(void); // Initialise the DMA for the Sound Blaster 16 device
void sb16_dma_free(void); // Free the DMA resources for the Sound Blaster 16 device
bool sb16_set_sample_rate(u16int rate); // Set the sample rate for the Sound Blaster 16 device

void sound_init(); // Initialise the sound system
void sound_note(u8int index, u8int octave, u8int note); // Set the note for a given index
void sound_master(u8int v); // Set the master volume
void sound_volume(u8int index, u8int v); // Set the volume for a given index
void sound_wave(u8int index, u8int wave); // Set the wave for a given index
void sound_note_off(u8int index); // Turn off the note for a given index
void sound_render_buffer(void); // Render the sound buffer for playback

void music_play(void); // Start playing the music
void music_stop(void); // Stop playing the music
void music_tick(); // Advance the music playback by one tick
void music_init(); // Initialise the music system with default settings

#endif