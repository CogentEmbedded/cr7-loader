/*
 * Copyright (c) 2015-2016, Renesas Electronics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Renesas nor the names of its contributors may be
 *     used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdarg.h>
#include <stdint.h>
#include "debug.h"
#include "scif.h"

static uint32_t uint32_print(uint32_t num)
{
	uint8_t  num_buf[10];
	uint32_t count = 0U;
	uint32_t i = 0U;
	uint32_t rem;
	uint32_t unum = num;

	while(1) {
		rem = unum % 10U;
		if (rem < 0x0AU) {
			num_buf[i] = '0' + rem;
		} else {
			num_buf[i] = 'a' + (rem - 0xa);
		}
		i++;
		unum = unum / 10U;
		if (unum < 1U ) {
			break;
		}
	}

	while (i != 0U) {
		i--;
		(void)console_putc(num_buf[i]);
		count++;
	};

	return count;
}

static uint32_t int32_print(int32_t num)
{
	uint32_t unum;
	uint32_t count = 0U;

	if (num < 0) {
		console_putc('-');
		count = 1U;
		unum = (uint32_t)-num;
	} else {
		unum = (uint32_t)num;
	}

	count += uint32_print(unum);

	return count;
}

static uint32_t uint32_hex_print(uint32_t num)
{
	uint32_t i;
	uint32_t count = 0U;
	uint8_t  c;

	for (i = 0U; i < 8U; i++) {
		c = (uint8_t)((num >> ((7U - i) * 4)) & 0x0FU);	/* 0-F */
		if ( c >= 0x0AU) {
			c += 'a' - 0x0AU;	/* A-F */
		} else {
			c += '0';		/* 0-9 */
		}
		(void)console_putc(c);
		count++;
	}

	return count;
}

static uint32_t str_print(const char *str)
{
	uint32_t count = 0;

	while (*str) {
		(void)console_putc(*str);
		str++;
		count++;
	}

	return count;
}

void tf_printf(const char *format, ...)
{
	va_list  args;
	int32_t  num;
	uint32_t unum;
	char     *str;
	uint32_t count = 0U;

	va_start(args, format);
	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
			case 'i':
			case 'd':
				num = va_arg(args, int32_t);
				count += int32_print(num);
				break;
			case 's':
				str = va_arg(args, char *);
				count += str_print(str);
				break;
			case 'x':
				unum = va_arg(args, uint32_t);
				count += uint32_hex_print(unum);
				break;
			case 'u':
				unum = va_arg(args, uint32_t);
				count += uint32_print(unum);
				break;
			default:
				break;
			}
		} else {
			(void)console_putc(*format);
			count++;
		}
		format++;
	}
	va_end(args);
}
