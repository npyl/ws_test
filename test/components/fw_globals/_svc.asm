
;-------------------------------------------------------------------------------
; SWI wrapper
        .global     c_svc
        .global     _svc
        .text
        .arm
        .armfunc    _svc

   .align 4
_svc:
        .asmfunc
        ; Preserver A1 and A2 these may hold parameters to the function which are needed in C level handler
        ; Note: This handler doesn't preserve callee saved (Save-on-call) registers, A1 to A4 and V9.
        ;       In other words the callee function has to preserve them which is ensured when function like SVC is used in C (#pragma SWI_ALIAS() or __svc())
        ;       Take care when assembly inlining SVC / SWI.

        MRS     A4, SPSR            ; Get spsr

        TST     A4, #0x20           ; Called in Thumb state?

        ; Note: When called from Thumb code only 256 unique SVC handlers can be distungished, as the Thumb SVC instruction has only a 8 bit field.
        ;       When called from ARM code 2^24 unique SVC handlers can be distungished, as the ARM SVC instruction has a 24 bit field.

        LDRNEH  A3, [lr,#-2]        ; Yes: Load halfword and...
        BICNE   A3, A3, #0xFF00     ; ...extract comment field
        LDREQ   A3, [lr,#-4]        ; No: Load word and...
        BICEQ   A3, A3, #0xFF000000 ; ...extract comment field
                                    ; r2/A3 now contains SVC number
                                    ; r3/A4 now contains SPSR (Saved Program Status Register)

        CMP     A3, #32
        BHI     _default            ; Branch if higher

		LDRLS   pc, [pc, A3, LSL #2]; Load address from table

		.word   0x00

_table: .word   (_case0)  ; unimplementedSVC
        .word   (_case1)  ; switchCpuMode
        .word   (_case2)  ; switchToSystemMode
        .word   (_case3)  ; switchToUserMode
        .word   (_case4)
        .word   (_case5)
        .word   (_case6)
        .word   (_case7)
        .word   (_case8)
        .word   (_case9)
        .word   (_case10)
        .word   (_case11)
        .word   (_case12)
        .word   (_case13)
        .word   (_case14)
        .word   (_case15)
        .word   (_case16)
        .word   (_case17)
        .word   (_case18)
        .word   (_case19)
        .word   (_case20)
        .word   (_case11)
        .word   (_case22)
        .word   (_case23)
        .word   (_case24)
        .word   (_case25)
        .word   (_case26)
        .word   (_case27)
        .word   (_case28)
        .word   (_case29)
        .word   (_case30)
        .word   (_case31)
        .word   (_case32)

        .word   0x00

_case0: ; unimplementedSVC (used to test fault handler)
        B       _default

_case1: ; switchCpuMode
        AND     A2, A1, #0x0000001F ; Ensure that only mode bits are in A1
        AND     A1, A4, #0x0000001F ; Store mode on entry in R0/A1 to return it to callee
        BIC     A4, A4, #0x0000001F ; Clear Mode bits
        ORR     A4, A4, A2          ; Set Mode bits as in A2 (former A1)
        MSR     SPSR_cxsf, A4       ; Restore spsr
        B       _exit_svc           ; Branch to exit handler

_case2: ; switchToSystemMode
        ;BIC     A4, A4, #0x0000001F
        ORR     A4, A4, #0x0000001F ; Set bits fro System Mode (M0-M4 are set)
        MSR     SPSR_cxsf, A4       ; Restore spsr
        B       _exit_svc

_case3: ; switchToUserMode
        BIC     A4, A4, #0x0000001F ; Clear Mode bits
        ORR     A4, A4, #0x00000010 ; Set Mode Bits for User Mode
        MSR     SPSR_cxsf, A4       ; Restore spsr
        B       _exit_svc

_case4:
_case5:
_case6:
_case7:
_case8:
_case9:
_case10:
_case11:
_case12:
_case13:
_case14:
_case15:
_case16:
_case17:
_case18:
_case19:
_case20:
_case21:
_case22:
_case23:
_case24:
_case25:
_case26:
_case27:
_case28:
_case29:
_case30:
_case31:
_case32:
        B       _default

_default:
        ; A4 holds SPSR
        STMFD   SP!, {A4, LR}    ; Store SPSR and LR on Stack

        BL      c_svc            ; Call _TI_SWI

        LDMFD   SP!, {A4, LR}    ; Get SPSR and LR from stack
        MSR     SPSR_cxsf, A4    ; Restore spsr

_exit_svc:
        MOVS PC, LR              ; Return from Exception

        .endasmfunc

        .end
