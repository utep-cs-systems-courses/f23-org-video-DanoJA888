#include <msp430.h>
#include "state_machines.h"
#include "led.h"
#include "switches.h"
#include "buzzer.h"
#include "drawing.h"

// blink state machine
static int jumpStateMachine = 0;
static int blinkCount = 0; // state var representing blink state
static int blinkLimit = 0;   //  state var representing reciprocal of duty cycle
static int reduce = 0;

void blinkUpdate() // called every 1/250s to blink with duty cycle 1/blinkLimit
{
  blinkCount ++;
  if (blinkCount >= blinkLimit) {
    blinkCount = 0;
    red_on = 0;
  } else {
    red_on = 1;
  }

  led_update();
}

void oncePerTimePeriod() // repeatedly start bright and gradually lower duty cycle, one step/sec
{
  if (reduce) {
    
    blinkLimit --;  // increase duty

    if (blinkLimit <= 0)
      reduce = 0;
  } else {
    
    blinkLimit ++;  // reduce duty cycle

    if (blinkLimit >= 10)  // but don't let duty cycle reduce = 1;
      reduce = 1;
    }
  
}

void timeUpdate()  // called every 1/250 sec
{
  static int timeCount = 0; // state variable representing repeating time in 1/20 steps

  timeCount ++;

  if (timeCount >= 250/20) { // once each 1/20 of a second
    timeCount = 0;
    oncePerTimePeriod();
  }
  
}

void oncePerSecond() // called every second
{
  if (red_on) {
    red_on = 0;
  } else {
    red_on = 1;
  }
  led_update();
}

void secondUpdate()  // called every 1/250 sec to call oncePerSecond once per second
{
  static int secondCount = 0;
  secondCount++;
  if(secondCount >= 250){
    secondCount = 0;
    oncePerSecond();
  }
}

void buzzerUpdate(){
  static int note = 1;
  if(note >= 4)
    note = 0;
  buzzer_set_period(song_a[note]);
  note++;
}

void buzzerTimeUpdate()
{
  static int buzzTimeCount = 0;
  buzzTimeCount++;
  // Check which switch is pressed and play the corresponding note
  if (switches & SW1){
    buzzer_set_period(song_a[0]);
      buzzerUpdate();
  }
  else if (switches & SW2){
      buzzer_set_period(song_a[1]);
      buzzerUpdate();
    }
  else if (switches & SW3){
      buzzer_set_period(song_a[2]);
      buzzerUpdate();
    }
  else if (switches & SW4){
      buzzer_set_period(song_a[3]);
      buzzerUpdate();
    }
  else{
      // If no switch is pressed, silence the buzzer
      buzzer_set_period(0);
    }
  // Ensure a single call to buzzerUpdate even if no switch is pressed
  if (buzzTimeCount >= 300)
    {
      buzzTimeCount = 0;
    }
}
short redrawScreen = 1;

void drawingShapesUpdate(){
  short oldRow = controlPos[1];
  short oldCol = controlPos[0];
  short newRow = oldRow;
  short newCol = oldCol;

  if (switches & SW1) newRow += 1;
  if (switches & SW2) newRow -= 1;
  if (switches & SW4) newCol += 1;
  if (switches & SW3) newCol -= 1;

  
  if (newRow <= rowLimits[0])
    newRow = rowLimits[0] + 1;

  if (newRow >= rowLimits[1])
    newRow = rowLimits[1] -1 ;

  if (newCol <= colLimits[0])
    newCol = colLimits[0] + 1;
  
  if (newCol >= colLimits[1])
    newCol = colLimits[1] - 1;

  controlPos[0] = newCol;
  controlPos[1] = newRow;

  update_draw();
}

void switchJump(){
  if (switch_state_changed) {
    jumpStateMachine = switch_state_down; // Setting a different state machine
  }
  switch_state_changed = 0;
} 
void timeAdvStateMachines() // called every 1/250 sec
{
  if (jumpStateMachine) {
    drawingShapesUpdate();
    secondUpdate();
    buzzer_set_period(0);
  } else {
    blinkUpdate();
    timeUpdate();
    buzzerTimeUpdate();
  }

}
