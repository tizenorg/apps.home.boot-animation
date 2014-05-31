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

enum running_state {
	STATE_UNKNOWN = 0x00,
	STATE_READY = 0x01,
	STATE_PLAY = 0x02,
};

struct player {
	/* initializer will be invoked when the player is ready to play wave file. */
	/* normally, it is invoked at right after complete the format header loading */
	int (*init) (struct player * player, struct wave * handle);
	/* Play if if is ready, normaly, this callback can
	   be invoked after the format header is loaded. */
	int (*play) (struct player * player, struct wave * handle);
	/* After parsing all chunks in the wave file, this function will be invoked */
	int (*fini) (struct player * player);

	void *data;
	enum running_state state;
};

extern struct player *create_player(void);
extern int set_player_initializer(struct player *player,
				  int (*init) (struct player * player,
					       struct wave * handle));
extern int set_player_player(struct player *player,
			     int (*play) (struct player * player,
					  struct wave * handle));
extern int set_player_finalizer(struct player *player,
				int (*fini) (struct player * player));
extern int init_player(struct player *player, struct wave *handle, void *data);
extern int play_player(struct player *player, struct wave *handle);
extern int fini_player(struct player *player);
extern void destroy_player(struct player *player);
