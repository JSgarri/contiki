/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
 * Copyright (c) 2018, Zolertia - http://www.zolertia.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup zoul
 * @{
 *
 * \defgroup zoul-examples Zoul examples
 * @{
 *
 * \defgroup zoul-demo Zoul demo application
 *
 *   Example project demonstrating the Zoul communication between them using 
 *   broadcast at 2.4GHz 
 *
 * @{
 *
 *  \author
 *          Javier Sanchez <jsgarri.89@gmail.com>
 *
 * \file
 *     Example demonstrating the Zoul module on the RE-Mote & Firefly platforms
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "cpu.h"
#include "sys/rtimer.h"
#include "dev/uart.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"
#include "net/netstack.h"
#include "net/rime/broadcast.h"
#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
PROCESS(zoul_chat_process, "Zoul chat process");
AUTOSTART_PROCESSES(&zoul_chat_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_toggle(LEDS_BLUE);
  printf("*** Received message from %u:%u:\n     %s\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks bc_rx = { broadcast_recv };
static struct broadcast_conn bc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(zoul_chat_process, ev, data)
{
  PROCESS_BEGIN();
  static uint8_t i = 0;
  static char name[10];
  static char msg[400];
  printf("Zoul chat application\n");

  printf("Enter your nickname:");
  PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message && data != NULL);
  strcpy(name, data);
  printf("Name: %s\n",name );
  name[strlen(name)] = 58;
  name[strlen(name)] = 32;
  
  PROCESS_EXITHANDLER(broadcast_close(&bc));

  /* Disable the radio duty cycle and keep the radio on */
  NETSTACK_MAC.off(1);

  broadcast_open(&bc, BROADCAST_CHANNEL, &bc_rx);

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message && data != NULL);
    for(i = 0; i < 251; i++){
      msg[i] = 0;
    }
    strcat(msg, name);
    strcat(msg, data);
    packetbuf_copyfrom(msg, strlen(msg));
    broadcast_send(&bc);
    leds_toggle(LEDS_RED);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */

