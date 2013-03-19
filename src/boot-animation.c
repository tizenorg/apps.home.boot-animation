/*
  * Copyright 2012 - 2013  Samsung Electronics Co., Ltd
  * 
  * Licensed under the Flora License, Version 1.0 (the License);
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  * 
  *     http://floralicense.org/license/
  * 
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
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

#include <vconf.h>

#define XRGB8888 4
extern char *optarg;
extern int optind, opterr, optopt;

struct args {
	int argc;
	char **argv;
	char *msg;
};

static void print_usages(char *argv0)
{
	printf("Usage) %s {--start|--stop}\n"
	       "  Ex:\n"
	       "    # %s --start\n"
	       "    # %s --stop\n"
	       "    # %s --off\n"
	       "    # %s --offmsg YOUR_MESSAGE\n", argv0, argv0, argv0, argv0,
	       argv0);
}

static int fb_init()
{
	int fd_fb;
	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;
	void *start;
	int length;

	fd_fb = open("/dev/fb0", O_RDWR);

	if (fd_fb < 0) {
		return -1;
	}

	if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix) < 0) {
		close(fd_fb);
		return -1;
	}

	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var) < 0) {
		close(fd_fb);
		return -1;
	}

	start = mmap(0, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	length = var.yres * var.xres * XRGB8888;

	if (start == MAP_FAILED) {
		munmap(0, fix.smem_len);
		close(fd_fb);
		return -1;
	}

	memset(start, 0, length);

	close(fd_fb);
	return 0;
}

static
void restart_cb(keynode_t * node, void *user_data)
{
	char *str;
	char cmd[128];
	char msg[256];
	int type = TYPE_UNKNOWN;

	if (!node) {
		fprintf(stderr, "Invalid node\n");
		return;
	}

	str = vconf_keynode_get_str(node);
	if (!str) {
		fprintf(stderr, "Failed to get string\n");
		return;
	}

	if (sscanf(str, "%127[^:]:%255s", cmd, msg) != 2) {
		fprintf(stderr, "Invalid format [%s]\n", str);
		free(str);
		return;
	}
	free(str);

	if (!strcasecmp(cmd, "start")) {
		type = TYPE_ON;
	} else if (!strcasecmp(cmd, "stop")) {
		type = TYPE_OFF;
	} else if (!strcasecmp(cmd, "off")) {
		type = TYPE_OFF_NOEXIT;
	} else if (!strcasecmp(cmd, "offmsg")) {
		type = TYPE_OFF_WITH_MSG;
	} else {
		fprintf(stderr, "Unknown command %s\n", cmd);
	}

	restart_animation(type, msg);
}

static
void xready_cb(keynode_t * node, void *user_data)
{
	int c;
	int argc;
	char **argv;
	int option_index = 0;
	int type = TYPE_UNKNOWN;
	int clear_type = TYPE_UNKNOWN;
	int errorcode = -1;
	int asm_handle;
	struct args *args = user_data;
	static struct option long_options[] = {
		{"start", 0, 0, 's'},
		{"stop", 0, 0, 'p'},
		{"off", 0, 0, 'o'},
		{"offmsg", 1, 0, 'm'},
		{"clear", 0, 0, 'c'},
		{0, 0, 0, 0},
	};
	static int invoked_flag = 0;

	if (invoked_flag == 1) {
		fprintf(stderr, "Already launched.\n");
		return;
	}

	invoked_flag = 1;

	argc = args->argc;
	argv = args->argv;

	nice(NICE_VALUE);

	while (1) {
		c = getopt_long(argc, argv, "spom:", long_options,
				&option_index);
		if (c < 0) {
			break;
		} else if (c == 0) {
			c = long_options[option_index].val;
		}

		switch (c) {
		case 's':
			type = TYPE_ON;
			break;
		case 'p':
			type = TYPE_OFF;
			break;
		case 'o':
			type = TYPE_OFF_NOEXIT;
			break;
		case 'm':
			type = TYPE_OFF_WITH_MSG;
			args->msg = strdup(optarg);
			if (!args->msg)
				perror("strdup");
			break;
		case 'c':
			clear_type = TYPE_CLEAR;
			break;
		default:
			type = TYPE_UNKNOWN;
			break;
		}
	}

	if (type == TYPE_UNKNOWN) {
		fprintf(stderr, "[Boot-ani] unknown arg [%s]\n", argv[1]);
		return;
	}

	if (clear_type == TYPE_CLEAR) {
		fprintf(stderr, "fb_init\n");
		fb_init();
	}

	init_animation(type, args->msg);
	if ((type == TYPE_OFF) || (type == TYPE_OFF_WITH_MSG)) {
		if (!ASM_register_sound
		    (-1, &asm_handle, ASM_EVENT_EXCLUSIVE_AVSYSTEM,
		     ASM_STATE_PLAYING, NULL, NULL, ASM_RESOURCE_NONE,
		     &errorcode)) {
			fprintf(stderr, "ASM_register_sound() failed 0x%x\n",
				errorcode);
			return;
		}
	}
}

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

	elm_init(argc, argv);

	vconf_notify_key_changed("memory/boot-animation/restart", restart_cb,
				 NULL);
	xready_cb(NULL, &args);

	elm_run();

	fini_animation();

	if (args.msg)
		free(args.msg);
	return 0;
}
