   ORG 7C00h  ; Dirección donde la BIOS carga el código

   CLI        ; Ignorar las interrupciones
   MOV SI, Prompt  
   MOV AH, 0eh
loop:
   LODSB      ; AL <- DS:[SI], SI++
   OR AL, AL
   JZ exit
   INT 10h
   JMP loop

exit:	
   HLT 

Prompt:	db "ProgHardSO 1.0", 13, 10,0   

times 510 - ($ - $$) db 0  ; Rellenar con ceros
dw 0xAA55  ; hasta los dos últimos bytes: firma bootloader
