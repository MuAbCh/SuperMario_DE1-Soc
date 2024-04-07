#ifndef IMAGES_H
#define IMAGES_H

// header file for all the images used in the game

// level images
extern unsigned short Level1[76800]; // level 1 image
extern unsigned short Level2[76800]; // Level 2 image
extern unsigned short Level_3[76800]; // Level 3 image
extern unsigned short Level_5_end[76800]; // Level 5 end image
extern unsigned short flag[1760]; // flag image
extern unsigned short Super_Ifti_Bros_start_screen[76800]; // start screen image
extern unsigned short flag_bd[1760]; // flag image
extern unsigned short flag_bd_flipped[1760]; // flag image
extern unsigned short game_over_success[76800]; // game over success image

// Iftikher images
extern unsigned short Iftikher_still_right[1600]; // ifitkher still image right
extern unsigned short Iftikher_still_left[1600]; // ifitkher still image left
extern unsigned short Iftikher_running_right[1600]; // ifitkher running right image
extern unsigned short Iftikher_running_left[1600]; // ifitkher running left image
extern unsigned short Iftikher_jump_right[1600]; // ifitkher jump image right
extern unsigned short Iftikher_jump_left[1600]; // ifitkher jump image left

// Iftier images
extern unsigned short Iftier_jump_left[1600]; // Iftier jump image
extern unsigned short Iftier_jump_right[1600]; // Iftier jump image
extern unsigned short Iftier_still_left[1600]; // Iftier still image
extern unsigned short Iftier_still_right[1600]; // Iftier still image
extern unsigned short Iftier_run_left[1600]; // Iftier running image
extern unsigned short Iftier_run_right[1600]; // Iftier running image

// power up images
extern unsigned short transfrom_power_up[1600]; // power up image to transfrom into Iftier
extern unsigned short transfrom_power_up_message[1600]; // image to show the message to transfrom into Iftier
extern unsigned short super_jump_message[2800]; // super jump message image

// sound files
extern int game_cleared[46184]; // game cleared sound
extern int start_sound[22151]; // start sound

#endif