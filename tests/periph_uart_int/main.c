/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level UART driver in blocking mode
 *
 * This application tests the interrupt driven mode for the low-level UART driver.
 *
 * The test will read characters from the UART into a receiving buffer, until a newline is received.
 * When this happens, the received string will be printed to stdout.
 *
 * In the same time, a string is written to the UART every 2 seconds, to make sure the transmission
 * is working as well.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "msg.h"
#include "thread.h"
#include "board.h"
#include "vtimer.h"
#include "ringbuffer.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* only build this test if the UART driver is supported */
#if UART_NUMOF

#define DEV             UART_DEV(0)
#define BAUD            115200

static volatile int main_pid;

static char uart_stack[THREAD_STACKSIZE_MAIN];

static char rx_mem[128];
static ringbuffer_t rx_buf;

static void rx_cb(void *arg, uint8_t data)
{
    msg_t msg;

    ringbuffer_add_one(&rx_buf, data);
    if (data == '\n') {
        msg_send(&msg, main_pid);
    }
}

void *uart_thread(void *arg)
{
    (void)arg;
    char *status = "I am written to the UART every 2 seconds\n";

    while (1) {
        uart_write(DEV, (uint8_t *)status, strlen(status) + 1);
        vtimer_usleep(2000 * 1000);
    }

    return 0;
}

int main(void)
{
    int ret;

    main_pid = thread_getpid();

    puts("\nTesting interrupt driven mode of UART driver\n");

    puts("Setting up buffers...");
    ringbuffer_init(&rx_buf, rx_mem, 128);

    printf("Initializing UART @ %i", BAUD);

    ret = uart_init(DEV, BAUD, rx_cb, 0);

    if (ret == -1) {
        printf("Error: Given baudrate (%u) not possible\n",
        (unsigned int)BAUD);
        return 1;
    }
    else if (ret < -1) {
        puts("Error: Unable to initialize UART device\n");
        return 1;
    }
    printf("Successfully initialized UART_DEV(%i)\n", DEV);

    puts("Starting timer thread that triggers UART output...");
    thread_create(uart_stack,
                   THREAD_STACKSIZE_MAIN,
                   THREAD_PRIORITY_MAIN - 1,
                   0,
                   uart_thread,
                   0,
                   "uart");

    while (1) {
        msg_t msg;
        msg_receive(&msg);

        printf("RECEIVED INPUT: ");
        char buf[128];
        int res = ringbuffer_get(&rx_buf, buf, rx_buf.avail);
        buf[res] = '\0';
        printf("%s", buf);
    }

    return 0;
}

#else

int main(void)
{
    puts("This platform does not support the low-level UART driver");

    return 0;
}

#endif /* UART_NUMOF */
