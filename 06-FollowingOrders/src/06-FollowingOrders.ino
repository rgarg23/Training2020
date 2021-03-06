#include <Zumo32U4.h>

Zumo32U4ProximitySensors proxSensors;
Zumo32U4Motors motors;
Zumo32U4LCD lcd;

// This enum contains the states we move through as the message is processed.
enum State
{
    Idle,           // No message is currently being received, wait for the leading pulse
    Lead,           // The lead pulse was received, wait for it to turn off
    LeadComplete,   // The lead pulse is finished, wait for the first message pulse
    WaitForDataOff, // A message pulse is being received, wait for it to turn off
    WaitForDataOn,  // A message pulse has ended, wait for the next one
    MessageComplete // The message is complete, look for repeat signals
};

unsigned long now;             // The time (in microseconds) that the last measurement was taken
unsigned long stateChangeTime; // The last time (in microseconds) that the state changed
State state = Idle;            // The current state - this is of type "State", referencing the enum above
int readPosition = 0;          // The current bit being read from 0 to 31
unsigned char address1 = 0;    // Message byte 1/4 - this should always be 0
unsigned char address2 = 0;    // Message byte 2/4 - this should always be 191 (this doesn't follow the standard NEC protocol)
unsigned char message1 = 0;    // Message byte 3/4 - this indicates which command is being sent (as seen in the "Command" enum)
unsigned char message2 = 0;    // Message byte 4/4 - this is the logical inverse of "message1", meaning it should equal "255 - message1"

// This function is run once, when the Zumo starts up
void setup()
{
    // This clears the LCD (in case something was already on the screen when the program starts)
    lcd.clear();

    // This sets up the proximity sensors to read all three sensors. These sensors can be used to receive IR data when the emitters aren't used
    proxSensors.initThreeSensors();
}

// This function can be used to update the state
void setState(State newState)
{
    state = newState;      // Update the state
    stateChangeTime = now; // Save the time the state changed
}

// This functions sets a specific bit in "var" at position "pos" (where 0 is the least significant bit)
void writeBit(unsigned char &var, int pos, bool value)
{
    if (value)
    {
        var |= (1 << pos); // Modifies a single bit to be positive (we can discuss how this works in more detail for those interested!)
    }
    else
    {
        var &= ~(1 << pos); // Modifies a single bit to be negative (we can discuss how this works in more detail for those interested!)
    }
}

void loop()
{
    // This records whether any of the sensors are receiving a pulse - "readBasic" reads the sensor without using the IR emitters
    bool activated = proxSensors.readBasicLeft() || proxSensors.readBasicFront() || proxSensors.readBasicRight();

    // This sets the yellow LED based on whether a pulse was detected.
    ledYellow(activated ? 1 : 0);

    // This records the time at which the measurement was taken, so that it remains constant throughout the processing logic. You can get the time since the state changed with the expression "now - stateChangeTime"
    now = micros();

    // This switch statement run different code depending on the current state. The "break" at the end of each "case" prevents other cases from running.
    switch (state)
    {
    case Idle: // No message is currently being received, wait for the leading pulse
        if (activated)
        {
            // ADD CODE HERE - The lead pulse is starting. Use "setState" to update the state in response.
        }
        break;

    case Lead: // The lead pulse was received, wait for it to turn off
        if (!activated)
        {
            // ADD CODE HERE - The lead pulse is over. What should be updated?
        }
        break;

    case LeadComplete: // The lead pulse is finished, wait for the first message pulse
        if (activated)
        {
            // ADD CODE HERE - The first message pulse is being received. What should be updated?
        }
        else if (now - stateChangeTime > 10000)
        {
            // ADD CODE HERE - 10ms have passed since the lead pulse. This seems to be a false trigger - reset to idle.
        }
        break;

    case WaitForDataOff: // A message pulse is being received, wait for it to turn off
        if (!activated)
        {
            // ADD CODE HERE - A message pulse is complete. Remember that the pulse itself contains no data. What should be updated?
        }
        break;

    case WaitForDataOn: // A message pulse has ended, wait for the next one
        if (activated)
        {
            // ADD CODE HERE - The next pulse was detected, meaning we can record one bit! First, use the timing since the last pulse to determine if it is a "1" or "0". Then, write that bit to the correct position in one of the address/message bytes using the "writeBit" function. Finally, move to the next state.
        }
        else if (now - stateChangeTime > 2000)
        {
            // ADD CODE HERE - It's been 2ms since the last message pulse, meaning the message is complete or there was an error. Check the integrity of the message (correct address, message bytes match) and call "runCommand" or go back to idle.
        }
        break;

    case MessageComplete: // The message is complete, look for repeat signals
        if (activated)
        {
            // ADD CODE HERE - A repeat pulse was detected, update the state to "MessageComplete" to reset the state change time.
        }
        else if (now - stateChangeTime > 200000)
        {
            // ADD CODE HERE - There was no repeat pulse for 200ms, meaning the message is over. Call "goIdle"
        }
    }
}

// This function should be run once a command is complete, and should update motors, LEDs, etc. based on said command.
void runCommand(unsigned char command)
{
    switch (command)
    {
    case 9: // Turn on red LED when "Enter/Save" is pressed
        ledRed(1);
        break;

        // ADD CODE HERE
    }
}

// This function should be run when a message ends to reset motors, LEDs, etc.
void goIdle()
{
    ledRed(0);
    // ADD CODE HERE
}