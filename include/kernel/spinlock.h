/**
 * spinlock.h
 *
 * mingxuan
 * 2018-12-26
 */

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <common/type.h>
#include <define/define.h>

struct spinlock {
    u32 locked;  // Is the lock held?

    // For debugging:
    char *name;     // Name of lock.
    int   cpu;      // The number of the cpu holding the lock.
    u32  pcs[10];  // The call stack (an array of program counters)
                    // that locked the lock.
};

void initlock(struct spinlock *lock, char *name);
// Acquire the lock.
// Loops (spins) until the lock is acquired.
// (Because contention is handled by spinning, must not
// go to sleep holding any locks.)
void acquire(struct spinlock *lock);
// Release the lock.
void release(struct spinlock *lock);

#endif /* _SPINLOCK_H_ */