; bool compare_and_swap(std::atomic<int>* x, int a, int b)
;
; Atomically compares the value at the memory location pointed to by x
; with the value a. If they are equal, the value b is stored at the
; memory location pointed to by x. The original value at the memory
; location is returned.
;
; Arguments:
;   x (rdi) - pointer to the atomic integer value to be compared and swapped
;   a (esi) - the value to compare with
;   b (edx) - the new value to swap in if the comparison succeeds
;
; Returns:
;   The original value at the memory location pointed to by x (eax)

section .text
global compare_and_swap

compare_and_swap:
    mov eax, esi        ; Move a into eax
    lock cmpxchg [rdi], edx ; Atomically compare *x with eax
                        ; If equal, swap in rdx (b)
                        ; Otherwise, load *x into eax
    setz al             ; Set al to 1 if ZF = 1, 0 otherwise
    ret                 ; Return the original value of *x and success/failure flag