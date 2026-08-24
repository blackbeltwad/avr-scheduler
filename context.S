// hey lol
.text
#include <avr/io.h>
.global TIMER1_COMPA_vect
.global ptrs
TIMER1_COMPA_vect:
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
  LDS R25, 0x005f; SREG REGISTER
  push R25
  LDS R25, 0x005E; SPH
  LDS R24, 0x005D; SPL
  ;ADIW R24, 1 THIS LINE WAS BREAKING EVERYTHING I GUESS POP DECREMENTS BEFORE POPPING 
  CLR R1 
  ; Get the ptrs address. ptrs is 12 bytes total, stacks is 10, cur and max are 1 
  ; Stack is (0 - 9), Cur is (10), Max is (11)
  ; We perform LSL on current because ptrs type is uint16_t, (0 - 1) is one address (2 - 3) is another
  ; On initialization ptrs.stacks[5]. Each index stores 2 bytes
  ; Save the stack location where we pushed SREG
  LDI R30, lo8(ptrs)
  LDI R31, hi8(ptrs)
  LDS R23, ptrs + 10; This is ptrs.cur
  LSL R23; multiply by 2
  ADD R30, R23; apply the address offset
  ADC R30, R1
  ST Z, R24; The address points to the low byte at ptrs.stacks[xx] put the low stack pointer there
  ADIW R30, 1; Move address to the high byte
  ST Z, R25; Store the high pointer there
  
  ; Context switch logic, if cur == max then cur = 0; else cur++
  LDS R23, ptrs + 10 
  LDS R22, ptrs + 11
  CLZ
  CP R23, R22 
  BREQ equal
  BRNE not_equal;

equal:
  LDI R28, lo8(ptrs + 10)
  LDI R29, hi8(ptrs + 10)
  ST Y, R1; R1 value is always supposed to be 0
  LDS R23, ptrs + 10
  LSL R23
  rjmp done
not_equal:
  LDI R28, lo8(ptrs + 10)
  LDI R29, hi8(ptrs + 10)
  LDS R23, ptrs + 10 
  INC R23; cur++
  ST Y, R23
  LSL R23; prepping it for loading the other stack pointer
  rjmp done
done:
  ; LOAD new Stack pointer
  LDI R28, lo8(ptrs)
  LDI R29, hi8(ptrs)
  
  ADD R28, R23; Apply the offset
  ADC R29, R1
  LD R24, Y; Load the low byte of ptrs.stack

  ADIW R28, 1
  LD R25, Y; Load the high byte
  STS 0x005E, R25; Change SPH to new high
  STS 0x005D, R24; Change SPL to low byte
  
  ;Pop everything

  pop R0; Store Sreg is R0 only trigger at the end
  STS 0x005f, R0
  pop R31
  pop R30
  pop R29
  pop R28
  pop R27
  pop R26
  pop R25
  pop R24
  pop R23
  pop R22
  pop R21
  pop R20
  pop R19
  pop R18
  pop R17
  pop R16
  pop R15
  pop R14
  pop R13
  pop R12
  pop R11
  pop R10
  pop R9
  pop R8
  pop R7
  pop R6
  pop R5
  pop R4
  pop R3
  pop R2
  pop R1
  pop R0
  SEI
  RETI; SUCCESSFUL CONTEXT SWITCH
