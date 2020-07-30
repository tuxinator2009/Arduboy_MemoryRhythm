#include <Arduboy2.h>
#include <Sprites.h>
#include <ArduboyTones.h>
#include "graphics.h"

Arduboy2Base arduboy;
ArduboyTones tones(arduboy.audio.enabled);
Sprites sprites;
//notes starts at 208hz since below that can't be heard well on the Arduboy's tiny piezo
//notes stops at 7902hz for similar reasons
//I've also removed all the sharps/flats to keep it to normal notes for increased pitch as the player progresses
//5 octaves with 7 notes per octave and 6 actions to perform leaving 1 note for the interim beat.
uint16_t notes[] =
{
	 262, 294, 330, 349, 392, 440, 494,
	 523, 587, 659, 698, 784, 880, 988,
	1047,1175,1319,1397,1568,1760,1976,
	2093,2349,2637,2794,3136,3520,3951,
	4186,4699,5274,5588,6272,7040,7902
};

int frame = 0;

void setup()
{
	arduboy.begin();
	arduboy.clear();
	arduboy.setFrameRate(60);
}

void loop()
{
	if (!arduboy.nextFrame())
		return;
	//Draw a horizontal line for the ground for now
	arduboy.drawFastHLine(0, 56, 128);
	sprites.drawSelfMasked(52, 24, sprite, frame);
	arduboy.display(CLEAR_BUFFER);
}
