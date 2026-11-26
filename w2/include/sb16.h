#ifndef SB16_H
#define SB16_H

#define SB16_BASE          0x220

#define SB16_MIXER_ADDR    (SB16_BASE + 0x04)
#define SB16_MIXER_DATA    (SB16_BASE + 0x05)

#define SB16_DSP_RESET     (SB16_BASE + 0x06)
#define SB16_DSP_READ      (SB16_BASE + 0x0A)
#define SB16_DSP_WRITE     (SB16_BASE + 0x0C)
#define SB16_DSP_STATUS    (SB16_BASE + 0x0C)
#define SB16_DSP_READ_STAT (SB16_BASE + 0x0E)

#define SB16_IRQ           5
#define SB16_DMA8          1
#define SB16_DMA16         5

#define SB16_SAMPLE_RATE   22050

#endif