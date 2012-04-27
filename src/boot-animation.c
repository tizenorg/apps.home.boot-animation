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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
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
	int errorcode = -1;
	int asm_handle;
	struct args *args = user_data;
	static struct option long_options[] = {
		{"start", 0, 0, 's'},
		{"stop", 0, 0, 'p'},
		{"off", 0, 0, 'o'},
		{"offmsg", 1, 0, 'm'},
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
		default:
			type = TYPE_UNKNOWN;
			break;
		}
	}

	if (type == TYPE_UNKNOWN) {
		fprintf(stderr, "[Boot-ani] unknown arg [%s]\n", argv[1]);
		return;
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
	elm_shutdown();
	return 0;
}
