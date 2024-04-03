#include <stdint.h>  // Include this header for using uint16_t type
#include <stdlib.h>  // Include the standard library for rand()

#include "images.h"  // Include the header file for the image arrays

volatile int pixel_buffer_start;  // global variable
short int Buffer1[240][512];      // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

// Define the struct for colors
struct color {
  uint16_t red;
  uint16_t green;
  uint16_t blue;
  uint16_t yellow;
  uint16_t cyan;
  uint16_t magenta;
  uint16_t orange;
  uint16_t purple;
  uint16_t brown;
  uint16_t skin;
  uint16_t black;
  uint16_t white;
  uint16_t gray;
};

// Initialize the struct with the given color values
struct color colors = {
    0xF800,  // Red
    0x07E0,  // Green
    0x001F,  // Blue
    0xFFE0,  // Yellow
    0x07FF,  // Cyan
    0xF81F,  // Magenta
    0xFC00,  // Orange
    0x8010,  // Purple
    0xA145,  // Brown
    0xFEA0,  // Skin
    0x0000,  // Black
    0xFFFF,  // White
    0x8410   // Gray
};

void swap(int *a, int *b);
void draw_level_1();
void draw_box(int x, int y, int short line_color);
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int y0, int x1, int y1, short int color);
int abs(int a);

// Function to draw a block
void drawBlock(int x, int y) {
  // Draw a block at position (x, y)
  // For simplicity, let's draw a brown block
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      plot_pixel(x + i, y + j, 0x88AA);  // Brown color for block
    }
  }
}

// Function to draw a pipe
void drawPipe(int x, int y) {
  // Draw a pipe at position (x, y)
  // For simplicity, let's draw a green pipe
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 32; j++) {
      plot_pixel(x + i, y + j, 0x00FF00);  // Green color for pipe
    }
  }
}

// Function to draw a castle
void drawCastle(int x, int y) {
  // Draw a castle at position (x, y)
  // For simplicity, let's draw a gray castle with towers

  // Main castle body
  for (int i = 0; i < 96; i++) {
    for (int j = 0; j < 64; j++) {
      plot_pixel(x + i, y + j, 0x7BEF);  // Gray color for castle
    }
  }

  // Draw towers
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 64; j++) {
      plot_pixel(x + i + 8, y + j, 0x7BEF);   // Light gray color for towers
      plot_pixel(x + i + 68, y + j, 0x7BEF);  // Light gray color for towers
    }
  }

  // Draw door
  for (int i = 32; i < 64; i++) {
    for (int j = 32; j < 64; j++) {
      plot_pixel(x + i, y + j, 0x0);  // Black color for door
    }
  }
}

void drawCloud(int x, int y) {
  // Draw a cloud at position (x, y)
  // For simplicity, let's draw a white cloud
  // Adjust the position and size for your specific scene
  plot_pixel(x + 0, y + 0, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 1, y + 0, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 2, y + 0, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 1, y + 1, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 3, y + 1, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 0, y + 2, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 1, y + 2, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 2, y + 2, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 3, y + 2, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 1, y + 3, 0xFFFFFF);  // White color for cloud
  plot_pixel(x + 2, y + 3, 0xFFFFFF);  // White color for cloud
}

// Function to draw a cloud
void drawBigCloud(int x, int y) {
  // Draw a cloud at position (x, y)
  // For simplicity, let's draw a white cloud
  // Adjust the position and size for your specific scene
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      plot_pixel(x + i, y + j, 0xFFFFFF);  // White color for cloud
    }
  }
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 6; j++) {
      plot_pixel(x + i + 2, y + j + 4, 0xFFFFFF);  // White color for cloud
    }
  }
  for (int i = 0; i < 14; i++) {
    for (int j = 0; j < 4; j++) {
      plot_pixel(x + i + 4, y + j + 8, 0xFFFFFF);  // White color for cloud
    }
  }
}

// Function to draw the Super Mario background
void drawSuperMarioBackground() {
  // Draw sky
  for (int y = 0; y < 160; y++) {
    for (int x = 0; x < 320; x++) {
      plot_pixel(x, y, 0x5DBF);  // Sky color (e.g., blue)
    }
  }

  // Draw grass
  for (int y = 160; y < 180; y++) {
    for (int x = 0; x < 320; x++) {
      plot_pixel(x, y, 0x663300);  // Green color for grass
    }
  }

  // Draw dirt 0x663300
  for (int y = 180; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      plot_pixel(x, y, 0x00AA00);  // Brown color for dirt
    }
  }

  // Draw blocks
  drawBlock(32, 128);
  drawBlock(80, 128);
  drawBlock(224, 128);
  drawBlock(272, 128);

  // Draw pipes
  drawPipe(128, 96);
  drawPipe(240, 96);

  // Draw castle
  drawCastle(160, 100);

  // Draw clouds
  drawCloud(30, 10);
  drawCloud(200, 30);
  drawCloud(100, 50);
  drawCloud(75, 15);
  drawCloud(190, 50);
  drawCloud(120, 30);
  drawCloud(60, 20);
  drawCloud(250, 40);
  drawCloud(150, 60);

  drawBigCloud(40, 20);
  drawBigCloud(200, 30);
  drawBigCloud(100, 50);
  drawBigCloud(180, 10);
  drawBigCloud(260, 20);
}

int main(void) {
  // ********* logic for pixel control and buffer *********
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

  /* set front pixel buffer to Buffer 1 */
  *(pixel_ctrl_ptr + 1) =
      (int)&Buffer1;  // first store the address in the  back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
  wait_for_vsync();
  /* initialize a pointer to the pixel buffer, used by drawing functions */
  pixel_buffer_start = *pixel_ctrl_ptr;
  draw_level_1();  // pixel_buffer_start points to the pixel buffer

  /* set back pixel buffer to Buffer 2 */
  *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // we draw on the back buffer
  draw_level_1();  // pixel_buffer_start points to the pixel buffer
  // ********* end of logic for pixel control and buffer *********

  // ********* initialization for implementing PS/2 Keyboard *********
  volatile int * ps2_ctrl_ptr = (int *)0xFF200100;
  volatile int * led_ctrl_ptr = (int *)0xFF200000;

  //Variables to store the data read from the PS/2 input.
  int ps2_data;
  char validRead;

  //The last 3 bytes read (byte1 is the most recent).
  unsigned char byte1, byte2, byte3;
  byte1 = 0;
  byte2 = 0;
  byte3 = 0;
  // ********* end of initialization for implementing PS/2 Keyboard *********

  while (1) {
    // draw the background again in order to erase previous stuff
    draw_level_1();

    // for (int y = 0; y < 40; y++) {
    //     for (int x = 0; x < 40; x++) {
    //         int index = y * 40 + x;
    // 	if (Iftikher_still[index] != 0xFFFF) {
    //         	plot_pixel(x, y+175, Iftikher_still[index]);
    // 	}
    //     }
    // }

    // for (int y = 0; y < 40; y++) {
    //     for (int x = 0; x < 40; x++) {
    //         int index = y * 40 + x;
    // 	if (Iftier_jump_left[index] != 0xFFFF) {
    //         	plot_pixel(x+145, y+130, Iftier_jump_left[index]);
    // 	}
    //     }
    // }

    // waiting stage for buffer swapping
    wait_for_vsync();  // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // new back buffer
  }
}

void swap(int *a, int *b) {
  int temp = *(a);
  *a = *b;
  *b = temp;
}

void draw_level_1() {
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      int index = y * 320 + x;
      plot_pixel(x, y, Level1[index]);
    }
  }
}

void draw_box(int x, int y, int short line_color) {
  plot_pixel(x, y, line_color);

  // corners
  plot_pixel(x + 1, y + 1, line_color);
  plot_pixel(x - 1, y - 1, line_color);
  plot_pixel(x + 1, y - 1, line_color);
  plot_pixel(x - 1, y + 1, line_color);

  // sides
  plot_pixel(x + 1, y, line_color);
  plot_pixel(x - 1, y, line_color);
  plot_pixel(x, y + 1, line_color);
  plot_pixel(x, y - 1, line_color);
}

void wait_for_vsync() {
  volatile int *pixel_ctrl_ptr = (int *)0xff203020;  // base address
  int statusControl;
  *pixel_ctrl_ptr = 1;  // start the synchronization process
  // write 1 into front buffer address register
  statusControl = *(pixel_ctrl_ptr + 3);  // read the status register
  while ((statusControl & 0x01) != 0) {
    statusControl = *(pixel_ctrl_ptr + 3);
  }
}

void plot_pixel(int x, int y, short int line_color) {
  volatile short int *one_pixel_address;

  one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);

  *one_pixel_address = line_color;
}

// Bresenham's line-drawing algorithm (from part 1)
void draw_line(int x0, int y0, int x1, int y1, short int color) {
  // Calculate the steepness of the line
  int is_steep = abs(y1 - y0) > abs(x1 - x0);
  if (is_steep) {
    // Swap the x and y values
    swap(&x0, &y0);
    swap(&x1, &y1);
  }
  if (x0 > x1) {
    // Ensure we are drawing from left to right
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  // Calculate line deltas
  int deltax = x1 - x0;
  int deltay = abs(y1 - y0);
  int error = -(deltax / 2);
  int y = y0;
  int y_step = (y0 < y1) ? 1 : -1;

  // Draw the pixels
  for (int x = x0; x <= x1; x++) {
    if (is_steep) {
      // If steep, plot pixel with swapped x and y
      plot_pixel(y, x, color);
    } else {
      // Otherwise, plot pixel normally
      plot_pixel(x, y, color);
    }
    error += deltay;
    if (error > 0) {
      y += y_step;
      error -= deltax;
    }
  }
}

int abs(int a) {
  if (a < 0) {
    return -a;
  }
  return a;
}
