  ORG 100h

loop:
  MOV AH,0   ; Get keypress
  INT 16h
 
  CMP AL, 27 ; Is it ESC key?
  JZ exit

  MOV AH,0ah ; Show
  MOV CX,1
  INT 10h

  JMP loop

exit:
  MOV AH,4Ch
  INT 21h

