/* For test  */
/* 测试平台 x86_64 64位 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <x86_64-linux-gnu/asm/unistd_64.h>
#include <sys/prctl.h>
/* For test end */

#define LLL_PRIVATE        0
#define LLL_SHARED        128

#define FUTEX_WAIT        0
#define FUTEX_WAKE        1

/* GCC 内置函数 __sync_synchronize() 和 __sync_lock_test_and_set() */
#define atomic_exchange_rel(mem, value) \
  (__sync_synchronize (), __sync_lock_test_and_set (mem, value))

long int lll_futex_wake(int *futexp, int nr, int private) {
    long int __ret;
    INTERNAL_SYSCALL_DECL(__err);

    __ret = INTERNAL_SYSCALL(futex, __err, 4, (long)futexp, FUTEX_WAKE, nr, 0);

    return __ret;
}
void __lll_lock_wait_private (int *futex)
{
    if (*futex == 2)
        lll_futex_wait (futex, 2, LLL_PRIVATE); /* Wait if *futex == 2.  */

    /* 功能：将 *futex 设置为 2 并返回 *futex 的原来的值。
     所以如果其它线程已经释放了 futex，也就是已经把 *futex
     设置为 0 的话，此时 atomic_exchange_acq() 的调用将会把
     *futex 设置为 2 并返回 0 退出 while，调用线程获得 futex */

    while (atomic_exchange_acq (futex, 2) != 0)
        lll_futex_wait (futex, 2, LLL_PRIVATE); /* Wait if *futex == 2.  */
}

/* Unconditionally set FUTEX to 0 (not acquired), releasing the lock.  If FUTEX
   was >1 (acquired, possibly with waiters), then wake any waiters.  The waiter
   that acquires the lock will set FUTEX to >1.  */
#define __lll_unlock(futex, private)                    \
  ((void)                                               \
   ({                                                   \
     int *__futex = (futex);                            \
     int __oldval = atomic_exchange_rel (__futex, 0);   \
     if (__glibc_unlikely (__oldval > 1))               \
       lll_futex_wake (__futex, 1, private);            \
   }))

#define lll_unlock(futex, private)    \
  __lll_unlock (&(futex), private)


/* CAS */
#define atomic_compare_and_exchange_bool_acq(mem, newval, oldval)    \
    (!__sync_bool_compare_and_swap(mem, oldval, newval))




/* This is an expression rather than a statement even though its value is
   void, so that it can be used in a comma expression or as an expression
   that's cast to void.  */
/* The inner conditional compiles to a call to __lll_lock_wait_private if
   private is known at compile time to be LLL_PRIVATE, and to a call to
   __lll_lock_wait otherwise.  */

/* If FUTEX is 0 (not acquired), set to 1 (acquired with no waiters) and
   return.  Otherwise, ensure that it is >1 (acquired, possibly with waiters)
   and then block until we acquire the lock, at which point FUTEX will still be
   >1.  The lock is always acquired on return. */


void lll_lock(int *futex) {
    if (atomic_compare_and_exchange_bool_acq(futex, 1, 0)) {
        __lll_lock_wait_private(futex);
    }
}



/* 1 if 'type' is a pointer type, 0 otherwise.  */
# define __pointer_type(type) (__builtin_classify_type ((type) 0) == 5)
/* __intptr_t if P is true, or T if P is false.  */
# define __integer_if_pointer_type_sub(T, P) \
  __typeof__ (*(0 ? (__typeof__ (0 ? (T *) 0 : (void *) (P))) 0 \
          : (__typeof__ (0 ? (__intptr_t *) 0 : (void *) (!(P)))) 0))

/* __intptr_t if EXPR has a pointer type, or the type of EXPR otherwise.  */
# define __integer_if_pointer_type(expr) \
  __integer_if_pointer_type_sub(__typeof__ ((__typeof__ (expr)) 0), \
                __pointer_type (__typeof__ (expr)))

/* Cast an integer or a pointer VAL to integer with proper type.  */
#define cast_to_integer(val) ((__integer_if_pointer_type (val)) (val))


/* x86_64 */
/* 使用 xchg 指令实现原子性交换 */
#define atomic_exchange_acq(mem, newvalue) \
  ({ __typeof (*mem) result;                              \
     if (sizeof (*mem) == 1)                              \
       __asm __volatile ("xchgb %b0, %1"                  \
             : "=q" (result), "=m" (*mem)                  \
             : "0" (newvalue), "m" (*mem));                  \
     else if (sizeof (*mem) == 2)                          \
       __asm __volatile ("xchgw %w0, %1"                  \
             : "=r" (result), "=m" (*mem)                  \
             : "0" (newvalue), "m" (*mem));                  \
     else if (sizeof (*mem) == 4)                          \
       __asm __volatile ("xchgl %0, %1"                      \
             : "=r" (result), "=m" (*mem)                  \
             : "0" (newvalue), "m" (*mem));                  \
     else                                                    \
       __asm __volatile ("xchgq %q0, %1"                  \
             : "=r" (result), "=m" (*mem)                  \
             : "0" ((int64_t) cast_to_integer (newvalue)),     \
               "m" (*mem));                                  \
     result; })


/* x86_64 */
# define LOAD_ARGS_0()
# define LOAD_REGS_0
# define ASM_ARGS_0

# define LOAD_ARGS_TYPES_1(t1, a1)                       \
  t1 __arg1 = (t1) (a1);                           \
  LOAD_ARGS_0 ()
# define LOAD_REGS_TYPES_1(t1, a1)                       \
  register t1 _a1 asm ("rdi") = __arg1;                       \
  LOAD_REGS_0
# define ASM_ARGS_1    ASM_ARGS_0, "r" (_a1)
# define LOAD_ARGS_1(a1)                           \
  LOAD_ARGS_TYPES_1 (long int, a1)
# define LOAD_REGS_1                               \
  LOAD_REGS_TYPES_1 (long int, a1)
# define LOAD_ARGS_TYPES_2(t1, a1, t2, a2)                   \
  t2 __arg2 = (t2) (a2);                           \
  LOAD_ARGS_TYPES_1 (t1, a1)
# define LOAD_REGS_TYPES_2(t1, a1, t2, a2)                   \
  register t2 _a2 asm ("rsi") = __arg2;                       \
  LOAD_REGS_TYPES_1(t1, a1)
# define ASM_ARGS_2    ASM_ARGS_1, "r" (_a2)
# define LOAD_ARGS_2(a1, a2)                           \
  LOAD_ARGS_TYPES_2 (long int, a1, long int, a2)
# define LOAD_REGS_2                               \
  LOAD_REGS_TYPES_2 (long int, a1, long int, a2)
# define LOAD_ARGS_TYPES_3(t1, a1, t2, a2, t3, a3)               \
  t3 __arg3 = (t3) (a3);                           \
  LOAD_ARGS_TYPES_2 (t1, a1, t2, a2)
# define LOAD_REGS_TYPES_3(t1, a1, t2, a2, t3, a3)               \
  register t3 _a3 asm ("rdx") = __arg3;                       \
  LOAD_REGS_TYPES_2(t1, a1, t2, a2)
# define ASM_ARGS_3    ASM_ARGS_2, "r" (_a3)
# define LOAD_ARGS_3(a1, a2, a3)                       \
  LOAD_ARGS_TYPES_3 (long int, a1, long int, a2, long int, a3)
# define LOAD_REGS_3                               \
  LOAD_REGS_TYPES_3 (long int, a1, long int, a2, long int, a3)
# define LOAD_ARGS_TYPES_4(t1, a1, t2, a2, t3, a3, t4, a4)           \
  t4 __arg4 = (t4) (a4);                           \
  LOAD_ARGS_TYPES_3 (t1, a1, t2, a2, t3, a3)
# define LOAD_REGS_TYPES_4(t1, a1, t2, a2, t3, a3, t4, a4)           \
  register t4 _a4 asm ("r10") = __arg4;                       \
  LOAD_REGS_TYPES_3(t1, a2, t2, a2, t3, a3)

#define INTERNAL_SYSCALL(name, err, nr, args...)    \
    INTERNAL_SYSCALL_NCS(__NR_##name, err, nr, ##args)


/* x86_64 */
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)    \
    ({                                        \
        unsigned long int resultvar;        \
        LOAD_ARGS_##nr (args)                \
        LOAD_REGS_##nr                        \
        asm volatile (                        \
            "syscall\n\t"                    \
            : "=a" (resultvar)                \
            : "0" (name) ASM_ARGS_##nr         \
            : "memory", "cc", "r11", "cx"     \
        );                                    \
        (long int) resultvar;                 \
    })


#define INTERNAL_SYSCALL_DECL(err)
/* Wait while *FUTEXP == VAL for an lll_futex_wake call on FUTEXP. */
#define lll_futex_wait(futexp, val, private) \
    lll_futex_timed_wait(futexp, val, NULL, private)
/* INTERNAL_SYSCALL 将执行系统调用 sys_futex() */
#define lll_futex_timed_wait(futexp, val, timespec, private)         \
    ({                                                                 \
        INTERNAL_SYSCALL_DECL(__err);                                 \
        long int __ret;                                                 \
        __ret = INTERNAL_SYSCALL(futex, __err, 4,                      \
                        (futexp), FUTEX_WAIT, (val), (timespec));    \
        __ret;                                                         \
    })

#define lll_futex_wake(futexp, nr, private)                \
  ({                                                    \
        INTERNAL_SYSCALL_DECL (__err);                    \
        long int __ret;                                    \
        __ret = INTERNAL_SYSCALL (futex, __err, 4,         \
                       (futexp), FUTEX_WAKE, (nr), 0);    \
        __ret;                                            \
  })




/* These functions don't get included in libc.so  */
/* 不会用到该函数 */
void __lll_lock_wait (int *futex, int private)
{

    if (*futex == 2)
        lll_futex_wait (futex, 2, private); /* Wait if *futex == 2.  */

    while (atomic_exchange_acq (futex, 2) != 0)
        lll_futex_wait (futex, 2, private); /* Wait if *futex == 2.  */
}


/* Test */
#define TASK_NUM 20
#define COUNT 100
static int sum = 0;
static int futex = 0;
static int go = 0;

static void *fn(void *arg)
{
    int i;
    static int n = 'A';
    char name[2] = {'\0'};
    name[0] = (char) (n+(unsigned long int)arg);

    prctl(PR_SET_NAME, name);
    while (!go);

    for (i=0; i<COUNT; i++) {
        lll_lock(&futex);
        sum++;
        lll_unlock(futex, LLL_PRIVATE);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t task[TASK_NUM];
    unsigned long int i;

    for (i=0; i<TASK_NUM; i++)
        pthread_create(&task[i], NULL, fn, (void*) i);
    go = 1;
    for (i=0; i<TASK_NUM; i++)
        pthread_join(task[i], NULL);

    printf("[Main pid=%d] sum=%d (should be %d)\n", getpid(), sum, TASK_NUM*COUNT);
    printf("Test %s\n", sum==TASK_NUM*COUNT ? "PASS" : "FAIL");

    return 0;
}