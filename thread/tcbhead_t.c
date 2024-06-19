#include <stdint.h>

typedef void dtv_t;
typedef struct {
    int i[4];
} __128bits;

typedef struct {
    void *tcb;        /* Pointer to the TCB.  Not necessarily the
			   thread descriptor used by libpthread.  */
    dtv_t *dtv;
    void *self;        /* Pointer to the thread descriptor.  */
    int multiple_threads;
    int gscope_flag;
    uintptr_t sysinfo;
    uintptr_t stack_guard;
    uintptr_t pointer_guard;
    unsigned long int vgetcpu_cache[2];
    int __glibc_reserved1;
    int __glibc_unused1;
    /* Reservation of some values for the TM ABI.  */
    void *__private_tm[4];
    /* GCC split stack support.  */
    void *__private_ss;
    long int __glibc_reserved2;
    /* Must be kept even if it is no longer used by glibc since programs,
       like AddressSanitizer, depend on the size of tcbhead_t.  */
    __128bits __glibc_unused2[8][4] __attribute__ ((aligned (32)));

    void *__padding[8];
} tcbhead_t;
