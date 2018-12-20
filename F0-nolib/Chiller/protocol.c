/*
 * This file is part of the Chiller project.
 * Copyright 2018 Edward V. Emelianov <edward.emelianoff@gmail.com>.
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
#include "protocol.h"
#include "usart.h"
#include "adc.h"

#ifdef EBUG
/**
 * @brief debugging_proc - debugging functions
 * @param command - rest of cmd
 */
static void debugging_proc(const char *command){
    const char *ptr = command;
    int i;
    switch(*ptr++){
        case 'w':
            usart1_send("Test watchdog", 0);
            while(1){nop();}
        break;
        case 'A': // raw ADC values depending on next symbol
            i = *ptr++ - '0';
            if(i < 0 || i > NUMBER_OF_ADC_CHANNELS){
                usart1_send("Wrong channel nuber!", 0);
                return;
            }
            put_string("ADC value: ");
            put_uint(ADC_array[i]);
            usart1_sendbuf();
        break;
        default:
        break;
    }
}
#endif

/**
 * @brief process_command - command parser
 * @param command - command text (all inside [] without spaces)
 * @return text to send over terminal or NULL
 */
char *process_command(const char *command){
    const char *ptr = command;
    char *ret = NULL;
    usart1_sendbuf(); // send buffer (if it is already filled)
    switch(*ptr++){
    	case '?': // help
    		put_string("R - reset\nt - get MCU temp\nV - get Vdd\n");
#ifdef EBUG
			put_string("d -> goto debug:\n");
			put_string("\tw - test watchdog\n\tAx - get raw ADCx value\n");
#endif
    	break;
        case 'R': // reset MCU
            NVIC_SystemReset();
        break;
        case 't': // get mcu T
            put_string("MCUTEMP10=");
            put_int(getMCUtemp());
        break;
        case 'V': // get Vdd
            put_string("VDD100=");
            put_uint(getVdd());
        break;
#ifdef EBUG
        case 'd':
            debugging_proc(ptr);
            return NULL;
        break;
#endif
    }
    usart1_sendbuf();
    return ret;
}
