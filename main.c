// code put together in one file in roder to test on cpulator
#include <stdbool.h>  // Include the standard library for boolean values
#include <stdint.h>   // Include this header for using uint16_t type
#include <stdlib.h>   // Include the standard library for rand()

#include "images.h"

volatile int pixel_buffer_start;  // global variable
short int Buffer1[240][512];      // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

int player_speed = 5;
int platform = 175;
int flag_y = 90;
int flagWaveCount = 0;

bool is_running_right = false;
bool is_running_left = false;
bool face_left = false;
bool face_right = true;
bool is_jumping = false;
bool top_of_jump = false;

bool on_lvl1_pipe = false;
bool on_lvl2_pipe = false;
bool on_lvl3_pipe = false;
bool on_lvl5_pipe = false;

//Control levels/stages of game
bool level_1 = true;
bool level_2 = false;
bool level_3 = false;
bool level_5_end = false;
bool flag_takedown = false;
bool game_complete = false;

bool falling_animation_of_level2 = true;
bool power_up_picked_up = false;
bool Iftiluigi = false;
bool isStartScreen = true;
bool superJump = false;
bool flagWave = false;
bool playStartSound = true;

bool isGameOverWait = false;
bool isGameOverScreen = false;

bool spaceBarPressed = false;
bool enterBarPressed = false;
bool s_clicked = false;

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

struct audio_t {
	volatile unsigned int control;
	volatile unsigned char rarc;
	volatile unsigned char ralc;
	volatile unsigned char warc;
	volatile unsigned char walc;
    volatile unsigned int ldata;
	volatile unsigned int rdata;
};

struct audio_t *const audiop = ((struct audio_t *)0xff203040);

void swap(int *a, int *b);
void draw_box(int x, int y, int short line_color);
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int y0, int x1, int y1, short int color);
int abs(int a);
void draw_level_1();
void draw_level_2();
void draw_iftimario_still_right(int x_offest);
void draw_iftimario_still_left(int x_offest);
void draw_iftimario_running_right(x_offset);
void draw_iftimario_running_left(x_offset);
void draw_iftimario_jumping_right(x_offset, y_offset);
void draw_iftimario_jumping_left(x_offset, y_offset);
void draw_transform_power_up(int x_offset, int y_offset);
void draw_transform_power_up_message();
void draw_super_jump_power_up_message();
void audio_playback_mono(int *samples, int n);
void draw_flag(x_offset, y_offset);
void draw_flag_bd(x_offset, y_offset);
void draw_game_over_success();

int main(void) {
  // The x value holder for moving the character
  int x_offset = 0;
  int y_offset = 0;
  bool up = true;
  bool t_clicked = false;

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
  volatile int *ps2_ctrl_ptr = (int *)0xFF200100;
  volatile int *led_ctrl_ptr = (int *)0xFF200000;

  // Variables to store the data read from the PS/2 input.
  int ps2_data;
  char validRead;

  // The last 3 bytes read (byte1 is the most recent).
  unsigned char byte1, byte2, byte3;
  byte1 = 0;
  byte2 = 0;
  byte3 = 0;
  // ********* end of initialization for implementing PS/2 Keyboard *********

  while (1) {
    // level 1
    if (level_1) {
      //reset variables in game
      flag_y = 90;
      power_up_picked_up = false;
      if (isStartScreen) {
        x_offset = 0;
      }

      // draw the background again in order to erase previous stuff
      draw_level_1();

      // Display Start Screen graphics
      if (isStartScreen) {
        draw_start_screen();
      }

      if (playStartSound) {
        audio_playback_mono(start_sound, 22151);
        playStartSound = false;
      }

      // ********* logic for PS/2 Keyboard *********
      // Keyboard input processing. Keyboard input is read one byte at a time.
      // Every time a read is performed of the PS/2 control register, it
      // discards the last byte.
      ps2_data = *(ps2_ctrl_ptr);
      validRead = ((ps2_data & 0x8000) != 0);

      if (validRead) {
        // Update the last 3 bytes read to reflect the current read.
        byte3 = byte2;
        byte2 = byte1;
        byte1 = (ps2_data & 0xFF);

        if (byte1 == 0x29) {
         // Space = Start game
         isStartScreen = false;
        } else if (byte1 == 0x23 && !isStartScreen) {
          // D = move to the right
          is_running_right = true;
          if (x_offset < 295) x_offset += player_speed;
        } else if (byte1 == 0x1C && !isStartScreen) {
          // A = move to the left
          is_running_left = true;
          if (x_offset > -15) x_offset -= player_speed;
        } else if (byte1 == 0x1D && !isStartScreen) {
          // W = jump
          is_jumping = true;
        } else if (byte1 == 0x1B && !isStartScreen) {
          // S = go down pipe
          s_clicked = true;
        }
      } else {
        if (is_running_right) {
          face_right = true;
          face_left = false;
        } else if (is_running_left) {
          face_left = true;
          face_right = false;
        }
        is_running_right = false;
        is_running_left = false;
        s_clicked = false;
        spaceBarPressed = false;
      }
      // ********* end of logic for PS/2 Keyboard *********

      if (is_jumping) {
        if (up) {
          y_offset -= 5;
        } else {
          y_offset += 5;
        }
        if (y_offset == -40) {
          up = false;
          top_of_jump = true;
        } else if (y_offset == 0) {
          up = true;
          is_jumping = false;
        }
      }

      // draw the character
      if (is_running_right) {
        if (is_jumping) {
          draw_iftimario_jumping_right(x_offset, y_offset);
        } else {
          draw_iftimario_running_right(x_offset);
        }
      } else if (is_running_left) {
        if (is_jumping) {
          draw_iftimario_jumping_left(x_offset, y_offset);
        } else {
          draw_iftimario_running_left(x_offset);
        }
      } else if (face_left) {
        if (is_jumping) {
          draw_iftimario_jumping_left(x_offset, y_offset);
        } else {
          draw_iftimario_still_left(x_offset);
        }
      } else if (face_right) {
        if (is_jumping) {
          draw_iftimario_jumping_right(x_offset, y_offset);
        } else {
          draw_iftimario_still_right(x_offset);
        }
      }

      // logic for collision with block platform in middle of screen
      if (x_offset > 102 && x_offset < 162) {
        if (top_of_jump) {
          platform = 130;
          top_of_jump = false;
        }
      } else if (x_offset > 265 && x_offset < 295) {
        // logic for collision with pipe at the right of the screen
        if (top_of_jump) {
          platform = 155;
          top_of_jump = false;
          on_lvl1_pipe = true;
        }
      } else {
        platform = 175;
      }

      if (on_lvl1_pipe) {
        if (s_clicked) {
          level_1 = false;
          level_2 = true;
          y_offset = 0;
          x_offset = 0;
          platform = 20;
        }
      }
    } else if (level_2) {
      draw_level_2();
      // ********* logic for PS/2 Keyboard *********
      // Keyboard input processing. Keyboard input is read one byte at a time.
      // Every time a read is performed of the PS/2 control register, it
      // discards the last byte.
      ps2_data = *(ps2_ctrl_ptr);
      validRead = ((ps2_data & 0x8000) != 0);

      if (validRead) {
        // Update the last 3 bytes read to reflect the current read.
        byte3 = byte2;
        byte2 = byte1;
        byte1 = (ps2_data & 0xFF);

        if (byte1 == 0x23) {
          // D = move to the right
          is_running_right = true;
          if (x_offset < 295) x_offset += player_speed;
        } else if (byte1 == 0x1C) {
          // A = move to the left
          is_running_left = true;
          if (x_offset > -15) x_offset -= player_speed;
        } else if (byte1 == 0x1D) {
          // W = jump
          is_jumping = true;
        } else if (byte1 == 0x1B) {
          // S = go down pipe
          s_clicked = true;
        } else if (byte1 == 0x2C) {
          // T = pick up power up
          t_clicked = true;
        }
      } else {
        if (is_running_right) {
          face_right = true;
          face_left = false;
        } else if (is_running_left) {
          face_left = true;
          face_right = false;
        }
        is_running_right = false;
        is_running_left = false;
        s_clicked = false;
        t_clicked = false;
      }
      // ********* end of logic for PS/2 Keyboard *********

      if (is_jumping) {
        if (up) {
          y_offset -= 5;
        } else {
          y_offset += 5;
        }
        if (y_offset == -40) {
          up = false;
          top_of_jump = true;
        } else if (y_offset == 0) {
          up = true;
          is_jumping = false;
        }
      }

      // logic for picking up Iftiluigi power up
      if (!power_up_picked_up) {
        draw_transform_power_up(140, 150);
      }
      if (x_offset > 120 && x_offset < 160 && y_offset < 30) {
        if (!power_up_picked_up) {
          draw_transform_power_up_message();
        }
        if (t_clicked) {
          power_up_picked_up = true;
          Iftiluigi = true;
        }
      }

      // draw the character
      if (falling_animation_of_level2) {
        draw_iftimario_still_right(x_offset);
        platform += 5;
        if (platform == 150) {
          falling_animation_of_level2 = false;
        }
      } else {
        if (is_running_right) {
          if (is_jumping) {
            draw_iftimario_jumping_right(x_offset, y_offset);
          } else {
            draw_iftimario_running_right(x_offset);
          }
        } else if (is_running_left) {
          if (is_jumping) {
            draw_iftimario_jumping_left(x_offset, y_offset);
          } else {
            draw_iftimario_running_left(x_offset);
          }
        } else if (face_left) {
          if (is_jumping) {
            draw_iftimario_jumping_left(x_offset, y_offset);
          } else {
            draw_iftimario_still_left(x_offset);
          }
        } else if (face_right) {
          if (is_jumping) {
            draw_iftimario_jumping_right(x_offset, y_offset);
          } else {
            draw_iftimario_still_right(x_offset);
          }
        }
      }

      if (x_offset >= 231 && platform == 150) {
        x_offset = 231;

        on_lvl2_pipe = true;
        
        if (on_lvl2_pipe) {
          level_2 = false;
          level_3 = true;
          y_offset = 0;
          x_offset = 20;
          platform = 167;
          power_up_picked_up = false;
        }
      }

    } else if (level_3) {
      draw_level_3();

      // ********* logic for PS/2 Keyboard *********
      // Keyboard input processing. Keyboard input is read one byte at a time.
      // Every time a read is performed of the PS/2 control register, it
      // discards the last byte.
      ps2_data = *(ps2_ctrl_ptr);
      validRead = ((ps2_data & 0x8000) != 0);

      if (validRead) {
        // Update the last 3 bytes read to reflect the current read.
        byte3 = byte2;
        byte2 = byte1;
        byte1 = (ps2_data & 0xFF);

        if (byte1 == 0x23) {
          // D = move to the right
          is_running_right = true;
          if (x_offset < 295) x_offset += player_speed;
        } else if (byte1 == 0x1C) {
          // A = move to the left
          is_running_left = true;
          if (x_offset > -15) x_offset -= player_speed;
        } else if (byte1 == 0x1D) {
          // W = jump
          is_jumping = true;
        } else if (byte1 == 0x1B) {
          // S = go down pipe
          s_clicked = true;
        } else if (byte1 == 0x2C) {
          // T = pick up power up
          t_clicked = true;
        }
      } else {
        if (is_running_right) {
          face_right = true;
          face_left = false;
        } else if (is_running_left) {
          face_left = true;
          face_right = false;
        }
        is_running_right = false;
        is_running_left = false;
        s_clicked = false;
        t_clicked = false;
      }
      // ********* end of logic for PS/2 Keyboard *********

      if (is_jumping) {
        if (up) {
          if (superJump) {
            y_offset -= 10;
          } else {
            y_offset -= 5;
          }
        } else {
          y_offset += 5;
        }
        if (y_offset == -40 && !superJump) {
          up = false;
          top_of_jump = true;
        } else if (superJump && y_offset == -80) {
          up = false;
          top_of_jump = true;
        } else if (y_offset == 0) {
          up = true;
          is_jumping = false;
        }
      }

      // logic for super jumping powers for ifti luigi
      if (!power_up_picked_up) {
        draw_transform_power_up(150, 90);
      }
      if (x_offset > 120 && x_offset < 160 && y_offset < 30) {
        if (!power_up_picked_up) {
          draw_super_jump_power_up_message();
        }
        if (t_clicked) {
          power_up_picked_up = true;
          superJump = true;
        }
      }

      if (is_running_right) {
          if (is_jumping) {
            draw_iftimario_jumping_right(x_offset, y_offset);
          } else {
            draw_iftimario_running_right(x_offset);
          }
        } else if (is_running_left) {
          if (is_jumping) {
            draw_iftimario_jumping_left(x_offset, y_offset);
          } else {
            draw_iftimario_running_left(x_offset);
          }
        } else if (face_left) {
          if (is_jumping) {
            draw_iftimario_jumping_left(x_offset, y_offset);
          } else {
            draw_iftimario_still_left(x_offset);
          }
        } else if (face_right) {
          if (is_jumping) {
            draw_iftimario_jumping_right(x_offset, y_offset);
          } else {
            draw_iftimario_still_right(x_offset);
          }
        }

      // logic for collision with block platform in middle of screen
      if (x_offset > 100 && x_offset < 192) {
        if (top_of_jump) {
          platform = 100;
          top_of_jump = false;
        }

      // logic for level 3 stair case
      } else if (x_offset >= 214 && x_offset < 225) {
        platform = 151;
        top_of_jump = false;
      } else if (x_offset >= 225 && x_offset < 253) {
        platform = 138;
        top_of_jump = false;
      } else if (x_offset >= 253 && x_offset < 281) {
        platform = 123;
        top_of_jump = false;
      } else if (x_offset >= 281 && x_offset < 320) {
        platform = 110;
        top_of_jump = false;
      
      // default ground level in level 3
      } else {
        platform = 167;
      }

      if (x_offset > 260 && x_offset < 320 && is_jumping && !up && power_up_picked_up) {
        level_3 = false;
        level_5_end = true;
        is_jumping = false;
        on_lvl5_pipe = true;
        x_offset = 40;
        platform = 150;
      }

    } else if (level_5_end) {
      draw_level_5_end();

      // ********* logic for PS/2 Keyboard *********
      // Keyboard input processing. Keyboard input is read one byte at a time.
      // Every time a read is performed of the PS/2 control register, it
      // discards the last byte.
      ps2_data = *(ps2_ctrl_ptr);
      validRead = ((ps2_data & 0x8000) != 0);

      if (validRead) {
        // Update the last 3 bytes read to reflect the current read.
        byte3 = byte2;
        byte2 = byte1;
        byte1 = (ps2_data & 0xFF);
        
        if (byte1 == 0x29) {
            spaceBarPressed = true;
        }
        if (byte1 == 0x23) {
          // D = move to the right
          is_running_right = true;
          if (x_offset < 295) x_offset += player_speed;
        } else if (byte1 == 0x1C) {
          // A = move to the left
          is_running_left = true;
          if (x_offset > -15) x_offset -= player_speed;
        } else if (byte1 == 0x1D) {
          // W = jump
          is_jumping = true;
        } else if (byte1 == 0x1B) {
          // S = go down pipe
          s_clicked = true;
        } else if (byte1 == 0x2C) {
          // T = pick up power up
          t_clicked = true;
        }
      } else {
        if (is_running_right) {
          face_right = true;
          face_left = false;
        } else if (is_running_left) {
          face_left = true;
          face_right = false;
        }
        is_running_right = false;
        is_running_left = false;
        s_clicked = false;
        t_clicked = false;
      }
      // ********* end of logic for PS/2 Keyboard *********

      if (is_jumping) {
        if (up) {
          if (superJump) {
            y_offset -= 10;
          } else {
            y_offset -= 5;
          }
        } else {
          y_offset += 5;
        }
        if (y_offset == -40 && !superJump) {
          up = false;
          top_of_jump = true;
        } else if (superJump && y_offset == -80) {
          up = false;
          top_of_jump = true;
        } else if (y_offset == 0) {
          up = true;
          is_jumping = false;
        }
      }

      if (is_running_right) {
          if (is_jumping) {
            draw_iftimario_jumping_right(x_offset, y_offset);
          } else {
            draw_iftimario_running_right(x_offset);
          }
        } else if (is_running_left) {
          if (is_jumping) {
            draw_iftimario_jumping_left(x_offset, y_offset);
          } else {
            draw_iftimario_running_left(x_offset);
          }
        } else if (face_left) {
          if (is_jumping) {
            draw_iftimario_jumping_left(x_offset, y_offset);
          } else {
            draw_iftimario_still_left(x_offset);
          }
        } else if (face_right) {
          if (is_jumping) {
            draw_iftimario_jumping_right(x_offset, y_offset);
          } else {
            draw_iftimario_still_right(x_offset);
          }
        }

      // logic for level 5 end stair case
      if (x_offset > (94 - 20) && x_offset < (101 - 20)) {
        platform = (202 - 40);
        top_of_jump = false;
      } else if (x_offset >= (101 - 20) && x_offset < (108 - 20)) {
        platform = (194 - 40);
        top_of_jump = false;
      } else if (x_offset >= (108 - 20) && x_offset < (115 - 20)) {
        platform = (188 - 40);
        top_of_jump = false;
      } else if (x_offset >= (115 - 20) && x_offset < (122 - 20)) {
        platform = (180 - 40);
        top_of_jump = false;
      } else if (x_offset >= (122 - 20) && x_offset < (129 - 20)) {
        platform = (172 - 40);
        top_of_jump = false;
      } else if (x_offset >= (129 - 20) && x_offset < (136 - 20)) {
        platform = (166 - 40);
        top_of_jump = false;
      } else if (x_offset >= (136 - 20) && x_offset < (152 - 20)) {
        platform = (158 - 40);
        top_of_jump = false;

      } else if (x_offset >= (152) && x_offset <= (162) && is_jumping && top_of_jump) {
          top_of_jump = false;
          flag_takedown = true;
      
      //default found level in level-5 end
      } else if (x_offset >= 248 && x_offset <= 260 && spaceBarPressed) {
        isGameOverWait = true;
        level_5_end = false;
      }
      
      else {
        platform = 169;
      }

      if (flag_takedown) {
        flag_y = flag_y + 5;
        draw_flag(180,flag_y);
        if (flag_y == 140) {
            audio_playback_mono(game_cleared, 46184);
            flag_takedown = false;
        }
      } else {
        draw_flag(180,flag_y);
      }

    } else if (isGameOverWait) {
        draw_level_5_end();

        // ********* logic for PS/2 Keyboard *********
        // Keyboard input processing. Keyboard input is read one byte at a time.
        // Every time a read is performed of the PS/2 control register, it
        // discards the last byte.
        ps2_data = *(ps2_ctrl_ptr);
        validRead = ((ps2_data & 0x8000) != 0);

        if (validRead) {
            // Update the last 3 bytes read to reflect the current read.
            byte3 = byte2;
            byte2 = byte1;
            byte1 = (ps2_data & 0xFF);
            
            if (byte1 == 0x5A) {
                enterBarPressed = true;
            }
        } else {
            enterBarPressed = false;
        }

        draw_flag(180,flag_y);
        draw_flag_bd(280, 100);
        if (flagWaveCount >= 10) {
          draw_game_over_success();
          isGameOverScreen = true;
        }
        flagWave = !flagWave;
        flagWaveCount++;

        if (enterBarPressed && isGameOverScreen) {
            isGameOverWait = false;
            level_1 = true;
            enterBarPressed  = false;
            isStartScreen = true;
            playStartSound = true;
        }

    }

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

void draw_start_screen() {
    for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      int index = y * 320 + x;
      if (Super_Ifti_Bros_start_screen[index] == 0x0000) {
        plot_pixel(x, y, Level1[index]);
      } else {
        plot_pixel(x, y, Super_Ifti_Bros_start_screen[index]);
      }
      
    }
  }
}

void draw_level_1() {
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      int index = y * 320 + x;
      plot_pixel(x, y, Level1[index]);
    }
  }
}

void draw_level_2() {
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      int index = y * 320 + x;
      plot_pixel(x, y, Level2[index]);
    }
  }
}

void draw_level_3() {
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      int index = y * 320 + x;
      plot_pixel(x, y, Level_3[index]);
    }
  }
}

void draw_game_over_success() {
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      int index = y * 320 + x;
      if (game_over_success[index] != 0x0000) {
       plot_pixel(x, y, game_over_success[index]);
      }
    }
  }
}

void draw_level_5_end() {
 for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      int index = y * 320 + x;
      plot_pixel(x, y, Level_5_end[index]);
    }
  }
}

void draw_flag_bd(x_offset, y_offset) {
 for (int y = 0; y < 40; y++) {
    for (int x = 0; x < 44; x++) {
      int index = y * 44 + x;
      if (!flagWave) {
        if (flag_bd[index] != 0xFFFF) {
            plot_pixel(x + x_offset, y + y_offset, flag_bd[index]);
        }
      } else {
        if (flag_bd_flipped[index] != 0xFFFF) {
            plot_pixel(x + (x_offset - 26), y + y_offset, flag_bd_flipped[index]);
        }
      }
    }
  }
}

void draw_flag(x_offset, y_offset) {
 for (int y = 0; y < 40; y++) {
    for (int x = 0; x < 44; x++) {
      int index = y * 44 + x;
      if (flag[index] != 0xFFFF) {
        plot_pixel(x + x_offset, y + y_offset, flag[index]);
      }
      
    }
  }
}

void draw_iftimario_still_right(int x_offest) {
  if (Iftiluigi) {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftier_still_right[index] != 0xFFFF) {
          plot_pixel(x + x_offest, y + platform, Iftier_still_right[index]);
        }
      }
    }
  } else {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftikher_still_right[index] != 0xFFFF) {
          plot_pixel(x + x_offest, y + platform, Iftikher_still_right[index]);
        }
      }
    }
  }
}

void draw_iftimario_still_left(int x_offest) {
  if (Iftiluigi) {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftier_still_left[index] != 0xFFFF) {
          plot_pixel(x + x_offest, y + platform, Iftier_still_left[index]);
        }
      }
    }
  } else {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftikher_still_left[index] != 0xFFFF) {
          plot_pixel(x + x_offest, y + platform, Iftikher_still_left[index]);
        }
      }
    }
  }
}

void draw_iftimario_running_right(x_offset) {
  if (Iftiluigi) {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftier_run_right[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform, Iftier_run_right[index]);
        }
      }
    }
  } else {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftikher_running_right[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform, Iftikher_running_right[index]);
        }
      }
    }
  }
}

void draw_iftimario_running_left(x_offset) {
  if (Iftiluigi) {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftier_run_left[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform, Iftier_run_left[index]);
        }
      }
    }
  } else {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftikher_running_left[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform, Iftikher_running_left[index]);
        }
      }
    }
  }
}

void draw_iftimario_jumping_right(x_offset, y_offset) {
  if (Iftiluigi) {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftier_jump_right[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform + y_offset,
                     Iftier_jump_right[index]);
        }
      }
    }
  } else {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftikher_jump_right[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform + y_offset,
                     Iftikher_jump_right[index]);
        }
      }
    }
  }
}

void draw_iftimario_jumping_left(x_offset, y_offset) {
  if (Iftiluigi) {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftier_jump_left[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform + y_offset,
                     Iftier_jump_left[index]);
        }
      }
    }
  } else {
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        int index = y * 40 + x;
        if (Iftikher_jump_left[index] != 0xFFFF) {
          plot_pixel(x + x_offset, y + platform + y_offset,
                     Iftikher_jump_left[index]);
        }
      }
    }
  }
}

void draw_transform_power_up(int x_offset, int y_offset) {
  for (int y = 0; y < 40; y++) {
    for (int x = 0; x < 40; x++) {
      int index = y * 40 + x;
      if (transfrom_power_up[index] != 0xFFFF) {
        plot_pixel(x + x_offset, y + y_offset, transfrom_power_up[index]);
      }
    }
  }
}

void draw_super_jump_power_up_message() {
  for (int y = 0; y < 40; y++) {
    for (int x = 0; x < 70; x++) {
      int index = y * 70 + x;
      if (super_jump_message[index] != 0x0000) {
        plot_pixel(x + 150, y + 90, super_jump_message[index]);
      }
    }
  }
}

void draw_transform_power_up_message() {
  for (int y = 0; y < 40; y++) {
    for (int x = 0; x < 70; x++) {
      int index = y * 70 + x;
      if (transfrom_power_up_message[index] != 0x0000) {
        plot_pixel(x + 20, y + 10, transfrom_power_up_message[index]);
      }
    }
  }
}

void audio_playback_mono(int *samples, int n) {
    audiop->control = 0x8; // clear the output FIFOs
    audiop->control = 0x0; // resume input conversion
    for (int i = 0; i < n; i++) {
        // wait till there is space in the output FIFO
        while (audiop->warc == 0);
        audiop->ldata = samples[i];
        audiop->rdata = samples[i];
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
