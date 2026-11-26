#include "sound.h"
#include "pmm.h"
#include "pic.h"
#include "math.h"
#include "tui.h"
#include "io.h"

volatile u32int sb16_debug_stage = 0;

u8int volume_master;     
u8int volumes[NUM_NOTES];
u8int notes[NUM_NOTES];  
u8int waves[NUM_NOTES];

u8int major, minor;

u32int dma_physical_address;
u8int *dma_buffer;

const f64 NOTES[NUM_OCTAVES * OCTAVE_SIZE] = {
    // O1
    32.703195662574764,
    34.647828872108946,
    36.708095989675876,
    38.890872965260044,
    41.203444614108669,
    43.653528929125407,
    46.249302838954222,
    48.99942949771858,
    51.913087197493056,
    54.999999999999915,
    58.270470189761156,
    61.735412657015416,

    // O2
    65.406391325149571,
    69.295657744217934,
    73.416191979351794,
    77.781745930520117,
    82.406889228217381,
    87.307057858250872,
    92.4986056779085,
    97.998858995437217,
    103.82617439498618,
    109.99999999999989,
    116.54094037952237,
    123.4708253140309,

    // O3
    130.8127826502992,
    138.59131548843592,
    146.83238395870364,
    155.56349186104035,
    164.81377845643485,
    174.61411571650183,
    184.99721135581709,
    195.99771799087452,
    207.65234878997245,
    219.99999999999989,
    233.08188075904488,
    246.94165062806198,

    // O4
    261.62556530059851,
    277.18263097687202,
    293.66476791740746,
    311.12698372208081,
    329.62755691286986,
    349.22823143300383,
    369.99442271163434,
    391.99543598174927,
    415.30469757994513,
    440,
    466.16376151808993,
    493.88330125612413,

    // O5
    523.25113060119736,
    554.36526195374427,
    587.32953583481526,
    622.25396744416196,
    659.25511382574007,
    698.456462866008,
    739.98884542326903,
    783.99087196349899,
    830.60939515989071,
    880.00000000000034,
    932.32752303618031,
    987.76660251224882,

    // O6
    1046.5022612023952,
    1108.7305239074892,
    1174.659071669631,
    1244.5079348883246,
    1318.5102276514808,
    1396.9129257320169,
    1479.977690846539,
    1567.9817439269987,
    1661.2187903197821,
    1760.000000000002,
    1864.6550460723618,
    1975.5332050244986,

    // O7
    2093.0045224047913,
    2217.4610478149793,
    2349.3181433392633,
    2489.0158697766506,
    2637.020455302963,
    2793.8258514640347,
    2959.9553816930793,
    3135.9634878539991,
    3322.437580639566,
    3520.0000000000055,
    3729.3100921447249,
    3951.0664100489994,
};

#define TEST_FREQUENCY 440
#define TEST_DURATION  1

void sb16_generate_test_tone(void) {
    serial_write_u32((u32int)dma_buffer);
    serial_write("\n");

    for (u32int i = 0; i < DMA_BUFFER_SIZE; i++) {
        dma_buffer[i] = (i % 50 < 25) ? 192 : 64;
    }
    serial_write_u32(dma_buffer[0]);
    serial_write("\n");
}

void music_init() {
    sound_wave(0, WAVE_TRIANGLE);
    sound_volume(0, 255);

    sound_wave(1, WAVE_NOISE);
    sound_volume(1, 128);

    sound_wave(2, WAVE_TRIANGLE);
    sound_volume(2, 196);

    sound_wave(3, WAVE_TRIANGLE);
    sound_volume(3, 196);
}

void sound_note(u8int index, u8int octave, u8int note) {
    notes[index] = (octave << 4) | note;
}

void sound_volume(u8int index, u8int v) {
    volumes[index] = v;
}

void sound_master(u8int v) {
    volume_master = v;
}

void sound_wave(u8int index, u8int wave) {
    waves[index] = wave;
}

// Initialise the sound system
void sound_init() {
    volume_master = 255;

    for (u8int i = 0; i < NUM_NOTES; i++) {
        volumes[i] = 0;
        notes[i] = 0;
        waves[i] = WAVE_SQUARE;
    }

    music_init();
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

// Reset the Sound Blaster 16 device
bool sb16_reset(void) {
    outb(SB16_DSP_RESET, 1);

    io_wait();
    io_wait();
    io_wait();
    io_wait();

    outb(SB16_DSP_RESET, 0);

    // Wait for DSP response, with timeout
    u32int timeout = 100000;

    while (timeout--) {
        if (inb(SB16_DSP_READ_STAT) & 0x80) {
            u8int response = inb(SB16_DSP_READ);

            return response == 0xAA;
        }
    }

    return false;
}

// Write a value to the Sound Blaster 16 device
volatile u8int sb16_last_write_status = 0;
volatile u32int sb16_write_attempts = 0;
volatile u8int sb16_write_value = 0;

bool sb16_write(u8int value) {
    u32int timeout = 100000;

    sb16_write_value = value;
    sb16_write_attempts = 0;

    while (timeout--) {
        sb16_write_attempts++;

        u8int status = inb(SB16_DSP_STATUS);
        sb16_last_write_status = status;

        if (!(status & 0x80)) {
            outb(SB16_DSP_WRITE, value);
            return true;
        }
    }

    return false;
}

// Read a value from the Sound Blaster 16 device
bool sb16_read(u8int *value) {
    u32int timeout = 100000;

    while (timeout--) {
        if (inb(SB16_DSP_READ_STAT) & 0x80) {
            *value = inb(SB16_DSP_READ);
            return true;
        }
    }

    return false;
}

// Get the version of the Sound Blaster 16 device
bool sb16_get_version(u8int *major, u8int *minor) {
    if (!sb16_write(0xE1)) {
        return false;
    }

    if (!sb16_read(major)) {
        return false;
    }

    if (!sb16_read(minor)) {
        return false;
    }

    return true;
}

bool sb16_write_command(u8int command, u8int parameter) {
    if (!sb16_write(command)) {
        return false;
    }

    if (!sb16_write(parameter)) {
        return false;
    }

    return true;
}

u8int sb16_mixer_read(u8int reg) {
    outb(SB16_MIXER_ADDR, reg);
    return inb(SB16_MIXER_DATA);
}

void sb16_mixer_write(u8int reg, u8int value) {
    outb(SB16_MIXER_ADDR, reg);
    outb(SB16_MIXER_DATA, value);
}

bool sb16_init() {
    if (!sb16_reset()) {
        // Handle reset failure
        return false;
    }

    if (!sb16_get_version(&major, &minor)) {
        // Handle version retrieval failure
        return false;
    }

    // configure mixer settings, e.g., set master volume, enable channels, etc.
    sb16_mixer_write(0x22, 0xFF); // Set master volume to maximum
    sb16_mixer_write(0x24, 0xFF); // Enable all channels

    // reports if initialization was successful or not

    return true;
}

bool sb16_start_playback(void) {
    sb16_debug_stage++;
    serial_write("[DEBUG] Entered sb16_start_playback\n");
    u16int count = DMA_BUFFER_SIZE - 1;

    if (!sb16_set_sample_rate(SB16_SAMPLE_RATE)) {
        serial_write("[FAIL] Sample rate\n");
        return false;
    }

    sb16_debug_stage++;

    if (!sb16_write(SB16_DSP_SPEAKER_ON)) {
        serial_write("[FAIL] Speaker on\n");
        return false;
    }

    sb16_debug_stage++;

    outb(DMA_MASK, DMA_CHANNEL);

    sb16_debug_stage++;

    if (!sb16_write(SB16_DSP_DMA8_AUTO_OUT)) {
        serial_write("[FAIL] DMA playback command\n");
        return false;
    }

    sb16_debug_stage++;

    if (!sb16_write(SB16_PCM_MODE)) {
        serial_write("[FAIL] PCM mode\n");
        return false;
    }

    sb16_debug_stage++;

    if (!sb16_write((u8int)(count & 0xFF))) {
        serial_write("[FAIL] Count low\n");
        return false;
    }

    sb16_debug_stage++;

    if (!sb16_write((u8int)(count >> 8))) {
        serial_write("[FAIL] Count high\n");
        return false;
    }

    serial_write("[PASS] sb16_start_playback worked\n");
    return true;
}

bool sb16_dma_init(void) {
    // Allocate a DMA buffer
    dma_buffer = (u8int *)pmm_alloc_dma_frames(DMA_PAGE_COUNT);

    if (!dma_buffer) {
        return false;
    }

    // Get the physical address of the DMA buffer
    dma_physical_address = (u32int)dma_buffer;

    if ((dma_physical_address & 0xFFFF) + DMA_BUFFER_SIZE > 0x10000) {
        sb16_dma_free();
        return false;
    }

    // Mask DMA channel 1
    outb(DMA_MASK, 0x04 | DMA_CHANNEL);

    // Set DMA mode
    outb(DMA_MODE, 0x59 | DMA_CHANNEL);

    // Set address
    outb(DMA_CLEAR_FF, 0x00);
    outb(DMA_ADDR, (u8int)(dma_physical_address & 0xFF));
    outb(DMA_ADDR, (u8int)((dma_physical_address >> 8) & 0xFF));

    // Set transfer count
    outb(DMA_CLEAR_FF, 0x00);
    outb(DMA_COUNT, (u8int)((DMA_BUFFER_SIZE - 1) & 0xFF));
    outb(DMA_COUNT, (u8int)(((DMA_BUFFER_SIZE - 1) >> 8) & 0xFF));

    // Set page
    outb(DMA_PAGE, (u8int)(dma_physical_address >> 16));

    return true;
}

void sound_blaster_irq_handler(void) {
    inb(SB16_DSP_READ_STAT); // Acknowledge 8-bit SB16 interrupt
    pic_acknowledge(5);
}

void sb16_dma_free(void) {
    if (dma_buffer) {
        pmm_free_dma_frames((u32int)dma_buffer, DMA_PAGE_COUNT);
        dma_buffer = NULL;
        dma_physical_address = 0;
    }
}

bool sb16_set_sample_rate(u16int rate) {
    if (!sb16_write(SB16_DSP_SET_OUTPUT_RATE))
        return false;

    if (!sb16_write((u8int)(rate >> 8)))
        return false;

    if (!sb16_write((u8int)(rate & 0xFF)))
        return false;

    return true;
}