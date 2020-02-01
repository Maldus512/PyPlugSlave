# NOTES

- The Reset and Boot pins should be held at state 1 and 0 (respectively) for normal operation. In its first revision the ESP32 uses pins that are possibly in the wrong state at reset (IO13 and IO15). Specifically, IO13 is marked as floating (it was a weak pull down in a previous revision) but IO15 has a weak pull up resistor. In the next revision you should switch them.
- Mettere un diodo piu' grosso sulle due alimentazioni.