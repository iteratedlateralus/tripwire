#ifndef __PLAYWAVE_HEADER__ 
#define __PLAYWAVE_HEADER__ 1

/*
  PLAYWAVE:  A test application for the SDL mixer library.
  Copyright (C) 1997-2012 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* $Id$ */

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef unix
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_mixer.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif


/*
 * rcg06132001 various mixer tests. Define the ones you want.
 */
/*#define TEST_MIX_DECODERS*/
/*#define TEST_MIX_VERSIONS*/
/*#define TEST_MIX_CHANNELFINISHED*/
/*#define TEST_MIX_PANNING*/
/*#define TEST_MIX_DISTANCE*/
/*#define TEST_MIX_POSITION*/


#if (defined TEST_MIX_POSITION)

#if (defined TEST_MIX_PANNING)
#error TEST_MIX_POSITION interferes with TEST_MIX_PANNING.
#endif

#if (defined TEST_MIX_DISTANCE)
#error TEST_MIX_POSITION interferes with TEST_MIX_DISTANCE.
#endif

#endif


/* rcg06192001 for debugging purposes. */
static void output_test_warnings(void);

static int audio_open = 0;
static Mix_Chunk *wave = NULL;

/* rcg06042009 Report available decoders. */
#if (defined TEST_MIX_DECODERS)
static void report_decoders(void);
#endif

static void output_versions(const char *libname, const SDL_version *compiled,
							const SDL_version *linked);

static void test_versions(void);

/* rcg06192001 abstract this out for testing purposes. */
static int still_playing(void);

#if (defined TEST_MIX_PANNING)
static void do_panning_update(void);
#endif


#if (defined TEST_MIX_DISTANCE)
static void do_distance_update(void);
#endif

static void CleanUp(int exitcode);


/*
 * rcg06182001 This is sick, but cool.
 *
 *  Actually, it's meant to be an example of how to manipulate a voice
 *  without having to use the mixer effects API. This is more processing
 *  up front, but no extra during the mixing process. Also, in a case like
 *  this, when you need to touch the whole sample at once, it's the only
 *  option you've got. And, with the effects API, you are altering a copy of
 *  the original sample for each playback, and thus, your changes aren't
 *  permanent; here, you've got a reversed sample, and that's that until
 *  you either reverse it again, or reload it.
 */
static void flip_sample(Mix_Chunk *wave);

extern "C" {
static int play_wave(char const*);
}
#endif
