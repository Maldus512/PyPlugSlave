# PyPlug slave firmware

This repository contains the firmware of the slave STM32 on the PyPlug board. 

## Compilation

This projects uses SCons as build tool. To compile it, run `scons`.

## Communication Protocol

The PyPlug slave respons to a very simple command-response protocol. Commands are strings terminated by a single newline characted (`\n`).
After it has been received the slave responds to the command with either "ERR\n" or "OK\n", depending on the outcome. In case of error outcome the communication is done; otherwise other lines might be printed that represent the command's output response (terminated by a line feed as well).

The exchange of commands and responses is regulated by the newline character, without fancy carriage returns; this is because:

 - The command interface is meant to be simple, but not to be handled directly by a human user (although it is not impossible)
 - The PyPlug slave should not be connected to a teleprinter
 - It is generally assumed the protocol will be used by civilized people and not savage apes

 Currently, there are 4 supported commands:
    
#### READ
Reads the current consumption by the load and returns it after the "OK\n" signal. It is a floating point number.

#### TEMP
Reads the temperature of the on-board I2C sensor and returns it after the "OK\n" signal. It is a floating point number (Celsius degrees).

#### ON
Activates the load. There is no response beyond "OK\n".

#### OFF
Deactivates the load. There is no response beyond "OK\n".

## TODO

 - Save log data at power off of the device - possibly on the internal EEPROM of the STM32 MCU instead of the I2C IC EEPROM


## Notes

- The Reset and Boot pins should be held at state 1 and 0 (respectively) for normal operation. In its first revision the ESP32 uses pins that are possibly in the wrong state at reset (IO13 and IO15). Specifically, IO13 is marked as floating (it was a weak pull down in a previous revision) but IO15 has a weak pull up resistor. In the next revision you should switch them.