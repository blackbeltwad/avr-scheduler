// hey lol
.text
#include <avr/io.h>
.global TIMER0_COMPA_vect
.extern context_switch
TIMER0_COMPA_vect:
  push R0
  push R1
  push R2
  push R4
  push R5
  push R6
  push R7
  push R8
  push R9
  push R10
  push R11
  push R12
  push R13
  push R14
  push R15
  push R16
  push R17
  push R18
  push R19
  push R20
  push R21
  push R22
  push R23
  push R24
  push R25
  push R26
  push R27
  push R28
  push R29
  push R30
  push R31
  call context_switch
  clr R1
  RET
