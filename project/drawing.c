#include <msp430.h>
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"
#include "drawing.h"

short drawPos[2] = {screenWidth/2 + 1, screenHeight/2};
short controlPos[2] = {screenWidth/2, screenHeight/2};
short rowLimits[2] = {1, screenHeight};
short colLimits[2] = {1, screenWidth};

void update_draw()
{
  for (char axis = 0; axis < 2; axis ++)
    if (drawPos[axis] != controlPos[axis]) /* position changed? */
      goto redraw;
  return;/* nothing to do */

 redraw:

  for (char axis = 0; axis < 2; axis++)
    drawPos[axis] = controlPos[axis];
  draw_shape(drawPos[0], drawPos[1], COLOR_GREEN); /* draw */
}

void draw_shape(int col, int row, unsigned short color)
{
  drawPixel(col, row - 1, color);

  drawPixel(col - 1, row, color);

  drawPixel(col, row + 1, color);

  drawPixel(col + 1, row, color);
}
