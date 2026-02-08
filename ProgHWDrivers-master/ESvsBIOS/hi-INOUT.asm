  ORG 100h

loop:
  MOV AL, 0ABh
  IN AL,64h   
  TEST AL,1  ; The bit is never set in DOSBOX
  ;JZ loop

  IN AL,60h  ; Read port A
  MOV AH, AL

  IN AL, 61h  ; ACK
  OR AL, 80h
  OUT 61h, AL
  AND AL, 7Fh
  OUT 61h, AL

  CMP AH,1  ; Is it ESC key?
  JZ exit

  TEST AH,80h ; Is it a release event?
  JNZ loop

  MOV AL, AH  ; Show
  MOV AH,0ah
  MOV CX,1
  INT 10h
  
  JMP loop

exit:
  MOV AH,4Ch ; 
  INT 21h

