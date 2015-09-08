/*
 * Copyright (c) 2009-2014 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <utilX.h>
#include <vconf.h>

#include <Elementary.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>

#include <boot-animation.h>
#include <animation.h>

static struct animation {
	Ecore_X_Window focus_win;
	Ecore_X_Window root_win;
	Evas_Object *win;
	Ecore_Window xwin;
	Evas_Coord w;
	Evas_Coord h;
	int t;
	Evas *evas;
	Ecore_Evas *ee;
	Evas_Object *layout;
	int state;
	Evas_Object *txt;
} s_animation = {
.txt = NULL,};

static void win_del(void *data, Evas_Object * obj, void *event_info)
{
	fprintf(stderr, "Window delete event received\n");
	elm_exit();
}

static void _edje_cb(void *d, Evas_Object * obj, const char *e, const char *s)
{
	fprintf(stderr, "edje calback is invoked\n");

	if (vconf_set_int("memory/boot-animation/finished", 1) != 0) {
		fprintf(stderr, "Failed to set finished set\n");
	}

	if (s_animation.state == TYPE_OFF || s_animation.state == TYPE_ON) {
		elm_exit();
	} else if (s_animation.state == TYPE_OFF_WITH_MSG) {
		if (s_animation.txt) {
			Evas_Coord w;
			Evas_Coord h;

			evas_object_size_hint_weight_set(s_animation.txt,
							 EVAS_HINT_EXPAND,
							 EVAS_HINT_EXPAND);
			evas_object_size_hint_fill_set(s_animation.txt,
						       EVAS_HINT_FILL,
						       EVAS_HINT_FILL);
			evas_object_resize(s_animation.txt, s_animation.w,
					   s_animation.h);
			evas_object_color_set(s_animation.txt, 255, 255, 255,
					      255);
			evas_object_text_font_set(s_animation.txt,
						  "SLP:style=medium", 30);
			evas_object_geometry_get(s_animation.txt, NULL, NULL,
						 &w, &h);
			evas_object_move(s_animation.txt,
					 (s_animation.w - w) >> 1,
					 (s_animation.h - h) >> 1);
			evas_object_show(s_animation.txt);
		}
	}
}

static
int init_layout(const char *msg)
{
	s_animation.layout = elm_layout_add(s_animation.win);
	if (!s_animation.layout) {
		fprintf(stderr, "Failed to create layout\n");
		return EXIT_FAILURE;
	}

	if (s_animation.state == TYPE_ON) {
		elm_layout_file_set(s_animation.layout, EDJ_ON, GRP_ON);
	} else {
		elm_layout_file_set(s_animation.layout, EDJ_OFF, GRP_OFF);
	}

	evas_object_size_hint_weight_set(s_animation.layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	printf("[Boot-ani] Start animation: %d mode\n", s_animation.state);
	elm_win_resize_object_add(s_animation.win, s_animation.layout);
	edje_object_signal_callback_add(elm_layout_edje_get(s_animation.layout),
					"end", "animation", _edje_cb, NULL);
	evas_object_show(s_animation.layout);

	Ecore_X_Window xwin;
	xwin = elm_win_xwindow_get(s_animation.win);
	ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);


	if (msg) {
		if (!s_animation.txt) {
			s_animation.txt =
			    evas_object_text_add(s_animation.evas);
			if (!s_animation.txt) {
				fprintf(stderr, "Failed to add text\n");
				evas_object_del(s_animation.layout);
				return EXIT_FAILURE;
			}
		}

		evas_object_text_text_set(s_animation.txt, msg);
		evas_object_hide(s_animation.txt);
	}

	if (vconf_set_int("memory/boot-animation/finished", 0) != 0) {
		fprintf(stderr, "Failed to set finished value to 0\n");
	}

	return EXIT_SUCCESS;
}

static
void fini_layout(void)
{
	if (s_animation.layout) {
		evas_object_del(s_animation.layout);
	}

	if (s_animation.txt) {
		evas_object_del(s_animation.txt);
		s_animation.txt = NULL;
	}
}

void restart_animation(int state, const char *msg)
{
	fini_layout();

	s_animation.state = state;

	init_layout(msg);

	evas_object_show(s_animation.win);
}

static
int create_window(void)
{
	s_animation.win = elm_win_add(NULL, "", ELM_WIN_BASIC);
	if (!s_animation.win) {
		fprintf(stderr, "Failed to create a new window\n");
		return EXIT_FAILURE;
	}
	evas_object_smart_callback_add(s_animation.win, "delete-request",
				       win_del, NULL);
	evas_object_resize(s_animation.win, s_animation.w, s_animation.h);

	s_animation.evas = evas_object_evas_get(s_animation.win);
	if (!s_animation.evas) {
		evas_object_del(s_animation.win);
		fprintf(stderr, "Failed to get the evas object\n");
		return EXIT_FAILURE;
	}
	elm_win_borderless_set(s_animation.win, 1);
	evas_object_move(s_animation.win, 0, 0);

	if (s_animation.w > s_animation.h) {
		int t;
		elm_win_rotation_with_resize_set(s_animation.win, 90);
		t = s_animation.w;
		s_animation.w = s_animation.h;
		s_animation.h = t;
	}
	evas_object_show(s_animation.win);

	s_animation.ee = ecore_evas_ecore_evas_get(s_animation.evas);
	if (!s_animation.ee) {
		evas_object_del(s_animation.win);
		fprintf(stderr, "Failed to get the ecore evas object\n");
		return EXIT_FAILURE;
	}

	s_animation.xwin = ecore_evas_window_get(s_animation.ee);
	ecore_x_netwm_window_type_set(s_animation.xwin,
				      ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_set_system_notification_level(ecore_x_display_get(),
					    s_animation.xwin,
					    UTILX_NOTIFICATION_LEVEL_HIGH);

	return EXIT_SUCCESS;
}

int init_animation(int state, const char *msg)
{
	s_animation.focus_win = ecore_x_window_focus_get();
	s_animation.root_win = ecore_x_window_root_get(s_animation.focus_win);
	s_animation.state = state;
	ecore_x_window_size_get(s_animation.root_win, &s_animation.w,
				&s_animation.h);

	if (create_window() == EXIT_FAILURE) {
		fprintf(stderr, "Failed to create a new window\n");
		return EXIT_FAILURE;
	}

	if (init_layout(msg) == EXIT_FAILURE) {
		fprintf(stderr, "Failed to init the layout object\n");
		if (msg)
			evas_object_del(s_animation.txt);
		evas_object_del(s_animation.win);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int fini_animation(void)
{
	fini_layout();
	evas_object_del(s_animation.win);
	return EXIT_SUCCESS;
}
