//color constants                  red  grn  blu
#define LCD_BLACK      0x0000   //   0,   0,   0
#define LCD_BLUE       0x001F   //   0,   0, 255
#define LCD_DARKBLUE   0x34BF   //  50, 150, 255
#define LCD_RED        0xF800   // 255,   0,   0
#define LCD_GREEN      0x07E0   //   0, 255,   0
#define LCD_LIGHTGREEN 0x07EF   //   0, 255, 120
#define LCD_ORANGE     0xFD60   // 255, 175,   0
#define LCD_CYAN       0x07FF   //   0, 255, 255
#define LCD_MAGENTA    0xF81F   // 255,   0, 255
#define LCD_YELLOW     0xFFE0   // 255, 255,   0
#define LCD_WHITE      0xFFFF   // 255, 255, 255
#define LCD_GREY       0x8410   // 128, 128, 


// ------------BSP_LCD_Init------------
// Initialize the SPI and GPIO, which correspond with
// BoosterPack pins J1.7 (SPI CLK), J2.13 (SPI CS), J2.15
// (SPI MOSI), J2.17 (LCD ~RST), and J4.31 (LCD DC).
// Input: none
// Output: none
void BSP_LCD_Init(void);


//------------BSP_LCD_DrawPixel------------
// Color the pixel at the given coordinates with the given color.
// Requires 13 bytes of transmission
// Input: x     horizontal position of the pixel, columns from the left edge
//               must be less than 128
//               0 is on the left, 126 is near the right
//        y     vertical position of the pixel, rows from the top edge
//               must be less than 128
//               126 is near the wires, 0 is the side opposite the wires
//        color 16-bit color, which can be produced by BSP_LCD_Color565()
// Output: none
void BSP_LCD_DrawPixel(int16_t x, int16_t y, uint16_t color);


//------------BSP_LCD_DrawFastVLine------------
// Draw a vertical line at the given coordinates with the given height and color.
// A vertical line is parallel to the longer side of the rectangular display
// Requires (11 + 2*h) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        h     vertical height of the line
//        color 16-bit color, which can be produced by BSP_LCD_Color565()
// Output: none
void BSP_LCD_DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);


//------------BSP_LCD_DrawFastHLine------------
// Draw a horizontal line at the given coordinates with the given width and color.
// A horizontal line is parallel to the shorter side of the rectangular display
// Requires (11 + 2*w) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        w     horizontal width of the line
//        color 16-bit color, which can be produced by BSP_LCD_Color565()
// Output: none
void BSP_LCD_DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);


//------------BSP_LCD_FillScreen------------
// Fill the screen with the given color.
// Requires 33,293 bytes of transmission
// Input: color 16-bit color, which can be produced by BSP_LCD_Color565()
// Output: none
void BSP_LCD_FillScreen(uint16_t color);


//------------BSP_LCD_FillRect------------
// Draw a filled rectangle at the given coordinates with the given width, height, and color.
// Requires (11 + 2*w*h) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the top left corner of the rectangle, columns from the left edge
//        y     vertical position of the top left corner of the rectangle, rows from the top edge
//        w     horizontal width of the rectangle
//        h     vertical height of the rectangle
//        color 16-bit color, which can be produced by BSP_LCD_Color565()
// Output: none
void BSP_LCD_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);


//------------BSP_LCD_Color565------------
// Pass 8-bit (each) R,G,B and get back 16-bit packed color.
// Input: r red value
//        g green value
//        b blue value
// Output: 16-bit color
uint16_t BSP_LCD_Color565(uint8_t r, uint8_t g, uint8_t b);


//------------BSP_LCD_SwapColor------------
// Swaps the red and blue values of the given 16-bit packed color;
// green is unchanged.
// Input: x 16-bit color in format B, G, R
// Output: 16-bit color in format R, G, B
uint16_t BSP_LCD_SwapColor(uint16_t x);


//------------BSP_LCD_DrawBitmap------------
// Displays a 16-bit color BMP image.  A bitmap file that is created
// by a PC image processing program has a header and may be padded
// with dummy columns so the data have four byte alignment.  This
// function assumes that all of that has been stripped out, and the
// array image[] has one 16-bit halfword for each pixel to be
// displayed on the screen (encoded in reverse order, which is
// standard for bitmap files).  An array can be created in this
// format from a 24-bit-per-pixel .bmp file using the associated
// converter program.
// (x,y) is the screen location of the lower left corner of BMP image
// Requires (11 + 2*w*h) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the bottom left corner of the image, columns from the left edge
//        y     vertical position of the bottom left corner of the image, rows from the top edge
//        image pointer to a 16-bit color BMP image
//        w     number of pixels wide
//        h     number of pixels tall
// Output: none
// Must be less than or equal to 128 pixels wide by 128 pixels high
void BSP_LCD_DrawBitmap(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h);


//------------BSP_LCD_DrawCharS------------
// Simple character draw function.  This is the same function from
// Adafruit_GFX.c but adapted for this processor.  However, each call
// to BSP_LCD_DrawPixel() calls setAddrWindow(), which needs to send
// many extra data and commands.  If the background color is the same
// as the text color, no background will be printed, and text can be
// drawn right over existing images without covering them with a box.
// Requires (11 + 2*size*size)*6*8 bytes of transmission (image fully on screen; textcolor != bgColor)
// Input: x         horizontal position of the top left corner of the character, columns from the left edge
//        y         vertical position of the top left corner of the character, rows from the top edge
//        c         character to be printed
//        textColor 16-bit color of the character
//        bgColor   16-bit color of the background
//        size      number of pixels per character pixel (e.g. size==2 prints each pixel of font as 2x2 square)
// Output: none
void BSP_LCD_DrawCharS(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size);


//------------BSP_LCD_DrawChar------------
// Advanced character draw function.  This is similar to the function
// from Adafruit_GFX.c but adapted for this processor.  However, this
// function only uses one call to setAddrWindow(), which allows it to
// run at least twice as fast.
// Requires (11 + size*size*6*8) bytes of transmission (assuming image fully on screen)
// Input: x         horizontal position of the top left corner of the character, columns from the left edge
//        y         vertical position of the top left corner of the character, rows from the top edge
//        c         character to be printed
//        textColor 16-bit color of the character
//        bgColor   16-bit color of the background
//        size      number of pixels per character pixel (e.g. size==2 prints each pixel of font as 2x2 square)
// Output: none
void BSP_LCD_DrawChar(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size);


//------------BSP_LCD_DrawString------------
// String draw function.
// 13 rows (0 to 12) and 21 characters (0 to 20)
// Requires (11 + size*size*6*8) bytes of transmission for each character
// Input: x         columns from the left edge (0 to 20)
//        y         rows from the top edge (0 to 12)
//        pt        pointer to a null terminated string to be printed
//        textColor 16-bit color of the characters
// bgColor is Black and size is 1
// Output: number of characters printed
uint32_t BSP_LCD_DrawString(uint16_t x, uint16_t y, char *pt, int16_t textColor);


//********BSP_LCD_SetCursor*****************
// Move the cursor to the desired X- and Y-position.  The
// next character of the next unsigned decimal will be
// printed here.  X=0 is the leftmost column.  Y=0 is the top
// row.
// inputs: newX  new X-position of the cursor (0<=newX<=20)
//         newY  new Y-position of the cursor (0<=newY<=12)
// outputs: none
void BSP_LCD_SetCursor(uint32_t newX, uint32_t newY);

//-----------------------BSP_LCD_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Position determined by BSP_LCD_SetCursor command
// Input: n         32-bit number to be transferred
//        textColor 16-bit color of the numbers
// Output: none
// Variable format 1-10 digits with no space before or after
void BSP_LCD_OutUDec(uint32_t n, int16_t textColor);

//-----------------------BSP_LCD_OutUDec4-----------------------
// Output a 32-bit number in unsigned 4-digit decimal format
// Position determined by BSP_LCD_SetCursor command
// Input: 32-bit number to be transferred
//        textColor 16-bit color of the numbers
// Output: none
// Fixed format 4 digits with no space before or after
void BSP_LCD_OutUDec4(uint32_t n, int16_t textColor);

//-----------------------BSP_LCD_OutUDec5-----------------------
// Output a 32-bit number in unsigned 5-digit decimal format
// Position determined by BSP_LCD_SetCursor command
// Input: 32-bit number to be transferred
//        textColor 16-bit color of the numbers
// Output: none
// Fixed format 5 digits with no space before or after
void BSP_LCD_OutUDec5(uint32_t n, int16_t textColor);

//-----------------------BSP_LCD_OutUFix2_1-----------------------
// Output a 32-bit number in unsigned 3-digit fixed point, 0.1 resolution
// numbers 0 to 999 printed as " 0.0" to "99.9"
// Position determined by BSP_LCD_SetCursor command
// Input: 32-bit number to be transferred
//        textColor 16-bit color of the numbers
// Output: none
// Fixed format 4 characters with no space before or after
void BSP_LCD_OutUFix2_1(uint32_t n, int16_t textColor);

//-----------------------BSP_LCD_OutUHex2-----------------------
// Output a 32-bit number in unsigned 2-digit hexadecimal format
// numbers 0 to 255 printed as "00," to "FF,"
// Position determined by BSP_LCD_SetCursor command
// Input: 32-bit number to be transferred
//        textColor 16-bit color of the numbers
// Output: none
// Fixed format 3 characters with comma after
void BSP_LCD_OutUHex2(uint32_t n, int16_t textColor);

// ------------BSP_LCD_Drawaxes------------
// Set up the axes, labels, and other variables to
// allow data to be plotted in a chart using the
// functions BSP_LCD_PlotPoint() and
// BSP_LCD_PlotIncrement().
// Input: axisColor   16-bit color for axes, which can be produced by BSP_LCD_Color565()
//        bgColor     16-bit color for plot background, which can be produced by BSP_LCD_Color565()
//        xLabel      pointer to a null terminated string for x-axis (~4 character space)
//        yLabel1     pointer to a null terminated string for top of y-axis (~3-5 character space)
//        label1Color 16-bit color for y-axis label1, which can be produced by BSP_LCD_Color565()
//        yLabel2     pointer to a null terminated string for bottom of y-axis (~3 character space)
//                      if yLabel2 is empty string, no yLabel2 is printed, and yLabel1 is centered
//        label2Color 16-bit color for y-axis label2, which can be produced by BSP_LCD_Color565()
//        ymax        maximum value to be printed
//        ymin        minimum value to be printed
// Output: none
// Assumes: BSP_LCD_Init() has been called
void BSP_LCD_Drawaxes(uint16_t axisColor, uint16_t bgColor, char *xLabel,
  char *yLabel1, uint16_t label1Color, char *yLabel2, uint16_t label2Color,
  int32_t ymax, int32_t ymin);


// ------------BSP_LCD_PlotPoint------------
// Plot a point on the chart.  To plot several points in the
// same column, call this function repeatedly before calling
// BSP_LCD_PlotIncrement().  The units of the data are the
// same as the ymax and ymin values specified in the
// initialization function.
// Input: data1  value to be plotted (units not specified)
//        color1 16-bit color for the point, which can be produced by BSP_LCD_Color565()
// Output: none
// Assumes: BSP_LCD_Init() and BSP_LCD_Drawaxes() have been called
void BSP_LCD_PlotPoint(int32_t data1, uint16_t color1);


// ------------BSP_LCD_PlotIncrement------------
// Increment the plot between subsequent calls to
// BSP_LCD_PlotPoint().  Automatically wrap and clear the
// column to be printed to.
// Input: none
// Output: none
// Assumes: BSP_LCD_Init() and BSP_LCD_Drawaxes() have been called
void BSP_LCD_PlotIncrement(void);

//------------BSP_LCD_Message-------------------
// Divide the LCD into two logical partitions and provide
// an interface to output a string
// inputs: 	device	specifies top(0) or bottom(1)
//					line 		specifies line number (0-5)
//                  col         specifies column number (0-20)
// 					string	pointer to NULL-terminated ASCII string
//  				value		16-bit number in unsigned decimal format
// outputs: none
void BSP_LCD_Message (int device, int line, int col, char *string, unsigned int value);


// Initial LCD in OS
void BSP_LCD_OutputInit(void);

//------------BSP_LCD_DrawCrosshaire-------------------
// Draw a crosshair at the given coordinates
// an interface to output a string
// inputs: 	x				specifies top(0) or bottom(1)
//					y 			specifies line number (0-5)
// outputs: none
void BSP_LCD_DrawCrosshair(int16_t x, int16_t y, int16_t bgColor);
