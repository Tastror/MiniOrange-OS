
;
; klib.asm
; Forrest Yu, 2005
;

%include "define/sconst.inc"

; 导入全局变量
extern disp_pos
extern saved_color


[SECTION .text]

; 导出函数
; global disp_str
; global disp_color_str
global kern_set_color  ; added by tastror 2022-12
global kern_display_string  ; added by tastror 2022-12
global kern_display_char  ; added by mingxuan 2019-5-19

; ; ========================================================================
; ;                  void disp_str(char * info);
; ; ========================================================================
; disp_str:
;     push    ebp
;     mov    ebp, esp
;     pushad

;     mov    esi, [ebp + 8]    ; pszInfo
;     mov    edi, [disp_pos]
;     mov    ah, 0Fh
; .1:
;     lodsb
;     test    al, al
;     jz    .2
;     cmp    al, 0Ah    ; 是回车吗?
;     jnz    .3
;     push    eax
;     mov    eax, edi
;     mov    bl, 160
;     div    bl
;     and    eax, 0FFh
;     inc    eax
;     mov    bl, 160
;     mul    bl
;     mov    edi, eax
;     pop    eax
;     jmp    .1
; .3:
; ;added by xw, 17/12/11
; ;added begin
;     cmp edi, 1F40h
;     jnz .4
;     mov edi, 0FA0h
; .4:
; ;added end
;     mov    [gs:edi], ax
;     add    edi, 2
;     jmp    .1

; .2:
;     mov    [disp_pos], edi

;     popad
;     pop    ebp
;     ret

; ; ========================================================================
; ;                  void disp_color_str(char * info, int color);
; ; ========================================================================
; disp_color_str:
;     push    ebp
;     mov    ebp, esp
;     pushad

;     mov    esi, [ebp + 8]    ; pszInfo
;     mov    edi, [disp_pos]
;     mov    ah, [ebp + 12]    ; color
; .1:
;     lodsb
;     test    al, al
;     jz    .2
;     cmp    al, 0Ah    ; 是回车吗?
;     jnz    .3
;     push    eax
;     mov    eax, edi
;     mov    bl, 160
;     div    bl
;     and    eax, 0FFh
;     inc    eax
;     mov    bl, 160
;     mul    bl
;     mov    edi, eax
;     pop    eax
;     jmp    .1
; .3:
;     mov    [gs:edi], ax
;     add    edi, 2
;     jmp    .1

; .2:
;     mov    [disp_pos], edi

;     popad
;     pop    ebp
;     ret

; ========================================================================
;           void kern_display_char(char);
; ========================================================================
kern_display_char:
    push ebp
    mov ebp,esp
    
    mov esi, [ebp + 8] 
    mov edi, [disp_pos]
    
    push eax
    mov eax, esi
    mov ah, [saved_color]  ; use color now
    mov [gs:edi], ax
    pop eax
    pop ebp
    ret


; void kern_set_color(int);

kern_set_color:
    push ebp
    mov ebp, esp
    pushad
    
    mov ah, [ebp + 8] 
    mov [saved_color], ah
    
    popad
    pop ebp
    ret

; void kern_display_string(char*);

kern_display_string:
    push ebp
    mov ebp, esp
    pushad

    mov esi, [ebp + 8]    ; pszInfo
    mov edi, [disp_pos]
    mov ah, [saved_color]    ; color

.kern_display_string_1:
    lodsb
    test al, al
    jz .kern_display_string_2
    cmp al, 0Ah
    jnz .kern_display_string_3
    push eax
    mov eax, edi
    mov bl, 160
    div bl
    and eax, 0FFh
    inc eax
    mov bl, 160
    mul bl
    mov edi, eax
    pop eax
    jmp .kern_display_string_1

.kern_display_string_3:
    mov [gs:edi], ax
    add edi, 2
    jmp .kern_display_string_1

.kern_display_string_2:
    mov [disp_pos], edi

    popad
    pop ebp
    ret
