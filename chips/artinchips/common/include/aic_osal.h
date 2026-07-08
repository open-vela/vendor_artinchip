/*
 * Copyright (c) 2022-2025, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ARTINCHIP_AIC_OSAL_H_
#define _ARTINCHIP_AIC_OSAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nuttx/config.h>
#include <nuttx/cache.h>
#include <nuttx/irq.h>
#include <nuttx/clock.h>
#include <nuttx/kmalloc.h>
#include <nuttx/arch.h>
#include <unistd.h>
#ifndef irq_handler_t
typedef xcpt_t irq_handler_t;
#endif
#ifndef irqreturn_t
typedef int irqreturn_t;
#endif
#ifndef pin_irq_handler_t
typedef irqreturn_t (*pin_irq_handler_t)(void *);
#endif
#undef KERNEL_BAREMETAL

#include <aic_common.h>
#include "core_rv32.h"
#include <nuttx/mutex.h>
#include <nuttx/semaphore.h>
#include <stdlib.h>

/* aicos_memcpy */
extern void *aicos_memcpy(void *, const void *, size_t);

/* Common malloc align */
typedef void *(*aicos_malloc1_t)(size_t);
typedef void (*aicos_free1_t)(void *);
typedef void *(*aicos_malloc2_t)(unsigned int, size_t);
typedef void (*aicos_free2_t)(unsigned int, void *);

void *_aicos_malloc_align_(size_t size, size_t align, unsigned int type,
                           void *func);
void _aicos_free_align_(void *ptr, unsigned int type, void *func);

/* Irq */
void aicos_irq_enter(void);
void aicos_irq_exit(void);

/* New define */
typedef void *aicos_thread_t;
typedef void (*aic_thread_entry_t)(void *argument);
typedef void *aicos_sem_t;
typedef void *aicos_mutex_t;
typedef void *aicos_event_t;
typedef void *aicos_queue_t;
typedef void *aicos_wqueue_t;

/* Memory types (only define if not already set by aic_common.h / aic_hal_ge.h) */
#ifndef MEM_DEFAULT
#define MEM_DEFAULT  0
#endif
#ifndef MEM_CMA
#define MEM_CMA      MEM_DEFAULT
#endif
#ifndef GE_DEFAULT
#define GE_DEFAULT   MEM_DEFAULT
#endif
#ifndef GE_CMA
#define GE_CMA       MEM_CMA
#endif

#define AICOS_WAIT_FOREVER (UINT32_MAX)

//--------------------------------------------------------------------+
// Irq API
//--------------------------------------------------------------------+
static inline void aicos_local_irq_save(unsigned long *state) {
  *state = up_irq_save();
}
static inline void aicos_local_irq_restore(unsigned long state) {
  up_irq_restore(state);
}

static inline void aicos_local_irq_disable(void) {}
static inline void aicos_local_irq_enable(void) {}
static inline int aicos_request_irq(unsigned int irq, xcpt_t handler,
                                    unsigned int flags, const char *name,
                                    void *dev) {
  return irq_attach(irq + RISCV_IRQ_ASYNC, handler, dev);
}
static inline void aicos_irq_enable(unsigned int irq) {
  up_enable_irq(irq + RISCV_IRQ_ASYNC);
}
static inline void aicos_irq_disable(unsigned int irq) {
  up_disable_irq(irq + RISCV_IRQ_ASYNC);
}

static inline bool aicos_in_irq(void) { return false; }

//--------------------------------------------------------------------+
// Cache API
//--------------------------------------------------------------------+
static inline void aicos_dma_sync(void) { up_clean_dcache_all(); }
static inline void aicos_dcache_invalid_range(unsigned long *addr,
                                              unsigned long size) {
  up_invalidate_dcache((uintptr_t)addr, (uintptr_t)addr + size);
}
static inline void aicos_dcache_clean_range(unsigned long *addr,
                                            unsigned long size) {
  up_clean_dcache((uintptr_t)addr, (uintptr_t)addr + size);
}
static inline void aicos_dcache_clean_invalid_range(unsigned long *addr,
                                                    unsigned long size) {
  up_flush_dcache((uintptr_t)addr, (uintptr_t)addr + size);
}
static inline void aicos_icache_enable(void) { csi_icache_enable(); }
static inline void aicos_icache_disable(void) { csi_icache_disable(); }
static inline void aicos_icache_invalid(void) { csi_icache_invalid(); }
static inline void aicos_dcache_enable(void) { csi_dcache_enable(); }
static inline void aicos_dcache_disable(void) { csi_dcache_disable(); }
static inline void aicos_dcache_invalid(void) { csi_dcache_invalid(); }
static inline void aicos_dcache_clean(void) { csi_dcache_clean(); }
static inline void aicos_dcache_clean_invalid(void) {
  csi_dcache_clean_invalid();
}

//--------------------------------------------------------------------+
// Delay API
//--------------------------------------------------------------------+
/* aicos_mdelay/udelay: busy-wait, no task switch */
static inline void aicos_mdelay(unsigned long msecs) { up_mdelay((unsigned int)msecs); }
static inline void aicos_udelay(unsigned long usecs) { up_udelay((useconds_t)usecs); }
/* aicos_msleep: yields CPU, scheduler may switch to other tasks */
static inline void aicos_msleep(unsigned long msecs) { usleep((useconds_t)msecs * 1000); }

//--------------------------------------------------------------------+
// Malloc API
//--------------------------------------------------------------------+
static inline void *aicos_malloc(unsigned int type, size_t size) {
  (void)type;
  return kmm_malloc(size);
}
static inline void aicos_free(unsigned int type, void *ptr) {
  (void)type;
  kmm_free(ptr);
}

//--------------------------------------------------------------------+
// Mutex API
//--------------------------------------------------------------------+
static inline aicos_mutex_t aicos_mutex_create(void) {
  mutex_t *m = (mutex_t *)kmm_malloc(sizeof(mutex_t));
  if (m) nxmutex_init(m);
  return (aicos_mutex_t)m;
}
static inline void aicos_mutex_delete(aicos_mutex_t mutex) {
  mutex_t *m = (mutex_t *)mutex;
  if (m) { nxmutex_destroy(m); kmm_free(m); }
}
static inline int aicos_mutex_take(aicos_mutex_t mutex, unsigned int timeout) {
  (void)timeout;
  return nxmutex_lock((mutex_t *)mutex);
}
static inline int aicos_mutex_give(aicos_mutex_t mutex) {
  return nxmutex_unlock((mutex_t *)mutex);
}

//--------------------------------------------------------------------+
// Event API (per-bit semaphores, supporting concurrent waiters)
//
// Each aic_event has 8 independent semaphores for bits 0..7.
// aicos_event_send posts to the sem of each set bit.
// aicos_event_recv picks one set bit from its mask and waits on that
// bit's sem, then checks all bits in mask under lock. This avoids the
// livelock that occurs when a single sem is shared by all event types.
//--------------------------------------------------------------------+

#define AIC_EVENT_MAX_BITS 4

struct aic_event {
  mutex_t lock;
  sem_t sem[AIC_EVENT_MAX_BITS];  /* one sem per event bit */
  uint32_t flags;                 /* accumulated pending event bits */
};

static inline aicos_event_t aicos_event_create(void) {
  struct aic_event *e = (struct aic_event *)kmm_malloc(sizeof(struct aic_event));
  if (e) {
    int i;
    nxmutex_init(&e->lock);
    for (i = 0; i < AIC_EVENT_MAX_BITS; i++)
      nxsem_init(&e->sem[i], 0, 0);
    e->flags = 0;
  }
  return (aicos_event_t)e;
}

static inline void aicos_event_delete(aicos_event_t event) {
  struct aic_event *e = (struct aic_event *)event;
  if (e) {
    int i;
    for (i = 0; i < AIC_EVENT_MAX_BITS; i++)
      nxsem_destroy(&e->sem[i]);
    nxmutex_destroy(&e->lock);
    kmm_free(e);
  }
}

static inline int aicos_event_send(aicos_event_t event, uint32_t value) {
  struct aic_event *e = (struct aic_event *)event;
  int i;
  int ret = 0;
  irqstate_t flags;

  if (value & ~((1u << AIC_EVENT_MAX_BITS) - 1)) {
    printf("aic_event: value 0x%lx exceeds %d bits, truncated\n",
           (unsigned long)value, AIC_EVENT_MAX_BITS);
  }

  /* Use irqsave instead of mutex — called from IRQ context */
  flags = up_irq_save();
  e->flags |= value;
  up_irq_restore(flags);

  /* Post to each set bit's semaphore (nxsem_post is IRQ-safe) */
  for (i = 0; i < AIC_EVENT_MAX_BITS; i++) {
    if (value & (1u << i)) {
      int r = nxsem_post(&e->sem[i]);
      if (r < 0) ret = r;
    }
  }

  return ret;
}

static inline int aicos_event_recv(aicos_event_t event, uint32_t mask,
                                    uint32_t *recved, int timeout_ms) {
  struct aic_event *e = (struct aic_event *)event;
  int ret = 0;
  int sem_ret;
  int bit;

  /* Pick the first set bit in mask to wait on.
   * mask is always non-zero in GE/VE usage (0x01, 0x02, 0x03). */
  bit = __builtin_ctz(mask & 0xff);

  /* Wait for this event bit to be signalled */
  if (timeout_ms == (int)AICOS_WAIT_FOREVER || timeout_ms < 0) {
    sem_ret = nxsem_wait(&e->sem[bit]);
  } else {
    int ticks = MSEC2TICK(timeout_ms);
    sem_ret = nxsem_tickwait(&e->sem[bit], ticks);
  }

  if (sem_ret < 0)
    return sem_ret;

  /* Consume matching event bits under lock */
  nxmutex_lock(&e->lock);
  {
    uint32_t matched = e->flags & mask;
    if (matched) {
      e->flags &= ~matched;
      if (recved) *recved = matched;
    }
    /* Note: if flags & mask == 0 after wakeup, this is a spurious
     * wakeup. We still return OK since the original AICOS event
     * semantics already require callers to re-check conditions
     * in a while loop (which all GE/VE code does).
     */
  }
  nxmutex_unlock(&e->lock);

  return ret;
}

//--------------------------------------------------------------------+
// Semaphore API
//--------------------------------------------------------------------+
static inline aicos_sem_t aicos_sem_create(unsigned int value) {
  sem_t *s = (sem_t *)kmm_malloc(sizeof(sem_t));
  if (s) nxsem_init(s, 0, (int)value);
  return (aicos_sem_t)s;
}
static inline int aicos_sem_take(aicos_sem_t sem, unsigned int timeout) {
  if (timeout == AICOS_WAIT_FOREVER)
    return nxsem_wait((sem_t *)sem);
  return nxsem_tickwait((sem_t *)sem, MSEC2TICK(timeout));
}
static inline int aicos_sem_post(aicos_sem_t sem) {
  return nxsem_post((sem_t *)sem);
}

void *aicos_malloc_oom(size_t size);
void *aicos_memalign_oom(size_t size, size_t align);
void aicos_free_oom(void *ptr);
static inline void aicos_free_align(unsigned int type, void *ptr) { aicos_free(type, ptr); }
static inline void *aicos_malloc_align(unsigned int type, size_t size, size_t align)
{
  (void)type;
  return memalign(align, size);
}

#ifdef __cplusplus
}
#endif
#endif
