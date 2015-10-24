/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#define __PACK		__attribute__((packed))

typedef struct chunk_header {
	char chunkID[4];
	long chunkSize;
} chunk_header;

#define RIFFID		"RIFF"
#define WAVEID		"WAVE"

typedef struct {
	char type[4];		/* WAVE */
} __PACK RIFFChunk;

/* FMT Sub chunk */
#define FormatID	"fmt "
typedef struct {
	short wFormatTag;	/* other than 1, compressed */
	unsigned short wChannels;
	unsigned long dwSamplesPerSec;	/* 11025, 22050, 44100 Khz ... */
	unsigned long dwAvBytesPerSec;	/* dwSamplesPerSec * wBlockAlign */

	/* 16-bit mono wave = 2 Bytes */
	/* 16-bit stereo wave = 4 Bytes */
	/* wChannels * (wBitsPerSample / 8)    == Size of a sample frame, in terms of byte */
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;	/* 16-bit waveform would have 16 */

	/* NOTE: Depends on wFormatTag, here could be more fields exists */

} __PACK FormatChunk;

enum FORMAT {
	IBM_FORMAT_MULAW = 0x0101,
	IBM_FORMAT_ALAW = 0x0102,
	IBM_FORMAT_ADPCM = 0x0103,
};

enum CHANNELS {
	MONOPHONIC = 1,
	STEREO = 2,
	FOR_CHANNEL = 4,
	/*  ...  ( 0x1 << nr of channels ) */
};

#define DataID		"DATA"

typedef struct {
	unsigned char waveformData[1];
} __PACK DataChunk;

typedef struct {
	long dwIdentifier;
	long dwPosition;
	unsigned long fccChunk;
	long dwChunkStart;
	long dwBlockStart;
	long dwSampleOffset;
} __PACK CuePoint;

#define CueID	"cue "

typedef struct {
	long dwCuePoints;
	CuePoint points[];
} __PACK CueChunk;

typedef struct {
	long dwIdentifier;
	long dwLength;
	long dwRepeats;
} __PACK Segment;

#define PlaylistID	"plst"

typedef struct {
	long dwSegments;
	Segment Segments[];
} __PACK PlaylistChunk;

typedef struct {
	unsigned long typeID;
} __PACK ListHeader;

#define LabelID	"labl"
typedef struct {
	long dwIdentifier;
	char dwText[];
} __PACK LabelChunk;

#define NoteID "note"
typedef struct {
	long dwIdentifier;
	char dwText[];
} __PACK NoteChunk;

#define LabelTextID	"ltxt"
typedef struct {
	long dwIdentifier;
	long dwSampleLength;
	long dwPurpose;
	short wCountry;
	short wLanguage;
	short wDialect;
	short wCodePage;
	char dwText[];
} __PACK LabelTextChunk;

#define SamplerID "smpl"

typedef struct {
	long dwIdentifier;
	long dwType;
	long dwStart;
	long dwEnd;
	long dwFraction;
	long dwPlayCount;
} __PACK SampleLoop;

typedef struct {
	long dwManufacturer;
	long dwProduct;
	long dwSamplePeriod;
	long dwMIDIUnityNote;
	long dwMIDIPitchFraction;
	long dwSMPTEFormat;
	long dwSMPTEOffset;
	long cSampleLoops;
	long cbSamplerData;
	SampleLoop Loops[];
} __PACK SamplerChunk;

enum SMPTE {
	FRAME24 = 24,
	FRAME25 = 25,
	FRAME30_WITH_DROP = 29,
	FRAME30 = 30,
};

enum LOOP_TYPE {
	FORWARD_LOOP = 0,
	ALTERNATE_LOOP = 1,
	BACKWARD_LOOP = 2,
};

#define InstrumentID	"inst"
typedef struct {
	unsigned char UnshiftedNote;
	char FineTune;
	char Gain;
	unsigned char LowNote;
	unsigned char HighNote;
	unsigned char LowVelocity;
	unsigned char HighVelocity;
} __PACK InstrumentChunk;

#define ListID	"list"
typedef struct {
	char typeID[4];
	char text[];
} __PACK ListChunk;

#define FactID "fact"
typedef struct {
	char data[1];
} __PACK FactChunk;

struct wave {
	char *filename;
	int fd;

	chunk_header riff_header;
	off_t riff_off;
	/* RIFFChunk *riff; */

	chunk_header fmt_header;
	off_t fmt_off;
	/* FormatChunk *format; */

	chunk_header data_header;
	off_t data_off;
	/* DataChunk *data; */

	chunk_header list_header;
	off_t list_off;
	/* ListChunk *list; */
};

extern struct wave *open_wave_file(const char *filename);
extern int get_wave_channel(struct wave *handle);
extern int get_wave_samples_per_sec(struct wave *handle);
extern int get_wave_bits_per_sample(struct wave *handle);
extern int get_wave_data_size(struct wave *handle);
extern int get_wave_data(struct wave *handle, void *data, int off, int size);
extern void close_wave_file(struct wave *handle);
