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
#include <string.h>
#include <stdlib.h>
#include <wav_parser.h>
#include <avsys-audio.h>
#include <avsys-audio-logical-volume.h>

struct cb_data {
	avsys_audio_param_t av_param;
	avsys_handle_t av_handle;
	int size;
	/* avsys_audio_volume_t av_volume; */
};

int init_player(struct player *player, void *data)
{
	struct cb_data *avsys = data;
	int ret;

	memset(&avsys->av_param, 0, sizeof(avsys->av_param));
	avsys->av_param.mode = AVSYS_AUDIO_MODE_OUTPUT;
	avsys->av_param.priority = AVSYS_AUDIO_PRIORITY_0;
	avsys->av_param.vol_type = AVSYS_AUDIO_LVOL_GAIN_TYPE_0;
	avsys->av_param.channels = get_wave_channel();
	avsys->av_param.samplerate = get_wave_samples_per_sec();
	avsys->av_param.format =
	    get_wave_bits_per_sample() ==
	    8 ? AVSYS_AUDIO_FORMAT_8BIT : AVSYS_AUDIO_FORMAT_16BIT;

	ret = avsys_audio_set_path_ex(AVSYS_AUDIO_GAIN_EX_KEYTONE,
				      AVSYS_AUDIO_PATH_EX_SPK,
				      AVSYS_AUDIO_PATH_EX_NONE,
				      AVSYS_AUDIO_PATH_OPTION_FORCED
	    );

	if (AVSYS_FAIL(ret)) {
		fprintf(stderr, "Failed to set path\n");
		return -1;
	}

	avsys_audio_ampon();

	ret =
	    avsys_audio_open(&avsys->av_param, &avsys->av_handle, &avsys->size);
	if (AVSYS_FAIL(ret)) {
		fprintf(stderr, "unable to set path avsys_audio_open\n");
		/* NOTE: Do I need to check the result of this?
		   But... I cannot handle it in this case. */
		avsys_audio_ampoff();
		return -1;
	}
	/* Deleted because avsys_audio_param_t is implemented.
	   av_volume.level[0] = 5;
	   av_volume.level[1] = 5;

	   ret = avsys_audio_set_volume_table(
	   AVSYS_AUDIO_LVOL_GAIN_TYPE_0,
	   AVSYS_AUDIO_LVOL_DEV_TYPE_SPK, 5, 0, 0);
	   ret = avsys_audio_set_tuning_volume(av_handle, &av_volume);
	   if (!AVSYS_FAIL(ret)) {
	   fprintf(stderr, "Failed to set volume\n");
	   avsys_audio_close(av_handle);
	   avsys_audio_ampoff();
	   return -1;
	   } 
	*/

	return 0;
}

int play(struct player *player, void *wav_data, int wav_size, void *data)
{
	struct cb_data *avsys = data;
	int writeSize;
	register int i;
	unsigned char *ptr;
	int ret;
	int size;

	size = avsys->size;
	writeSize = wav_size < size ? wav_size : size;
	ptr = wav_data;
	i = 0;
	while (i < wav_size) {
		ret = avsys_audio_write(avsys->av_handle, ptr, writeSize);
		if (AVSYS_FAIL(ret)) {
			fprintf(stderr,
				"unable to play using avsys_audio_write\n");
		}
		ptr += writeSize;
		i += writeSize;

		size = wav_size - i;
		writeSize = writeSize < size ? writeSize : size;
		DbgPrint("Playing %3d remained bytes %6d - playing bytes %6d\r",
			 i * 100 / wav_size, size, writeSize);
	}
	DbgPrint("\n");
	return 0;
}

int fini_player(struct player *player)
{
	struct cb_data *avsys;
	int ret;

	if (!player) {
		fprintf(stderr, "Failed to get play object\b");
		return -1;
	}

	avsys = player->data;

	ret = avsys_audio_close(avsys->av_handle);
	if (AVSYS_FAIL(ret)) {
		fprintf(stderr, "unable to close using avsys_audio_close\n");
	}

	ret = avsys_audio_ampoff();
	if (AVSYS_FAIL(ret)) {
		fprintf(stderr, "avsys_audio_ampoff is failed\n");
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct player *play_object;
	struct cb_data *cb;

	if (argc != 2) {
		fprintf(stderr, "Invalid argument\n");
		return -1;
	}

	open_wave_file(argv[1]);

	play_object = create_player();
	if (!play_object) {
		fprintf(stderr, "Failed to create a player\n");
		close_wave_file();
		return -1;
	}

	if (set_player_initializer(play_object, init_player) < 0) {
		fprintf(stderr, "Failed to set initializer\n");
		close_wave_file();
		return -1;
	}

	if (set_player_player(play_object, play) < 0) {
		fprintf(stderr, "Failed to set player\n");
		destroy_player(play_object);
		close_wave_file();
		return -1;
	}

	if (set_player_finalizer(play_object, fini_player) < 0) {
		fprintf(stderr, "Failed to set finalizer\n");
		destroy_player(play_object);
		close_wave_file();
		return -1;
	}

	cb = calloc(1, sizeof(struct cb_data));
	if (!cb) {
		perror(__func__);
		destroy_player(play_object);
		close_wave_file();
		return -1;
	}

	if (play_player(play_object, cb) < 0) {
		fprintf(stderr, "Failed to play\n");
		free(cb);
		destroy_player(play_object);
		close_wave_file();
		return -1;
	}

	destroy_player(play_object);
	close_wave_file();
	free(cb);
	return 0;
}
