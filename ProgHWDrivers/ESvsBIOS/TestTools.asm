  ORG 100h

  MOV AL, 'X' ; Escribir 2000 'X'
  MOV BL, 1fh ; en blanco sobre azul
  MOV CX,2000

  MOV AH, 9h
  INT 10h

  MOV AH,4Ch  ; Salir al DOS
  INT 21h

