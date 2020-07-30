#include <Arduboy2.h>
#include <Sprites.h>
#include <ArduboyTones.h>
#include "graphics.h"

static const uint8_t actionButtons[] = {0, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, A_BUTTON, B_BUTTON};
static const uint8_t ACTION_IDLE = 0;
static const uint8_t ACTION_JUMP = 1;
static const uint8_t ACTION_DUCK = 2;
static const uint8_t ACTION_SLIDE_LEFT = 3;
static const uint8_t ACTION_SLIDE_RIGHT = 4;
static const uint8_t ACTION_RAISE_LEFT = 5;
static const uint8_t ACTION_RAISE_RIGHT = 6;

//Using Arduboy2 instead of Arduboy2Base for now so I can use arduboy.print for debug info
Arduboy2 arduboy;
ArduboyTones tones(arduboy.audio.enabled);
Sprites sprites;
//notes starts at 208hz since below that can't be heard well on the Arduboy's tiny piezo
//notes stops at 7902hz for similar reasons
//I've also removed all the sharps/flats to keep it to normal notes for increased pitch as the player progresses
//5 octaves with 7 notes per octave and 6 actions to perform leaving 1 note for the interim beat.
uint16_t notes[] =
{
	 262, 294, 330, 349, 392, 440, 494, //octave 0
	 523, 587, 659, 698, 784, 880, 988, //octave 1
	1047,1175,1319,1397,1568,1760,1976, //octave 2
	2093,2349,2637,2794,3136,3520,3951, //octave 3
	4186,4699,5274,5588,6272,7040,7902  //octave 4
};
uint8_t jumpOffsets[] =
{
	4, 8, 11, 14, 16, 18, 19, 20,
	20, 19, 18, 16, 14, 11, 8, 4
};

uint32_t seed = 0x12345678;
uint8_t octave = 0;
uint8_t action = 0;
uint8_t actionDuration = 0;
uint8_t actionRate = 5;
uint8_t increaseRate = (6 - actionRate) * 2;
int8_t roadOffset = 0;

void rnd()
{
	seed ^= seed << 13;
	seed ^= seed >> 7;
	seed ^= seed << 17;
}

void setup()
{
	arduboy.begin();
	arduboy.clear();
	arduboy.setFrameRate(60);
	seed = arduboy.generateRandomSeed();
}

void loop()
{
	if (!arduboy.nextFrame())
		return;
	if (actionDuration == 0)
	{
		/*action = 0;
		arduboy.pollButtons();
		for (uint8_t i = 1; i < 7; ++i)
		{
			if (arduboy.justPressed(actionButtons[i]))
			{
				actionDuration = 15;
				action = i;
			}
		}*/
		rnd();
		action = (seed >> 1) % 6 + 1;
		actionDuration = 15;
		if (action == 0)
			tones.noTone();
		else
			tones.tone(notes[action - 1 + octave * 7] + TONE_HIGH_VOLUME);
	}
	else if (arduboy.everyXFrames(actionRate))
	{
		--actionDuration;
		if (actionDuration == 0)
		{
			--increaseRate;
			if (increaseRate == 0 && actionRate > 1)
			{
				--actionRate;
				++octave;
				increaseRate = (6 - actionRate) * 2;
			}
		}
		if (actionDuration <= 4)
		{
			tones.noTone();
			if (action == ACTION_DUCK) //end ducking animation slightly early
				action = ACTION_IDLE;
		}
		if (action == ACTION_SLIDE_LEFT)
			--roadOffset;
		else if (action == ACTION_SLIDE_RIGHT)
			++roadOffset;
	}
	if (roadOffset < 0)
		roadOffset += 8;
	else if (roadOffset >= 8)
		roadOffset -= 8;
	//Draw some debug info
	arduboy.setCursor(0, 0);
	//Always surround constant strings in F() in order to put them in PROGMEM and save precious RAM
	arduboy.print(F("Octave: "));
	arduboy.print(octave);
	//Draw the sprite
	if (action == ACTION_JUMP)
		sprites.drawSelfMasked(52, 24 - jumpOffsets[actionDuration], sprite, action);
	else
		sprites.drawSelfMasked(52, 24, sprite, action);
	//Draw a horizontal line for the ground for now
	arduboy.drawFastHLine(0, 55, 128);
	//The screen is 16 tiles wide meaning at most 17 can be visible
	for (uint8_t i = 0; i <= 17; ++i)
		sprites.drawSelfMasked(i * 8 - roadOffset, 56, road, 0);
	arduboy.display(CLEAR_BUFFER);
}
