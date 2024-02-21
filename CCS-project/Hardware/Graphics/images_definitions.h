/*
 * img_vars.h
 *
 *  Created on: 13 dic 2022
 *      Author: damiano
 */

#ifndef IMAGES_DEFINITIONS_H_
#define IMAGES_DEFINITIONS_H_

#include <ti/grlib/grlib.h>
#include <Hardware/status.h>

extern const Graphics_Image BLACK_CELL_1BPP_UNCOMP;

extern const Graphics_Image ARROW_1BPP_UNCOMP[];
extern const Graphics_Image CELL_1BPP_UNCOMP[];

#define UP      ARROW_1BPP_UNCOMP[color *4 +0]
#define SX      ARROW_1BPP_UNCOMP[color *4 +1]
#define DOWN    ARROW_1BPP_UNCOMP[color *4 +2]
#define DX      ARROW_1BPP_UNCOMP[color *4 +3]

#define WHITE_CELL CELL_1BPP_UNCOMP[color]
#define BLACK_CELL BLACK_CELL_1BPP_UNCOMP

// foreground color
const int GRAPHICS_COLOR[4] = {GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_GREEN, GRAPHICS_COLOR_BLUE, GRAPHICS_COLOR_PURPLE};
#define BACKGROUNG_COLOR GRAPHICS_COLOR[color]

#endif /* IMAGES_DEFINITIONS_H_ */
