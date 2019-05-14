/*
  Blink Task
  Turns on an LED on for one second, then off for one second, via a Linkit One timer.
  This repeats 10 times and then removes the timer.

  Overview of circuit:
  Digital pin out (3.3v) -> led resistor -> led -> ground
 */

#include "LTask.h"
#include "vmtimer.h"

int pin = 13;

void setup() {
  Serial.begin(9600);
  // set our pin to output
  pinMode(pin, OUTPUT);

  // Start low
  digitalWrite(pin, LOW);
  
  // delay 1 second 'off'
  delay(1000);

  // LTask - class handles threading for the Linkit One 
  // the function you will most often be using from LTask is remoteCall
  // it is defined as:
  // remoteCall(remote_call_ptr func,  void* userdata)
  // where remote_call_ptr is a typedef:
  // typedef boolean (*remote_call_ptr)(void* user_data);
  // and userdata is any object you want to pass to your callback function
  LTask.remoteCall((remote_call_ptr)createTimer, NULL);
}

void loop() {
  // Do nothing in the loop.
  // Our led will be blinking in response to the system timer
}

// createTimer is called by LTask.remoteCall
// You must call the vm_create_timer in a remoteCall callback.
void createTimer(void)
{
  // VMINT is defined in vmtssys.h as an int
  VMINT timerId = 0;

  // vm_create_timer is defined vmtimer.h
  // VMINT vm_create_timer(VMUINT32 millisec, VM_TIMERPROC_T timerproc);
  // parameters are the milliseconds between timer callbacks
  // and the function to callback
  // vm_create_timer returns the id of the timer that the Linkit One system creates
  // it will be less than zero if the system could not create a timer
  // NOTE: there can be a max of 10 of these timers at any time
  timerId = vm_create_timer(1000, (VM_TIMERPROC_T)timerTick);
  if (timerId < 0)
    Serial.println("vm_create_timer failed"); 
  else 
    Serial.println("vm_create_timer succeeded");
}

// timerTick matches the function signature of VM_TIMER_PROC_T
// The VM_TIMER_PROC_T is defined as a function that takes an integer parameter - the task id
// typedef void (*VM_TIMERPROC_T)(VMINT tid); 
void timerTick(VMINT tid)
{
  static VMINT blinkCount = 0;
  blinkCount++;

  if (blinkCount & 1) {
    digitalWrite(pin, HIGH);
    Serial.println("LED On");
  }
  else {
    digitalWrite(pin, LOW);
    Serial.println("LED Off");
  }

  if (blinkCount > 10) {
    // Turn off LED
    digitalWrite(pin, LOW);
    Serial.println("LED Off");
    
    // Delete the timer
    vm_delete_timer(tid);
    Serial.println("Removing timer");
  }
}

