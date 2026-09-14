/****************************************************************************
 * drivers/segger/segger.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/spinlock.h>
#include <nuttx/segger/rtt.h>

#include <SEGGER_RTT.h>

/****************************************************************************
 * Public Data
 ****************************************************************************/

rspinlock_t g_segger_lock = RSPINLOCK_INITIALIZER;
ptrdiff_t g_segger_offset = PTRDIFF_MAX;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: segger_rtt_write_overwrite
 *
 * Description:
 *   Locked ring-buffer write.  Unlike SEGGER_RTT_Write, this overwrites the
 *   OLDEST data when the up-buffer is full, so the newest output is always
 *   available and the caller can never block on a detached debug probe.
 *   The SEGGER_RTT_WriteWithOverwriteNoLock primitive is part of the
 *   upstream RTT library; the lock is taken here so console and syslog
 *   writers stay serialized.
 *
 ****************************************************************************/

unsigned int segger_rtt_write_overwrite(unsigned int channel,
                                        FAR const void *buffer,
                                        unsigned int length)
{
  FAR volatile SEGGER_RTT_CB *rtt;

  /* Match the lazy init performed by the upstream API (INIT macro): the
   * control block is only initialized once, on first use.
   */

  rtt = (FAR volatile SEGGER_RTT_CB *)
        ((uintptr_t)&_SEGGER_RTT + SEGGER_RTT_UNCACHED_OFF);
  if (rtt->acID[0] != 'S')
    {
      SEGGER_RTT_Init();
    }

  SEGGER_RTT_LOCK();
  SEGGER_RTT_WriteWithOverwriteNoLock(channel, buffer, length);
  SEGGER_RTT_UNLOCK();

  return length;
}
