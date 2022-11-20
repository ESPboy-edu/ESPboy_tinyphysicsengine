#ifndef SAF_H
#define SAF_H

/**
  @file saf.h

  Small Abstract Fish (SAF)

      []  [][][][][]
      [][][]      [][]
      [][]          []
      []    XX    XX[]
      []      XXXX  []
      [][]          []
      [][][]      [][]
      []  [][][][][] 

  Simple interface for programming small portable games, especially for open
  consoles but also the PC and other platforms.
 
  Some attributes of the SAF console are:

  - 64 x 64 pixels display
  - framebuffer
  - 256 colors, RGB332 palette
  - 25 FPS
  - 7 buttons
  - simple speaker
  - Von Neumann architecture (single memory space for program and data)
  - no limit on resource usage (RAM, cycles, cores, ...)
  - behavior such as rasterization should be the same on all platforms (i.e.
    rasterization or circle is implemented internally rather than relying on
    the platform's circle rasterization)
  - without extensions, SAF programs should be deterministic

  by drummyfish, 2021

  Released under CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/)
  plus a waiver of all other intellectual property. The goal of this work is
  be and remain completely in the public domain forever, available for any use
  whatsoever. */

/* user settings, these can be redefined before including the library (platform
  specific settings are listed under each platform implementation later): */

#ifndef SAF_SETTING_FORCE_1BIT
  /** Forces monochrome (1 bit) graphics even on platforms that can display more
  than 2 colors. This can be good for testing how a color game would look like
  on 1 bit displays. */
  #define SAF_SETTING_FORCE_1BIT 0
#endif

#ifndef SAF_SETTING_1BIT_DITHER
  /** Says if dithering should be used for monochrome (1 bit) platforms. Whether
  to use dithering or not depends on each program, some look better with it,
  some don't. Dithering consumes significantly more CPU power.*/
  #define SAF_SETTING_1BIT_DITHER 0
#endif

#ifndef SAF_SETTING_FASTER_1BIT
  /** If non-zero, the conversion of color to 1bit (monochromatic) will be done
  with an approximation that is faster but gives a slightly different
  (incorrect) result. 1 will set a mild approximation, 2 will set a faster one,
  3 a fastest one. This may be good for slow platforms. */
  #define SAF_SETTING_FASTER_1BIT 1
#endif

#ifndef SAF_SETTING_ENABLE_SOUND
  /** Can be used to disable sound at compile time. This is good to do if your
  game doesn't use any sounds so that the frontend doesn't have to
  unnecessarily manage sound libraries. Disabling sound may increase
  performance. */
  #define SAF_SETTING_ENABLE_SOUND 1
#endif

#ifndef SAF_SETTING_ENABLE_SAVES
  /** If 0, persistent memory for saves will be disabled so that saved data will
  only last during the program run. Disabling saves for games that don't use
  them may help the compiler optimize the program and not include libraries it
  won't need. */
  #define SAF_SETTING_ENABLE_SAVES 1
#endif

#ifndef SAF_SETTING_BACKGROUND_COLOR
  /** Specifies the color that should be used as a background, e.g. on platforms
  that have regions on screen where the game isn't drawn due to non-square
  resolution. */
  #define SAF_SETTING_BACKGROUND_COLOR 0
#endif

#include <stdint.h>

/* ============================= FOR PROGRAMS ==================================

  These are resources (functions, macros, ...) that are to be used by SAF client
  programs. If you are creating a program (a game etc.), only use these. A
  program is REQUIRED to implement:

  - SAF_PROGRAM_NAME macro: this must be set to a string with the program's name
    (e.g. #define SAF_PROGRAM_NAME "My game"). All version of the program should
    keep the same name as this name may be used e.g. to compute a hash that will
    determine its save address in EEPROM.
  - SAF_init function: in this function program should be initialized
  - SAF_loop function: this function handles the main loop 

  Before including saf.h a platform also needs to be specified by defining one
  of the possible SAF_PLATFORM_* macros.

  The program must NOT implement the main() function. */

// do NOT redefine these macros, they're read-only:
#define SAF_SCREEN_WIDTH   64
#define SAF_SCREEN_HEIGHT  64
#define SAF_FPS            25 ///< A divisor of 1000 prevents desync with RT.
#define SAF_SAVE_SIZE      32
#define SAF_MS_PER_FRAME   (1000 / SAF_FPS)
#define SAF_VERSION_STRING "1.0d"

#define SAF_BUTTON_UP             0x00
#define SAF_BUTTON_DOWN           0x01
#define SAF_BUTTON_LEFT           0x02
#define SAF_BUTTON_RIGHT          0x03
#define SAF_BUTTON_A              0x04
#define SAF_BUTTON_B              0x05
#define SAF_BUTTON_C              0x06

#define SAF_BUTTONS               7    ///< number of buttons

#define SAF_COLOR_BLACK           0x00
#define SAF_COLOR_WHITE           0xff
#define SAF_COLOR_GRAY            0x92
#define SAF_COLOR_GRAY_DARK       0x49
#define SAF_COLOR_RED             0xe0
#define SAF_COLOR_RED_DARK        0x80
#define SAF_COLOR_GREEN           0x1c
#define SAF_COLOR_GREEN_DARK      0x10
#define SAF_COLOR_BLUE            0x03
#define SAF_COLOR_BLUE_DARK       0x01
#define SAF_COLOR_YELLOW          0xf8
#define SAF_COLOR_ORANGE          0xf0
#define SAF_COLOR_BROWN           0x8d

#define SAF_COLOR_RGB(r,g,b) (((r / 32) << 5) | ((g / 32) << 2) | (b / 64))

#define SAF_SOUND_BEEP            0x00 ///< beep sound for special events
#define SAF_SOUND_CLICK           0x01 ///< click sound, e.g. for menu
#define SAF_SOUND_BOOM            0x02 ///< boom sound, e.g. for shooting
#define SAF_SOUND_BUMP            0x03 ///< bump sound, e.g. for hitting walls

#define SAF_SOUNDS                4    ///< number of sounds

#define SAF_TRANSFORM_NONE        0x00
#define SAF_TRANSFORM_ROTATE_90   0x01
#define SAF_TRANSFORM_ROTATE_180  0x02
#define SAF_TRANSFORM_ROTATE_270  0x03
#define SAF_TRANSFORM_FLIP        0x04 ///< horizontal flip before rotation
#define SAF_TRANSFORM_SCALE_2     0x08
#define SAF_TRANSFORM_SCALE_3     0x10
#define SAF_TRANSFORM_SCALE_4     0x18
#define SAF_TRANSFORM_INVERT      0x20 ///< invert colors

#define SAF_INFO_STRING \
  "made with SAF (SmallAbstractFish) library v. " SAF_VERSION_STRING

// these will potentially be redefined by each platform
#define SAF_PLATFORM_NAME         "platform"
#define SAF_PLATFORM_COLOR_COUNT  256
#define SAF_PLATFORM_BUTTON_COUNT 7
#define SAF_PLATFORM_RAM          0
#define SAF_PLATFORM_FREQUENCY    0
#define SAF_PLATFORM_HAS_SAVES    1
#define SAF_PLATFORM_HAS_SOUND    1
#define SAF_PLATFORM_HARWARD      0    ///< Harward architecture

#define SAF_LOGO_IMAGE 0xbee3c1938dc1e3be ///< 8x8 1b logo as 64 bit int

#ifndef SAF_PROGRAM_NAME
#error SAF_PROGRAM_NAME has to be defined before including the library.
#endif

/** Implement this function in your program and put initialization code in it.
  Frontend will call this when the program starts to initialize it. */
void SAF_init(void);

/** Implement this function in your program and put main loop code inside it.
  This function will be called periodically SAF_FPS times per second. When the
  function finishes, the framebuffer is presented to screen. The frame buffer
  is NOT cleared before this function is called. The function should return
  non-zero if the program continues or 0 if the program has ended. */
uint8_t SAF_loop(void);

/** Returns the number of frames for which a button has been continuously held,
  up to 255. If button >= SAF_BUTTONS, 0 will be returned. */
uint8_t SAF_buttonPressed(uint8_t button);

/** Checks if the button has been pressed exactly in the current frame. */
static inline uint8_t SAF_buttonJustPressed(uint8_t button);

/** Plays given sound. */
void SAF_playSound(uint8_t sound);

/** Saves a byte of data to persistent storage (e.g. a file, cookie etc.). If
  index >= SAF_SAVE_SIZE, nothing happens. WARNING: it may potentially be bad to
  call this function extremely often as on some platforms the save memory may be
  slow (disk) or prone to wearing off (EEPROM). The function tries to eliminate
  the writes, but you should also try to reduce the calls if possible. */
void SAF_save(uint8_t index, uint8_t data);

/** Loads a byte from persistent storage (saved with SAF_save). If no data were
  ever saved with SAF_save at the index, 0 is returned. 0 is always returned for
  index >= SAF_SAVE_SIZE. WARNING: The function keeps a cache of loaded values
  so that loading from the actual save memory only happens at most once per
  program run. */
uint8_t SAF_load(uint8_t index);

/** Gets the number of frames from start of the program. */
static inline uint32_t SAF_frame(void);

/** Gets the time from start of the program in milliseconds. */
static inline uint32_t SAF_time(void);

/** Returns a simple pseudorandom number. The number sequence will be the same
  in each program run and will repeat after 256 calls. SAF_randomSeed() can be
  called to seed this pseudorandom generator. */
uint8_t SAF_random(void);

/** Seeds the pseudorandom generator with an initial number. Numbers returned
  by SAF_random depend on this value. The generator is automatically seeded with
  0 at the start of a program. */
static inline void SAF_randomSeed(uint8_t seed);

/** Computes sin function of the argument (255 corresponds to 2*pi, i.e. the
  full angle). Returns a value between -127 to 127 (including). */
int8_t SAF_sin(uint8_t phase);

int8_t SAF_cos(uint8_t phase);

/** Computes integer square root of a number. */
uint16_t SAF_sqrt(uint32_t number);

/** Returns a 332 color closest to given RGB values. */
uint8_t SAF_colorFromRGB(uint8_t red, uint8_t green, uint8_t blue);

/** Converts given 332 color to amount of red, green and blue. This conversion
  aligns the blue levels with red/green levels so that it is possible to get
  true gray. */
void SAF_colorToRGB(uint8_t colorIndex, uint8_t *red, uint8_t *green, uint8_t *blue);

/** Converts given 332 color to an approximate 8bit grayscale value. Note that
  doing this per-pixel can negatively affect performance, in which case you may
  consider creating a lookup table using this function. */
static inline uint8_t SAF_colorToGrayscale(uint8_t colorIndex);

/** Converts given 332 color to a 1 bit value (black&white). The result returned
  will either be 0 (black) or non-zero (white). The conversion performed by this
  function is affected by SAF_SETTING_FASTER_1BIT. */
static inline uint8_t SAF_colorTo1Bit(uint8_t colorIndex);

/** Returns an "opposite" color of given 332 color. */
static inline uint8_t SAF_colorInvert(uint8_t color);
 
/** Sets a single pixel of the frame buffer to given color. */
void SAF_drawPixel(int8_t x, int8_t y, uint8_t color);

/** Draws a rectangle. */
void SAF_drawRect(int8_t x, int8_t y, int8_t width, int8_t height, uint8_t color, uint8_t filled);

/** Draws a line using the DDA algorithm. */
void SAF_drawLine(int8_t x1, int8_t y1, int8_t x2, int8_t y2, uint8_t color);

/** Draws a circle. */
void SAF_drawCircle(int8_t x, int8_t y, uint8_t radius, uint8_t color, uint8_t filled);

/** Clears the screen with given color, typically called before rendering a new
  frame. */
static inline void SAF_clearScreen(uint8_t color);

/** Draws given text with the built-in 4x4 font. */
int8_t SAF_drawText(const char *text, int8_t x, int8_t y, uint8_t color, uint8_t size);

/** Gets the built-in character mask, in case you want to draw the font 
  character yourself. The 4x4 character is returned as a 2 byte binary image. */
void SAF_getFontCharacter(uint8_t asciiIndex, uint8_t result[2]);

/** Draws an uncompressed 332 color image. Transformation can be applies by
  passing a bitwise or value or SAF_TRANSFORM_* values.

  The image format is following: 1st byte unsigned width, 2nd byte is unsigned
  height, following bytes each hold the 332 color of pixels starting from top
  left of the image and going right and down. */
void SAF_drawImage(const uint8_t *image, int8_t x, int8_t y, uint8_t transform, uint8_t transparentColor);

/** Same as SAF_drawImage but takes a compressed image as an input. This will
  most likely be slower than SAF_drawImage but will save ~2/3 memory on images.

  The compressed format is both lossy (palette reduction) and lossless (RLE). It
  is following: 1st byte is unsigned width, 2nd byte is unsigned height, the
  following 16 bytes are the image palette, then RLE bytes follow: each RLE byte
  specifies the palette color index in its lower 4 bits and the number of
  repetitions of that color in upper 4 bits (so e.g. value 0 means 1 pixel will
  be drawn). Images can be compressed to this format by tools that come with
  SAF. */
void SAF_drawImageCompressed(const uint8_t *image, int8_t x, int8_t y, uint8_t transform, uint8_t transparentColor);

/** Same as SAF_drawImage but for 1bit (monochrome) images. Transparency mask
  (in the same format as the drawn image) can be used.

  The 1bit image format is following: 1st byte is unsigned width, 2nd byte is
  unsigned height, following bytes encode bits if the image, each byte holds
  8 bits, MSB coming as 1st bit of the byte, going from top left of the image
  to the right and down. */
void SAF_drawImage1Bit(const uint8_t *image, int8_t x, int8_t y, const uint8_t *mask, uint8_t color1, uint8_t color2, uint8_t transform);

/** Converts integer to string. The string must have enough space allocated
  (safe size is 12). The string will be zero terminated by the function. Pointer
  identical to "string" will be returned. */
char *SAF_intToStr(int32_t number, char *string);

/** Same as SAF_intToStr but for floats. The safe allocated size for the string
  is 23. Maximum decimals in the result will be 10. */
char *SAF_floatToStr(float number, char *string, uint8_t decimals);

/** RESERVED for possible future implementation of extension via a text
  protocol, at the moment this function does nothing.

  The extension should work like this: the client program will send a string
  (call this function) and receive a string from the frontend (the return
  value). This may be exploited e.g. for network communication or file system
  operations. Empty string means an empty answer and will returned if the
  the extension is unsupported or similar cases.

  The string passed to the function can be dealocated or changed after the call,
  the frontend should make a copy if it needs it. */
const char *SAF_extension(const char *string);

/* ============================ FOR FRONTENDS ==================================

  These functions are NOT to be used by SAF client programs, they are for
  frontend implementations, i.e. if you're adding a new platform support. A
  frontend is normally required to do the following (but see below):

  - Redefine some or all of SAF_PLATFORM_* macros (e.g. the platform name,
    button count etc.). First undefine the macro and define again (to prevent
    warnings).
  - Implement the specific SAF_FE_* functions that are required to be
    implemented (see below); The functions mostly correspond to the client
    functions (e.g. SAF_drawPixel vs SAF_FE_drawPixel), but don't have to
    preform parameter checks. Call SAF_FE_init and SAF_FE_loop in the right
    places. The remaining SAF_FE_* functions are for the convenience of
    frontends that you may or may not use as you wish.
  - The frame buffer should be initialized to all zeros at the beginning of your
    frontend program.
  - Implement the main program body (e.g. the main() function or setup/loop
    Arduino functions) and call SAF_FE_init and SAF_FE_loop. Frame buffer should
    be presented to the screen after SAF_FE_loop finishes. Do NOT clear the
    frame buffer in main loop, the program is supposed to do this.
  - Init the console's state, i.e. clear screen to all black, buttons states to
    0 etc.
  - Try to respect and take into account SAF_SETTING_* macros.
  - Try to make your global identifiers unlikely to collide with the user
    program, i.e. you may e.g. prefix them with '_'.

  When implementing a new frontend take a look at already implemented ones to
  see how it's done.

  Another way to implement a frontend is to use a partly preimplemented PC
  stdlib generic frontend with advanced. For details look at
  SAF_FE_GENERIC_FRONTEND. */

//~~~ IMPLEMENT THE FOLLOWING ~~~

/** DO NOT USE IN PROGRAMS, this is for frontends only! Programs should use
  SAF_drawPixel instead.

  Implement this function in your platform.

  Draws pixel to the screen back buffer, i.e. this shouldn't be visible on the
  display right after calling this function, but only when the screen is updated
  by the frontend at the end of the frame. The coordinates passed are
  guaranteed to be from 0 to 63, therefore no bound check is needed to be
  performed by this function. */
static inline void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color);

/** DO NOT USE IN PROGRAMS, this is for frontends only! Programs should use
  SAF_playSound instead.

  Implement this function in your platform. */
static inline void SAF_FE_playSound(uint8_t sound);

/** DO NOT USE IN PROGRAMS, this is for frontends only! Programs should use
  SAF_save instead.

  Implement this function in your platform. 

  This function should save given data byte to a specified address (index) in
  the persistent storage to last between HW resets. You can use e.g. files,
  cookies or EEPROM to implement this memory. Index passed to this function will
  always be < SAF_SAVE_SIZE. You don't have to implement any optimizations
  (e.g. buffers, ignoring overwrites of same values etc.) as SAF already does
  them internally. */
static inline void SAF_FE_save(uint8_t index, uint8_t data);

/** DO NOT USE IN PROGRAMS, this is for frontends only! Programs should use
  SAF_load instead.

  Implement this function in your platform. 

  This function should load and return data byte from specified address (index)
  in the persistent storage. This data was saved with SAF_FE_save. If no
  data have ever been written to that memory address, 0 should be returned.
  Index passed to this function will always be < SAF_SAVE_SIZE. You don't have
  to implement any optimizations (e.g. buffers) as SAF already does this
  internally. */
static inline uint8_t SAF_FE_load(uint8_t index);

/** DO NOT USE IN PROGRAMS, this is for frontends only! Programs should use
  SAF_buttonPressed instead.

  Implement this function in your platform. 

  This function should return a non-zero value if given button is pressed, or 0
  if the button is not pressed. The function will be called for each button
  exactly one per frame (so there is no need to worry about returning a
  consistent value during a frame). Button number passed to this function will
  always be < SAF_BUTTONS. */
static inline uint8_t SAF_FE_buttonPressed(uint8_t button);

/* RESERVED, at this moment this function should always return an empty string
   (a pointer to value 0). */
static inline const char *SAF_FE_extension(const char *string);

// ~~~ CALL THE FOLLOWING IN RIGHT PLACES ~~~

/** DO NOT USE IN PROGRAMS, this is for frontends only!

  In your platform implementation call this function once in SAF_MS_PER_FRAME.

  This function calls the client program's SAF_loop (don't call this directly).
  If this function returns 0, halt the program, otherwise continue. */
static inline uint8_t SAF_FE_loop(void);

/** DO NOT USE IN PROGRAMS, this is for frontends only!

  In your platform implementation call this function at the start of the
  program. */
static inline void SAF_FE_init(void);

// ~~~ FOR FRONTEND CONVENIENCE ~~~

/* The following macros can optinally be defined by your frontend:

  SAF_FE_GENERIC_FRONTEND     enables a partly preimplemented generic frontend
                              that uses stdio functions and has advanced
                              features (screenshot taking, volume control, ...).
                              If this is defined, you don't have to implement
                              SAF_FE_save, SAF_FE_load, SAF_FE_drawPixel,
                              SAF_FE_buttonPressed, SAF_FE_extension and the
                              main function, but you need to implement some
                              other functions: see SAF_FE_GF_* functions. This
                              frontend also handles emscripten integration.

  SAF_FE_STDIO_SAVE_LOAD      includes the stdio.h library and implements
                              SAF_FE_save and SAF_FE_load using stdio files (so
                              you don't have to implement these). With
                              emscripten cookies are used instead of stdio
                              files. */

char SAF_FE_emptyString[1] = {0};

#define _SAF_UNUSED(identifier) (void)(identifier) ///< for suppressing warnings

/** Returns a simple 16bit hash of given string, useful for e.g. determining the
  save location in EEPROM based on the program's name. If you need an 8bit hash,
  just take the lower 8 bits of this hash. */
uint16_t SAF_FE_hashStr(const char *str);

/** Parses CLI arguments of form '-x' or '-xN' (x and N being chars). After
  calling, paramValues will hold the value of corresponding flags, e.g. if -s2
  was passed, paramValues['s'] will hold value '2'. If parameter wasn't present,
  the value will be 0. If the parameter was present without a value (-x), the
  value will be 1. */
void SAF_FE_paramParse(int argc, char **argv, uint8_t paramValues[128]);

/** Converts a 332 color to monochrome (1bit) color, taking into account
  potential dithering. This function should be used by monochromatic platforms.
*/
static inline uint8_t SAF_FE_colorTo1Bit(uint8_t color, uint8_t x, uint8_t y);

/** Uses the pixel art scaling algorithm "scale2x" to expand a single pixel into
  four pixels depending on its neighbouring pixels. To scale whole screen use
  SAF_FE_scale2xScreen. */
void SAF_FE_scale2xPixel(uint8_t middle, uint8_t top, uint8_t right,
  uint8_t bottom, uint8_t left, uint8_t result[4]);

/** Quickly Scales the whole SAF screen 4 times (twice in each dimention) using
  the smart "scale2x" pixel art scaling algorithm. */
void SAF_FE_scale2xScreen(
  const uint8_t screen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT],
  uint8_t result[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT * 4]);

#define SAF_FE_SOUND_SAMPLE_COUNT 1024

/** The function returns an 8 bit 8 KHz sample of a default built-in sound of
  this library, for implementing SAF_FE_playSound. If you're using custom
  external sounds or the platform's built-in beeps, don't use this. Each sound
  has SAF_FE_SOUND_SAMPLE_COUNT samples. */
int8_t SAF_FE_getSoundSample(uint8_t sound, uint16_t sampleNumber);

/** If SAF_FE_GENERIC_FRONTEND is defined, you need to implement this function
  in which you initialize your frontend. CLIParameters holds values of parsed
  command line arguments of format as returned by SAF_FE_paramParse. Certain
  flags (see generic frontend's help) are used by the generic frontend and these
  are guaranteed to hold only valid values when accessed (e.g. 's' will always
  have values '1' to '8'). Your frontend can use the rest of the flags as it
  wishes (to include these into help define SAF_FE_GF_EXTRA_HELP). */
void SAF_FE_GF_init(uint8_t CLIParameters[128]);

/**  If SAF_FE_GENERIC_FRONTEND is defined, you need to implement this function
  in which you free your allocated resources. This function will be called
  before the program exit. */
void SAF_FE_GF_end();

/** If SAF_FE_GENERIC_FRONTEND is defined, you need to implement this function
  in which you handle the main loop (only things that your frontend needs, the
  rest us handled by the generic frontend). CLIParameters is the same array as
  CLIParameters in SAF_FE_GF_init. The function should return a non-zero value
  if the program keeps running and 0 if the program has been exited (e.g. by
  closing the window). */
uint8_t SAF_FE_GF_loop(uint8_t params[128]);

const uint8_t *SAF_FE_GF_getScreenPointer();

/** If SAF_FE_GENERIC_FRONTEND is defined, you need to implement this function
  in which you copy the passed screen data into your frontend's screen. */
void SAF_FE_GF_present(
  const uint8_t screen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT]);

/** If SAF_FE_GENERIC_FRONTEND is defined, you need to implement this function
  which should return a bool value indicating whether specific keyboard key is
  pressed or not. Lowercase letters ('a', 'b', 'c', ...) represent letter keys,
  'U', 'D', 'L', 'R' represent arrow keys, 'E' is escape, 'X', 'Y', 'Z' are
  controller buttons. */
uint8_t SAF_FE_GF_keyPressed(char keyChar);

/** If SAF_FE_GENERIC_FRONTEND is defined, you need to implement this function
  which should do two things: sleep (yield CPU) for sleepMs milliseconds, then
  return the current number of milliseconds (after the sleep) from the start of
  the program. */
uint32_t SAF_FE_GF_sleep(uint16_t sleepMs);

/** If SAF_FE_GENERIC_FRONTEND is defined, you need to implement this function
  which should behave the same as SAF_FE_extension (at this moment should only
  return an empty string pointer).*/
const char *SAF_FE_GF_extension(const char *string);

/// SAF palette as 565 values.
#define SAF_FE_PALETTE_565 \
0,9,18,31,288,297,306,319,576,585,594,607,864,873,882,895,1152,1161,1170,1183,\
1440,1449,1458,1471,1728,1737,1746,1759,2016,2025,2034,2047,8192,8201,8210,\
8223,8480,8489,8498,8511,8768,8777,8786,8799,9056,9065,9074,9087,9344,9353,\
9362,9375,9632,9641,9650,9663,9920,9929,9938,9951,10208,10217,10226,10239,\
18432,18441,18450,18463,18720,18729,18738,18751,19008,19017,19026,19039,19296,\
19305,19314,19327,19584,19593,19602,19615,19872,19881,19890,19903,20160,20169,\
20178,20191,20448,20457,20466,20479,26624,26633,26642,26655,26912,26921,26930,\
26943,27200,27209,27218,27231,27488,27497,27506,27519,27776,27785,27794,27807,\
28064,28073,28082,28095,28352,28361,28370,28383,28640,28649,28658,28671,36864,\
36873,36882,36895,37152,37161,37170,37183,37440,37449,37458,37471,37728,37737,\
37746,37759,38016,38025,38034,38047,38304,38313,38322,38335,38592,38601,38610,\
38623,38880,38889,38898,38911,45056,45065,45074,45087,45344,45353,45362,45375,\
45632,45641,45650,45663,45920,45929,45938,45951,46208,46217,46226,46239,46496,\
46505,46514,46527,46784,46793,46802,46815,47072,47081,47090,47103,55296,55305,\
55314,55327,55584,55593,55602,55615,55872,55881,55890,55903,56160,56169,56178,\
56191,56448,56457,56466,56479,56736,56745,56754,56767,57024,57033,57042,57055,\
57312,57321,57330,57343,63488,63497,63506,63519,63776,63785,63794,63807,64064,\
64073,64082,64095,64352,64361,64370,64383,64640,64649,64658,64671,64928,64937,\
64946,64959,65216,65225,65234,65247,65504,65513,65522,65535

//======================= PLATFORM FRONTENDS ===================================

#if defined(SAF_PLATFORM_SDL2) || defined(SAF_PLATFORM_SDL2_TINY)
#include <SDL2/SDL.h>
// code common to all SDL frontends

uint8_t _SDL_volume = 0;
int8_t _SDL_currentSound = -1;
uint16_t _SDL_soundPosition = 0;

#if SAF_SETTING_ENABLE_SOUND

void SAF_SDL_playSound(uint8_t sound)
{
  _SDL_currentSound = sound;
  _SDL_soundPosition = 0;
}

void SAF_SDL_audioFillCallback(void *userdata, uint8_t *s, int l)
{
  _SAF_UNUSED(userdata);

  int16_t *s16 = (int16_t *) s;

  l /= 2;

  for (int i = 0; i < l; ++i)
  {
    if (_SDL_currentSound < 0)
      *s16 = 0;
    else
    {
      *s16 = SAF_FE_getSoundSample(_SDL_currentSound,_SDL_soundPosition);

      _SDL_soundPosition++;

      if (_SDL_soundPosition >= SAF_FE_SOUND_SAMPLE_COUNT)
      {
        _SDL_currentSound = -1;
        _SDL_soundPosition = 0;
      }
    }

    s16++;
  }
}

uint8_t SAF_SDL_initAudio(void)
{
  SDL_AudioSpec audioSpec;

  SDL_memset(&audioSpec, 0, sizeof(audioSpec));
  audioSpec.callback = SAF_SDL_audioFillCallback;
  audioSpec.freq = 8000;
  audioSpec.format = AUDIO_S16;
  audioSpec.channels = 1;

#ifdef __EMSCRIPTEN__
  audioSpec.samples = 1024;
#else
  audioSpec.samples = 256;
#endif

  if (SDL_OpenAudio(&audioSpec,NULL) < 0)
    return 0;

  SDL_PauseAudio(0);

  return 1;
}
#endif // SAF_SETTING_ENABLE_SOUND

#endif // SAF_PLATFORM_SDL2 || SAF_PLATFORM_SDL2_TINY

#ifdef SAF_PLATFORM_NULL
/* Null frontend, has no I/O implemented and runs at highest reachable FPS
  instead of SAF_FPS. This can be useful for testing, e.g. if you want to see
  the compiled size or performance of just the game without any frontend.
------------------------------------------------------------------------------*/

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color) 
  { _SAF_UNUSED(x); _SAF_UNUSED(y); _SAF_UNUSED(color); }
void SAF_FE_playSound(uint8_t sound) 
  { _SAF_UNUSED(sound); }
void SAF_FE_save(uint8_t index, uint8_t data) 
  { _SAF_UNUSED(index); _SAF_UNUSED(data); }
uint8_t SAF_FE_load(uint8_t index) 
  { _SAF_UNUSED(index); return 0; }
uint8_t SAF_FE_buttonPressed(uint8_t button) 
  { _SAF_UNUSED(button); return 0; }
const char *SAF_FE_extension(const char *string) 
  { _SAF_UNUSED(string); return SAF_FE_emptyString; }

int main(void)
{
  SAF_FE_init();
  while (SAF_FE_loop());
  return 0;
}

#elif defined(SAF_PLATFORM_SDL2) 
/* SDL2 platform using the SAF_FE_GENERIC_FRONTEND, also usable with emscripten
  (browser JavaScript).

  requirements: libsdl2-dev, stdio.h, unistd.h, stdlib.h
  compiling: link SDL2, e.g. -lSDL2 (emscripten: -s USE_SDL=2)
------------------------------------------------------------------------------*/

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "SDL2"

#define SAF_FE_GENERIC_FRONTEND

#include <SDL2/SDL.h>
#include <stdlib.h> // for malloc/free
#include <unistd.h> // for usleep

const uint8_t *_SDL_keyboardState;

SDL_Window *_SDL_window;
SDL_Renderer *_SDL_renderer;
SDL_Texture *_SDL_texture;
uint8_t _SDL_pixelArtUpscale = 0;
uint8_t *_SDL_upscaleScreen = 0;
SDL_GameController *_SDL_controller = 0;

void SAF_FE_playSound(uint8_t sound)
{
  SAF_SDL_playSound(sound);
}

void SAF_FE_GF_init(uint8_t CLIParameters[128])
{
  SDL_Init(
    SDL_INIT_EVENTS |
#if SAF_SETTING_ENABLE_SOUND
    SDL_INIT_AUDIO | 
#endif
    SDL_INIT_JOYSTICK);
  
  _SDL_volume = CLIParameters['v'] - '0';

  if (CLIParameters['u'] != 0)
  {
    _SDL_pixelArtUpscale = 1;
    _SDL_upscaleScreen = malloc(SAF_SCREEN_WIDTH * SAF_SCREEN_WIDTH * 4);
  }

  uint16_t screenScale = CLIParameters['s'] - '0';

  uint8_t fullscreen = screenScale == 0;

  if (screenScale == 0)
    screenScale = 1;

  _SDL_window = SDL_CreateWindow(SAF_PROGRAM_NAME, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, SAF_SCREEN_WIDTH * screenScale,
    SAF_SCREEN_HEIGHT * screenScale,SDL_WINDOW_SHOWN);

  if (fullscreen)
    SDL_SetWindowFullscreen(_SDL_window,SDL_WINDOW_FULLSCREEN_DESKTOP);

  _SDL_renderer = SDL_CreateRenderer(_SDL_window,-1,0);

  _SDL_texture = SDL_CreateTexture(_SDL_renderer,
    SDL_PIXELFORMAT_RGB332,SDL_TEXTUREACCESS_STATIC,
    SAF_SCREEN_WIDTH * (_SDL_pixelArtUpscale ? 2 : 1),
    SAF_SCREEN_HEIGHT * (_SDL_pixelArtUpscale ? 2 : 1));

  _SDL_keyboardState = SDL_GetKeyboardState(NULL);

  _SDL_controller = SDL_GameControllerOpen(0);

  SDL_PumpEvents();
  SDL_GameControllerUpdate();

#if SAF_SETTING_ENABLE_SOUND
  if (!SAF_SDL_initAudio())
    puts("SDL: could not initialize audio");
#endif
}

void SAF_FE_GF_end()
{
#if SAF_SETTING_ENABLE_SOUND
  SDL_PauseAudio(1);
  SDL_CloseAudio();
#endif

  if (_SDL_controller != 0)
    SDL_GameControllerClose(_SDL_controller);

  SDL_DestroyTexture(_SDL_texture);
  SDL_DestroyRenderer(_SDL_renderer); 
  SDL_DestroyWindow(_SDL_window); 

  if (_SDL_pixelArtUpscale)
    free(_SDL_upscaleScreen);
}

uint8_t SAF_FE_GF_loop(uint8_t params[128])
{
  _SDL_volume = params['v'] - '0';

  SDL_Event event;

  while (SDL_PollEvent(&event))
    if (event.type == SDL_QUIT)
      return 0;

  return 1;
}

void
  SAF_FE_GF_present(const uint8_t screen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT])
{
  if (_SDL_pixelArtUpscale)
  {
    SAF_FE_scale2xScreen(screen,_SDL_upscaleScreen);
    SDL_UpdateTexture(_SDL_texture,NULL,_SDL_upscaleScreen,SAF_SCREEN_WIDTH * 2);
  }
  else
    SDL_UpdateTexture(_SDL_texture,NULL,screen,SAF_SCREEN_WIDTH);

  SDL_RenderClear(_SDL_renderer);
  SDL_RenderCopy(_SDL_renderer,_SDL_texture,NULL,NULL);
  SDL_RenderPresent(_SDL_renderer);
}

uint8_t SAF_FE_GF_keyPressed(char keyChar)
{
  if (keyChar <= 'z' && keyChar >= 'a')
    return _SDL_keyboardState[SDL_SCANCODE_A + (keyChar - 'a')];

  #define b(x) ((_SDL_controller != NULL) && \
    SDL_GameControllerGetButton(_SDL_controller,SDL_CONTROLLER_BUTTON_ ## x))

  switch (keyChar)
  {
    case 'U': return 
      _SDL_keyboardState[SDL_SCANCODE_UP] || 
      b(DPAD_UP);
      break;

    case 'R': return 
      _SDL_keyboardState[SDL_SCANCODE_RIGHT] ||
      b(DPAD_RIGHT); 
      break;

    case 'D': return 
      _SDL_keyboardState[SDL_SCANCODE_DOWN] ||
      b(DPAD_DOWN); 
      break;

    case 'L': return 
      _SDL_keyboardState[SDL_SCANCODE_LEFT] ||
      b(DPAD_LEFT); 
      break;

    case 'E': return _SDL_keyboardState[SDL_SCANCODE_ESCAPE]; break;
    case 'X': return b(X); break;
    case 'Y': return b(Y) || b(B); break;
    case 'Z': return b(A) || b(START); break;

    default: return 0; break;
  }

  #undef b
}

uint32_t SAF_FE_GF_sleep(uint16_t sleepMs)
{
  if (sleepMs != 0)
    usleep(sleepMs * 1000);

  return SDL_GetTicks();
}

const char *SAF_FE_GF_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString; 
}

#elif defined(SAF_PLATFORM_SDL2_TINY)
/* Minimal SDL2 frontend, this frontend does NOT work with Emscripten (use
  normal SDL2 platform for that).

  requirements: libsdl2-dev, stdio.h, unistd.h, stdlib.h
  compiling: link SDL2, e.g. -lSDL2
------------------------------------------------------------------------------*/

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "SDL2 tiny"

#define SAF_FE_STDIO_SAVE_LOAD

#include <SDL2/SDL.h>
#include <unistd.h> // for usleep

#ifndef SAF_SETTING_SDL2_TINY_SCALE
  #define SAF_SETTING_SDL2_TINY_SCALE 4
#endif

#define SDL_UPSCALE
#define SDL_SCREEN_WIDTH \
  (SAF_SCREEN_WIDTH * SAF_SETTING_SDL2_TINY_SCALE)
#define SDL_SCREEN_HEIGHT \
  (SAF_SCREEN_HEIGHT * SAF_SETTING_SDL2_TINY_SCALE)

const uint8_t *_SDL_keyboardState;
uint8_t _SDL_screen[SDL_SCREEN_WIDTH * SDL_SCREEN_HEIGHT];

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
#if SAF_SETTING_SDL2_TINY_SCALE == 1
  _SDL_screen[y * SDL_SCREEN_WIDTH + x] = color;
#else
  uint8_t *pixel = _SDL_screen +
    y * (SDL_SCREEN_WIDTH * SAF_SETTING_SDL2_TINY_SCALE) + 
    x * SAF_SETTING_SDL2_TINY_SCALE;

  for (int y = 0; y < SAF_SETTING_SDL2_TINY_SCALE; ++y)
  {
    for (int x = 0; x < SAF_SETTING_SDL2_TINY_SCALE; ++x)
    {
      *pixel = color;
      pixel++;
    }

    pixel += SDL_SCREEN_WIDTH - SAF_SETTING_SDL2_TINY_SCALE;
  }
#endif
}

void SAF_FE_playSound(uint8_t sound)
{
  SAF_SDL_playSound(sound);
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  switch (button)
  {
    case SAF_BUTTON_UP: return 
      _SDL_keyboardState[SDL_SCANCODE_W] ||
      _SDL_keyboardState[SDL_SCANCODE_UP]; 
      break;

    case SAF_BUTTON_DOWN: return 
      _SDL_keyboardState[SDL_SCANCODE_S] ||
      _SDL_keyboardState[SDL_SCANCODE_DOWN]; 
      break;

    case SAF_BUTTON_LEFT: return 
      _SDL_keyboardState[SDL_SCANCODE_A] ||
      _SDL_keyboardState[SDL_SCANCODE_LEFT]; 
      break;

    case SAF_BUTTON_RIGHT: return 
      _SDL_keyboardState[SDL_SCANCODE_D] ||
      _SDL_keyboardState[SDL_SCANCODE_RIGHT]; 
      break;

    case SAF_BUTTON_A: return 
      _SDL_keyboardState[SDL_SCANCODE_Y] ||
      _SDL_keyboardState[SDL_SCANCODE_Z] ||
      _SDL_keyboardState[SDL_SCANCODE_J] ||
      _SDL_keyboardState[SDL_SCANCODE_SPACE]; 
      break;

    case SAF_BUTTON_B: return 
      _SDL_keyboardState[SDL_SCANCODE_X] ||
      _SDL_keyboardState[SDL_SCANCODE_K] ||
      _SDL_keyboardState[SDL_SCANCODE_RETURN]; 
      break;

    case SAF_BUTTON_C: return 
      _SDL_keyboardState[SDL_SCANCODE_C] ||
      _SDL_keyboardState[SDL_SCANCODE_L];
      break;

    default: return 0; break;
  }
}

static inline const char *SAF_FE_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString;
}

int main(int argc, char **argv)
{
  uint8_t fullscreen = 0;
  _SDL_volume = 8; 

  if (argc > 1 && argv[1][0] == '-' && argv[1][2] == 0)
  {
    if (argv[1][1] == 'f')
      fullscreen = 1;
    else if (argv[1][1] == 'm')
      _SDL_volume = 0;
    else if (argv[1][1] == 'h')
    {
      puts(SAF_PROGRAM_NAME ", " SAF_INFO_STRING " (" SAF_PLATFORM_NAME ")");
      puts("-f = fullscreen, -m = mute");
      puts("controls: WSAD (arrows), JKL (XYZC), ESC = exit");
      return 0;
    }
  }

  SDL_Init(
    SDL_INIT_EVENTS |
#if SAF_SETTING_ENABLE_SOUND
    SDL_INIT_AUDIO | 
#endif
    SDL_INIT_JOYSTICK);

  SDL_Window *window = SDL_CreateWindow(SAF_PROGRAM_NAME,
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
    SDL_SCREEN_WIDTH,SDL_SCREEN_HEIGHT,SDL_WINDOW_SHOWN);

  if (fullscreen)
    SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);

  SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);

  SDL_Texture *texture = 
    SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGB332,SDL_TEXTUREACCESS_STATIC,
    SDL_SCREEN_WIDTH,
    SDL_SCREEN_HEIGHT);

  _SDL_keyboardState = SDL_GetKeyboardState(NULL);
  
  for (int i = 0; i < SDL_SCREEN_WIDTH * SDL_SCREEN_HEIGHT; ++i)
    _SDL_screen[i] = 0;

  SDL_PumpEvents();
  SDL_GameControllerUpdate();

#if SAF_SETTING_ENABLE_SOUND
  if (!SAF_SDL_initAudio())
    puts("SDL: could not initialize audio");
#endif

  SAF_FE_init();

  uint32_t SAF_FE_GF_nextFrameTime = 0;

  while (1)
  {
//    SDL_PumpEvents();
    SDL_Event event;

    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT)
        return 0;
      
    if (_SDL_keyboardState[SDL_SCANCODE_ESCAPE])
      break;

    uint32_t time = SDL_GetTicks();

    while (time >= SAF_FE_GF_nextFrameTime)
    {
      if (!SAF_FE_loop())
        break;

      SAF_FE_GF_nextFrameTime += SAF_MS_PER_FRAME;
    }

    SDL_UpdateTexture(texture,NULL,_SDL_screen,SDL_SCREEN_WIDTH);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
    
    usleep(((SAF_FE_GF_nextFrameTime - time) * 3 / 4) * 1000); // relieve CPU
  }

#if SAF_SETTING_ENABLE_SOUND
  SDL_PauseAudio(1);
  SDL_CloseAudio();
#endif

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer); 
  SDL_DestroyWindow(window); 

  return 0;
}
#elif defined(SAF_PLATFORM_CSFML)
/* CSFML (C binding for SFML) platform with SAF_FE_GENERIC_FRONTEND.

  requirements: libscfml-dev
  compiling: link CSFML, usually -lcsfml-graphics -lcsfml-window -lcsfml-system
    -lcsfml-audio
------------------------------------------------------------------------------*/

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "CSFML"

#define SAF_FE_GENERIC_FRONTEND

#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/System.h>
#include <SFML/Audio/Types.h>
#include <stdio.h>
#include <stdlib.h> // for malloc/free

sfClock *_CSFML_clock;
sfRenderWindow* _CSFML_window;
sfTexture* _CSFML_windowTexture;
sfSprite* _CSFML_windowSprite;

uint8_t _CSFML_screenSize = SAF_SCREEN_WIDTH;

uint8_t _CSFML_SDL_pixelArtUpscale = 0;
uint8_t *_CSFML_upscaledScreen = 0;

#if SAF_SETTING_ENABLE_SOUND
  sfSound *_CSFML_sound;
  sfSoundBuffer *_CSFML_sounds[SAF_SOUNDS];
  uint8_t _CSFML_previousVolume = 0;
#endif

uint32_t _CSFML_windowPixels[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT * 4];
uint32_t _CSFML_paletteRGB32[256]; // SFML can't do 332, so precompute RGB here

void SAF_FE_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  sfSound_setBuffer(_CSFML_sound,_CSFML_sounds[sound]);
  sfSound_play(_CSFML_sound);
#else
  _SAF_UNUSED(sound);
#endif
}

void SAF_FE_GF_init(uint8_t CLIParameters[128])
{
  for (int i = 0; i < 256; ++i) // precompute RGB palette
  {
    uint8_t r,g,b;

    SAF_colorToRGB(i,&r,&g,&b);

    _CSFML_paletteRGB32[i] = 0xff000000 | (((uint32_t) b) << 16) |
      (((uint32_t) g) << 8) | r;
  }

  if (CLIParameters['u'] != 0)
  {
    _CSFML_SDL_pixelArtUpscale = 1;
    _CSFML_upscaledScreen = malloc(SAF_SCREEN_WIDTH * SAF_SCREEN_WIDTH * 4);
    _CSFML_screenSize *= 2;
  }

  for (int i = 0; i < _CSFML_screenSize * _CSFML_screenSize; ++i)
    _CSFML_windowPixels[i] = 0;
  
  _CSFML_clock = sfClock_create();
  sfClock_restart(_CSFML_clock);

  uint16_t screenScale = CLIParameters['s'] - '0';

  uint8_t fullscreen = screenScale == 0;

  _CSFML_windowTexture = sfTexture_create(_CSFML_screenSize,
    _CSFML_screenSize);

  sfTexture_setSmooth(_CSFML_windowTexture,sfFalse);

  _CSFML_windowSprite = sfSprite_create();

  sfVideoMode mode = {_CSFML_screenSize, _CSFML_screenSize, 32};

  _CSFML_window = sfRenderWindow_create(mode, SAF_PROGRAM_NAME, 
    fullscreen ? sfFullscreen : (sfResize | sfClose ), NULL);

  sfVector2i winPos;
  winPos.x = 1;
  winPos.y = 1;

  sfWindow_setPosition(_CSFML_window,winPos);

  sfSprite_setTexture(_CSFML_windowSprite,_CSFML_windowTexture,sfTrue);

  sfWindow_setVerticalSyncEnabled((sfWindow *) _CSFML_window,sfFalse);

  if (screenScale != 0)
  {
    sfVector2u winSize;
    winSize.x = _CSFML_screenSize * screenScale;
    winSize.y = winSize.x;
    sfWindow_setSize(_CSFML_window,winSize);
  }

#if SAF_SETTING_ENABLE_SOUND
  _CSFML_sound = sfSound_create();

  {
    int16_t samples[SAF_FE_SOUND_SAMPLE_COUNT];

    for (int j = 0; j < SAF_SOUNDS; ++j)
    {
      for (int i = 0; i < SAF_FE_SOUND_SAMPLE_COUNT; ++i)
        samples[i] = ((int16_t) SAF_FE_getSoundSample(j,i)) << 7;

      _CSFML_sounds[j] = sfSoundBuffer_createFromSamples(
        samples,SAF_FE_SOUND_SAMPLE_COUNT,1,8000);
    }
  }
#endif
}

void SAF_FE_GF_end()
{
#if SAF_SETTING_ENABLE_SOUND
  sfSound_destroy(_CSFML_sound);

  for (int i = 0; i < SAF_SOUNDS; ++i)
    sfSoundBuffer_destroy(_CSFML_sounds[i]);
#endif

  sfClock_destroy(_CSFML_clock);
  sfRenderWindow_destroy(_CSFML_window);
  sfSprite_destroy(_CSFML_windowSprite);
  sfTexture_destroy(_CSFML_windowTexture);

  if (_CSFML_SDL_pixelArtUpscale)
    free(_CSFML_upscaledScreen);
}

uint8_t SAF_FE_GF_loop(uint8_t params[128])
{
  sfEvent event;

  while (sfRenderWindow_pollEvent(_CSFML_window,&event));

  if (!sfRenderWindow_isOpen(_CSFML_window))
    return 0;
  
#if SAF_SETTING_ENABLE_SOUND
  int v = params['v'] - '0';

  if (_CSFML_previousVolume != v)
  {
    sfSound_setVolume(_CSFML_sound,(v * 100) / 8);
    _CSFML_previousVolume = v;
  }
#endif

  return 1;
}

void SAF_FE_GF_present(
  const uint8_t screen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT])
{
  const uint8_t *pixel = screen;
  uint32_t *pixel2 = _CSFML_windowPixels;

  if (_CSFML_SDL_pixelArtUpscale)
  {
    SAF_FE_scale2xScreen(screen,_CSFML_upscaledScreen);
    pixel = _CSFML_upscaledScreen;
  }

  for (int i = 0; i < _CSFML_screenSize * _CSFML_screenSize; ++i)
  {
    *pixel2 = _CSFML_paletteRGB32[*pixel];
    pixel++;
    pixel2++;
  }

  sfTexture_updateFromPixels(_CSFML_windowTexture,
    (const sfUint8 *) _CSFML_windowPixels,
    _CSFML_screenSize,_CSFML_screenSize,0,0);

  sfRenderWindow_drawSprite(_CSFML_window,_CSFML_windowSprite,NULL);
  sfRenderWindow_display(_CSFML_window);
}

uint8_t SAF_FE_GF_keyPressed(char keyChar)
{
  if (keyChar <= 'z' && keyChar >= 'a')
    return sfKeyboard_isKeyPressed(sfKeyA + (keyChar - 'a'));

  #define k(x) sfKeyboard_isKeyPressed(sfKey ## x)

  switch (keyChar)
  {
    case 'U': 
      return k(Up) || (sfJoystick_getAxisPosition(0,sfJoystickY) <= -50);
      break;

    case 'D': 
      return k(Down) || (sfJoystick_getAxisPosition(0,sfJoystickY) >= 50); 
      break;

    case 'L': 
      return k(Left) || (sfJoystick_getAxisPosition(0,sfJoystickX) <= -50); 
      break;

    case 'R': 
      return k(Right) || (sfJoystick_getAxisPosition(0,sfJoystickX) >= 50); 
      break;

    case 'E': return k(Escape); break;
    case 'X': return sfJoystick_isButtonPressed(0,0) ||
                     sfJoystick_isButtonPressed(0,3); break;
    case 'Y': return sfJoystick_isButtonPressed(0,1); break;
    case 'Z': return sfJoystick_isButtonPressed(0,2); break;

    default: break;
  }

  #undef k

  return 0;
}

uint32_t SAF_FE_GF_sleep(uint16_t sleepMs)
{
  sfTime t;
  t.microseconds = sleepMs * 1000;
  sfSleep(t);
  return sfClock_getElapsedTime(_CSFML_clock).microseconds / 1000;
}

const char *SAF_FE_GF_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString; 
}

#elif defined(SAF_PLATFORM_POKITTO)
/* Pokitto platform using the official PokittoLib.

  requirements: PokittoLib
  compiling: as any other pokitto program, leave My_settings.h empty
------------------------------------------------------------------------------*/

#ifndef SAF_SETTING_POKITTO_SCALE
  #define SAF_SETTING_POKITTO_SCALE 0 /**< type of screen scale for Pokitto,
                                           possible values: 0 (176x176), 1
                                           (128x128), 2 (220x176) */
#endif

#ifndef SAF_SETTING_POKITTO_VOLUME
  #define SAF_SETTING_POKITTO_VOLUME 4 //< 1 to 8 volume (if sound is on)
#endif

#ifndef SAF_SETTING_POKITTO_JOYHAT
  #define SAF_SETTING_POKITTO_JOYHAT 0
#endif

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "Pokitto"

#undef SAF_PLATFORM_RAM
#define SAF_PLATFORM_RAM 36000

#undef SAF_PLATFORM_FREQUENCY
#define SAF_PLATFORM_FREQUENCY ((_OSCT != 2) ? 48000000 : 72000000)    

#include "Pokitto.h"
#include "POKITTO_HW/HWLCD.h"
#include "POKITTO_CORE/PokittoCookie.h"

#if SAF_SETTING_ENABLE_SOUND
#include "POKITTO_HW/HWSound.h"
#include "POKITTO_HW/clock_11u6x.h"
#include "POKITTO_HW/timer_11u6x.h"
#endif

#if SAF_SETTING_POKITTO_JOYHAT
#include "JoyHat/JoyHat.h"

JoyHat pokittoJoy;

uint16_t pokittoAxisThreshold1, pokittoAxisThreshold2;
uint16_t pokittoRumbleCooldown = 0;
#endif

uint8_t *pokittoScreen;

static const uint16_t pokittoPalette[256] = // 332 palette in 565 format
{
  SAF_FE_PALETTE_565
};

#define CUSTOM_SCREEN_BUFFER \
  (PROJ_SCREENBUFFERSIZE < SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT)

#if CUSTOM_SCREEN_BUFFER
// if PokittoLib doesn't have large enough screen buffer, we create our own:
uint8_t pokittoScreenBuffer[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT];
#endif

#if SAF_SETTING_ENABLE_SAVES
class SaveCookie: public Pokitto::Cookie
{
  public:
    uint8_t data[SAF_SAVE_SIZE];
};

SaveCookie pokittoSave;
#endif

#if SAF_SETTING_ENABLE_SOUND
int8_t pokittoCurrentSound = -1;
uint16_t pokittoSoundPos = 0;

void pokittoOnTimer() // for sound
{
  if (Chip_TIMER_MatchPending(LPC_TIMER32_0,1))
  {
    Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);

    if (pokittoCurrentSound >= 0)
    {
      Pokitto::dac_write((SAF_FE_getSoundSample(pokittoCurrentSound,
        pokittoSoundPos) >> (8 - SAF_SETTING_POKITTO_VOLUME)
        & (0xff >> (8 - SAF_SETTING_POKITTO_VOLUME))));

      pokittoSoundPos++;

      if (pokittoSoundPos >= SAF_FE_SOUND_SAMPLE_COUNT)
      {
        pokittoCurrentSound = -1;
        pokittoSoundPos = 0;
      }
    }
  }
}

void pokittoTimerInit(uint32_t samplingRate)
{
  Chip_TIMER_Init(LPC_TIMER32_0);
  Chip_TIMER_Reset(LPC_TIMER32_0);
  Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
  Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, 
    (Chip_Clock_GetSystemClockRate() / samplingRate));
  Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);
  Chip_TIMER_Enable(LPC_TIMER32_0);

  #define weirdNumber ((IRQn_Type) 18)
  NVIC_ClearPendingIRQ(weirdNumber);
  NVIC_SetVector(weirdNumber,(uint32_t) &pokittoOnTimer);
  NVIC_EnableIRQ(weirdNumber);
  #undef weirdNumber
}
#endif // if SAF_SETTING_ENABLE_SOUND

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
#if SAF_SETTING_POKITTO_JOYHAT
  pokittoScreen[x * SAF_SCREEN_WIDTH + (SAF_SCREEN_HEIGHT - 1 - y)] = color;
#else
  pokittoScreen[y * SAF_SCREEN_WIDTH + x] = color;
#endif
}

void SAF_FE_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  pokittoCurrentSound = sound;

  #if SAF_SETTING_POKITTO_JOYHAT
  if (sound == SAF_SOUND_BOOM && pokittoRumbleCooldown == 0)
  {     
    pokittoJoy.Rumble(0.025);
    pokittoRumbleCooldown = 32;
  }
  #endif
#else
  _SAF_UNUSED(sound);
#endif
}

void SAF_FE_save(uint8_t index, uint8_t data)
{
#if SAF_SETTING_ENABLE_SAVES
  pokittoSave.data[index] = data;
  pokittoSave.saveCookie();

#if SAF_SETTING_ENABLE_SOUND
  // PokittoLib bug: saving disables timer, so re-enable it:
  pokittoTimerInit(8000);
#endif

#else
  _SAF_UNUSED(index);
  _SAF_UNUSED(data);
#endif
}

uint8_t SAF_FE_load(uint8_t index)
{
#if SAF_SETTING_ENABLE_SAVES
  return pokittoSave.data[index];
#else
  _SAF_UNUSED(index);
  return 0;
#endif
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  switch (button)
  {
#if !SAF_SETTING_POKITTO_JOYHAT
    case SAF_BUTTON_UP:    return Pokitto::Core::upBtn(); break;
    case SAF_BUTTON_RIGHT: return Pokitto::Core::rightBtn(); break;
    case SAF_BUTTON_DOWN:  return Pokitto::Core::downBtn(); break;
    case SAF_BUTTON_LEFT:  return Pokitto::Core::leftBtn(); break;
    case SAF_BUTTON_A:     return Pokitto::Core::aBtn(); break;
    case SAF_BUTTON_B:     return Pokitto::Core::bBtn(); break;
    case SAF_BUTTON_C:     return Pokitto::Core::cBtn(); break;
#else
    case SAF_BUTTON_UP: return Pokitto::Core::rightBtn() || 
      (pokittoJoy.JoyX() < pokittoAxisThreshold1);
      break;

    case SAF_BUTTON_RIGHT: return Pokitto::Core::downBtn() || 
      (pokittoJoy.JoyY() > pokittoAxisThreshold2);
      break;

    case SAF_BUTTON_DOWN: return Pokitto::Core::leftBtn() ||
      (pokittoJoy.JoyX() > pokittoAxisThreshold2);
      break;

    case SAF_BUTTON_LEFT: return Pokitto::Core::upBtn() ||
      (pokittoJoy.JoyY() < pokittoAxisThreshold1);
      break;

    case SAF_BUTTON_A: 
      return Pokitto::Core::aBtn() || pokittoJoy.Button1(); break;

    case SAF_BUTTON_B:
      return Pokitto::Core::bBtn() || pokittoJoy.Button2(); break;

    case SAF_BUTTON_C:     return Pokitto::Core::cBtn(); break;
#endif
    default:               return 0; break;
  }

#undef AXIS_THRES
}

static inline const char *SAF_FE_extension(const char *string)
{
  return SAF_FE_emptyString;
}

#if SAF_SETTING_POKITTO_SCALE == 0 || SAF_SETTING_POKITTO_SCALE == 2
static const uint8_t upscaleMap[176] =
{
  0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5,5,6,6,6,7,7,7,8,8,9,9,9,10,10,10,11,11,11,12,
  12,13,13,13,14,14,14,15,15,15,16,16,17,17,17,18,18,18,19,19,19,20,20,21,21,21,
  22,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,27,28,28,29,29,29,30,30,30,31,
  31,31,32,32,33,33,33,34,34,34,35,35,35,36,36,37,37,37,38,38,38,39,39,39,40,40,
  41,41,41,42,42,42,43,43,43,44,44,45,45,45,46,46,46,47,47,47,48,48,49,49,49,50,
  50,50,51,51,51,52,52,53,53,53,54,54,54,55,55,55,56,56,57,57,57,58,58,58,59,59,
  59,60,60,61,61,61,62,62,62,63,63
};
#endif

#if SAF_SETTING_POKITTO_SCALE == 2
static const uint8_t upscaleMap2[220] =
{
  0,0,0,0,1,1,1,2,2,2,2,3,3,3,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,9,9,10,10,
  10,11,11,11,11,12,12,12,13,13,13,13,14,14,14,15,15,15,16,16,16,16,17,17,17,18,
  18,18,18,19,19,19,20,20,20,20,21,21,21,22,22,22,22,23,23,23,24,24,24,25,25,25,
  25,26,26,26,27,27,27,27,28,28,28,29,29,29,29,30,30,30,31,31,31,32,32,32,32,33,
  33,33,34,34,34,34,35,35,35,36,36,36,36,37,37,37,38,38,38,38,39,39,39,40,40,40,
  41,41,41,41,42,42,42,43,43,43,43,44,44,44,45,45,45,45,46,46,46,47,47,47,48,48,
  48,48,49,49,49,50,50,50,50,51,51,51,52,52,52,52,53,53,53,54,54,54,54,55,55,55,
  56,56,56,57,57,57,57,58,58,58,59,59,59,59,60,60,60,61,61,61,61,62,62,62,63,63,
  63
};
#endif

int main()
{
#if SAF_SETTING_ENABLE_SAVES
  pokittoSave.begin(
    "SAF" SAF_PROGRAM_NAME,sizeof(pokittoSave),(char*) &pokittoSave);
#endif

  Pokitto::Core::begin(); 
  
#if SAF_SETTING_ENABLE_SOUND
  pokittoTimerInit(8000);
#endif

#if CUSTOM_SCREEN_BUFFER
  pokittoScreen = pokittoScreenBuffer;
#else
  pokittoScreen = Pokitto::Display::screenbuffer;
#endif

  Pokitto::Core::setFrameRate(SAF_FPS);
  Pokitto::Display::persistence = 1;
  Pokitto::Display::setInvisibleColor(-1);

  for (uint16_t y = 0; y < 176; ++y)
    for (uint16_t x = 0; x < 220; ++x)
      Pokitto::Display::directPixel(x,y,
        pokittoPalette[SAF_SETTING_BACKGROUND_COLOR]);

#if SAF_SETTING_POKITTO_JOYHAT
  pokittoAxisThreshold1 = pokittoJoy.joyScale / 4;
  pokittoAxisThreshold2 = pokittoJoy.joyScale - pokittoAxisThreshold1;
#endif

  SAF_FE_init();

  uint32_t nextFrame = 0;

  while (Pokitto::Core::isRunning())
  {
    Pokitto::Core::update(true);

#if SAF_SETTING_POKITTO_JOYHAT
    if (pokittoRumbleCooldown > 0)
      pokittoRumbleCooldown--;
#endif

    uint32_t time = Pokitto::Core::getTime();
    // we handle FPS ourselves as Pokittolib has a bug

    if (time >= nextFrame)
    {
      while (time >= nextFrame)
      {
        SAF_FE_loop();
        nextFrame += SAF_MS_PER_FRAME;
      }

      const uint8_t *p = pokittoScreen;

#if SAF_SETTING_POKITTO_SCALE == 0 || SAF_SETTING_POKITTO_SCALE == 2
      // 176x176, 220x176

#if SAF_SETTING_POKITTO_SCALE == 0
  #define SCR_W 176
  #define SCR_X 22
#else
  #define SCR_W 220
  #define SCR_X 0
#endif
      uint16_t line[SCR_W];
      uint8_t previousLine = 255;

      for (int16_t j = 0; j < 176; ++j)
      {
        int16_t upscaleMapRow = upscaleMap[j];

        if (previousLine != upscaleMapRow)
        {
          const uint8_t *l = pokittoScreen + SAF_SCREEN_WIDTH * upscaleMapRow;

          uint16_t *ll = line;

          const uint8_t *m = 
          #if SAF_SETTING_POKITTO_SCALE == 0
            upscaleMap;
          #else
            upscaleMap2;
          #endif

          for (int16_t i = 0; i < SCR_W; ++i)
          {
            uint16_t c = pokittoPalette[*(l + *m)];
            *ll = c;
            ll++;
            m++;
            p++;
          }
        }

        previousLine = upscaleMapRow;

        Pokitto::setDRAMpoint(SCR_X,j);
        Pokitto::pumpDRAMdata(line,SCR_W);
      }
#elif SAF_SETTING_POKITTO_SCALE == 1 // 128x128
      uint16_t line[SAF_SCREEN_WIDTH * 2];
      int16_t yPos = 24;

      for (int16_t j = 0; j < SAF_SCREEN_WIDTH * 2; j += 2)
      {
        uint16_t *l = line;

        for (int16_t i = 0; i < SAF_SCREEN_WIDTH; ++i)
        {
          uint16_t c = pokittoPalette[*p];
          *l = c;
          l++;
          *l = c;
          l++;
          p++;
        }

        Pokitto::setDRAMpoint(46,yPos);
        Pokitto::pumpDRAMdata(line,SAF_SCREEN_WIDTH * 2);

        yPos++;

        Pokitto::setDRAMpoint(46,yPos);
        Pokitto::pumpDRAMdata(line,SAF_SCREEN_WIDTH * 2);

        yPos++;
      }
#endif
    }
  }

  return 0;
}

#elif defined(SAF_PLATFORM_NCURSES)
/* ncuses (terminal, text-based) platform, this does not offer a "full"
  experience as terminal real time I/O handling and image displaying are
  limited, but some games are playable

  requirements: libncurses-dev, sys/time.h, stdio
  compiling: link ncurses, e.g. -lncurses
------------------------------------------------------------------------------*/

#define SAF_FE_STDIO_SAVE_LOAD

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "ncurses"

#undef SAF_PLATFORM_COLOR_COUNT
#define SAF_PLATFORM_COLOR_COUNT 2

#include <ncurses.h>
#include <sys/time.h>
#include <stdio.h> // for files

#define OFFSET_X 1
#define OFFSET_Y 1

uint8_t ncScreen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT];
uint8_t ncButtonStates[SAF_BUTTONS];

uint8_t ncCurrentSound = 0;
uint32_t ncSoundEnd = 0;

uint32_t ncGetTime()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
}

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  ncScreen[y * SAF_SCREEN_WIDTH + x] = color;
}

void SAF_FE_playSound(uint8_t sound)
{
  ncCurrentSound = sound;
  ncSoundEnd = ncGetTime() + 1000;
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  return ncButtonStates[button];
}

const char *SAF_FE_extension(const char *string)
{
  return SAF_FE_emptyString;
}

void printHelp(void)
{
  puts(SAF_PROGRAM_NAME
  "\n  " SAF_INFO_STRING " (" SAF_PLATFORM_NAME ")"
  "\n  controls: WSAD arrows, JKL YZXC space, Q = quit"
  "\n  possible arguments: -h (help), -i (invert colors)"
  );
}

int main(int argc, char **argv)
{
  int invert = 0;

  for (int i = 0; i < argc; ++i)
    if (argv[i][0] == '-' &&
      argv[i][1] != 0 &&
      argv[i][2] == 0)
    {
      switch (argv[i][1])
      {
        case 'h': printHelp(); return 0; break;
        case 'i': invert = 1;
        default: break;
      }
    }

  for (int i = 0; i < SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT; ++i)
    ncScreen[i] = 0;

  initscr();
  halfdelay(1);
  keypad(stdscr,TRUE);
  noecho();
  curs_set(0);

  SAF_FE_init();

  uint32_t nextFrame = ncGetTime();

  char c00 = ' ', c01 = ',', c10 = '\'', c11 = ';';

  if (invert)
  {
    c00 = ';'; c01 = '\''; c10 = ','; c11 = ' ';
  }

  while (1)
  {
    for (int i = 0; i < SAF_BUTTONS; ++i)
      ncButtonStates[i] = 0;

    int c = getch();

    int goOn = 1;

    switch (c)
    {
      case KEY_UP: case 'w':
        ncButtonStates[SAF_BUTTON_UP] = 1; break;
 
      case KEY_LEFT: case 'a':
        ncButtonStates[SAF_BUTTON_LEFT] = 1; break;

      case KEY_RIGHT: case 'd':
        ncButtonStates[SAF_BUTTON_RIGHT] = 1; break;

      case KEY_DOWN: case 's':
        ncButtonStates[SAF_BUTTON_DOWN] = 1; break;

      case ' ': case 'y': case 'z': case 'j':
        ncButtonStates[SAF_BUTTON_A] = 1; break;

      case 'x': case 'k':
        ncButtonStates[SAF_BUTTON_B] = 1; break;

      case 'c': case 'l':
        ncButtonStates[SAF_BUTTON_C] = 1; break;

      case 'q':
        goOn = 0; break;

      default: break;
    }

    uint32_t time = ncGetTime();

    while (time >= nextFrame)
    {
      if (!SAF_FE_loop())
      {
        goOn = 0;
        break;
      }

      nextFrame += SAF_MS_PER_FRAME;
    }

    if (!goOn)
      break;

    /* One terminal character will correspong to two pixels vertically nexto
    to each other, so we'll be scanning two display lines at once. */

    const uint8_t* scr = ncScreen;
    const uint8_t* scr2 = ncScreen + SAF_SCREEN_WIDTH;

    erase();

    move(OFFSET_Y,OFFSET_X + 1);

    for (int i = 0; i < SAF_SCREEN_WIDTH; ++i)
      addch('_');

    for (int y = 0; y < SAF_SCREEN_HEIGHT / 2; ++y)
    {
      move(y + OFFSET_Y + 1,1);

      addch('|');

      for (int x = 0; x < SAF_SCREEN_WIDTH; ++x)
      {
        uint8_t pixels = ((SAF_colorTo1Bit(*scr) != 0) << 1) |
          (SAF_colorTo1Bit(*scr2) != 0);

        char p;

        switch (pixels)
        {
          case 0: p = c00; break;
          case 1: p = c01; break;
          case 2: p = c10; break;
          case 3: p = c11; break;
          default: p = ' '; break;
        }

        addch(p);

        scr++;
        scr2++;
      }

      addch('|');

      scr += SAF_SCREEN_WIDTH;
      scr2 += SAF_SCREEN_WIDTH;
    }

    move(OFFSET_Y + 1 + SAF_SCREEN_HEIGHT / 2,2);

    for (int i = 0; i < SAF_SCREEN_WIDTH; ++i)
      addch('-');

    move(0,1);
    printw(SAF_PROGRAM_NAME);

    if (time < ncSoundEnd)
    {
      switch (ncCurrentSound)
      {
        case SAF_SOUND_BEEP: printw(" (BEEP)"); break;
        case SAF_SOUND_CLICK: printw(" (click)"); break;
        case SAF_SOUND_BOOM: printw(" (BOOM!)"); break;
        case SAF_SOUND_BUMP: printw(" (bump!)"); break;
        default: break;
      }
    }

    refresh();
  }

  endwin();

  return 0;
}
#elif defined(SAF_PLATFORM_X11)
/* X11 (xwindow, XLib) frontend

   requirements: XLib, stdio.h, unistd.h, sys/time.h
   compiling: link XLib, e.g. -lX11
------------------------------------------------------------------------------*/

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h> // for usleep

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "X11"

#define SAF_FE_STDIO_SAVE_LOAD

#define SOUND_BOOM "BOOM!"
#define SOUND_CLICK "click"
#define SOUND_BEEP "Beep"
#define SOUND_BUMP "bump!"

uint8_t _x11Scr[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT];

uint8_t _x11Buttons[SAF_BUTTONS];

uint8_t _x11CurrentSound = 0;
uint32_t _x11SoundEnd = 0;

uint32_t _x11GetTime()
{
  struct timeval now;
  gettimeofday(&now,NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
}

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  _x11Scr[y * SAF_SCREEN_WIDTH + x] = color;
}

void SAF_FE_playSound(uint8_t sound)
{
  _x11CurrentSound = sound;
  _x11SoundEnd = _x11GetTime() + 1000;

  switch (sound)
  {
    case SAF_SOUND_CLICK: puts(SOUND_CLICK); break;
    case SAF_SOUND_BEEP: puts(SOUND_BEEP); break;
    case SAF_SOUND_BOOM: puts(SOUND_BOOM); break;
    case SAF_SOUND_BUMP: puts(SOUND_BUMP); break;
    default: break;
  }
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  return _x11Buttons[button];
}

const char *SAF_FE_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString;
}

void printHelp(void)
{
  puts(SAF_PROGRAM_NAME 
  "\n  " SAF_INFO_STRING " (" SAF_PLATFORM_NAME ")"
  "\n  controls: WASD arrows, JKL YZXC space return, Esc = quit"
  "\n  possible arguments: -h (print help), -N (N = 1..8, scale)"
  );
}

unsigned long _palette[256];

int main(int argc, char **argv)
{
  int scale = 4;

  for (int i = 0; i < argc; ++i)
  {
    char *arg = argv[i];

    if (arg[0] != 0 && arg[1] != 0 && arg[2] == 0)
    {
      if (arg[1] == 'h')
      {
        printHelp();
        return 0;
      }

      if (arg[1] >= '1' && arg[1] <= '8')
        scale = arg[1] - '0';
    }
  }      

  for (int i = 0; i < SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT; ++i)
    _x11Scr[i] = 0;

  for (int i = 0; i < SAF_BUTTONS; ++i)
    _x11Buttons[i] = 0;

  SAF_FE_init();

  Display *display = XOpenDisplay(0);
  
  if (display == 0) 
  {
    puts("could not open a display");
    return 0;
  }

  int screen = DefaultScreen(display);

  Window window = XCreateSimpleWindow(display,RootWindow(display,screen),10,10,
    SAF_SCREEN_WIDTH * scale,SAF_SCREEN_HEIGHT * scale,1,
    BlackPixel(display,screen),WhitePixel(display,screen));

  XMapWindow(display,window);

  XSelectInput(display,window,KeyPressMask | KeyReleaseMask);

  // create the palette:

  for (int i = 0; i < 256; ++i)
  {
    XColor color;

    uint8_t r, g, b;

    SAF_colorToRGB(i,&r,&g,&b);

    color.red = ((uint16_t) r) * 256;
    color.green = ((uint16_t) g) * 256;
    color.blue = ((uint16_t) b) * 256;
    color.flags = DoRed | DoGreen | DoBlue;

    XAllocColor(display,DefaultColormap(display,0),&color);

    _palette[i] = color.pixel;
  }

  GContext context = DefaultGC(display,screen);

  uint32_t nextFrame = _x11GetTime();

  int goOn = 1;

  uint8_t previousSound = 255;

  while (1) // main loop
  {
    uint32_t time = _x11GetTime();

    while (time >= nextFrame)
    {
      if (!SAF_FE_loop())
      {
        goOn = 0;
        break;
      }

      nextFrame += SAF_MS_PER_FRAME;
    }

    usleep(((nextFrame - time) * 3 / 4) * 1000); // relieve CPU

    if (!goOn)
      break;

    const uint8_t *pixel = _x11Scr;

    int drawX = 0, drawY = 0;

    for (int y = 0; y < SAF_SCREEN_HEIGHT; ++y)
    {
      drawX = 0;

      for (int x = 0; x < SAF_SCREEN_WIDTH; ++x)
      {
        XSetForeground(display,context,_palette[*pixel]);

        XFillRectangle(display,window,context,drawX,drawY,
          scale,scale);

        pixel++;
        drawX += scale;
      }

      drawY += scale;
    }

    if (time >= _x11SoundEnd)
      _x11CurrentSound = 255;
    
    if (_x11CurrentSound != previousSound)
    {
      switch (_x11CurrentSound)
      {
        case SAF_SOUND_BEEP: 
          XStoreName(display, window, SAF_PROGRAM_NAME " " SOUND_BEEP); break;

        case SAF_SOUND_CLICK: 
          XStoreName(display, window, SAF_PROGRAM_NAME " " SOUND_CLICK); break;

        case SAF_SOUND_BOOM: 
          XStoreName(display, window, SAF_PROGRAM_NAME " " SOUND_BOOM); break;

        case SAF_SOUND_BUMP: 
          XStoreName(display, window, SAF_PROGRAM_NAME " " SOUND_BUMP); break;

        default: XStoreName(display, window, SAF_PROGRAM_NAME); break;
      }
    }

    previousSound = _x11CurrentSound;

    XEvent event;

    while (XCheckWindowEvent(display,window,KeyPressMask | KeyReleaseMask,&event) != False)
    {
      uint8_t state = event.xkey.type == KeyPress;

      switch (XKeycodeToKeysym(display,event.xkey.keycode,0))
      {
        case XK_Escape: goOn = 0; break;

        case XK_Up: case XK_w: 
          _x11Buttons[SAF_BUTTON_UP] = state; break;

        case XK_Left: case XK_a:
          _x11Buttons[SAF_BUTTON_LEFT] = state; break;
          
        case XK_Right: case XK_d:
          _x11Buttons[SAF_BUTTON_RIGHT] = state; break;

        case XK_Down: case XK_s:
          _x11Buttons[SAF_BUTTON_DOWN] = state; break;

        case XK_y: case XK_z: case XK_j: case XK_space:
          _x11Buttons[SAF_BUTTON_A] = state; break;
        
        case XK_x: case XK_k: case XK_Return:
          _x11Buttons[SAF_BUTTON_B] = state; break;

        case XK_c: case XK_l:
          _x11Buttons[SAF_BUTTON_C] = state; break;

        default: break;
      }
    }

    if (!goOn)
      break;
  }
 
  XCloseDisplay(display);

  return 0;
}

#elif defined(SAF_PLATFORM_ARDUBOY)
/* Arduboy platform using the official Arduboy2 library.

  requirements: Arduino environment, Arduboy2 library
  compiling: compile with Arduino IDE
------------------------------------------------------------------------------*/

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "Arduboy"

#undef SAF_PLATFORM_COLOR_COUNT
#define SAF_PLATFORM_COLOR_COUNT 2

#undef SAF_PLATFORM_BUTTON_COUNT
#define SAF_PLATFORM_BUTTON_COUNT 6

#undef SAF_PLATFORM_HARWARD
#define SAF_PLATFORM_HARWARD 1

#undef SAF_PLATFORM_RAM
#define SAF_PLATFORM_RAM 2500

#undef SAF_PLATFORM_FREQUENCY
#define SAF_PLATFORM_FREQUENCY 16000000

#include <Arduboy2.h>
Arduboy2 arduboy;

#if SAF_SETTING_ENABLE_SOUND
BeepPin1 arduboyBeep;
#endif

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  arduboy.drawPixel(32 + x,y,color ? WHITE : BLACK);
}

void SAF_FE_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  uint16_t f = 0;
  uint8_t t = 0;

  switch (sound)
  {
    case SAF_SOUND_BEEP: f = 300; t = 10; break;
    case SAF_SOUND_CLICK: f = 800; t = 1; break;
    case SAF_SOUND_BOOM: f = 50; t = 8; break;
    case SAF_SOUND_BUMP: f = 100; t = 2; break;
    default: break;
  }

  arduboyBeep.tone(arduboyBeep.freq(f),t);
#endif
}

#define SAVE_VALID_VALUE 133
uint16_t saveAddress = 0;
uint8_t saveLoaded = 0;

void arduboyLoadSave(void)
{
  if (!saveLoaded)
  {
    if (EEPROM.read(saveAddress) != SAVE_VALID_VALUE)
    {
      EEPROM.update(saveAddress,SAVE_VALID_VALUE);

      for (uint8_t i = 0; i < SAF_SAVE_SIZE; ++i)
        EEPROM.update(saveAddress + 1 + i,0);
    }

    saveLoaded = 1;
  }
}

void SAF_FE_save(uint8_t index, uint8_t data)
{
  arduboyLoadSave();
  EEPROM.update(saveAddress + 1 + index,data);
}

uint8_t SAF_FE_load(uint8_t index)
{
  arduboyLoadSave();
  return EEPROM.read(saveAddress + 1 + index);
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  switch (button)
  {
    case SAF_BUTTON_UP: button = UP_BUTTON; break;
    case SAF_BUTTON_RIGHT: button = RIGHT_BUTTON; break;
    case SAF_BUTTON_DOWN: button = DOWN_BUTTON; break;
    case SAF_BUTTON_LEFT: button = LEFT_BUTTON; break;
    case SAF_BUTTON_A: button = A_BUTTON; break;
    case SAF_BUTTON_B: button = B_BUTTON; break;
    default: return 0; break;
  }

  return arduboy.pressed(button);
}

const char *SAF_FE_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString;
}

void setup()
{
  arduboy.begin();
  arduboy.clear();

#if SAF_SETTING_ENABLE_SOUND
  arduboyBeep.begin();
  arduboy.audio.on();
#endif

  uint8_t c = SAF_colorTo1Bit(SAF_SETTING_BACKGROUND_COLOR) ? WHITE : BLACK;

  for (uint8_t y = 0; y < 64; ++y)
    for (uint8_t x = 0; x < 128; ++x)
      arduboy.drawPixel(x,y,c);

  arduboy.setFrameRate(SAF_FPS);
  SAF_FE_init();
 
  saveAddress = SAF_FE_hashStr(SAF_PROGRAM_NAME) & 0x03ff;

  if (saveAddress > (1024 - SAF_SAVE_SIZE - 1)) // -1 for valid value
    saveAddress = 1024 - SAF_SAVE_SIZE - 1;
}

void loop()
{
  if (!(arduboy.nextFrame()))
    return;

#if SAF_SETTING_ENABLE_SOUND
  arduboyBeep.timer();
#endif

  arduboy.pollButtons();    
  SAF_FE_loop();
  arduboy.display();
}
#elif defined(SAF_PLATFORM_ESPBOY)
/* ESPBoy platform.

  requirements: Arduino environment and required libraries
  compiling: compile with Arduino IDE
------------------------------------------------------------------------------*/

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "ESPBoy"

#undef SAF_PLATFORM_RAM
#define SAF_PLATFORM_RAM 4000000 

#undef SAF_PLATFORM_FREQUENCY
#define SAF_PLATFORM_FREQUENCY F_CPU

#undef SAF_PLATFORM_HARWARD
#define SAF_PLATFORM_HARWARD 1

#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_MCP4725.h>
#include <TFT_eSPI.h>

#if SAF_SETTING_ENABLE_SAVES
#include <ESP_EEPROM.h>
uint8_t espboySaveValidValue = 0;
EEPROMClass espboyEeprom;
#endif

#define MCP23017address 0
#define MCP4725address  0x60

Adafruit_MCP23017 espboyMcp;
Adafruit_MCP4725 espboyDac;
TFT_eSPI espboyTft;
uint8_t espboyScreen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT];
uint8_t espboyKeys = 0;

PROGMEM uint16_t espboyPalette[256] =
{
  SAF_FE_PALETTE_565
};

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  espboyScreen[y * SAF_SCREEN_WIDTH + x] = color;
}

void SAF_FE_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  int freq = 400;
  int dur = 75;
 
  switch (sound)
  {
    case SAF_SOUND_CLICK: freq = 400; dur = 20;  break;
    case SAF_SOUND_BEEP:  freq = 300; dur = 150; break;
    case SAF_SOUND_BOOM:  freq = 70;  dur = 200; break;
    case SAF_SOUND_BUMP:  freq = 130; dur = 50;  break;
    default: break;
  }

  tone(D3,freq,dur);
#else
  _SAF_UNUSED(sound);
#endif
}

#if SAF_SETTING_ENABLE_SAVES
void espboyCheckEeprom(void)
{
  if (espboyEeprom.read(0) == espboySaveValidValue)
    return;

  espboyEeprom.write(0,espboySaveValidValue);

  for (uint8_t i = 0; i < SAF_SAVE_SIZE; ++i)
    espboyEeprom.write(i + 1,0);

  espboyEeprom.commit();
}
#endif

void SAF_FE_save(uint8_t index, uint8_t data)
{
#if SAF_SETTING_ENABLE_SAVES
  espboyCheckEeprom();
  espboyEeprom.write(index + 1,data);
  espboyEeprom.commit();
#else
  _SAF_UNUSED(index);
  _SAF_UNUSED(data);
#endif
}

uint8_t SAF_FE_load(uint8_t index)
{
#if SAF_SETTING_ENABLE_SAVES
  espboyCheckEeprom();
  return espboyEeprom.read(index + 1);
#else
  _SAF_UNUSED(index);
#endif
return false;
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  switch (button)
  {
    case SAF_BUTTON_UP:    return espboyKeys & 0x02; break;
    case SAF_BUTTON_DOWN:  return espboyKeys & 0x04; break;
    case SAF_BUTTON_RIGHT: return espboyKeys & 0x08; break;
    case SAF_BUTTON_LEFT:  return espboyKeys & 0x01; break;
    case SAF_BUTTON_A:     return espboyKeys & 0x10; break;
    case SAF_BUTTON_B:     return espboyKeys & 0x20; break;
    case SAF_BUTTON_C:     return espboyKeys & 0x80; break;
    default: return 0; break;
  }
}

const char *SAF_FE_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString;
}

void setup()
{ 
  
  espboyDac.begin(MCP4725address);
  delay(100);
  espboyDac.setVoltage(0,false);
  espboyMcp.begin(MCP23017address);
  delay(100);

#if SAF_SETTING_ENABLE_SAVES 
  espboySaveValidValue = SAF_FE_hashStr(SAF_PROGRAM_NAME);
#endif

  // buttons
  for (uint8_t i = 0; i < 8; i++)
  {
    espboyMcp.pinMode(i,INPUT);
    espboyMcp.pullUp(i,HIGH);
  }

  espboyMcp.pinMode(8,OUTPUT);
  espboyMcp.digitalWrite(8,LOW);
  
  espboyTft.begin();
  delay(100);
  espboyTft.setRotation(0);

  espboyTft.setAddrWindow(0,128,0,128);
  espboyTft.fillScreen(TFT_BLACK);

  espboyDac.setVoltage(4095,true); // backlight

#if SAF_SETTING_ENABLE_SOUND
  pinMode(D3,OUTPUT);
#endif

#if SAF_SETTING_ENABLE_SAVES
  espboyEeprom.begin(SAF_SAVE_SIZE + 1);
#endif

  for (uint16_t i = 0; i < SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT; ++i)
    espboyScreen[i] = 0;

  SAF_FE_init();
}

uint32_t espboyNextFrame = 0;

void loop()
{
  uint32_t time = millis();

  if (time < espboyNextFrame)
    return;

  while (time >= espboyNextFrame)
  {
    SAF_FE_loop();
    espboyNextFrame += SAF_MS_PER_FRAME;
  }

  espboyKeys = ~espboyMcp.readGPIOAB() & 255;

  const uint8_t *pixel = espboyScreen;

  uint16_t line[SAF_SCREEN_WIDTH * 2];

  for (int y = 0; y < SAF_SCREEN_HEIGHT; ++y)
  {
    uint16_t *p = line;

    for (int x = 0; x < SAF_SCREEN_WIDTH; ++x)
    {
      uint16_t c = pgm_read_word(espboyPalette + *pixel);
      *p = c;
      p++;
      *p = c;
      p++;
      pixel++;
    }

    espboyTft.pushColors(line,SAF_SCREEN_WIDTH * 2);
    espboyTft.pushColors(line,SAF_SCREEN_WIDTH * 2);
  }
}
#elif defined(SAF_PLATFORM_NIBBLE)
/* Circuitmess Nibble platform. Persistent save/load isn't supported (EEPROM
  somehow doesn't work).

  requirements: Arduino environment and required libraries
  compiling: compile with Arduino IDE
------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <CircuitOS.h>
#include <Nibble.h>

#undef SAF_PLATFORM_HARWARD
#define SAF_PLATFORM_HARWARD 1

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "Nibble"

#undef SAF_PLATFORM_FREQUENCY
#define SAF_PLATFORM_FREQUENCY 160000000

#undef SAF_PLATFORM_RAM
#define SAF_PLATFORM_RAM 80000

#undef SAF_PLATFORM_HAS_SAVES
#define SAF_PLATFORM_HAS_SAVES 0

Display *nibbleDisplay;
uint16_t *nibbleFrameBuffer;
uint8_t nibbleButtons[SAF_BUTTONS];

PROGMEM uint16_t nibblePalette[256] =
{
  SAF_FE_PALETTE_565
};

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  uint16_t c = pgm_read_word(nibblePalette + color);

  c = ((c << 8) | (c >> 8)); // endien, TODO: could rather switch in the palette

  uint16_t *p = nibbleFrameBuffer + y * SAF_SCREEN_WIDTH * 4 + x * 2;

  *p = c;
  p++;
  *p = c;
  p += SAF_SCREEN_WIDTH * 2 - 1;
  *p = c;
  p++;
  *p = c;
}

void SAF_FE_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  switch (sound)
  {
    case SAF_SOUND_CLICK: Piezo.tone(400,20);  break;
    case SAF_SOUND_BEEP:  Piezo.tone(300,150); break;
    case SAF_SOUND_BOOM:  Piezo.tone(70,200);  break;
    case SAF_SOUND_BUMP:  Piezo.tone(130,50);  break;
    default: break;
  }
#else
  _SAF_UNUSED(sound);
#endif
}

void SAF_FE_save(uint8_t index, uint8_t data)
{
  _SAF_UNUSED(index);
  _SAF_UNUSED(data);
}

uint8_t SAF_FE_load(uint8_t index)
{
  _SAF_UNUSED(index);
  return 0;
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  return nibbleButtons[button];
}

const char *SAF_FE_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString;
}

// create button callbacks:

#define cbf(b,n)\
  void b ## _down() { nibbleButtons[n] = 255; }\
  void b ## _up() { nibbleButtons[n] = 0; }

cbf(BTN_UP,SAF_BUTTON_UP)
cbf(BTN_RIGHT,SAF_BUTTON_RIGHT)
cbf(BTN_DOWN,SAF_BUTTON_DOWN)
cbf(BTN_LEFT,SAF_BUTTON_LEFT)
cbf(BTN_A,SAF_BUTTON_A)
cbf(BTN_B,SAF_BUTTON_B)
cbf(BTN_C,SAF_BUTTON_C)
#undef cbf

void setup()
{
  Nibble.begin();
  nibbleDisplay = Nibble.getDisplay();
  nibbleFrameBuffer = 
    (uint16_t *) nibbleDisplay->getBaseSprite()->frameBuffer(0);

  for (uint8_t i = 0; i < SAF_BUTTONS; ++i)
    nibbleButtons[i] = 0;

  // register button callbacks:

  #define cb(b) \
    Input::getInstance()->setBtnPressCallback(b,b ## _down); \
    Input::getInstance()->setBtnReleaseCallback(b,b ## _up);

  cb(BTN_UP)
  cb(BTN_DOWN)
  cb(BTN_LEFT)
  cb(BTN_RIGHT)
  cb(BTN_A)
  cb(BTN_B)
  cb(BTN_C)

  #undef cb

  SAF_FE_init();
}

uint32_t nibbleNextFrame = 0;

void loop()
{
  uint32_t time = millis();

  Input::getInstance()->loop(0);

  if (time < nibbleNextFrame)
    return;

  while (time >= nibbleNextFrame)
  {
    SAF_FE_loop();
    nibbleNextFrame += SAF_MS_PER_FRAME;
  }

  nibbleDisplay->commit(); 
}
#elif defined(SAF_PLATFORM_GAMEBUINO_META)
/* Gamebuino META frontend.
  
  requirements: Arduino environment, Gamebuino-Meta.h
  compiling: compile with Arduino IDE
------------------------------------------------------------------------------*/

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "GB META"

#undef SAF_PLATFORM_RAM
#define SAF_PLATFORM_RAM 32000

#undef SAF_PLATFORM_FREQUENCY
#define SAF_PLATFORM_FREQUENCY 48000000

#undef SAF_PLATFORM_HARWARD
#define SAF_PLATFORM_HARWARD 1

#include <Gamebuino-Meta.h>

#if SAF_SETTING_ENABLE_SAVES
const Gamebuino_Meta::SaveDefault metaSaveDefault[] =
  { { 0, SAVETYPE_BLOB, SAF_SAVE_SIZE, 0 } };
#endif

uint8_t metaLEDCountdown = 0;

PROGMEM uint16_t gbmPalette[256] =
{
  SAF_FE_PALETTE_565
};

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  gb.display.drawPixel(8 + x,y,(Color) pgm_read_word(gbmPalette + color));
}

void SAF_FE_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  switch (sound)
  {
    case SAF_SOUND_CLICK: gb.sound.tone(800,60); break;
    case SAF_SOUND_BEEP: gb.sound.tone(300,150); break; 
    case SAF_SOUND_BUMP: gb.sound.tone(130,50); break;
    case SAF_SOUND_BOOM: 
      gb.sound.playCancel(); 
      gb.lights.fill(RED);
      metaLEDCountdown = 5;
      break;
    default: break;
  }
#else
  _SAF_UNUSED(sound);
#endif
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  Gamebuino_Meta::Button b;

  switch (button)
  {
    case SAF_BUTTON_UP: b = BUTTON_UP; break;
    case SAF_BUTTON_RIGHT: b = BUTTON_RIGHT; break;
    case SAF_BUTTON_DOWN: b = BUTTON_DOWN; break;
    case SAF_BUTTON_LEFT: b = BUTTON_LEFT; break;
    case SAF_BUTTON_A: b = BUTTON_A; break;
    case SAF_BUTTON_B: b = BUTTON_B; break;
    case SAF_BUTTON_C: b = BUTTON_MENU; break;
    default: return 0; break;
  }

  return gb.buttons.timeHeld(b) > 0;
}

const char *SAF_FE_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString;
}

uint8_t SAF_FE_load(uint8_t index)
{
#if SAF_SETTING_ENABLE_SAVES
  uint8_t data[SAF_SAVE_SIZE];
  gb.save.get(0,data,SAF_SAVE_SIZE);
  return data[index];
#else
  _SAF_UNUSED(index);
#endif
}

void SAF_FE_save(uint8_t index, uint8_t data)
{
#if SAF_SETTING_ENABLE_SAVES
  uint8_t d[SAF_SAVE_SIZE];
  gb.save.get(0,d,SAF_SAVE_SIZE);
  d[index] = data;
  gb.save.set(0,d,SAF_SAVE_SIZE);
#else
  _SAF_UNUSED(index);
  _SAF_UNUSED(data);
#endif
}

void setup()
{
  gb.begin();
  gb.setFrameRate(SAF_FPS);

  for (uint8_t y = 0; y < 64; ++y)
    for (uint8_t x = 0; x < 80; ++x)
      gb.display.drawPixel(x,y,(Color) pgm_read_word(gbmPalette + 
        SAF_SETTING_BACKGROUND_COLOR));

#if SAF_SETTING_ENABLE_SAVES
  gb.save.config(metaSaveDefault);
#endif

  SAF_FE_init();
}

void loop()
{
  if (!gb.update())
    return;

  if (metaLEDCountdown > 0)
  {
    metaLEDCountdown--;

    if (metaLEDCountdown == 0)
      gb.lights.clear();
  }

  SAF_FE_loop();
}
#elif defined(SAF_PLATFORM_RINGO)
/* Circuitmess Ringo (MAKERphone) frontend.
  
  requirements: Arduino environment, required libraries
  compiling: compile with Arduino IDE
------------------------------------------------------------------------------*/

#include <Arduino.h>
#include <MAKERphone.h>
//#include <utility/soundLib/MPWavLib.h>

#undef SAF_PLATFORM_NAME
#define SAF_PLATFORM_NAME "GB META"

#undef SAF_PLATFORM_RAM
#define SAF_PLATFORM_RAM 520000

#undef SAF_PLATFORM_FREQUENCY
#define SAF_PLATFORM_FREQUENCY 160000000

#define RINGO_SAVE_FILE_NAME ("/" SAF_PROGRAM_NAME ".sav")

uint16_t ringoPalette[256] =
{
  SAF_FE_PALETTE_565
};

MAKERphone mp;

#if SAF_SETTING_ENABLE_SOUND
Oscillator *ringoOsc;
#endif

uint8_t ringoArrows;

//uint8_t ringoScreen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT];

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  mp.display.fillRect(16 + x * 2,y * 2,2,2,ringoPalette[color]);
}

void SAF_FE_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  uint8_t wave = SINE;
  uint16_t freq = 1000;
  float dur = 0.2;

  switch (sound)
  {
    case SAF_SOUND_BEEP: freq = 500; dur = 0.08; wave = SINE; break;        // plasma
    case SAF_SOUND_CLICK: freq = 900; dur = 0.003; wave = SAW; break;  // click
    case SAF_SOUND_BOOM: freq = 100;  dur = 0.06; wave = SAW; break;             // explosion
    case SAF_SOUND_BUMP: freq = 200; dur = 0.02; wave = SQUARE; break;        // plasma
    default: break;
  }

  ringoOsc->setWaveform(wave);
  ringoOsc->beep(freq,dur);
#else
  _SAF_UNUSED(sound);
#endif
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  switch (button)
  {
    #define b(but) (mp.buttons.timeHeld(but) > 0)
    #define r(but) return b(but); break;

    case SAF_BUTTON_UP: return (ringoArrows & 0x04) || b(BTN_2); break;
    case SAF_BUTTON_DOWN: return (ringoArrows & 0x08) || b(BTN_5); break;
    case SAF_BUTTON_RIGHT: return (ringoArrows & 0x01) || b(BTN_6); break;
    case SAF_BUTTON_LEFT: return (ringoArrows & 0x02) || b(BTN_4); break;
    case SAF_BUTTON_A: return b(BTN_A) || b(BTN_8); break;
    case SAF_BUTTON_B: return b(BTN_B) || b(BTN_9); break;
    case SAF_BUTTON_C: r(BTN_7);
    default: return 0; break;

    #undef b
    #undef r
  }

  return 0;
  return 0;
}

const char *SAF_FE_extension(const char *string)
{
  _SAF_UNUSED(string);
  return SAF_FE_emptyString;
}

#if 1//SAF_SETTING_ENABLE_SAVES
uint8_t ringoSaveLoaded = 0;
uint8_t ringoSaveData[SAF_SAVE_SIZE];

void ringoCheckSave()
{
  if (!ringoSaveLoaded)
  {
    if (SD.exists(RINGO_SAVE_FILE_NAME))
    {
      File f = SD.open(RINGO_SAVE_FILE_NAME,FILE_READ);
      f.read(ringoSaveData,SAF_SAVE_SIZE);
      f.close();
    }
    else
      for (uint8_t i = 0; i < SAF_SAVE_SIZE; ++i)
        ringoSaveData[i] = 0;

    ringoSaveLoaded = 1;
  }
}
#endif

uint8_t SAF_FE_load(uint8_t index)
{
#if SAF_SETTING_ENABLE_SAVES
  ringoCheckSave();
  return ringoSaveData[index];
#else
  _SAF_UNUSED(index);
  return 0;
#endif
}

void SAF_FE_save(uint8_t index, uint8_t data)
{
#if SAF_SETTING_ENABLE_SAVES
  ringoSaveData[index] = data;

  ringoCheckSave();

  SD.remove(RINGO_SAVE_FILE_NAME);
  File f = SD.open(RINGO_SAVE_FILE_NAME,FILE_WRITE);
  f.write(ringoSaveData,SAF_SAVE_SIZE);
  f.close();
#else
  _SAF_UNUSED(index);
  _SAF_UNUSED(data);
#endif
}

void setup()
{
  mp.begin();

#if SAF_SETTING_ENABLE_SOUND
  ringoOsc = new Oscillator(SINE);
  addOscillator(ringoOsc);
  ringoOsc->setVolume(64);
#endif
  
  SAF_FE_init();
}

uint32_t ringoNextFrame = 0;

void loop()
{
  uint32_t time = millis();

  if (time < ringoNextFrame)
    return;

  mp.display.fillRect(
    0,0,LCDWIDTH,LCDHEIGHT,ringoPalette[SAF_SETTING_BACKGROUND_COLOR]);

  while (time >= ringoNextFrame)
  {
    SAF_FE_loop();
    ringoNextFrame += SAF_MS_PER_FRAME;
  }

  ringoArrows = 0x00 | 
    ((mp.buttons.getJoystickX() < 200)) << 0 |
    ((mp.buttons.getJoystickX() > 900)) << 1 |
    ((mp.buttons.getJoystickY() < 200)) << 2 |
    ((mp.buttons.getJoystickY() > 900)) << 3;

  mp.update();
}
#else
  #error No known SAF frontend specified.
#endif // platform frontends

//===================== FUNCTION IMPLEMENTATIONS ===============================

#ifdef SAF_FE_GENERIC_FRONTEND

#ifndef SAF_FE_STDIO_SAVE_LOAD
  #define SAF_FE_STDIO_SAVE_LOAD
#endif

#include <stdio.h>

#define SAF_FE_GF_SAVE_FILE_NAME (SAF_PROGRAM_NAME ".rec")

uint8_t SAF_FE_GF_running = 1;
uint8_t SAF_FE_GF_volume = 4;
uint8_t SAF_FE_GF_screen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT];
uint8_t SAF_FE_GF_parameters[128]; // CLI parameters
uint32_t SAF_FE_GF_keyStates = 0;
uint8_t SAF_FE_GF_buttonStates = 0;
uint8_t SAF_FE_GF_paused = 0;
uint32_t SAF_FE_GF_demoNextFrame = 0; // when to load the next record item
uint8_t SAF_FE_GF_demoNextButtons = 0;
int32_t SAF_FE_GF_frameTimes = 0; // for debug, measures time spent in a frame
uint32_t SAF_FE_GF_nextFrameTime = 0;
FILE *SAF_FE_GF_recordFile;

/** Records a specific key state and returns 0 if the key is not pressed, 1 if 
  it was just pressed or 2 if it's been pressed for multiple frames. */
uint8_t SAF_FE_GF_handleKey(char key, uint8_t position)
{
  position = 0x01 << position;

  uint8_t previous = (SAF_FE_GF_keyStates & position) != 0;

  if (SAF_FE_GF_keyPressed(key))
  {
    SAF_FE_GF_keyStates |= position;
    return previous ? 2 : 1;
  }
  else
  {
    SAF_FE_GF_keyStates &= ~position;
    return 0;
  }
}

uint8_t SAF_FE_buttonPressed(uint8_t button)
{
  char key = 0, key2 = 0, key3 = 0, key4 = 0;
  uint8_t bit = 255;

  switch (button)
  {
    case SAF_BUTTON_UP:    key = 'w'; key2 = 'U'; bit = 0; break;
    case SAF_BUTTON_RIGHT: key = 'd'; key2 = 'R'; bit = 1; break;
    case SAF_BUTTON_DOWN:  key = 's'; key2 = 'D'; bit = 2; break;
    case SAF_BUTTON_LEFT:  key = 'a'; key2 = 'L'; bit = 3; break;
    case SAF_BUTTON_A:     key = 'j'; key2 = 'y'; key3 = 'z'; key4 = 'X'; bit = 4; break;
    case SAF_BUTTON_B:     key = 'k'; key2 = 'x'; key3 = 'Y'; bit = 5; break;
    case SAF_BUTTON_C:     key = 'l'; key2 = 'c'; key3 = 'Z'; bit = 6; break;
    default: break;
  }

  return SAF_FE_GF_keyPressed(key) || 
    (key2 && SAF_FE_GF_keyPressed(key2)) || 
    (key3 && SAF_FE_GF_keyPressed(key3)) ||
    (key4 && SAF_FE_GF_keyPressed(key4)) ||
      (SAF_FE_GF_parameters['p'] && bit != 255 && 
      (SAF_FE_GF_buttonStates & (0x01 << bit)));
}

void SAF_FE_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  SAF_FE_GF_screen[y * SAF_SCREEN_WIDTH + x] = color;
}

void SAF_FE_GF_saveScreenshot(const char *file)
{
  FILE *f = fopen(file,"wb");

  if (!f)
    return;

  fwrite("P6 64 64 255\n",13,1,f);

  for (uint16_t i = 0; i < SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT; ++i)
  {
    uint8_t t[3];
    SAF_colorToRGB(SAF_FE_GF_screen[i],t,t + 1,t + 2);

    fwrite(t,3,1,f);
  }

  fclose(f);
 
  puts("screenshot taken");
}

void SAF_FE_GF_printHelp()
{
  puts(
      SAF_PROGRAM_NAME 
      "\n  " SAF_INFO_STRING 
      "\n  " SAF_PLATFORM_NAME " (generic frontend)\n"
      "controls:\n"
      "  WSAD, arrows     direction buttons\n"
      "  JKL              A, B and C buttons\n"
      "  P                pause/resume\n"
      "  escape           quit\n"
      "  T                take screenshot\n"
      "  U/I              volume -/+\n"
      "  N/M              speed -/+\n"
      "possible arguments (if supported):\n"
      "  -h               print help and quit\n"
      "  -sX              scale X times (0 = fullscreen)\n"
      "  -vX              set volume to X (0 to 8)\n"
      "  -bX              boost speed (1 to 5, 3 = normal)\n"
      "  -d               show debug info\n"
      "  -u               use pixel art scaling (scale2x)\n"
      "  -r               record inputs (demo, can be used for saves)\n"
      "  -p               play recorded inputs (demo)\n"
      "  -P               like -p but rewind to end (\"load state\")\n"
      "  -l               turns on -P and -r (save/load via demos)\n"
      "  -S               start paused");

#ifdef SAF_FE_GF_EXTRA_HELP
  puts(SAF_FE_GF_EXTRA_HELP);
#endif 
}

char *SAF_FE_GF_byteToHex(uint8_t b, char s[3])
{
  for (uint8_t i = 0; i < 2; ++i)
  {
    uint8_t r = b % 16;
    s[1 - i] = r + ((r < 10) ? '0' : ('a' - 10));
    b /= 16;
  }

  s[2] = 0;

  return s;
}

uint8_t SAF_FE_GF_byteFromHex(const char *hex)
{
  uint8_t result = 0;

  for (int8_t i = 0; i < 2; ++i)
    result = result * 16 + hex[i] - 
      ((hex[i] >= '0' && hex [i] <= '9') ? '0' : ('a' - 10));

  return result;
}

void readNextDemoRecord(void)
{
  char line[64];
  SAF_FE_GF_buttonStates = SAF_FE_GF_demoNextButtons;

  if (fgets(line,64,SAF_FE_GF_recordFile) != 0)
  {
    SAF_FE_GF_demoNextFrame = 
      (((uint32_t) SAF_FE_GF_byteFromHex(line)) << 24) +
      (((uint32_t) SAF_FE_GF_byteFromHex(line + 2)) << 16) +
      (((uint32_t) SAF_FE_GF_byteFromHex(line + 4)) << 8) +
      ((uint32_t) SAF_FE_GF_byteFromHex(line + 6));

    SAF_FE_GF_demoNextButtons = SAF_FE_GF_byteFromHex(line + 9);
  }
  else
  {
    puts("replaying inputs finished");
    SAF_FE_GF_demoNextFrame = 0xffffffff;

    fclose(SAF_FE_GF_recordFile);

    if (SAF_FE_GF_parameters['r'])
    {
      // after "loading" the state open again for appending

      SAF_FE_GF_parameters['p'] = 0;
      SAF_FE_GF_parameters['P'] = 0;

      SAF_FE_GF_recordFile = fopen(SAF_FE_GF_SAVE_FILE_NAME,"a");
    }

    if (SAF_FE_GF_parameters['S'])
      SAF_FE_GF_paused = 1;
  }
}

void _SAF_FE_GF_mainLoopIteration(void)
{
  if (!SAF_FE_GF_loop(SAF_FE_GF_parameters))
  {
    SAF_FE_GF_running = 0;
    return;
  }

  uint32_t time = 
#ifdef __EMSCRIPTEN__
  0;
#else
  SAF_FE_GF_sleep(0);  
#endif

#ifndef __EMSCRIPTEN__
  if (time >= SAF_FE_GF_nextFrameTime)
#endif
  {
    if (SAF_FE_GF_handleKey('t',0) == 1)
    {
      char fileName[64] = SAF_PROGRAM_NAME "_";

      char *c = fileName;
       
      while (*c != 0)
        c++;

      SAF_FE_GF_byteToHex((SAF_frame() / 256) % 256,c);
      c += 2;
      SAF_FE_GF_byteToHex(SAF_frame() % 256,c);
      c += 2;

      *c = '.'; c++; *c = 'p'; c++; *c = 'p'; c++; *c = 'm'; c++; *c = 0;

      SAF_FE_GF_saveScreenshot(fileName);
    }

    if (SAF_FE_GF_handleKey('i',1) == 1 && SAF_FE_GF_parameters['v'] < '8')
    {
      SAF_FE_GF_parameters['v']++;
      puts("volume +");
    }
    else if (SAF_FE_GF_handleKey('u',2) == 1 && SAF_FE_GF_parameters['v'] > '0')
    {
      SAF_FE_GF_parameters['v']--;
      puts("volume -");
    }

    if (SAF_FE_GF_handleKey('m',3) == 1 && SAF_FE_GF_parameters['b'] < '5')
    {
      SAF_FE_GF_parameters['b']++;
      puts("speed +");
    }
    else if (SAF_FE_GF_handleKey('n',4) == 1 && SAF_FE_GF_parameters['b'] > '1')
    {
      SAF_FE_GF_parameters['b']--;
      puts("speed -");
    }

    if (SAF_FE_GF_handleKey('p',5) == 1)
    {
      SAF_FE_GF_paused = !SAF_FE_GF_paused;
      puts(SAF_FE_GF_paused ? "paused" : "resumed");
    }

    uint16_t mult = 2;

    for (uint8_t i = 0; i < '5' - SAF_FE_GF_parameters['b']; ++i)
      mult *= 2;

    uint8_t frames = 0;
    uint8_t rewind = 0;

#ifndef __EMSCRIPTEN__
    while (time >= SAF_FE_GF_nextFrameTime || rewind)
#endif
    {
      rewind = SAF_FE_GF_parameters['P'] && 
        SAF_FE_GF_demoNextFrame != 0xffffffff;

      if (!SAF_FE_GF_paused)
      {
#ifndef __EMSCRIPTEN__
        SAF_FE_GF_frameTimes -= SAF_FE_GF_sleep(0);
#endif
        // demo recording/playing:

        if (SAF_FE_GF_parameters['r'] && !SAF_FE_GF_parameters['p'])
        {
          uint8_t previousButtonStates = SAF_FE_GF_buttonStates;

          SAF_FE_GF_buttonStates = 
            ((SAF_buttonPressed(SAF_BUTTON_UP) != 0)) |
            ((SAF_buttonPressed(SAF_BUTTON_RIGHT) != 0) << 1) |
            ((SAF_buttonPressed(SAF_BUTTON_DOWN) != 0) << 2) |
            ((SAF_buttonPressed(SAF_BUTTON_LEFT) != 0) << 3) |
            ((SAF_buttonPressed(SAF_BUTTON_A) != 0) << 4) |
            ((SAF_buttonPressed(SAF_BUTTON_B) != 0) << 5) |
            ((SAF_buttonPressed(SAF_BUTTON_C) != 0) << 6);

          if (SAF_FE_GF_buttonStates != previousButtonStates)
          {
            char s[3];

            uint32_t f = SAF_frame() - 1;

            fprintf(SAF_FE_GF_recordFile,"%s",SAF_FE_GF_byteToHex((f >> 24) & 0xff,s));
            fprintf(SAF_FE_GF_recordFile,"%s",SAF_FE_GF_byteToHex((f >> 16) & 0xff,s));
            fprintf(SAF_FE_GF_recordFile,"%s",SAF_FE_GF_byteToHex((f >> 8) & 0xff,s));
            fprintf(SAF_FE_GF_recordFile,"%s ",SAF_FE_GF_byteToHex(f & 0xff,s));
            fprintf(SAF_FE_GF_recordFile,"%s\n",SAF_FE_GF_byteToHex(SAF_FE_GF_buttonStates,s));
          }
        }
        else if (SAF_FE_GF_parameters['p'] && SAF_frame() >= SAF_FE_GF_demoNextFrame)
        {
          readNextDemoRecord();
        } // demo handling

        SAF_FE_GF_running = SAF_FE_loop();

#ifndef __EMSCRIPTEN__
        SAF_FE_GF_frameTimes += SAF_FE_GF_sleep(0);
#endif

        if (SAF_FE_GF_parameters['d'] && SAF_frame() % 64 == 0)
        {
          char debugStr[] = "frame         us/frame         ";
          SAF_intToStr(SAF_frame(),debugStr + 6)[0] = ' ';
          SAF_intToStr((SAF_FE_GF_frameTimes * 1000) / 64,debugStr + 23);
          puts(debugStr);
          SAF_FE_GF_frameTimes = 0;
        }
      } // if (!paused)

      if (!rewind)
        SAF_FE_GF_nextFrameTime += (SAF_MS_PER_FRAME * mult) / 8;

      frames++;
    }

    if (SAF_FE_GF_parameters['d'] && frames > 1)
    {
      char debugStr[] = "skipped x frames";
      debugStr[8] = '0' + frames - 1;
      puts(debugStr);
    }

    SAF_FE_GF_present(SAF_FE_GF_screen);
  } // if (time > nextFrameTime)

#ifndef __EMSCRIPTEN__
  SAF_FE_GF_sleep((SAF_FE_GF_nextFrameTime - time) * 3 / 4); // relieve CPU
#endif

  if (SAF_FE_GF_keyPressed('E'))
    SAF_FE_GF_running = 0;
}

#ifdef __EMSCRIPTEN__
typedef void (*em_callback_func)(void);
void emscripten_set_main_loop(
       em_callback_func func, int fps, int simulate_infinite_loop);
#endif

int main(int argc, char **argv)
{
  SAF_FE_paramParse(argc,argv,SAF_FE_GF_parameters);

  if (SAF_FE_GF_parameters['h'])
  {
    SAF_FE_GF_printHelp();
    return 0;
  }

  if (SAF_FE_GF_parameters['s'] < '0' || SAF_FE_GF_parameters['s'] > '8')
    SAF_FE_GF_parameters['s'] = '4';

  if (SAF_FE_GF_parameters['v'] < '0' || SAF_FE_GF_parameters['v'] > '8')
    SAF_FE_GF_parameters['v'] = '4';

  if (SAF_FE_GF_parameters['b'] < '1' || SAF_FE_GF_parameters['b'] > '5')
    SAF_FE_GF_parameters['b'] = '3';

  if (SAF_FE_GF_parameters['l'])
  {
    SAF_FE_GF_parameters['P'] = 1;
    SAF_FE_GF_parameters['r'] = 1;
  }

  if (SAF_FE_GF_parameters['P'])
    SAF_FE_GF_parameters['p'] = 1; // automatically turn this on as well

  if (SAF_FE_GF_parameters['S'] && !SAF_FE_GF_parameters['p'])
    SAF_FE_GF_paused = 1;

  for (uint16_t i = 0; i < SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT; ++i)
    SAF_FE_GF_screen[i] = 0;

  puts("starting " SAF_PROGRAM_NAME);
  puts("run with -h for help");

  if (SAF_FE_GF_parameters['d'])
    puts("initializing frontend");

  SAF_FE_GF_init(SAF_FE_GF_parameters); 

  if (SAF_FE_GF_parameters['d'])
    puts("initializing client program");

  SAF_FE_init();

  if (SAF_FE_GF_parameters['p'])
  {
    SAF_FE_GF_recordFile = fopen(SAF_FE_GF_SAVE_FILE_NAME,"r"); 

    if (!SAF_FE_GF_recordFile)
    {
      puts("couldn't open demo file for reading");
      SAF_FE_GF_parameters['p'] = 0;
      SAF_FE_GF_parameters['P'] = 0;
    }
    else
      readNextDemoRecord();
  }

  if (!SAF_FE_GF_parameters['p'] && SAF_FE_GF_parameters['r'])
  {
    SAF_FE_GF_recordFile = fopen(SAF_FE_GF_SAVE_FILE_NAME,"w"); 

    if (!SAF_FE_GF_recordFile)
    {
      puts("couldn't open demo file for writing");
      SAF_FE_GF_parameters['r'] = 0;
    }
  }

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(_SAF_FE_GF_mainLoopIteration,SAF_FPS,1);
#else
  while (SAF_FE_GF_running) // main loop
    _SAF_FE_GF_mainLoopIteration();
#endif

  if (SAF_FE_GF_recordFile)
    fclose(SAF_FE_GF_recordFile);

  printf("ending %s\n",SAF_PROGRAM_NAME);

  SAF_FE_GF_end();

  return 0;
}

const uint8_t *SAF_FE_GF_getScreenPointer()
{
  return SAF_FE_GF_screen;
}

const char *SAF_FE_extension(const char *string)
{
  return SAF_FE_GF_extension(string); 
}
#endif // SAF_FE_GENERIC_FRONTEND

#ifdef SAF_FE_STDIO_SAVE_LOAD

#if SAF_SETTING_ENABLE_SAVES

#include <stdio.h>

uint8_t _SAF_FE_saveMemory[SAF_SAVE_SIZE];
uint8_t _SAF_FE_saveMemoryLoaded = 0;

#ifdef __EMSCRIPTEN__
char *emscripten_run_script_string(const char *script);
int emscripten_run_script_int(const char *script);
void emscripten_run_script(const char *script);

char emscriptenCookie[17];
#endif

void _SAF_FE_loadSaveMemory()
{
#ifdef __EMSCRIPTEN__
  // generate the cookie name

  char *progName = SAF_PROGRAM_NAME;

  emscriptenCookie[0] = 'S';
  emscriptenCookie[1] = 'A';
  emscriptenCookie[2] = 'F';

  for (uint8_t i = 3; i < 16; ++i)
    emscriptenCookie[i] = 'x';

  uint8_t p = 0;

  while (progName[p] != 0 && p < 16 - 3)
  {
    emscriptenCookie[p + 3] = progName[p];
    p++;
  }

  emscriptenCookie[16] = 0; 

  char script[] = "document.cookie.search('................=')";

  for (uint8_t i = 0; i < 16; ++i)
    script[24 + i] = emscriptenCookie[i];

  int cookieIndex = emscripten_run_script_int(script);

  if (cookieIndex >= 0)
  {
    char *cookie = emscripten_run_script_string("document.cookie")
      + cookieIndex + 17;

    for (uint8_t i = 0; i < SAF_SAVE_SIZE; ++i)
      _SAF_FE_saveMemory[i] = (cookie[2 * i] - 'a') * 16 + (cookie[2 * i + 1] - 'a');
  }
  else
    for (uint16_t i = 0; i < SAF_SAVE_SIZE; ++i)
      _SAF_FE_saveMemory[i] = 0;

#else
  FILE *f = fopen(SAF_PROGRAM_NAME ".sav","rb");

  if (f)
  {
    fread(_SAF_FE_saveMemory,SAF_SAVE_SIZE,1,f);
    fclose(f);
  }
  else
    for (uint16_t i = 0; i < SAF_SAVE_SIZE; ++i)
      _SAF_FE_saveMemory[i] = 0;
#endif // emscripten
  
  _SAF_FE_saveMemoryLoaded = 1;
}
#endif // if SAF_SETTING_ENABLE_SAVES

void SAF_FE_save(uint8_t index, uint8_t data)
{
#if SAF_SETTING_ENABLE_SAVES
  if (!_SAF_FE_saveMemoryLoaded)
    _SAF_FE_loadSaveMemory();

  _SAF_FE_saveMemory[index] = data;

#ifdef __EMSCRIPTEN__
  char str[] = "document.cookie = '................="
    "                                                '";

  for (uint8_t i = 0; i < 16; ++i)
    str[19 + i] = emscriptenCookie[i];

  uint8_t p = 36;

  for (uint8_t i = 0; i < SAF_SAVE_SIZE; ++i)
  {
    char c1 = 'a' + _SAF_FE_saveMemory[i] / 16;
    char c2 = 'a' + _SAF_FE_saveMemory[i] % 16;

    str[p] = c1;
    str[p + 1] = c2;

    p += 2;
  }

  str[p] = '\'';
  str[p + 1] = ';';
  str[p + 2] = 0;

  emscripten_run_script(str);
#else
  FILE *f = fopen(SAF_PROGRAM_NAME ".sav","wb");
  fwrite(_SAF_FE_saveMemory,SAF_SAVE_SIZE,1,f);
  fclose(f);
#endif // emscripten
#else
  _SAF_UNUSED(index);
  _SAF_UNUSED(data);
#endif
}

uint8_t SAF_FE_load(uint8_t index)
{
#if SAF_SETTING_ENABLE_SAVES
  if (!_SAF_FE_saveMemoryLoaded)
    _SAF_FE_loadSaveMemory(); 

  return _SAF_FE_saveMemory[index];
#else
  return 0;
#endif
}

#endif // ifded SAF_FE_STDIO_SAVE_LOAD

void SAF_FE_paramParse(int argc, char **argv, uint8_t paramValues[128])
{
  for (uint8_t i = 0; i < 128; ++i)
    paramValues[i] = 0;
 
  for (uint16_t i = 0; i < argc; ++i)
  {
    const char *a = *argv; 

    if (*a == '-')
    {
      a++;

      if (a != 0)
      {
        if (a[1] == 0) // -p
          paramValues[(uint8_t) a[0]] = 1;
        else if (a[2] == 0) //-pX
          paramValues[(uint8_t) a[0]] = a[1];
      }
    }

    argv++;
  }
}

#if SAF_SETTING_FORCE_1BIT
  #undef SAF_PLATFORM_COLOR_COUNT
  #define SAF_PLATFORM_COLOR_COUNT 2
#endif

#if SAF_PLATFORM_HARWARD
  #include <avr/pgmspace.h>
  #define _SAF_CONST PROGMEM const
  #define _SAF_READ_CONST(addr) ((uint8_t) pgm_read_byte(addr))
#else
  #define _SAF_CONST static const
  #define _SAF_READ_CONST(addr) *(addr)
#endif

uint32_t _SAF_frame = 0;
uint8_t _SAF_currentRandom = 0;
uint8_t _SAF_buttonStates[SAF_BUTTONS] = {0,0,0,0,0,0,0};
uint8_t _SAF_saveMemory[SAF_SAVE_SIZE] = 
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t _SAF_saveMemoryLoaded = 0;

_SAF_CONST uint8_t _SAF_font[] =
{
  0x00,0x00, // 32 ' '
  0x22,0x20, // 33 '!'
  0x55,0x00, // 34 '"'
  0xea,0x57, // 35 '#'
  0x36,0x36, // 36 '$'
  0x49,0x92, // 37 '%'
  0x52,0xb6, // 38 '&'
  0x22,0x00, // 39 '''
  0x24,0x42, // 40 '('
  0x42,0x24, // 41 ')'
  0x25,0x05, // 42 '*'
  0x20,0x27, // 43 '+'
  0x00,0x22, // 44 ','
  0x00,0x07, // 45 '-'
  0x00,0x20, // 46 '.'
  0x24,0x12, // 47 '/'
  0x57,0x75, // 48 '0'
  0x46,0xe4, // 49 '1'
  0x67,0x71, // 50 '2'
  0x27,0x34, // 51 '3'
  0x55,0x47, // 52 '4'
  0x17,0x76, // 53 '5'
  0x17,0x77, // 54 '6'
  0x47,0x22, // 55 '7'
  0x77,0x75, // 56 '8'
  0x77,0x74, // 57 '9'
  0x20,0x20, // 58 ':'
  0x02,0x22, // 59 ';'
  0x40,0x42, // 60 '<'
  0x70,0x70, // 61 '='
  0x20,0x24, // 62 '>'
  0x96,0x44, // 63 '?'
  0xde,0x61, // 64 '@'
  0x57,0x57, // 65 'A'
  0x73,0x75, // 66 'B'
  0x16,0x61, // 67 'C'
  0x53,0x35, // 68 'D'
  0x37,0x71, // 69 'E'
  0x17,0x13, // 70 'F'
  0x16,0x65, // 71 'G'
  0x75,0x55, // 72 'H'
  0x27,0x72, // 73 'I'
  0x44,0x75, // 74 'J'
  0x35,0x55, // 75 'K'
  0x11,0x71, // 76 'L'
  0xfb,0x99, // 77 'M'
  0xb9,0x9d, // 78 'N'
  0x96,0x69, // 79 'O'
  0x57,0x17, // 80 'P'
  0x96,0xed, // 81 'Q'
  0x57,0x53, // 82 'R'
  0x36,0x34, // 83 'S'
  0x27,0x22, // 84 'T'
  0x55,0x75, // 85 'U'
  0x55,0x25, // 86 'V'
  0x99,0xbf, // 87 'W'
  0x25,0x52, // 88 'X'
  0x75,0x22, // 89 'Y'
  0x27,0x71, // 90 'Z'
  0x26,0x62, // 91 '['
  0x21,0x42, // 92 '\'
  0x46,0x64, // 93 ']'
  0x52,0x00, // 94 '^'
  0x00,0x70, // 95 '_'
  0x42,0x00, // 96 '`'
  0x60,0x75, // 97 'a'
  0x31,0x35, // 98 'b'
  0x60,0x61, // 99 'c'
  0x64,0x65, // 100 'd'
  0xd6,0x63, // 101 'e'
  0x26,0x27, // 102 'f'
  0x76,0x34, // 103 'g'
  0x31,0x55, // 104 'h'
  0x02,0x22, // 105 'i'
  0x04,0x64, // 106 'j'
  0x51,0x53, // 107 'k'
  0x22,0x42, // 108 'l'
  0xf0,0x9d, // 109 'm'
  0x30,0x55, // 110 'n'
  0x70,0x75, // 111 'o'
  0x70,0x17, // 112 'p'
  0x70,0x47, // 113 'q'
  0x60,0x22, // 114 'r'
  0x60,0x32, // 115 's'
  0x72,0x62, // 116 't'
  0x50,0x75, // 117 'u'
  0x50,0x25, // 118 'v'
  0x90,0xfb, // 119 'w'
  0x90,0x96, // 120 'x'
  0xa0,0x36, // 121 'y'
  0x30,0x62, // 122 'z'
  0x36,0x62, // 123 '{'
  0x22,0x22, // 124 '|'
  0x63,0x32, // 125 '}'
  0xa0,0x05, // 126 '~'
  0x00,0x00  // 127 ' '
};

_SAF_CONST int8_t _SAF_cosTable[64] =
{
  127,127,127,127,127,127,126,126,125,124,124,123,122,121,120,119,118,117,115,
  114,112,111,109,108,106,104,102,100,98,96,94,92,90,88,85,83,81,78,76,73,71,68,
  65,63,60,57,54,51,48,46,43,40,37,34,31,28,24,21,18,15,12,9,6,3
};

int8_t SAF_FE_getSoundSample(uint8_t sound, uint16_t sampleNumber)
{
  switch (sound)
  {
    case SAF_SOUND_BEEP:
      return SAF_sin(sampleNumber * 16);
      break;

    case SAF_SOUND_CLICK:
      return (sampleNumber / 2) >> + (sampleNumber & 0x02);
      break;

    case SAF_SOUND_BOOM: 
      return sampleNumber * 2;
      break;

    case SAF_SOUND_BUMP:
      return ((sampleNumber >> 3) + sampleNumber) ^ 0x24;
      break;

    default:
      return 0;
      break;
  }
}

void SAF_FE_scale2xScreen(
  const uint8_t screen[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT],
  uint8_t result[SAF_SCREEN_WIDTH * SAF_SCREEN_HEIGHT * 4])
{
  /* Here we try to optimize by handling the border cases separately which
  should considerably reduce amount of branching per pixel (but code size will
  be greater). */

  uint8_t p[4] = {0,0,0,0};

  const uint8_t *sCurr = screen;
  const uint8_t *sBott = sCurr + SAF_SCREEN_WIDTH;
  const uint8_t *sTop = sCurr - SAF_SCREEN_WIDTH;

  uint8_t *rCurr = result;
  uint8_t *rBott = rCurr + SAF_SCREEN_WIDTH * 2;

  #define step(t,r,b,l)\
    {SAF_FE_scale2xPixel(*sCurr,t,r,b,l,p);\
     *rCurr = p[0]; rCurr++; *rCurr = p[1]; rCurr++;\
     *rBott = p[2]; rBott++; *rBott = p[3]; rBott++;\
     sCurr++; sBott++; sTop++;}
   
  #define nextLine\
    {rCurr += SAF_SCREEN_WIDTH * 2;\
     rBott += SAF_SCREEN_WIDTH * 2;}

  #define T *sTop
  #define R *(sCurr + 1)
  #define B *sBott
  #define L *(sCurr - 1)

  step(0,R,B,0) // first pixel (top-left)

  // first row from second to second to last (top edge)
  for (uint8_t i = 1; i < SAF_SCREEN_WIDTH - 1; ++i)
    step(0,R,B,L)

  step(0,0,B,L) // lest pixel of first row (top-right)
  nextLine

  // rows from second to second to last
  for (uint8_t j = 1; j < SAF_SCREEN_HEIGHT - 1; ++j)
  {
    step(T,R,B,0) // first pixel of the row (left edge)

    // middle pixels (not touching any edge)
    for (uint8_t i = 1; i < SAF_SCREEN_WIDTH - 1; ++i)
      step(T,R,B,L)

    step(T,0,B,L) // last pixel of the row (right edge)
    nextLine
  }

  step(T,R,0,0) // first pixel of the last row (top-bottom)

  // last row from second to second to last (bottom edge)
  for (uint8_t i = 1; i < SAF_SCREEN_WIDTH - 1; ++i)
    step(T,R,0,L)

  step(T,0,0,L) // last pixel (bottom-right)
  nextLine

  #undef step
  #undef nextLine
  #undef T
  #undef R
  #undef B
  #undef L
}

void SAF_FE_scale2xPixel(uint8_t middle, uint8_t top, uint8_t right,
  uint8_t bottom, uint8_t left, uint8_t result[4])
{
  uint8_t rightBottom = right == bottom;

  if (top == left)
  {
    result[1] = middle;
    result[2] = middle;
    result[0] = (bottom == left || top == right) ? middle : top;
  }
  else
  {
    result[0] = middle;

    if (rightBottom)
    {
      result[1] = middle;
      result[2] = middle;
    }
    else
    {
      result[1] = (top != right) ? middle : right;
      result[2] = (bottom != left) ? middle : left;
    }
  }

  result[3] = (!rightBottom || top == right || bottom == left) ? 
    middle : bottom;
}

int8_t SAF_cos(uint8_t phase)
{
  uint8_t index = phase % 64;

  uint8_t part = phase / 64;

  if (part % 2)
    index = 63 - index;

  int8_t result = _SAF_READ_CONST(_SAF_cosTable + index);

  if (part == 1 || part == 2)
    result *= -1;

  return result;
}

int8_t SAF_sin(uint8_t phase)
{
  return SAF_cos(phase - 64);
}

void SAF_getFontCharacter(uint8_t asciiIndex, uint8_t result[2])
{
  asciiIndex = (asciiIndex >= ' ' && asciiIndex < '~') ? asciiIndex : ' ';
  asciiIndex = (asciiIndex - ' ') * 2;

  result[0] = _SAF_READ_CONST(_SAF_font + asciiIndex);
  result[1] = _SAF_READ_CONST(_SAF_font + asciiIndex + 1);
}

void _SAF_preprocessPosSize(int8_t *x, int8_t *y, int8_t *w, int8_t *h)
{
#define p(s,c)\
  if (*s >= 0)\
  {\
    if (*c + *s < *c) /* overflow? */\
      *s = 127 - *s;\
  }\
  else\
  {\
    if (*c + *s > *c)\
      *c = -1 * (*c + 128);\
    *c += *s;\
    *s *= -1;\
  }
 
  p(w,x)
  p(h,y)

#undef p
}

void SAF_drawRect(int8_t x, int8_t y, int8_t width, int8_t height, uint8_t color, uint8_t filled)
{
  if (width == 0 || height == 0)
    return;

  _SAF_preprocessPosSize(&x,&y,&width,&height);

  int8_t x2 = x + width;

  if (filled)
  {
    while (height > 0)
    {
      int8_t x2 = x;

      for (uint8_t i = 0; i < width; ++i)
      {
        SAF_drawPixel(x2,y,color);
        x2++;
      }

      height--;
      y++;
    }
  }
  else
  {
    int8_t y2 = y;

    while (height > 0)
    {
      SAF_drawPixel(x,y,color);
      SAF_drawPixel(x2 - 1,y,color);
 
      height--;
      y++;
    }

    y--;

    while (width > 0)
    {
      SAF_drawPixel(x,y,color);
      SAF_drawPixel(x,y2,color);

      width--;
      x++;
    }
  }
}

int8_t SAF_drawText(const char *text, int8_t x, int8_t y, uint8_t color, uint8_t size)
{
  if (size == 0)
    return x;

  int8_t originalX = x;

  while (1) // for each string character
  {
    char c = *text;

    if (c == 0)
      break;

    uint8_t character[2];

    uint8_t width = 4 * size;

    if (c == '\n')
    {
      x = originalX;
      y += 5;
      text++;
      continue;
    }

    SAF_getFontCharacter(c,character);
      
    for (int8_t i = 0; i < 2; ++i) // for both bytes
    {
      uint8_t byte = character[i];

      for (uint8_t j = 0; j < 8; ++j)
      {
        if (byte & 0x01)
        {
          int8_t x2, y2 = y; /* we have to iterate this way sa to prevent
                                infinite for loops */

          for (int8_t k = 0; k < size; ++k)
          {
            x2 = x;

            for (int8_t l = 0; l < size; ++l)
            {
              SAF_drawPixel(x2,y2,color);
              x2++;
            }
 
            y2++;
          }
        }

        byte >>= 1;
        x += size;

        if (j == 3 || j == 7)
        {
          x -= width;
          y += size;
        }
      }
    }

    x += 6 * size;
    y -= width;

    text++;
  }

  return x;
}

void SAF_drawLine(int8_t x1, int8_t y1, int8_t x2, int8_t y2, uint8_t color)
{
  if (x1 > x2)
  {
    uint8_t tmp = x1;
    x1 = x2;
    x2 = tmp;

    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  uint8_t dx = x2 - x1;
  uint8_t dy;

  int8_t *drawX = &x1;
  int8_t *drawY = &y1;

  int8_t stepX = 1;
  int8_t stepY = 1;
  uint8_t length = dx;
  uint8_t add;
  uint8_t compare = dx;

  if (y2 > y1)
  {
    dy = y2 - y1;
    add = dy;

    if (dy > dx)
    {
      drawX = &y1;
      drawY = &x1;
      length = dy;
      add = dx;
      compare = dy;
    }
  }
  else
  {
    dy = y1 - y2;
    add = dy;

    if (dy < dx)
    {
      stepY = -1;
    }
    else
    {
      drawX = &y1;
      drawY = &x1;
      length = dy;
      stepX = -1;
      add = dx;
      compare = dy;
    }
  }
  
  uint8_t accumulator = compare / 2;

  length++;

  while (length > 0)
  {
    SAF_drawPixel(x1,y1,color);
    *drawX += stepX;
    length--;

    accumulator += add;

    if (accumulator >= compare)
    {
      accumulator -= compare;
      *drawY += stepY;
    }
  }  
}

uint32_t SAF_frame(void)
{
  return _SAF_frame;
}

uint32_t SAF_time(void)
{
  return _SAF_frame * SAF_MS_PER_FRAME;
}

void SAF_FE_init(void)
{
  SAF_init();
}

uint8_t SAF_FE_loop(void)
{
  for (uint8_t i = 0; i < SAF_BUTTONS; ++i)
  {
    uint8_t *b = _SAF_buttonStates + i;
    uint8_t state = *b;
    *b = SAF_FE_buttonPressed(i) ? (state < 255 ? state + 1 : state) : 0;
  }

  uint8_t result = SAF_loop();
  _SAF_frame++;
  return result;
}

static inline void SAF_clearScreen(uint8_t color)
{
  SAF_drawRect(0,0,SAF_SCREEN_WIDTH,SAF_SCREEN_HEIGHT,color,1);
}

uint8_t SAF_random()
{
  /* We reorder each sequence of 8 values to give some variety to specific bit
     patterns. */
  uint8_t remap[8] = {5,7,2,0,3,6,4,1};

  _SAF_currentRandom *= 13;
  _SAF_currentRandom += 7;
  
  uint8_t remainder = _SAF_currentRandom % 8;

  return _SAF_currentRandom - remainder + remap[remainder];
}

uint8_t SAF_buttonJustPressed(uint8_t button)
{
  return SAF_buttonPressed(button) == 1;
}

uint8_t SAF_buttonPressed(uint8_t button)
{
  return (button < SAF_BUTTONS) ? _SAF_buttonStates[button] : 0;
}

uint8_t SAF_colorFromRGB(uint8_t red, uint8_t green, uint8_t blue)
{
  return SAF_COLOR_RGB(red,green,blue);
}

void SAF_colorToRGB(uint8_t colorIndex, uint8_t *red, uint8_t *green, uint8_t *blue)
{
  uint8_t value = (colorIndex >> 5) & 0x07;
  *red = value != 7 ? value * 36 : 255;

  value = (colorIndex >> 2) & 0x07;
  *green = value != 7 ? value * 36 : 255;

  value = colorIndex & 0x03;
  *blue = (value != 3) ? value * 72 : 255;
}

uint8_t SAF_colorInvert(uint8_t color)
{
  return ~color;
}

uint16_t SAF_sqrt(uint32_t number)
{
  uint32_t result = 0;
  uint32_t a = number;
  uint32_t b = 1u << 30;

  while (b > a)
    b >>= 2;

  while (b != 0)
  {
    if (a >= result + b)
    {
      a -= result + b;
      result = result +  2 * b;
    }

    b >>= 2;
    result >>= 1;
  }

  return result;
}

#define _SAF_IMAGE_MODE_NORMAL 0
#define _SAF_IMAGE_MODE_COMPRESSED 1
#define _SAF_IMAGE_MODE_1BIT 2

struct 
{
  uint8_t mode;
  const uint8_t *image;
  uint8_t transparentColor;
  const uint8_t *binaryMask;
  uint8_t binaryLine;
  uint8_t binaryMaskLine;
  uint8_t binaryPosition;
  uint8_t binaryColor1;
  uint8_t binaryColor2;
  uint8_t rleCount;
  uint8_t rleLastColor;
  const uint8_t *palette;
} _SAF_drawnImage;

uint8_t _SAF_getNextImagePixel()
{
  uint8_t result = 0;

  switch (_SAF_drawnImage.mode)
  {
    case _SAF_IMAGE_MODE_NORMAL:
      result = *_SAF_drawnImage.image;
      _SAF_drawnImage.image++;
      break;

    case _SAF_IMAGE_MODE_1BIT:
      if (_SAF_drawnImage.binaryPosition == 0)
      {
        _SAF_drawnImage.binaryLine = *_SAF_drawnImage.image;
        _SAF_drawnImage.image++;
        _SAF_drawnImage.binaryPosition = 8;

        if (_SAF_drawnImage.binaryMask != 0)
        {
          _SAF_drawnImage.binaryMaskLine = ~(*_SAF_drawnImage.binaryMask);
          /* We negate the mask because we want 1 to mean transparency; this
          allows to avoid an if-check before mask line shift later on. */

          _SAF_drawnImage.binaryMask++;
        }
      }
      
      result = ((_SAF_drawnImage.binaryMaskLine & 0x80) == 0) ?
        ( 
          (_SAF_drawnImage.binaryLine & 0x80) ? 
          _SAF_drawnImage.binaryColor1 : _SAF_drawnImage.binaryColor2
        ) : _SAF_drawnImage.transparentColor;

      _SAF_drawnImage.binaryPosition--;
      _SAF_drawnImage.binaryLine <<= 1;
      _SAF_drawnImage.binaryMaskLine <<= 1;

      break;

    case _SAF_IMAGE_MODE_COMPRESSED:
      if (_SAF_drawnImage.rleCount == 0)
      {
        uint8_t b = *_SAF_drawnImage.image;
        _SAF_drawnImage.rleLastColor = _SAF_drawnImage.palette[b & 0x0f];
        _SAF_drawnImage.rleCount = (b >> 4) + 1;
        _SAF_drawnImage.image++;
      }
 
      result = _SAF_drawnImage.rleLastColor;
      _SAF_drawnImage.rleCount--;

      break;

    default: break;
  }

  return result;
}

void _SAF_drawImageGeneral(int8_t x, int8_t y, uint8_t transform)
{
  int8_t stepX = 1;
  int8_t stepY = 1;

  uint8_t invert = transform & SAF_TRANSFORM_INVERT;

  uint8_t scale = 1;

  switch (transform & 0x18)
  {
    case SAF_TRANSFORM_SCALE_2: scale = 2; break;
    case SAF_TRANSFORM_SCALE_3: scale = 3; break;
    case SAF_TRANSFORM_SCALE_4: scale = 4; break;
    default: break;
  } 

  uint8_t width = *(_SAF_drawnImage.image);
  _SAF_drawnImage.image++;

  uint8_t h = *(_SAF_drawnImage.image);
  _SAF_drawnImage.image++;

  if (_SAF_drawnImage.mode == _SAF_IMAGE_MODE_COMPRESSED)
    _SAF_drawnImage.image += 16; // skip the palette

  uint8_t scaledWidth = (width - 1) * scale;
  uint8_t scaledHeight = (h - 1) * scale;

  int8_t *drawX = &x;
  int8_t *drawY = &y;

  switch (transform & 0x07)
  {
    case SAF_TRANSFORM_ROTATE_90:
      drawY = &x; drawX = &y; stepY = -1; x += scaledHeight; break;

    case SAF_TRANSFORM_ROTATE_180:
      stepX = -1; stepY = -1; x += scaledWidth; y += scaledHeight; break;

    case SAF_TRANSFORM_ROTATE_270:
      drawY = &x; drawX = &y; stepX = -1; y += scaledWidth; break;

    case SAF_TRANSFORM_FLIP:
      stepX = -1; x += scaledWidth; break;

    case (SAF_TRANSFORM_ROTATE_90 | SAF_TRANSFORM_FLIP):
      drawY = &x; drawX = &y; stepX = -1; stepY = -1; x += scaledHeight; y += scaledWidth; break;

    case (SAF_TRANSFORM_ROTATE_180 | SAF_TRANSFORM_FLIP):
      stepY = -1; y += scaledHeight; break;

    case (SAF_TRANSFORM_ROTATE_270 | SAF_TRANSFORM_FLIP):
      drawY = &x; drawX = &y; break;
  
    default: break;
  }

  stepX *= scale;
  stepY *= scale;

  int8_t lineBack = -1 * stepX * width;

  while (h > 0)
  {
    uint8_t w = width;

    while (w > 0)
    {
      uint8_t pixel = _SAF_getNextImagePixel();

      if (pixel != _SAF_drawnImage.transparentColor)
        for (int8_t x2 = x; x2 < x + scale; ++x2)
          for (int8_t y2 = y; y2 < y + scale; ++y2)
            SAF_drawPixel(x2,y2,invert ? ~(pixel) : pixel);

      *drawX += stepX;
      w--;
    }

    *drawX += lineBack;

    h--;
    *drawY += stepY;
  }
}

/** Performs the most common cases of image drawing faster than the general
  function. */
void _SAF_drawImageFast(const uint8_t *image, int8_t x, int8_t y, uint8_t flip,
  uint8_t transparentColor, uint8_t is1Bit, const uint8_t *mask, uint8_t color1,
  uint8_t color2)
{
  uint8_t width = *image;
  image++;

  uint8_t height = *image;
  image ++;

  int8_t x0 = x;
  int8_t xPlus = 1;

  if (flip)
  {
    x0 = x + width - 1;
    xPlus = -1;
  }

  #define loopStart\
    while (height != 0) {\
      x = x0;\
      for (uint8_t w = width; w != 0; --w, x += xPlus) {

  #define loopEnd\
    }\
    height--;\
    y++; }

   if (is1Bit)
   {
     uint8_t bitCount = 0, imgLine = 0, maskLine = 0;

     mask += mask != 0 ? 2 : 0; // skip width and height

     loopStart
       if (bitCount == 0)
       {
         imgLine = *image;
         bitCount = 8;
         image++;

         if (mask != 0)
         {
           maskLine = ~(*mask); // negation helps avoid branching later
           mask++;
         }
       }

       if ((maskLine & 0x80) == 0)
         SAF_drawPixel(x,y,(imgLine & 0x80) ? color1 : color2);

       bitCount--;
       imgLine <<= 1;
       maskLine <<= 1;
     loopEnd
   }
   else
   {
     loopStart
        uint8_t color = *image;

        if (color != transparentColor)
          SAF_drawPixel(x,y,*image);

        image++;
     loopEnd
   }

  #undef loopStart
  #undef loopEnd
}

void SAF_drawImage(const uint8_t *image, int8_t x, int8_t y, uint8_t transform,
  uint8_t transparentColor)
{
  if ((transform & ~SAF_TRANSFORM_FLIP) == 0)
  {
    _SAF_drawImageFast(image,x,y,transform & SAF_TRANSFORM_FLIP,
      transparentColor,0,0,0,0);
    return;
  }

  _SAF_drawnImage.image = image;
  _SAF_drawnImage.mode = _SAF_IMAGE_MODE_NORMAL;
  _SAF_drawnImage.transparentColor = transparentColor;
  _SAF_drawImageGeneral(x,y,transform);
}

void SAF_drawImageCompressed(const uint8_t *image, int8_t x, int8_t y,
  uint8_t transform, uint8_t transparentColor)
{
  _SAF_drawnImage.image = image;
  _SAF_drawnImage.mode = _SAF_IMAGE_MODE_COMPRESSED;
  _SAF_drawnImage.transparentColor = transparentColor;
  _SAF_drawnImage.rleCount = 0;
  _SAF_drawnImage.rleLastColor = 0;
  _SAF_drawnImage.palette = image + 2;
  _SAF_drawImageGeneral(x,y,transform);
}

void SAF_drawImage1Bit(const uint8_t *image, int8_t x, int8_t y,
  const uint8_t *mask, uint8_t color1, uint8_t color2, uint8_t transform)
{
  if ((transform & ~SAF_TRANSFORM_FLIP) == 0)
  {
    _SAF_drawImageFast(image,x,y,transform & SAF_TRANSFORM_FLIP,0,1,mask,color1,
      color2);
    return;
  }

  _SAF_drawnImage.image = image;
  _SAF_drawnImage.binaryMask = (mask != 0) ? mask + 2 : 0; // skip width/height
  _SAF_drawnImage.binaryLine = 0;
  _SAF_drawnImage.binaryMaskLine = 0; // 0 will be negated
  _SAF_drawnImage.binaryPosition = 0;

  for (int i = 0; i < 3; ++i)
    if (i != color1 && i != color2)
    {
      _SAF_drawnImage.transparentColor = i;
      break;
    }

  _SAF_drawnImage.mode = _SAF_IMAGE_MODE_1BIT;
  _SAF_drawnImage.binaryColor1 = color1;
  _SAF_drawnImage.binaryColor2 = color2;
  _SAF_drawImageGeneral(x,y,transform);
}

void SAF_drawPixel(int8_t x, int8_t y, uint8_t color)
{
  if ((x & 0xc0) == 0 && (y & 0xc0) == 0)
#if SAF_PLATFORM_COLOR_COUNT <= 2
    SAF_FE_drawPixel(x,y,SAF_FE_colorTo1Bit(color,x,y) ?
      SAF_COLOR_WHITE : SAF_COLOR_BLACK);
#else
    SAF_FE_drawPixel(x,y,color);
#endif
}

void SAF_playSound(uint8_t sound)
{
#if SAF_SETTING_ENABLE_SOUND
  if (sound < SAF_SOUNDS)
    SAF_FE_playSound(sound);
#else
  _SAF_UNUSED(sound);
#endif
}

void _SAF_reloadSaveMemory()
{
  if (!_SAF_saveMemoryLoaded)
  {
    for (uint8_t i = 0; i < SAF_SAVE_SIZE; ++i)
      _SAF_saveMemory[i] = 
#if SAF_SETTING_ENABLE_SAVES
        SAF_FE_load(i);
#else
        0;
#endif

    _SAF_saveMemoryLoaded = 1;
  }
}

void SAF_save(uint8_t index, uint8_t data)
{
  if (index >= SAF_SAVE_SIZE)
    return;

  _SAF_reloadSaveMemory();

  if (_SAF_saveMemory[index] != data)
  {
    _SAF_saveMemory[index] = data;
#if SAF_SETTING_ENABLE_SAVES
    SAF_FE_save(index,data);
#endif
  }  
}

uint8_t SAF_load(uint8_t index)
{
  if (index >= SAF_SAVE_SIZE)
    return 0;

  _SAF_reloadSaveMemory();

  return _SAF_saveMemory[index];
}

void SAF_randomSeed(uint8_t seed)
{
  _SAF_currentRandom = seed;
}

char *SAF_intToStr(int32_t number, char *string)
{
  if (number == 0)
  {
    *string = '0';
    *(string + 1) = 0;
    return string;
  }

  char *start = string;

  if (number < 0)
  {
    *string = '-';
    string++;
    number *= -1;
    start++;
  }

  while (number > 0)
  {
    *string = '0' + number % 10;
    number /= 10;
    string++;
  }
  
  *string = 0; // terminate 

  char *end = string - 1;

  string = start;   

  while (end > start)
  {
    char tmp = *start;
    *start = *end;
    *end = tmp;

    start++;
    end--;
  }

  return string;
}

char *SAF_floatToStr(float number, char *string, uint8_t decimals)
{
  int32_t whole = number;

  SAF_intToStr(whole,string);

  if (decimals == 0)
  {
    *string = 0;
    return string;
  }

  char *c = string;

  while (*c != 0)
    c++;

  *c = '.';
  c++;

  if (number < 0)
  {
    number *= -1;
    whole *= -1;
  }

  if (decimals > 10)
    decimals = 10;

  int32_t factor = 1;

  while (decimals > 0)
  {
    factor *= 10;
    decimals--;
  }

  SAF_intToStr((number - whole) * factor,c);

  return string;
}

void SAF_drawCircle(int8_t x, int8_t y, uint8_t radius, uint8_t color, uint8_t filled)
{
  int8_t drawX = 0;
  int8_t drawY = radius;

  int16_t d = 3 - 2 * radius;

  if (!filled)
  {
    SAF_drawPixel(x,y + radius,color);
    SAF_drawPixel(x,y - radius,color);
    SAF_drawPixel(x + radius,y,color);
    SAF_drawPixel(x - radius,y,color);
  }
  else
    for (int8_t i = x - radius; i <= x + radius; ++i)
      SAF_drawPixel(i,y,color);

  while (drawY >= drawX)
  {
    if (d < 0)
    {
      d += 4 * drawX + 6;
    }
    else
    {
      d += 4 * (drawX - drawY) + 10;
      drawY--;
    }

    drawX++;

    int8_t xPlus  = x + drawX;
    int8_t xMinus = x - drawX;
    int8_t yPlus  = y + drawY;
    int8_t yMinus = y - drawY;
    int8_t x2Plus  = x + drawY;
    int8_t x2Minus = x - drawY;
    int8_t y2Plus  = y + drawX;
    int8_t y2Minus = y - drawX;

    if (!filled)
    {
      SAF_drawPixel(xPlus,yPlus,color);
      SAF_drawPixel(xPlus,yMinus,color);
      SAF_drawPixel(xMinus,yPlus,color);
      SAF_drawPixel(xMinus,yMinus,color);
      SAF_drawPixel(x2Plus,y2Plus,color);
      SAF_drawPixel(x2Plus,y2Minus,color);
      SAF_drawPixel(x2Minus,y2Plus,color);
      SAF_drawPixel(x2Minus,y2Minus,color);
    }
    else
    {
      for (int8_t i = xMinus; i <= xPlus; ++i)
      {
        SAF_drawPixel(i,yPlus,color);
        SAF_drawPixel(i,yMinus,color);
      }

      for (int8_t i = x2Minus; i <= x2Plus; ++i)
      {
        SAF_drawPixel(i,y2Plus,color);
        SAF_drawPixel(i,y2Minus,color);
      }
    }
  }
}

const char *SAF_extension(const char *string)
{
  return SAF_FE_extension(string);
}

uint8_t SAF_colorTo1Bit(uint8_t colorIndex)
{
  return
#if SAF_SETTING_FASTER_1BIT == 0
    // more accurate: 7 operations
    ((colorIndex & 0x03) +
    ((colorIndex >> 2) & 0x07) + 
    (colorIndex >> 5)) > 8;
#elif SAF_SETTING_FASTER_1BIT == 1
    // faster: 4 operations
    ((colorIndex >> 3) +
    (colorIndex & 0x1f)) >= ((0x1f + 0x1c) / 2);
#elif SAF_SETTING_FASTER_1BIT == 2
    // fastest: 1 operation
    colorIndex & 0x90;
#else
    // fastester: 0 operations
    colorIndex;
#endif
}

uint8_t SAF_colorToGrayscale(uint8_t colorIndex)
{
  uint8_t tmp = colorIndex >> 2;

  return
    (((colorIndex << 2) & 0x7f) + tmp + 
    ((colorIndex << 4) & 0x3f)) | tmp;
}

uint16_t SAF_FE_hashStr(const char *str)
{
  uint16_t r = 7621;
  
  while (*str != 0)
  {
    r = (r << 4) ^ (r + ((uint16_t) *str));
    str++;
  }
  
  return r;
}

uint8_t SAF_FE_colorTo1Bit(uint8_t color, uint8_t x, uint8_t y)
{
#if SAF_SETTING_1BIT_DITHER
  color = SAF_colorToGrayscale(color);

  if (color < 85)
    return 0;
  else if (color > 170)
    return 1;
  else
    return (x % 2) == (y % 2);
#else
  _SAF_UNUSED(x);
  _SAF_UNUSED(y);

  return SAF_colorTo1Bit(color);
#endif
}

#endif // guard
