/*
 * Serial/UART emulation for Elkulator.
 *
 * See: SCN2681 Dual asynchronous receiver/transmitter (DUART)
 *
 * Copyright (C) 2022, 2023 Paul Boddie <paul@boddie.org.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include "elk.h"



/* IRQ status. */

int serial_irq = 0;

/* Debugging control. */

int serial_debug = 0;

// TODO: Convert following code to be cross platform
#ifndef WIN32
/* Socket communication. */

static int socket_fd = -1;


/* Serial cycle counters. */

static int receive_cycles, transmit_cycles;



/* Serial register locations in the Electron memory map. */

enum sr_fc_address
{
        FC_BASE  = 0xFC60,
        FC_MRxA  = 0xFC60, /* Mode register A (MR1A, MR2A) */
        FC_SRA   = 0xFC61, /* Status register A (SRA) */
        FC_CSRA  = 0xFC61, /* Clock select register A (CSRA) */
        FC_CRA   = 0xFC62, /* Command register A (CRA) */
        FC_RHRA  = 0xFC63, /* Receive holding register A (RHRA) */
        FC_THRA  = 0xFC63, /* Transmit holding register A (THRA) */
        FC_IPCR  = 0xFC64, /* Input port change register (IPCR) */
        FC_ACR   = 0xFC64, /* Aux control register (ACR) */
        FC_ISR   = 0xFC65, /* Interrupt status register (ISR) */
        FC_IMR   = 0xFC65, /* Interrupt mask register (IMR) */
        FC_MRxB  = 0xFC68, /* Mode register B (MR1B, MR2B) */
        FC_SRB   = 0xFC69, /* Status register B (SRB) */
        FC_CSRB  = 0xFC69, /* Clock select register B (CSRB) */
        FC_CRB   = 0xFC6A, /* Command register B (CRB) */
        FC_RHRB  = 0xFC6B, /* Receive holding register B (RHRB) */
        FC_THRB  = 0xFC6B, /* Transmit holding register B (THRB) */
        FC_IP06  = 0xFC6D, /* Input ports IP0 to IP6 */
        FC_OPCR  = 0xFC6D, /* Output port configuration register (OPCR) */
        FC_STCC  = 0xFC6E, /* Start counter command */
        FC_SOPBC = 0xFC6E, /* Set output port bits command */
        FC_SPCC  = 0xFC6F, /* Stop counter command */
        FC_ROPBC = 0xFC6F  /* Reset output port bits command */
};

/* Serial register storage. */

static uint8_t sr[10];

/* Serial register locations for registers requiring storage. */

enum sr_pos
{
        IPCR, ACR, ISR, IMR, CTU, CRUR, CTL, CTLR, IP06, OPCR
};

static const int SR_max = OPCR;

/* Mode register locations. */

enum MR_indexes
{
        MR1 = 0, MR2 = 1
};

/* Register field definitions. */

enum MR1_bits
{
        MR1_RxRTS = 0x80,
        MR1_RxINT_FFULL = 0x40,
        MR1_block_error = 0x20,
        MR1_parity_mode = 0x18,
        MR1_parity_mode_shift = 3,
        MR1_parity_type = 0x04,
        MR1_bpc = 0x03
};

enum MR2_bits
{
        MR2_channel = 0xc0,
        MR2_channel_shift = 6,
        MR2_TxRTS = 0x20,
        MR2_TxCTS = 0x10,
        MR2_stop_bits = 0x0f
};

enum SR_bits
{
        SR_receive_break = 0x80,
        SR_framing_error = 0x40,
        SR_parity_error = 0x20,
        SR_overrun_error = 0x10,
        SR_TxEMT = 0x08,
        SR_TxRDY = 0x04,
        SR_FFULL = 0x02,
        SR_RxRDY = 0x01
};

enum OPCR_bits
{
        OPCR_TxRDYB = 0x80,
        OPCR_TxRDYA = 0x40,
        OPCR_RxRDY_FFULL_B = 0x20,
        OPCR_RxRDY_FFULL_A = 0x10,
        OPCR_OP3 = 0x0c,
        OPCR_OP3_shift = 2,
        OPCR_OP2 = 0x03
};

enum CSR_bits
{
        CSR_receive = 0xf0,
        CSR_receive_shift = 4,
        CSR_transmit = 0x0f
};

enum CR_bits
{
        CR_command = 0x70,
        CR_command_shift = 4,
        CR_disable_Tx = 0x08,
        CR_enable_Tx = 0x04,
        CR_disable_Rx = 0x02,
        CR_enable_Rx = 0x01
};

enum CR_command_values
{
        CR_command_reset_MR = 1,
        CR_command_reset_receiver = 2,
        CR_command_reset_transmitter = 3,
        CR_command_reset_error = 4,
        CR_command_reset_break_change = 5,
        CR_command_start_break = 6,
        CR_command_stop_break = 7
};

enum ACR_bits
{
        ACR_BRG = 0x80,
        ACR_BRG_shift = 7,
        ACR_CTMS = 0x70,
        ACR_CTMS_shift = 4,
        ACR_delta_IP3 = 0x08,
        ACR_delta_IP2 = 0x04,
        ACR_delta_IP1 = 0x02,
        ACR_delta_IP0 = 0x01
};

enum IMR_bits
{
        IMR_IP_change = 0x80,
        IMR_delta_break_B = 0x40,
        IMR_RxRDY_FFULL_B = 0x20,
        IMR_TxRDYB = 0x10,
        IMR_counter_ready = 0x08,
        IMR_delta_break_A = 0x04,
        IMR_RxRDY_FFULL_A = 0x02,
        IMR_TxRDYA = 0x01
};

enum ISR_bits
{
        ISR_IP_change = 0x80,
        ISR_delta_break_B = 0x40,
        ISR_RxRDY_FFULL_B = 0x20,
        ISR_TxRDYB = 0x10,
        ISR_counter_ready = 0x08,
        ISR_delta_break_A = 0x04,
        ISR_RxRDY_FFULL_A = 0x02,
        ISR_TxRDYA = 0x01
};

enum IP06_bits
{
        IP06_IP2 = 0x04,
        IP06_CTSBN = 0x02,
        IP06_CTSAN = 0x01
};

enum OPBC_bits
{
        OPBC_RTSBN = 0x02,
        OPBC_RTSAN = 0x01
};



/* Readable forms for register names and various fields. */

static char *sr_read_names[] = {
        "MRxA", "SRA", "-", "RHRA", "IPCR", "ISR", "CTU", "CTL",
        "MRxB", "SRB", "-", "RHRB", "-", "IP0-6", "STCC", "SPCC"
        };

static char *sr_write_names[] = {
        "MRxA", "CSRA", "CRA", "THRA", "ACR", "IMR", "CRUR", "CTLR",
        "MRxB", "CSRB", "CRB", "THRB", "-", "OPCR", "SOPBC", "ROPBC"
        };

static char *parity_mode[] = {
        "with parity", "force parity", "no parity", "multidrop"
        };

static char *channel_mode[] = {
        "normal", "auto-echo", "local loop", "remote loop"
        };

static char *commands[] = {
        "no command", "reset MR", "reset receiver", "reset transmitter",
        "reset error", "reset break change A", "start break", "stop break"
        };

static char *stop_bits[] = {
        "0.563", "0.625", "0.688", "0.750", "0.813", "0.875", "0.938", "1.000",
        "1.563", "1.625", "1.688", "1.750", "1.813", "1.875", "1.938", "2.000"
        };

static char *stop_bits_5bpc[] = {
        "1.063", "1.125", "1.188", "1.250", "1.313", "1.375", "1.438", "1.500",
        "2.063", "2.125", "2.188", "2.250", "2.313", "2.375", "2.438", "2.500"
        };

static char *opcr_op2[] = {
        "OPR[2]", "TxCA(16x)", "TxCA(1x)", "RxCA(1x)"
        };

static char *opcr_op3[] = {
        "OPR[3]", "C/T output", "TxCB(1x)", "RxCB(1x)"
        };



/* Baud rate labels. */

static char *baud_rate_labels[2][16] = {
        {
                "50", "110", "134.5", "200", "300", "600", "1200", "1050",
                "2400", "4800", "7200", "9600", "38400", "timer", "IPx-16X", "IPx-1X"
        },
        {
                "75", "110", "134.5", "150", "300", "600", "1200", "2000",
                "2400", "4800", "1800", "9600", "19200", "timer", "IPx-16X", "IPx-1X"
        }};

/* Baud rates with unsupported rates represented by zero. */

static float baud_rates[2][16] = {
        {
                50, 110, 134.5, 200, 300, 600, 1200, 1050,
                2400, 4800, 7200, 9600, 38400, 0, 0, 0
        },
        {
                75, 110, 134.5, 150, 300, 600, 1200, 2000,
                2400, 4800, 1800, 9600, 19200, 0, 0, 0
        }};

/* Functions for accessing baud rate settings. */

static int baud_rate_group()
{
        return (sr[ACR] & ACR_BRG) >> ACR_BRG_shift;
}



/* Input port handling. */

static uint8_t get_input_port_change()
{
        uint8_t changed = 0;
        int i;

        /* Use the previous contents of IPCR together with IP06 for IP0..3 to
           determine the new contents. */

        for (i = 0; i < 4; i++)
        {
                if ((sr[IP06] & (1 << i)) != (sr[IPCR] & (1 << i)))
                        changed |= (1 << i);
        }

        /* IPCR = delta (IP3..0) | IP3..0 */

        sr[IPCR] = (changed << 4) | (sr[IP06] & 0x0f);
        return changed;
}

static uint8_t read_input_port_change()
{
        /* Reset the interrupt status flag. */

        sr[ISR] &= ~ISR_IP_change;
        return sr[IPCR];
}

/* Change input port state. */

static void set_input_state(int enable)
{
        if (enable)
                sr[IP06] |= IP06_IP2;
        else
                sr[IP06] &= ~IP06_IP2;

        /* Test for changed state and assert an interrupt condition. */

        if (get_input_port_change())
                sr[ISR] |= ISR_IP_change;
}



/* Channel-related state abstractions. */

/* Receive holding register FIFO for RHRA and RHRB. */

struct rhr_fifo
{
        /* FIFO data and flag storage. */

        uint8_t data[3], flags[3];

        /* Front and back of the received data. */

        int front, back;
        int empty;
};

static void reset_fifo(struct rhr_fifo *fifo)
{
        int i;

        for (i = 0; i < 3; i++)
        {
                fifo->data[i] = 0;
                fifo->flags[i] = 0;
        }

        fifo->front = 0;
        fifo->back = 0;
        fifo->empty = 1;
}

/* A channel abstraction. */

struct channel
{
        /* Conditions. */

        int rts;

        /* Transmit and receive shift registers. */

        uint8_t tsr, rsr;
        int tsr_bits_remaining, rsr_bits_received;

        /* Receive holding register FIFO. */

        struct rhr_fifo fifo;

        /* Channel state flags. */

        int transmit_enabled, receive_enabled;
        int break_started;
        int rsr_occupied;

        /* State to detect THR usage separately from TxEMT. */

        int THR_set;

        /* Mode registers and index. */

        uint8_t mr[2];
        int mr_index;

        /* Channel-specific registers. */

        uint8_t CSR, SR, THR;

        /* Channel-dependent values for common registers. */

        uint8_t IP06_CTS_flag, ISR_delta_break_flag, ISR_RxRDY_FFULL_flag,
                ISR_TxRDY_flag;

        /* Cycles per character. */

        int receive_cycles, transmit_cycles;

        /* The input and output characters actually exchanged. */

        uint8_t input_char, output_char;
        int have_input;
};

/* The channel objects. */

static struct channel channels[2];



/* Channel settings. */

static char channel_id(struct channel *ch)
{
        return ch == &channels[0] ? 'A' : 'B';
}

static int channel_baud_rate_receive_index(struct channel *ch)
{
        return (ch->CSR & CSR_receive) >> CSR_receive_shift;
}

static float channel_baud_rate_receive(struct channel *ch)
{
        return baud_rates[baud_rate_group()][channel_baud_rate_receive_index(ch)];
}

static int channel_baud_rate_transmit_index(struct channel *ch)
{
        return ch->CSR & CSR_transmit;
}

static float channel_baud_rate_transmit(struct channel *ch)
{
        return baud_rates[baud_rate_group()][channel_baud_rate_transmit_index(ch)];
}

static void channel_baud_rate_update(struct channel *ch)
{
        ch->receive_cycles = 2000000 / channel_baud_rate_receive(ch);
        ch->transmit_cycles = 2000000 / channel_baud_rate_transmit(ch);

        if (serial_debug)
                fprintf(stderr, "Cycles: transmit=%d receive=%d\n", ch->transmit_cycles, ch->receive_cycles);
}

static int channel_bpc(struct channel *ch)
{
        return (ch->mr[MR1] & MR1_bpc) + 5;
}

/* Channel operations. */

static void channel_rts_clear(struct channel *ch)
{
        if (ch->rts)
        {
                if (serial_debug)
                        fprintf(stderr, "%c: ~RTS\n", channel_id(ch));
                ch->rts = 0;
        }
}

static void channel_rts_set(struct channel *ch)
{
        if (!ch->rts)
        {
                if (serial_debug)
                        fprintf(stderr, "%c: RTS\n", channel_id(ch));
                ch->rts = 1;
        }
}

static void channel_clock_select(struct channel *ch, uint8_t val)
{
        ch->CSR = val;

        if (serial_debug)
                fprintf(stderr, "CSR%c: receiver=%s transmitter=%s\n",
                        channel_id(ch),
                        baud_rate_labels[baud_rate_group()][channel_baud_rate_receive_index(ch)],
                        baud_rate_labels[baud_rate_group()][channel_baud_rate_transmit_index(ch)]);

        channel_baud_rate_update(ch);
}

static void channel_transmit_not_ready(struct channel *ch)
{
        ch->SR &= ~SR_TxRDY;
        sr[ISR] &= ~ch->ISR_TxRDY_flag;
}

static void channel_transmit_ready(struct channel *ch)
{
        ch->SR |= SR_TxRDY;
        sr[ISR] |= ch->ISR_TxRDY_flag;
}

static void channel_disable_receiver(struct channel *ch)
{
        ch->receive_enabled = 0;
}

static void channel_disable_transmitter(struct channel *ch)
{
        ch->transmit_enabled = 0;
        ch->SR &= ~SR_TxEMT;
        channel_transmit_not_ready(ch);
}

static void channel_enable_receiver(struct channel *ch)
{
        ch->receive_enabled = 1;
}

static void channel_enable_transmitter(struct channel *ch)
{
        ch->transmit_enabled = 1;
        ch->SR |= SR_TxEMT;
        ch->THR_set = 0;
        channel_transmit_ready(ch);
}

static uint8_t channel_get_mode(struct channel *ch)
{
        uint8_t val = ch->mr[ch->mr_index];
        ch->mr_index = 1;
        return val;
}

static void channel_push_fifo(struct channel *ch)
{
        ch->fifo.data[ch->fifo.back] = ch->rsr;
        ch->fifo.back = (ch->fifo.back + 1) % 3;

        /* Set the RxRDY condition when an empty FIFO has been populated. */

        if (ch->fifo.empty)
        {
                ch->SR |= SR_RxRDY;

                if (!(ch->mr[MR1] & MR1_RxINT_FFULL))
                        sr[ISR] |= ch->ISR_RxRDY_FFULL_flag;

                ch->fifo.empty = 0;
        }

        /* Handle newly filled FIFO. */

        if (ch->fifo.front == ch->fifo.back)
        {
                ch->SR |= SR_FFULL;

                if (ch->mr[MR1] & MR1_RxINT_FFULL)
                        sr[ISR] |= ch->ISR_RxRDY_FFULL_flag;

                /* Receiver request-to-send (RTS)
                   control. */

                if (ch->mr[MR1] & MR1_RxRTS)
                        channel_rts_set(ch);

                if (serial_debug)
                        fprintf(stderr, "FIFO full.\n");
        }

        /* Vacate the shift register. */

        ch->rsr_occupied = 0;
}

static uint8_t channel_read(struct channel *ch)
{
        uint8_t val = ch->fifo.data[ch->fifo.front];

        /* Detect any remaining data. */

        if (!ch->fifo.empty)
                ch->fifo.front = (ch->fifo.front + 1) % 3;

        /* Detect any empty condition. */

        if (ch->fifo.front == ch->fifo.back)
        {
                ch->fifo.empty = 1;
                ch->SR &= ~SR_RxRDY;

                if (!(ch->mr[MR1] & MR1_RxINT_FFULL))
                        sr[ISR] &= ~ch->ISR_RxRDY_FFULL_flag;
        }

        /* Clear any full condition. */

        ch->SR &= ~SR_FFULL;

        if (ch->mr[MR1] & MR1_RxINT_FFULL)
                sr[ISR] &= ~ch->ISR_RxRDY_FFULL_flag;

        /* Vacate the shift register, if occupied. */

        if (ch->rsr_occupied)
                channel_push_fifo(ch);
        else
        {
                /* Receiver request-to-send (RTS) control. */

                if (ch->mr[MR1] & MR1_RxRTS)
                        channel_rts_clear(ch);
        }

        /* Update status register error flags. */

        if (!(ch->mr[MR1] & MR1_block_error))
                ch->SR &= ~(SR_receive_break | SR_parity_error | SR_framing_error);

        if (!ch->fifo.empty)
                ch->SR |= ch->fifo.flags[ch->fifo.front];

        return val;
}

static void channel_reset_output(struct channel *ch)
{
        ch->rts = 0;
}

static void channel_reset_receiver(struct channel *ch)
{
        reset_fifo(&ch->fifo);

        ch->receive_enabled = 0;
        ch->mr_index = 0;
        ch->rsr = 0;
        ch->rsr_bits_received = 0;
        ch->rsr_occupied = 0;
}

static void channel_reset_transmitter(struct channel *ch)
{
        ch->SR |= SR_TxEMT;
        ch->THR_set = 0;

        ch->transmit_enabled = 0;
        ch->break_started = 0;
        ch->tsr = 0;
        ch->tsr_bits_remaining = 0;
}

static void channel_reset(struct channel *ch)
{
        int pos;

        channel_reset_output(ch);
        channel_reset_receiver(ch);
        channel_reset_transmitter(ch);

        ch->receive_cycles = 0;
        ch->transmit_cycles = 0;

        ch->CSR = 0;
        ch->SR = 0;
        ch->THR = 0;

        /* Set mode registers. */

        for (pos = 0; pos < 2; pos++)
                ch->mr[pos] = 0;

        ch->have_input = 0;
}

static void channel_set_mode(struct channel *ch, uint8_t val)
{
        switch (ch->mr_index)
        {
                case MR1:
                if (serial_debug)
                        fprintf(stderr, "MR1%c: BPC=%d parity=\"%s\" mode=\"%s\" error=%s RxINT=%s RxRTS=%s\n",
                                channel_id(ch),
                                channel_bpc(ch),
                                val & MR1_parity_type ? "odd" : "even",
                                parity_mode[(val & MR1_parity_mode) >> MR1_parity_mode_shift],
                                val & MR1_block_error ? "block" : "char",
                                val & MR1_RxINT_FFULL ? "FFULL" : "RxRDY",
                                val & MR1_RxRTS ? "yes" : "no");
                break;

                case MR2:
                if (serial_debug)
                        fprintf(stderr, "MR2%c: stop=%s TxCTS=%s TxRTS=%s channel=%s\n",
                                channel_id(ch),
                                (channel_bpc(ch) == 5 ? stop_bits_5bpc : stop_bits)[val & MR2_stop_bits],
                                val & MR2_TxCTS ? "yes" : "no",
                                val & MR2_TxRTS ? "yes" : "no",
                                channel_mode[(val & MR2_channel) >> MR2_channel_shift]);
                break;
        }

        ch->mr[ch->mr_index] = val;
        ch->mr_index = 1;
}

static void channel_write(struct channel *ch, uint8_t val)
{
        if (serial_debug)
                fprintf(stderr, "THR%c: %02x\n", channel_id(ch), val);

        if (ch->SR & CR_enable_Tx)
        {
                ch->THR = val;
                ch->SR &= ~SR_TxEMT;
                ch->THR_set = 1;
                channel_transmit_not_ready(ch);
        }
}

static void channel_command(struct channel *ch, uint8_t val)
{
        struct rhr_fifo *fifo = &ch->fifo;
        int Rx_op = val & (CR_enable_Rx | CR_disable_Rx);
        int Tx_op = val & (CR_enable_Tx | CR_disable_Tx);

        if (serial_debug)
        {
                fprintf(stderr, "CR%c:", channel_id(ch));

                fprintf(stderr, " command=\"%s\"",
                        commands[(val & CR_command) >> CR_command_shift]);
        }

        switch ((val & CR_command) >> CR_command_shift)
        {
                case CR_command_reset_MR:
                ch->mr_index = 0;
                break;

                case CR_command_reset_receiver:
                if (!Rx_op)
                        channel_reset_receiver(ch);
                break;

                case CR_command_reset_transmitter:
                if (!Tx_op)
                        channel_reset_transmitter(ch);
                break;

                case CR_command_reset_error:
                ch->SR &= ~(SR_receive_break | SR_parity_error | SR_framing_error | SR_overrun_error);
                break;

                case CR_command_reset_break_change:
                sr[ISR] &= ~(ch->ISR_delta_break_flag);
                break;

                case CR_command_start_break:
                if (ch->transmit_enabled)
                        ch->break_started = 1;
                break;

                case CR_command_stop_break:
                ch->break_started = 0;
                break;

                default:
                break;
        }

        /* Separate channel modification bits. */

        if (!ch->receive_enabled && (val & CR_enable_Rx))
        {
                if (serial_debug)
                        fprintf(stderr, " enable Rx=%s",
                                val & CR_enable_Rx ? "yes" : "no");
                channel_enable_receiver(ch);
        }
        else if (ch->receive_enabled && (val & CR_disable_Rx))
        {
                if (serial_debug)
                        fprintf(stderr, " disable Rx=%s",
                                val & CR_disable_Rx ? "yes" : "no");
                channel_disable_receiver(ch);
        }

        if (!ch->transmit_enabled && (val & CR_enable_Tx))
        {
                if (serial_debug)
                        fprintf(stderr, " enable Tx=%s",
                                val & CR_enable_Tx ? "yes" : "no");
                channel_enable_transmitter(ch);
        }
        else if (ch->transmit_enabled && (val & CR_disable_Tx))
        {
                if (serial_debug)
                        fprintf(stderr, " disable Tx=%s",
                                val & CR_disable_Tx ? "yes" : "no");
                channel_disable_transmitter(ch);
        }

        if (serial_debug)
                fprintf(stderr, "\n");
}

static void channel_get_input(struct channel *ch)
{
        if ((socket_fd != -1) && socket_input(socket_fd, &ch->input_char, 1))
                ch->have_input = 1;
}

static void channel_set_output(struct channel *ch)
{
        if (socket_fd != -1)
                write(socket_fd, &ch->output_char, 1);
}



/* Receive data.

   The receive shift register is populated as bits are detected on the RxD pin,
   for the start bit, data bits (least significant bit first), parity and stop
   bits.

   The data is then transferred to the FIFO for this register at the back of the
   queue, with any unused data bits being set to zero.

   Any parity and framing errors along with any received break state are
   recorded along with the data in the FIFO. These conditions along with any
   overrun condition are also set in the status register, depending on the error
   mode chosen.

   In character error mode, the status register reflects the next available
   character in the FIFO.

   In block error mode, the status register reflects the cumulative error state,
   with each successive character setting (but not clearing) the status
   register bits. The error state is reset using the reset error command or in
   a receiver reset.

   After any updates to the status register, RxRDY is then set in the status
   register.

   A received break condition causes a zero character to be loaded into the FIFO
   to accompany the received break bit being set in the status register.

   When all FIFO elements are full, the FFULL condition occurs and is set in the
   status register. Any subsequently received characters will populate only the
   shift register, and if the shift register is overwritten, an overrun
   condition will be recorded.

   Disabling the receiver loses the shift register contents but preserves the
   FIFO data. Resetting the receiver discards the shift register contents and
   resets the FIFO pointers to an empty state, although the FIFO will retain
   previously received data.
*/

static void receive_data(struct channel *ch)
{
        /* NOTE: Multidrop mode supposedly has the receiver active even if not
                 enabled. */

        if (!ch->receive_enabled)
                return;

        /* Test for an appropriate moment to receive at the configured rate.
           ignoring unsupported rate settings. */

        if (!ch->receive_cycles)
                return;

        /* Catch up with reception at the configured rate, potentially receiving
           many characters. */

        while (receive_cycles >= ch->receive_cycles)
        {
                receive_cycles -= ch->receive_cycles;

                /* Test for available input. */

                if (!ch->rsr_bits_received)
                {
                        channel_get_input(ch);

                        if (!ch->have_input)
                                continue;
                }

                /* At the appropriate rate, send any bits in the shift register. */

                if (ch->rsr_bits_received < channel_bpc(ch))
                {
                        /* Receive each bit from somewhere. */

                        ch->rsr |= ch->input_char & (1 << ch->rsr_bits_received);

                        /* Test for an occupied shift register. */

                        if (ch->rsr_occupied)
                                ch->SR |= SR_overrun_error;

                        ch->rsr_bits_received++;

                        /* Test for the end of input. */

                        if (ch->rsr_bits_received == channel_bpc(ch))
                        {
                                /* Empty or non-full FIFO. */

                                if (ch->fifo.empty || (ch->fifo.front != ch->fifo.back))
                                        channel_push_fifo(ch);
                                else
                                        ch->rsr_occupied = 1;

                                ch->rsr = 0;
                                ch->rsr_bits_received = 0;
                                ch->have_input = 0;
                        }
                }
        }
}

/* Transmit data.

   When able to transmit, the TxRDY bit is set in the status register. TxRDY is
   cleared when a value is written to the transmit holding register. When a new
   character can be sent, the value is then transferred to the transmit shift
   register, and TxRDY is set again.

   The shift register bits are converted to signals on the TxD pin. A start bit,
   data bits (least significant bit first), parity and stop bits.

   Between characters, the TxD output remains high and the TxEMT (empty) bit in
   the status register is set to 1. TxEMT is cleared when a value is written to
   the transmit holding register.

   A send break command causes the transmitter to send a continuous low
   condition.
*/

static void transmit_data(struct channel *ch)
{
        /* Any partially transmitted character will be sent. */

        if (!ch->transmit_enabled && !ch->tsr_bits_remaining)
                return;

        /* Test for an appropriate moment to send at the configured rate,
           ignoring unsupported rate settings. */

        if (!ch->transmit_cycles)
                return;

        /* Catch up with transmission at the configured rate, potentially
           sending many characters. */

        while (transmit_cycles >= ch->transmit_cycles)
        {
                transmit_cycles -= ch->transmit_cycles;

                /* Attempt to send a new character. */

                if (!(ch->SR & SR_TxEMT) && (!ch->tsr_bits_remaining))
                {
                        ch->tsr = ch->THR;
                        ch->tsr_bits_remaining = channel_bpc(ch);
                        ch->THR_set = 0;
                        channel_transmit_ready(ch);
                        ch->output_char = 0;
                }

                /* Prevent transmission if controlled by CTSxN. */

                if ((ch->mr[MR2] & MR2_TxCTS) && !(sr[IP06] & ch->IP06_CTS_flag))
                        continue;

                /* At the appropriate rate, send any bits in the shift register. */

                if (ch->tsr_bits_remaining)
                {
                        ch->tsr_bits_remaining--;

                        /* Send each bit somewhere. */

                        ch->output_char |= ch->tsr & (1 << ch->tsr_bits_remaining);

                        /* Test for the end of output. */

                        if (!ch->tsr_bits_remaining)
                        {
                                /* Set the empty condition if the holding register was
                                   not set. */

                                if (!ch->THR_set)
                                        ch->SR |= SR_TxEMT;

                                /* Write to the communications socket. */

                                channel_set_output(ch);
                        }
                }
        }
}



/* Output port handling. */

static void reset_output_port(uint8_t val)
{
        if (val & OPBC_RTSAN)
                channel_rts_set(&channels[0]);
        if (val & OPBC_RTSBN)
                channel_rts_set(&channels[1]);
}

void serialinput(int);

static void set_output_port(uint8_t val)
{
        if (val & OPBC_RTSAN)
                channel_rts_clear(&channels[0]);
        if (val & OPBC_RTSBN)
                channel_rts_clear(&channels[1]);
}



/* Interrupt handling. */

static void update_interrupts()
{
        if (sr[IMR] & sr[ISR])
                serial_irq = 1;
        else
                serial_irq = 0;
}



/* Reset common registers. */

static void reset_registers()
{
        int i;

        for (i = 0; i <= SR_max; i++)
                sr[i] = 0;
}



/* Exported functions. */

/* Reset the serial controller's registers. */

void resetserial()
{
        extern char serialname[512];
        int i;

        /* Initialise channels. */

        channels[0].CSR = 0;
        channels[1].CSR = 0;

        channels[0].SR = 0;
        channels[1].SR = 0;

        channels[0].THR = 0;
        channels[1].THR = 0;

        channels[0].IP06_CTS_flag = IP06_CTSAN;
        channels[1].IP06_CTS_flag = IP06_CTSBN;

        channels[0].ISR_delta_break_flag = ISR_delta_break_A;
        channels[1].ISR_delta_break_flag = ISR_delta_break_B;

        channels[0].ISR_RxRDY_FFULL_flag = ISR_RxRDY_FFULL_A;
        channels[1].ISR_RxRDY_FFULL_flag = ISR_RxRDY_FFULL_B;

        channels[0].ISR_TxRDY_flag = ISR_TxRDYA;
        channels[1].ISR_TxRDY_flag = ISR_TxRDYB;

        /* Reset channels. */

        for (i = 0; i < 2; i++)
                channel_reset(&channels[i]);

        reset_registers();

        /* Reset cycle counters. */

        receive_cycles = 0;
        transmit_cycles = 0;

        /* Reset the IRQ line. */

        serial_irq = 0;

        /* Open a socket and attempt to establish a serial connection. */

        if (socket_fd == -1)
                socket_fd = socket_open(serialname);
}

/* Read from a serial controller register. */

uint8_t readserial(uint16_t addr)
{
        uint8_t val = 0;

        if (serial_debug > 1)
                fprintf(stderr, "read 0x%04x (%s) ->", addr, sr_read_names[addr - FC_BASE]);

        switch (addr)
        {
                case FC_MRxA:
                val = channel_get_mode(&channels[0]);
                break;

                case FC_SRA:
                val = channels[0].SR;
                break;

                case FC_RHRA:
                val = channel_read(&channels[0]);
                break;

                case FC_IPCR:
                val = read_input_port_change();
                break;

                case FC_ISR:
                val = sr[ISR];
                break;

                case FC_MRxB:
                val = channel_get_mode(&channels[1]);
                break;

                case FC_SRB:
                val = channels[1].SR;
                break;

                case FC_RHRB:
                val = channel_read(&channels[1]);
                break;

                case FC_IP06:
                val = sr[IP06];
                break;

                case FC_STCC:

                case FC_SPCC:
                default: break;
        }

        if (serial_debug > 1)
                fprintf(stderr, " %02x\n", val);
        return val;
}

/* Write to a serial controller register. */

void writeserial(uint16_t addr, uint8_t val)
{
        switch (addr)
        {
                case FC_MRxA:
                channel_set_mode(&channels[0], val);
                break;

                case FC_CSRA:
                channel_clock_select(&channels[0], val);
                break;

                case FC_CRA:
                channel_command(&channels[0], val);
                break;

                case FC_THRA:
                channel_write(&channels[0], val);
                break;

                case FC_ACR:
                if (serial_debug)
                        fprintf(stderr, "ACR: set=%d counter/timer=%d "
                                        "delta IP3 int=%s delta IP2 int=%s "
                                        "delta IP1 int=%s delta IP0 int=%s\n",
                                ((val & ACR_BRG) >> ACR_BRG_shift) + 1,
                                (val & ACR_CTMS) >> ACR_CTMS_shift,
                                val & ACR_delta_IP3 ? "on" : "off",
                                val & ACR_delta_IP2 ? "on" : "off",
                                val & ACR_delta_IP1 ? "on" : "off",
                                val & ACR_delta_IP0 ? "on" : "off");
                sr[ACR] = val;
                break;

                case FC_IMR:
                if (serial_debug)
                        fprintf(stderr, "IMR: IP change=%s delta break B=%s "
                                        "RxRDY/FFULL B=%s TxRDYB=%s counter ready=%s "
                                        "delta break A=%s RxRDY/FFULL A=%s TxRDYA=%s\n",
                                val & IMR_IP_change ? "on" : "off",
                                val & IMR_delta_break_B ? "on" : "off",
                                val & IMR_RxRDY_FFULL_B ? "on" : "off",
                                val & IMR_TxRDYB ? "on" : "off",
                                val & IMR_counter_ready ? "on" : "off",
                                val & IMR_delta_break_A ? "on" : "off",
                                val & IMR_RxRDY_FFULL_A ? "on" : "off",
                                val & IMR_TxRDYA ? "on" : "off");
                sr[IMR] = val;
                break;

                case FC_MRxB:
                channel_set_mode(&channels[1], val);
                break;

                case FC_CSRB:
                channel_clock_select(&channels[1], val);
                break;

                case FC_CRB:
                channel_command(&channels[1], val);
                break;

                case FC_THRB:
                channel_write(&channels[1], val);
                break;

                case FC_OPCR:
                if (serial_debug)
                        fprintf(stderr, "OPCR: OP7=%s OP6=%s OP5=%s OP4=%s OP3=%s OP2=%s\n",
                                val & OPCR_TxRDYB ? "TxRDYB" : "OPR[7]",
                                val & OPCR_TxRDYA ? "TxRDYA" : "OPR[6]",
                                val & OPCR_RxRDY_FFULL_B ? "RxRDY/FFULL B" : "OPR[5]",
                                val & OPCR_RxRDY_FFULL_A ? "RxRDY/FFULL A" : "OPR[4]",
                                opcr_op3[(val & OPCR_OP3) >> OPCR_OP3_shift],
                                opcr_op2[val & OPCR_OP2]);
                sr[OPCR] = val;
                break;

                case FC_SOPBC:
                set_output_port(val);
                break;

                case FC_ROPBC:
                reset_output_port(val);
                break;

                default:
                if (serial_debug > 1)
                        fprintf(stderr, "write 0x%04x (%s) <- %02x\n", addr, sr_write_names[addr - FC_BASE], val);
                break;
        }
}

/* Poll the serial controller, allowing it to do work for the given number of
   2MHz cycles. */

void pollserial(int cycles)
{
        int i;

        receive_cycles += cycles;
        transmit_cycles += cycles;

        for (i = 0; i < 2; i++)
        {
                /* If the receiver is enabled, obtain any shift register contents. */

                receive_data(&channels[i]);

                /* If the transmitter is enabled, send any shift register contents. */

                transmit_data(&channels[i]);
        }

        /* Raise interrupt conditions. */

        update_interrupts();
}

#endif // WIN32 