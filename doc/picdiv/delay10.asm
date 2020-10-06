;
; Below is a collection fixed delay loops for a PIC at 10 MHz
;
; - The times are cycle perfect and include the call and return.
; - With a 10 MHz clock the instruction execution time is 400 ns.
;
;   24-Oct-2010  Tom Van Baak (tvb)  www.LeapSecond.com/pic
;
; ----------------------------------------------------------------------------

; Delay exactly 1 ms (which is 2,500 PIC cycles at 10 MHz).

Delay1ms:                       ; (      2) call
        movlw   d'24'           ; (      1)
        call    DelayW100       ; (   2400) delay W*100
        movlw   d'94'           ; (      1)
        call    DelayW1         ; (     94) delay (15 <= W <= 255)
        return                  ; (      2)

; Delay exactly 2 ms (which is 5,000 PIC cycles at 10 MHz).

Delay2ms:                       ; (      2) call
        movlw   d'49'           ; (      1)
        call    DelayW100       ; (   4900) delay W*100
        movlw   d'94'           ; (      1)
        call    DelayW1         ; (     94) delay (15 <= W <= 255)
        return                  ; (      2)

; Delay exactly 5 ms (which is 12,500 PIC cycles at 10 MHz).

Delay5ms:                       ; (      2) call
        movlw   d'124'          ; (      1)
        call    DelayW100       ; (  12400) delay W*100
        movlw   d'94'           ; (      1)
        call    DelayW1         ; (     94) delay (15 <= W <= 255)
        return                  ; (      2)

; ----------------------------------------------------------------------------

; Delay exactly 10 ms (which is 25,000 PIC cycles at 10 MHz).

Delay10ms:                      ; (      2) call
        movlw   d'2'            ; (      1)
        call    DelayW10k       ; (  20000) delay W*10000
        movlw   d'49'           ; (      1)
        call    DelayW100       ; (   4900) delay W*100
        movlw   d'93'           ; (      1)
        call    DelayW1         ; (     93) delay (15 <= W <= 255)
        return                  ; (      2)

; Delay exactly 20 ms (which is 50,000 PIC cycles at 10 MHz).

Delay20ms:                      ; (      2) call
        movlw   d'4'            ; (      1)
        call    DelayW10k       ; (  40000) delay W*10000
        movlw   d'99'           ; (      1)
        call    DelayW100       ; (   9900) delay W*100
        movlw   d'93'           ; (      1)
        call    DelayW1         ; (     93) delay (15 <= W <= 255)
        return                  ; (      2)

; Delay exactly 50 ms (which is 125,000 PIC cycles at 10 MHz).

Delay50ms:                      ; (      2) call
        movlw   d'12'           ; (      1)
        call    DelayW10k       ; ( 120000) delay W*10000
        movlw   d'49'           ; (      1)
        call    DelayW100       ; (   4900) delay W*100
        movlw   d'93'           ; (      1)
        call    DelayW1         ; (     93) delay (15 <= W <= 255)
        return                  ; (      2)

; ----------------------------------------------------------------------------

; Delay exactly 100 ms (which is 250,000 PIC cycles at 10 MHz).

Delay100ms:                     ; (      2) call
        movlw   d'24'           ; (      1)
        call    DelayW10k       ; ( 240000) delay W*10000
        movlw   d'99'           ; (      1)
        call    DelayW100       ; (   9900) delay W*100
        movlw   d'93'           ; (      1)
        call    DelayW1         ; (     93) delay (15 <= W <= 255)
        return                  ; (      2)

; Delay exactly 200 ms (which is 500,000 PIC cycles at 10 MHz).

Delay200ms:                     ; (      2) call
        movlw   d'49'           ; (      1)
        call    DelayW10k       ; ( 490000) delay W*10000
        movlw   d'99'           ; (      1)
        call    DelayW100       ; (   9900) delay W*100
        movlw   d'93'           ; (      1)
        call    DelayW1         ; (     93) delay (15 <= W <= 255)
        return                  ; (      2)

; Delay exactly 500 ms (which is 1,250,000 PIC cycles at 10 MHz).

Delay500ms:                     ; (      2) call
        movlw   d'124'          ; (      1)
        call    DelayW10k       ; (1240000) delay W*10000
        movlw   d'99'           ; (      1)
        call    DelayW100       ; (   9900) delay W*100
        movlw   d'93'           ; (      1)
        call    DelayW1         ; (     93) delay (15 <= W <= 255)
        return                  ; (      2)

; ----------------------------------------------------------------------------

        include delayw.asm      ; precise delay functions
