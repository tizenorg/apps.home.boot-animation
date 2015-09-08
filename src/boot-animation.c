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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <stddef.h>
#include <unistd.h>

#include <pthread.h>
#include <utilX.h>

#include <Elementary.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include <audio-session-manager.h>

#include <getopt.h>
#include <boot-animation.h>
#include <animation.h>
#include <mm_sound_private.h>

#include <vconf.h>

#include "log.h"

#define XRGB8888 4

extern char *optarg;
extern int optind, opterr, optopt;

struct args {
	int argc;
	char **argv;
	char *msg;
};

static int use_csc_ani;

static void print_usages(char *argv0)
{
	printf("Usage) %s {--start|--stop}\n"
	       "  Ex:"
	       "    # %s --start\n"
	       "    # %s --stop\n"
	       "    # %s --off\n"
	       "    # %s --offmsg YOUR_MESSAGE\n", argv0, argv0, argv0, argv0,
	       argv0);
}

static int get_wav_file(int state, char *wavpath)
{
	char *csc_dir;
	csc_dir = vconf_get_str(VCONFKEY_CSC_POWER_ANI_FILE_PATH);
	_D("Wave file PATH : %s", csc_dir);
	if (state == TYPE_ON) {
		if (csc_dir && get_use_csc()) {
			snprintf(wavpath, FILE_PATH_MAX-1, "%s/%s", csc_dir, POWERON_WAV);
			if (check_csc_data(wavpath, TYPE_CHECK_SOUND) < 0)
				return -1;
			else
				return 0;
		}
		snprintf(wavpath, FILE_PATH_MAX-1, "%s", DEFAULT_POWERON_WAV);
	} else {
		if (csc_dir && get_use_csc()) {
			snprintf(wavpath, FILE_PATH_MAX-1, "%s/%s", csc_dir, POWEROFF_WAV);
			if (check_csc_data(wavpath, TYPE_CHECK_SOUND) < 0)
				return -1;
			else
				return 0;
		}
		return -1;
	}
	return 0;
}

static
int xready_cb(keynode_t * node, void *user_data)
{
	int c;
	int argc;
	char **argv;
	int type = TYPE_UNKNOWN;
	int soundon = 1;	/* default sound on */
	struct args *args = user_data;
	char wav_path[256];
	static struct option long_options[] = {
		{"start",	no_argument,		0,	's'	},
		{"stop",	no_argument,		0,	'p'	},
		{"off",		no_argument,		0,	'o'	},
		{"offmsg",	required_argument,	0,	'm'	},
		{"clear",	no_argument,		0,	'c'	},
		{0,		0,			0,	0	},
	};
	static int invoked_flag = 0;

	_D("xready_cb");

	if (invoked_flag == 1) {
		_E("Already launched");
		return EXIT_FAILURE;
	}

	invoked_flag = 1;

	argc = args->argc;
	argv = args->argv;

	while ((c = getopt_long(argc, argv, "spom:c", long_options, NULL)) >= 0) {

		switch (c) {
		case 's':
			type = TYPE_ON;
			continue;
		case 'p':
			type = TYPE_OFF;
			continue;
		case 'o':
			type = TYPE_OFF_NOEXIT;
			continue;
		case 'm':
			if (args->msg) continue;
			type = TYPE_OFF_WITH_MSG;
			args->msg = strdup(optarg);
			if (!args->msg)
				perror("strdup");
			continue;
		default:
			type = TYPE_UNKNOWN;
			_D("[Boot-ani] unknown arg [%s]", optarg);
			return EXIT_FAILURE;
		}
	}

	/* check sound profile */
	if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &soundon) < 0) {
		_D("VCONFKEY_SETAPPL_SOUND_STATUS_BOOL ==> FAIL!!");
	}

	if (init_animation(type, args->msg) != EXIT_SUCCESS) {
		_D("Exit boot-animation");
		return EXIT_FAILURE;
	}

	if (soundon) {
		if (!get_wav_file(type, wav_path)) {
			mm_sound_boot_ready(3);
			mm_sound_boot_play_sound(wav_path);
		}
	}

	return EXIT_SUCCESS;
}

#if 0
static void _boot_ani_ui_set_scale(void)
{
	double root_height = 0.0;
	double root_width = 0.0;
	char buf[128] = { 0, };
	Display *disp;
	int screen_num;

	disp = XOpenDisplay(NULL);
	if (disp == NULL)
		return;

	screen_num = DefaultScreen(disp);

	root_height = DisplayHeight(disp, screen_num);
	root_width = DisplayWidth(disp, screen_num);

	XCloseDisplay(disp);

	snprintf(buf, sizeof(buf), "%lf", root_height / BA_DEFAULT_WINDOW_H);
	//snprintf(buf, sizeof(buf), "%lf", root_width / BA_DEFAULT_WINDOW_W);
	_D("Boot animation scale : [%s]", buf);

	setenv("ELM_SCALE", buf, 1);
	setenv("SCALE_FACTOR", buf, 1);
}
#endif

int check_csc_data(char *filepath, int type)
{
	FILE *f;
	if (!filepath)
		return -1;
	f = fopen(filepath, "r");

	if (!f) {
		_E("File doesn't exist : %s", filepath);
		return -1;
	}

	_D("Fount csc file : %s", filepath);
	fclose(f);

	if (type == TYPE_CHECK_ANI)
		use_csc_ani = 1;
	return 0;
}

int get_use_csc(void)
{
	return use_csc_ani;
}

//int elm_main(int argc, char *argv[])
int main(int argc, char *argv[])
{
	struct args args;

	if (argc < 2) {
		print_usages(argv[0]);
		return EXIT_FAILURE;
	}

	args.argc = argc;
	args.argv = argv;
	args.msg = NULL;

#if 0
	_boot_ani_ui_set_scale();
#endif

	elm_init(argc, argv);

	if (vconf_set_int(VCONFKEY_BOOT_ANIMATION_FINISHED, 0) != 0) {
		_D("Failed to set finished value to 0\n");
	}
	if (xready_cb(NULL, &args) != EXIT_SUCCESS) {
		vconf_set_int(VCONFKEY_BOOT_ANIMATION_FINISHED, 1);
		return 1;
	}

	elm_run();

	fini_animation();

	if (args.msg)
		free(args.msg);
	return 0;
}

//ELM_MAIN()
