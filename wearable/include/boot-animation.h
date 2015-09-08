/*
 *  boot-animation
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Seungtaek Chung <seungtaek.chung@samsung.com>, Mi-Ju Lee <miju52.lee@samsung.com>, Xi Zhichan <zhichan.xi@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef PREFIX
#define PREFIX "/usr"
#endif

#define DEFAULT_EDJ_OFF PREFIX"/share/edje/poweroff.edj"
#define DEFAULT_EDJ_ON PREFIX"/share/edje/poweron.edj"
#define EDJ_OFF "poweroff.edj"
#define EDJ_ON "poweron.edj"
#define GRP_OFF "poweroff"
#define GRP_ON "poweron"

#define DEFAULT_POWERON_WAV		PREFIX"/share/keysound/poweron.wav"
#define DEFAULT_POWEROFF_WAV	PREFIX"/share/keysound/poweroff.wav"

#define POWERON_WAV		"poweron.wav"
#define POWEROFF_WAV	"poweroff.wav"

#define NICE_VALUE -20

#define TITLE "Password Application"

#define BA_DEFAULT_WINDOW_H	800	/* Default is HD(720 X 1280) */
#define BA_DEFAULT_WINDOW_W	1280	/* Default is WXGA(1280 X 800) */

#define FILE_PATH_MAX 128

enum {
	TYPE_UNKNOWN,
	TYPE_ON,
	TYPE_OFF,
	TYPE_OFF_NOEXIT,
	TYPE_OFF_WITH_MSG,
	TYPE_CLEAR,
	TYPE_MAX
};

enum {
	TYPE_CHECK_ANI,
	TYPE_CHECK_SOUND,
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

	/* 16-bit mono wave = 2 Bytes */
	/* 16-bit stereo wave = 4 Bytes */
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

int check_csc_data(char *filepath, int type);
int get_use_csc(void);
