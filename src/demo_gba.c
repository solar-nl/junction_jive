#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include "audio.h"
#include "assets.h"
#include "script.h"
#include "demo.h"
#include "rendering.h"
#include "vcount.h"

// Define memory addresses for frame buffers
// Utilizing both VRAM banks for smooth double buffering
#define FB (uint16_t*)0x6000000  // Front buffer: Start of VRAM
#define BB (uint16_t*)0x600A000  // Back buffer: 0xA000 bytes offset from FB

// Pointers to the current frame and back buffers
// These will be swapped during the presentation phase for seamless animation
uint16_t *framebuffer = BB;
uint16_t *backbuffer = FB;

// Counter for the current frame
// Used for timing and synchronization of demo effects
uint32_t current_frame = 0;

/**
 * @brief Vertical Blank Interrupt handler
 * 
 * Called during the VBlank period. In a demoscene context, this is crucial for:
 * 1. Synchronizing audio with visual effects
 * 2. Updating global timing for precisely timed effects
 * 3. Performing any CPU-intensive calculations that can't be done during active render
 */
void vblank()
{
  update_audio();  // Keeps audio in sync with visuals
  ++current_frame;
}

/**
 * @brief Vertical Count Interrupt handler
 * 
 * Called during the HBlank period. While currently empty, this is a powerful tool for demoscene effects:
 * - Raster effects: Changing color palettes or scroll positions per scanline
 * - Mid-screen modifications: Altering rendering parameters partway through a frame
 * - Timing-critical effects: Executing precise code at exact screen positions
 */
void vcount()
{
  // Potential for advanced raster effects
}

/**
 * @brief Swaps the front and back buffers
 * 
 * Implements double buffering, essential for smooth animations in demos.
 * This technique allows complex frame rendering without visible tearing.
 */
void present()
{
  if (REG_DISPCNT & BACKBUFFER) {
    REG_DISPCNT &= ~BACKBUFFER;
    framebuffer = BB;
    backbuffer = FB;
  }
  else {
    REG_DISPCNT |= BACKBUFFER;
    framebuffer = FB;
    backbuffer = BB;
  } 
}

/**
 * @brief Main function to run the demo
 * 
 * This is the core of the demoscene production. It:
 * 1. Sets up the GBA's display and interrupt systems
 * 2. Initializes audio for music/sound effects
 * 3. Starts the demo script, which likely controls the sequence of effects
 * 4. Enters the main loop where the magic happens
 */
void run_demo()
{
  // Initialize interrupts for precise timing
  irqInit();
  irqEnable(IRQ_VBLANK | IRQ_HBLANK);
  irqSet(IRQ_VBLANK, vblank);
  irqSet(IRQ_HBLANK, vcount);

  // Set up display mode
  // MODE_4: 8bpp paletted bitmap mode, good for complex 2D effects
  // BG2_ON: Enable background 2 (main canvas for 2D effects)
  // OBJ_ENABLE | OBJ_1D_MAP: Enable sprites, useful for overlays or particle effects
  // BG_MOSAIC: Enable mosaic effect, can be used for transitions or pixelation effects
  SetMode(MODE_4 | BG2_ON | OBJ_ENABLE | OBJ_1D_MAP | BG_MOSAIC);

  // Start audio playback
  // In a demo, this is likely a tracked module or streamed audio
  play_audio((uint8_t*)junction_jive_data, (1300645 >> 4) - 1024, 124);

  // Initialize the demo script
  // This likely sets up the sequence of effects, transitions, etc.
  start_script();

  // Main demo loop
  while (1) {
    // Synchronize with VBlank for consistent timing
    VBlankIntrWait();

    // Update demo state - this drives the main visuals and effects
    tick_script(current_frame);

    // Swap buffers to display the newly rendered frame
    present();    
  }
}