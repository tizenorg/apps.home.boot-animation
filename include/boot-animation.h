/*
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd All Rights Reserved 
 *
 * This file is part of <boot-animation>
 * Written by <Seungtaek Chung> <seungtaek.chung@samsung.com>, <Mi-Ju Lee> <miju52.lee@samsung.com>, <Xi Zhichan> <zhichan.xi@samsung.com>
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall use it only in accordance
 * with the terms of the license agreement you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability of the software,
 * either express or implied, including but not limited to the implied warranties of merchantability,
 * fitness for a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as a result of using,
 * modifying or distributing this software or its derivatives.
 *
 */

#ifndef PREFIX
#define PREFIX "/usr"
#endif

#define EDJ_OFF PREFIX"/share/edje/poweroff.edj"
#define EDJ_ON PREFIX"/share/edje/poweron.edj"
#define GRP_OFF "poweroff"
#define GRP_ON "poweron"

#define NICE_VALUE -20

#define TITLE "Password Application"

#define BA_DEFAULT_WINDOW_H	800
#define BA_DEFAULT_WINDOW_W	1280

enum {
	TYPE_UNKNOWN,
	TYPE_ON,
	TYPE_OFF,
	TYPE_OFF_NOEXIT,
	TYPE_OFF_WITH_MSG,
};

#if defined(_DEBUG)

static int _func_depth;

#define ENTER_FUNCTION()			\
 		do { 						\
 				_func_depth++; 	\
 				printf("%*.c [32m[%s:%d][0m\n", _func_depth*4, '-', __func__, __LINE__); \
 		} while (0)
#define LEAVE_FUNCTION()			\
 		do { 						\
 				_func_depth--; 	\
 			} while (0)

inline static void dump_riff_header(RIFFChunk * riff)
{
	ENTER_FUNCTION();
	fprintf(stderr, "%*.c Type ID: %.4s\n", _func_depth * 4, '-',
		riff->type);
	LEAVE_FUNCTION();
}

inline static void dump_format_header(FormatChunk * format)
{
	ENTER_FUNCTION();
	fprintf(stderr, "%*.c Format tag: %d\n", _func_depth * 4, '-',
		format->wFormatTag);
	fprintf(stderr, "%*.c Channel : %d\n", _func_depth * 4, '-',
		format->wChannels);
	fprintf(stderr, "%*.c Samples per sec : %lu\n", _func_depth * 4, '-',
		format->dwSamplesPerSec);

	fprintf(stderr, "%*.c Byte per sec : %lu\n", _func_depth * 4, '-',
		format->dwAvBytesPerSec);

	fprintf(stderr, "%*.c Block align : %d\n", _func_depth * 4, '-',
		format->wBlockAlign);
	fprintf(stderr, "%*.c Size of a sample frame : %d bytes\n",
		_func_depth * 4, '-',
		format->wChannels * (format->wBitsPerSample / 8));
	fprintf(stderr, "%*.c Bits per sample : %d\n", _func_depth * 4, '-',
		format->wBitsPerSample);
	LEAVE_FUNCTION();
}

#define DbgPrint(fmt, arg...)	fprintf(stderr, "%*.c "fmt, _func_depth*4, '-', ##arg);

#else
#define DbgPrint(fmt, ...)
#define ENTER_FUNCTION()
#define LEAVE_FUNCTION()

#define dump_riff_header(riff)
#define dump_format_header(format)
#endif
