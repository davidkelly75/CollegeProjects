/*

Demonstration video: https://youtu.be/t2jz5UvljHI

Copyright (C) 2014  Frank Duignan
Modified by David Kelly

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "realm.h"
#include "stm32l031lib.h"
// Find types: h(ealth),s(trength),m(agic),g(old),w(eapon)
static const char FindTypes[]={'h','s','m','g','w'};


// The following arrays define the bad guys and 
// their battle properies - ordering matters!
// Baddie types : O(gre),T(roll),D(ragon),H(ag)
static const char Baddies[]={'O','T','D','H'};
// The following is 4 sets of 4 damage types
static const byte WeaponDamage[]={10,10,5,25,10,10,5,25,10,15,5,15,5,5,2,10};
#define ICE_SPELL_COST 10
#define FIRE_SPELL_COST 20
#define LIGHTNING_SPELL_COST 30

// Define Musical notes as their frequencies
#define C4 261.63  // C4
#define Cs4 277.18 // C sharp 4 (Cs4)
#define D4 293.66
#define Ds4 311.13
#define E4 329.63
#define F4 349.23
#define Fs4 369.99
#define G4 392
#define Gs4 415.3
#define A4 440
#define As4 466.16
#define B4 493.88
#define C5 523.25 // C5
#define Cs5 554.37
#define D5 587.33
#define Ds5 622.25
#define E5 659.25
#define F5 698.46
#define Fs5 739.99
#define G5 783.99
#define Gs5 830.61
#define A5 880
#define As5 932.33
#define B5 987.77
#define C6 1046.5 // C6
#define Cs6 1108.73
#define D6 1174.66
#define Ds6 1244. 51
#define E6 1318.51
#define F6 1396.91
#define Fs6 1479.98
#define G6 1567.98
#define Gs6 1661.22
#define A6 1760
#define As6 1864.66
#define B6 1975.53
#define C7 2093 // C7
#define Cs7 2217.46

static const byte FreezeSpellDamage[]={10,20,5,0};
static const byte FireSpellDamage[]={20,10,5,0};
static const byte LightningSpellDamage[]={15,10,25,0};
static const byte BadGuyDamage[]={10,10,15,5};
static int GameStarted = 0;
static tPlayer thePlayer;
static tRealm theRealm;
volatile uint32_t CycleCount=0;

void SysTick_Handler(void)
{
	
	if (CycleCount > 0) // Any cycles remaining?
	{ // yes
		GPIOA->ODR ^= 1; // Toggle Port A bit 0
		CycleCount--;
	}
	else
	{
		GPIOA->ODR &= ~1; // otherwise turn off the speaker.
	}
}

// This function allows you to make a specific pitch using the buzzer.
// It takes two parameters; the frequency and duration.
void playNote(int Frequency, int Duration)
{
	uint32_t LoadValue;
	LoadValue = 16000000/(2*Frequency); // The speed of the chip (16000000) is divided by two times the desired frequency.
	SysTick->LOAD = LoadValue;
	CycleCount = (Duration * Frequency)/1000;
	while(CycleCount > 0); // wait for note to finish
}

void playStartSound() // Startup sound
{
	playNote(C5,200);
	playNote(E5,200);
	playNote(G5,200);
	playNote(C6,200);
	playNote(E5,200);
	playNote(G5,200);
	playNote(C6,200);
	playNote(E6,200);
}

void playMovementSound() // play a blip sound on every movement
{
	playNote(C4,100);
}

void playRockSound() // Plays a bump sound when you hit a rock
{
	playNote(200,100);
}

void playPowerUpSound() // Mario Power Up Sound
{
	playNote(C4, 100);
	playNote(G4, 100);
	playNote(Gs4, 100);
	playNote(Cs4, 100);
	playNote(Gs4, 100);
	playNote(A4, 100);
	playNote(D4, 100);
	playNote(A4, 100);
	playNote(As4, 100);
	playNote(Ds4, 100);
	playNote(As4, 100);
	playNote(B4, 100);
}

void playNewWeaponSound() // Another Mario Power Up Sound
{
	int d = 50;
	playNote(C4, d);
	playNote(E4, d);
	playNote(G4, d);
	playNote(C5, d);
	playNote(G4, d);
	playNote(C5, d);
	playNote(E5, d);
	playNote(G5, d);
	playNote(E6, d);
	playNote(G5, d);
	
	playNote(D4, d);
	playNote(Fs4, d);
	playNote(A4, d);
	playNote(D5, d);
	playNote(A4, d);
	playNote(D5, d);
	playNote(Fs5, d);
	playNote(A5, d);
	playNote(Fs6, d);
	playNote(A5, d);
	
	playNote(E4, d);
	playNote(Gs4, d);
	playNote(B4, d);
	playNote(E5, d);
	playNote(B4, d);
	playNote(E5, d);
	playNote(Gs5, d);
	playNote(B5, d);
	playNote(Gs6, d);
	playNote(B5, d);
}

void playBadGuySound() // Evil sound when a bad guy appears
{
	playNote(A4, 100);
	playNote(C5, 100);
	playNote(E5, 100);
	playNote(C5, 100);
	playNote(Gs4, 100);
	playNote(B4, 100);
	playNote(Ds5, 100);
	playNote(B4, 100);
	
	playNote(A4, 100);
	playNote(C5, 100);
	playNote(E5, 100);
	playNote(C5, 100);
	playNote(Gs4, 100);
	playNote(B4, 100);
	playNote(Ds5, 100);
	playNote(B4, 100);
}

void playHelpSound()
{
	playNote(A5,200);
	playNote(F5,200);
	playNote(A5,200);
	playNote(F5,200);
}

void playNewRealmSound() // Plays a C major 7 chord for a new realm
{
	playNote(C4,500);
	playNote(E4,500);
	playNote(G4,500);
	playNote(B4,500);
	playNote(D5,500);
}

void playFightMusic() // This is taken from the 3rd movement of Beethoven's Moonlight Sonata.
{
	int d = 200;
	playNote(Cs4,d);
	playNote(Gs4,d);
	playNote(Cs5,d);
	playNote(E5,d);
	playNote(Gs5,d);
	playNote(Cs5,d);
	playNote(E5,d);
	playNote(Gs5,d);
	playNote(Cs6,d);
	playNote(E5,d);
	playNote(Gs5,d);
	playNote(Cs6,d);
	playNote(E6,d);
	playNote(Gs5,d);
	playNote(Cs6,d);
	playNote(E6,d);
	playNote(Gs6,d);
	playNote(Cs6,d);
	playNote(E6,d);
	playNote(Gs6,d);
	playNote(Cs7,d);
}

void playChickenOutSound() // Plays an unhappy sound when you chicken out
{
	playNote(Ds4,200);
	playNote(D4,200);
	playNote(Cs4,200);
	playNote(D4,200);
}

void playAttackSound() // Plays a really quick blip every time you attack
{
	playNote(C4,50);
	playNote(C5,50);
	playNote(C6,50);
	playNote(C7,50);
}

void playWinningSound() // Plays a happy tune to let you know you've won!
{
	playNote(Cs6,200);
	playNote(C6,200);
	playNote(Cs6,200);
	playNote(Gs5,200);
	playNote(F5,200);
	playNote(Gs5,200);
	playNote(Cs6,200);
	playNote(C6,200);
	playNote(Cs6,200);
}

void playDeathSound() // Plays a short descending chromatic scale when you die
{
	playNote(Ds4,300);
	playNote(Ds5,300);
	playNote(D4,300);
	playNote(D5,300);
	playNote(Cs4,300);
	playNote(Cs5,300);
	playNote(C4,300);
	playNote(C5,300);	
}

__attribute__((noreturn)) void runGame(void)  
{
	char ch;
	
	// Set up the ports to make the LEDs light up
	RCC->IOPENR = (1 << 2) + (1 << 1) + (1 << 0); // set bits 0,1,2, to turn on ports A, B and C.
	pinMode(GPIOA,1,1); // Make GPIOA1 an output.
	pinMode(GPIOA,3,1); // Make GPIOA3 an output.
	pinMode(GPIOA,4,1); // Make GPIOA4 an output.
	
	// Set the SysTick system up to make sound from the buzzer
	pinMode(GPIOA,0,1); // Make PORTA Bit 0 an output
	SysTick->LOAD = 15999;   // 16MHz / 16000 = 1kHz
	SysTick->CTRL = 7;       // enable systick counting and interrupts, use core clock
	enable_interrupts();
	
	eputs("MicroRealms on the STM32L031\r\n");	
	showHelp();		
	while(GameStarted == 0)
	{
		
		showGameMessage("Press S to start a new game\r\n");
		ch = getUserInput();			
		
		if ( (ch == 'S') || (ch == 's') )
			GameStarted = 1;
	}
	
	initRealm(&theRealm);	
	initPlayer(&thePlayer,&theRealm);
	showPlayer(&thePlayer);
	showRealm(&theRealm,&thePlayer);
	playStartSound();
	showGameMessage("Press H for help");
	
	while (1)
	{
		ch = getUserInput();
		ch = ch | 32; // enforce lower case
		switch (ch) {
			case 'h' : {
				showHelp();
				playHelpSound();
				break;
			}
			case 'w' : {
				showGameMessage("North");
				playMovementSound();
				step('n',&thePlayer,&theRealm);
				break;
			}
			case 's' : {
				showGameMessage("South");
				playMovementSound();
				step('s',&thePlayer,&theRealm);
				break;

			}
			case 'd' : {
				showGameMessage("East");
				playMovementSound();
				step('e',&thePlayer,&theRealm);
				break;
			}
			case 'a' : {
				showGameMessage("West");
				playMovementSound();
				step('w',&thePlayer,&theRealm);
				break;
			}
			case '#' : {		
				if (thePlayer.wealth)		
				{
					showRealm(&theRealm,&thePlayer);
					thePlayer.wealth--;
				}
				else
					showGameMessage("No gold!");
				break;
			}
			case 'p' : {				
				showPlayer(&thePlayer);
				break;
			}
		} // end switch
	} // end while
}
void step(char Direction,tPlayer *Player,tRealm *Realm)
{
	uint8_t new_x, new_y;
	new_x = Player->x;
	new_y = Player->y;
	byte AreaContents;
	switch (Direction) {
		case 'n' :
		{
			if (new_y > 0)
				new_y--;
			break;
		}
		case 's' :
		{
			if (new_y < MAP_HEIGHT-1)
				new_y++;
			break;
		}
		case 'e' :
		{
			if (new_x <  MAP_WIDTH-1)
				new_x++;
			break;
		}
		case 'w' :
		{
			if (new_x > 0)
				new_x--;
			break;
		}		
	}
	AreaContents = Realm->map[new_y][new_x];
	if ( AreaContents == '*')
	{
		showGameMessage("A rock blocks your path.");
		playRockSound();
		return;
	}
	Player->x = new_x;
	Player->y = new_y;
	int Consumed = 0;
	switch (AreaContents)
	{
		
		// const char Baddies[]={'O','T','B','H'};
		case 'O' :{
			showGameMessage("A smelly green Ogre appears before you");
			playBadGuySound();
			
			// Print ASCII Ogre
			eputs("         __,='`````'=/__ \r\n");
			eputs("        '//  (o) \\(o) \\ `'         _,-, \r\n");
			eputs("        //|     ,_)   (`\\      ,-'`_,-\\ \r\n");
			eputs("      ,-~~~\\  `'==='  /-,      \\==```` \\__ \r\n");
			eputs("     /        `----'     `\\     \\       \\/ \r\n");
			eputs("  ,-`                  ,   \\  ,.-\\       \\ \r\n");
			eputs(" /      ,               \\,-`\\`_,-`\\_,..--'\\ \r\n");
			eputs(",`    ,/,              ,>,   )     \\--`````\\ \r\n");
			eputs("(      `\\`---'`  `-,-'`_,<   \\      \\_,.--'` \r\n");
			eputs(" `.      `--. _,-'`_,-`  |    \\ \r\n");
			eputs(" [`-.___   <`_,-'`------(    / \r\n");
			eputs("  (`` _,-\\   \\ --`````````|--` \r\n");
			eputs("   >-`_,-`\\,-` ,          | \r\n");
			eputs(" <`_,'     ,  /\\          / \r\n");
			eputs("  `  \\/\\,-/ `/  \\/`\\_/V\\_/ \r\n");
			eputs("     (  ._. )    ( .__. ) \r\n");
			eputs("     |      |    |      | \r\n");
			eputs("      \\,---_|    |_---./ \r\n");
			eputs("      ooOO(_)    (_)OOoo \r\n\n");
			
			Consumed = doChallenge(Player,0);
			break;
		}
		case 'T' :{
			showGameMessage("An evil troll challenges you");
			playBadGuySound();
			
			// Print ASCII Troll
			eputs("                          .--. \r\n");
			eputs("          .-'   '-.    |\\/    \\| \r\n");
			eputs("        .'         '-._\\   (o)O) \r\n");
			eputs("       /         /         _.--.\\ \r\n");
			eputs("      /|    (    |  /  -. ( -._( \r\n");
			eputs("     /  \\    \\-.__\\ \\_.-'`.`.__' \r\n");
			eputs("     |  /\\    |  / \\ \\     `--')/ \r\n");
			eputs(" .._/  /  /  /  / / \\ \\ \r\n");
			eputs("/  ___/  |  /   \\ \\  \\ \\__ \r\n");
			eputs("\\  \\___  \\ (     \\ \\  `._ `. \r\n");
			eputs(" \\ `-._\\ (  `-.__ | \\    )// \r\n");
			eputs("  \\_-._\\  \\  `-._\\)//    "" \r\n");
			eputs("    `-'    \\ -._\\ "" \r\n");
			eputs("            \\/ \r\n\n");
			
			Consumed = doChallenge(Player,1);
			break;
		}
		case 'D' :{
			showGameMessage("A smouldering Dragon blocks your way !");
			playBadGuySound();
			
			// Print ASCII Dragon
			eputs("_<>=======() \r\n");
			eputs("(/\\___   /|\\\\          ()==========<>_ \r\n");
			eputs("      \\_/ | \\\\        //|\\   ______/ \\) \r\n");
			eputs("        \\_|  \\\\      // | \\_/ \r\n");
			eputs("          \\|\\/|\\_   //  /\\/ \r\n");
			eputs("           (oo)\\ \\_//  / \r\n");
			eputs("          \\//_/\\_\\/ /  | \r\n");
			eputs("         @@/  |=\\  \\  | \r\n");
			eputs("              \\_=\\_ \\ | \r\n");
			eputs("                \\==\\ \\|\\_  \r\n");
			eputs("             __(\\===\\(  )\\ \r\n");
			eputs("            (((~) __(_/   | \r\n");
			eputs("                 (((~) \\  / \r\n");
			eputs("                 ______/ / \r\n");
			eputs("                 '------' \r\n\n");
			
			Consumed = doChallenge(Player,2);
			break;
		}
		case 'H' :{
			showGameMessage("A withered hag cackles at you wickedly");
			playBadGuySound();
			
			// Print ASCII Hag
			eputs("                                      __,,, \r\n");
			eputs("                                 _.--'    / \r\n");
			eputs("                              .-'        / \r\n");
			eputs("                            .'          | \r\n");
			eputs("                          .'           / \r\n");
			eputs("                         /_____________| \r\n");
			eputs("                       /`~~~~~~~~~~~~~~/ \r\n");
			eputs("                     /`               / \r\n");
			eputs("      ____,....----'/_________________|---....,___ \r\n");
			eputs(",--""`             `~~~~~~~~~~~~~~~~~~`           `""--, \r\n");
			eputs("`'----------------.----,------------------------,-------'` \r\n");
			eputs("               _,'.--. \\                         \\ \r\n");
			eputs("             .'  (o   ) \\                        | \r\n");
			eputs("            c   , '--'  |                        / \r\n");
			eputs("           /    )'-.    \\                       / \r\n");
			eputs("          |  .-;        \\                       | \r\n");
			eputs("          \\_/  |___,'    |                    .-' \r\n");
			eputs("         .---.___|_      |                   / \r\n");
			eputs("        (          `     |               .--' \r\n");
			eputs("         '.         __,-'\\             .' \r\n");
			eputs("           `'-----'`      \\        __.' \r\n");
			eputs("                           `-..--'` \r\n\n");
			
			Consumed = doChallenge(Player,3);
			break;
		}
		case 'h' :{
			showGameMessage("You find an elixer of health");
			playPowerUpSound();
			setHealth(Player,Player->health+10);
			Consumed = 1;		
			break;
			
		}
		case 's' :{
			showGameMessage("You find a potion of strength");
			playPowerUpSound();
			Consumed = 1;
			setStrength(Player,Player->strength+1);
			break;
		}
		case 'g' :{
			showGameMessage("You find a shiny golden nugget");
			playPowerUpSound();
			Player->wealth++;			
			Consumed = 1;
			break;
		}
		case 'm' :{
			showGameMessage("You find a magic charm");
			playPowerUpSound();
			Player->magic++;						
			Consumed = 1;
			break;
		}
		case 'w' :{
			Consumed = addWeapon(Player,(int)random(MAX_WEAPONS-1)+1);
			showPlayer(Player);
			break;			
		}
		case 'X' : {
			// Player landed on the exit
			eputs("A door! You exit into a new realm");
			setHealth(Player,100); // maximize health
			initRealm(&theRealm);
			showRealm(&theRealm,Player);
		}
	}
	if (Consumed)
		Realm->map[new_y][new_x] = '.'; // remove any item that was found
}
int doChallenge(tPlayer *Player,int BadGuyIndex)
{
	char ch;
	char Damage;
	const byte *dmg;
	int BadGuyHealth = 100;
	eputs("Press F to fight");
	ch = getUserInput() | 32; // get user input and force lower case
	if (ch == 'f')
	{
		playFightMusic();
		eputs("\r\nChoose action");
		while ( (Player->health > 0) && (BadGuyHealth > 0) )
		{
			// Green, Yellow and Red LEDs indicate level of player's health during fights.
			
			// If health is high, Green LED on.
			if (Player->health > 66)
			{
				// Turn on the Green LED
				GPIOA->ODR |= (1 << 1); // Set bit 1 on Port A to 1
				
				// Turn off the other two LEDs
				GPIOA->ODR &= ~(1u << 3); // Clear bit 3 on Port A to 1
				GPIOA->ODR &= ~(1u << 4); // Clear bit 4 on Port A to 1
			}
			
			// If health is Medium, Yellow LED on.
			if ((Player->health < 66) && (Player->health > 33))
			{
				// Turn on the Yellow LED
				GPIOA->ODR |= (1 << 3); // Set bit 3 on Port A to 1
				
				// Turn off the other two LEDs
				GPIOA->ODR &= ~(1u << 1); // Clear bit 1 on Port A to 1
				GPIOA->ODR &= ~(1u << 4); // Clear bit 4 on Port A to 1
			}
			
			// If health is Low, Red LED on.
			if (Player->health < 33)
			{
				// Turn on the Red LED
				GPIOA->ODR |= (1 << 4); // Set bit 4 on Port A to 1
				
				// Turn off the other two LEDs
				GPIOA->ODR &= ~(1u << 1); // Clear bit 1 on Port A to 1
				GPIOA->ODR &= ~(1u << 3); // Clear bit 3 on Port A to 1
			}
			
			
			// Player takes turn first
			if (Player->magic > ICE_SPELL_COST)
				eputs("(I)CE spell");
			if (Player->magic > FIRE_SPELL_COST)
				eputs("(F)ire spell");
			if (Player->magic > LIGHTNING_SPELL_COST)
				eputs("(L)ightning spell");
			if (Player->Weapon1)
			{
				eputs("(1)Use ");
				eputs(getWeaponName(Player->Weapon1));
			}	
			if (Player->Weapon2)
			{
				eputs("(2)Use ");
				eputs(getWeaponName(Player->Weapon2));
			}
			eputs("(P)unch");
			ch = getUserInput();
			switch (ch)
			{
				case 'i':
				case 'I':
				{
					eputs("\r\nFREEZE!");
					playAttackSound();
					Player->magic -= ICE_SPELL_COST;
					BadGuyHealth -= FreezeSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'f':
				case 'F':
				{
					eputs("\r\nBURN!");
					playAttackSound();
					Player->magic -= FIRE_SPELL_COST;
					BadGuyHealth -= FireSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'l':
				case 'L':
				{
					eputs("\r\nZAP!");
					playAttackSound();
					Player->magic -= LIGHTNING_SPELL_COST;
					BadGuyHealth -= LightningSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case '1':
				{
					dmg = WeaponDamage+(Player->Weapon1<<2)+BadGuyIndex;
					eputs("\r\nTake that!");
					playAttackSound();
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case '2':
				{
					dmg = WeaponDamage+(Player->Weapon2<<2)+BadGuyIndex;
					eputs("\r\nTake that!");
					playAttackSound();
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case 'p':
				case 'P':
				{
					eputs("\r\nThump!");
					playAttackSound();
					BadGuyHealth -= 1+random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				default: {
					eputs("\r\nYou fumble. Uh oh");
				}
			}
			// Bad guy then gets a go 
			
			if (BadGuyHealth < 0)
				BadGuyHealth = 0;
			Damage = (uint8_t)(BadGuyDamage[BadGuyIndex]+(int)random(5));
			setHealth(Player,Player->health - Damage);
			eputs("\r\nHealth: you "); printDecimal(Player->health);
			eputs(", them " );printDecimal((uint32_t)BadGuyHealth);
			eputs("\r\n");
		}
		if (Player->health == 0)
		{ // You died
			eputs("You are dead. Press Reset to restart");
			playDeathSound();
			while(1);
		}
		else
		{ // You won!
			Player->wealth = (uint8_t)(50 + random(50));			
			playWinningSound();
			showGameMessage("You win! Their gold is yours");
			
			// Turn off all the LEDs
			
			GPIOA->ODR &= ~(1u << 1); // Clear bit 3 on Port A to 1
			GPIOA->ODR &= ~(1u << 3); // Clear bit 3 on Port A to 1
			GPIOA->ODR &= ~(1u << 4); // Clear bit 4 on Port A to 1
			
			return 1;
		}
		
	}
	else
	{
		showGameMessage("\r\nOur 'hero' chickens out");
		playChickenOutSound();
		return 0;
	}
}
int addWeapon(tPlayer *Player, int Weapon)
{
	char c;
	eputs("You stumble upon ");
	playNewWeaponSound();
	switch (Weapon)
	{
		case 1:
		{	
			eputs("a mighty axe\r\n");
			
			// Print ASCII axe
			eputs("  ,:\\      /:. \r\n");
			eputs(" //  \\_()_/  \\\\ \r\n");
			eputs("||   |    |   || \r\n");
			eputs("||   |    |   || \r\n");
			eputs("||   |    |   || \r\n");
			eputs(" \\\\  / || \\  // \r\n");
			eputs("  `:/  ||  \\;' \r\n");
			eputs("       || \r\n");
			eputs("       || \r\n");
			eputs("       XX \r\n");
			eputs("       XX \r\n");
			eputs("       XX \r\n");
			eputs("       XX \r\n");
			eputs("       OO \r\n");
			eputs("       `' \r\n");
			break;
		}
		case 2:
		{	
			eputs("a sword with mystical runes\r\n");
			
			// Print ASCII sword
			eputs("               , \r\n");
			eputs("              (@| \r\n");
			eputs(" ,,           ,)|_____________________________________ \r\n");
			eputs("//\\\\8@8@8@8@8@8 / _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \\ \r\n");
			eputs("\\\\//8@8@8@8@8@8 \\_____________________________________/ \r\n");
			eputs(" ``           `)| \r\n");
			eputs("              (@| \r\n");
			eputs("               ` \r\n");
			break;
		}
		case 3:
		{	
			eputs("a bloody flail\r\n");
			
			// Print ASCII flail
			eputs("        |\\ \r\n");
			eputs("        | \\        /| \r\n");
			eputs("        |  \\____  / | \r\n");
			eputs("       /|__/AMMA\\/  | \r\n");
			eputs("     /AMMMMMMMMMMM\\_| \r\n");
			eputs(" ___/AMMMMMMMMMMMMMMA \r\n");
			eputs(" \\   |MVKMMM/ .\\MMMMM\\ \r\n");
			eputs("  \\__/MMMMMM\\  /MMMMMM--- \r\n");
			eputs("  |MMMMMMMMMMMMMMMMMM|  / \r\n");
			eputs("  |MMMM/. \\MM.--MMMMMM\\/ \r\n");
			eputs("  /\\MMM\\  /MM\\  |MMMMMM   ___ \r\n");
			eputs(" /  |MMMMMMMMM\\ |MMMMMM--/   \\-. \r\n");
			eputs("/___/MMMMMMMMMM\\\\|M.--M/___/_|   \\ \r\n");
			eputs("     \\VMM/\\MMMMMMM\\  |      /\\ \\/ \r\n");
			eputs("      \\V/  \\MMMMMMM\\ |     /_  / \r\n");
			eputs("        |  /MMMV'   \\|    |/ _/ \r\n");
			eputs("        | /              _/  / \r\n");
			eputs("        |/              /| \\' \r\n");
			eputs("                       /_  / \r\n");
			eputs("                       /  / \r\n");			
			break;
		}		
		default:
			printDecimal((uint32_t)Weapon);
	}
	if ( (Player->Weapon1) && (Player->Weapon2) )
	{
		// The player has two weapons already.
		showPlayer(Player);
		eputs("You already have two weapons\r\n");		
		eputs("(1) drop Weapon1, (2) for Weapon2, (0) skip");
		c = getUserInput();
		eputchar(c);
		switch(c)
		{
			case '0':{
				return 0; // don't pick up
			}
			case '1':{
				Player->Weapon1 = (uint8_t)Weapon;
				break;
			}
			case '2':{
				Player->Weapon2 = (uint8_t)Weapon;
				break;
			}
		}
	}
	else
	{
		if (!Player->Weapon1)
		{
			Player->Weapon1 = (uint8_t)Weapon;	
		}
		else if (!Player->Weapon2)
		{
			Player->Weapon2 = (uint8_t)Weapon;
		}
	}	
	return 1;
}
const char *getWeaponName(int index)
{
	switch (index)
	{
		case 0:return "Empty"; 
		case 1:return "Axe";
		case 2:return "Sword"; 
		case 3:return "Flail"; 
	}
	return "Unknown";
}

void setHealth(tPlayer *Player,int health)
{
	if (health > 100)
		health = 100;
	if (health < 0)
		health = 0;
	Player->health = (uint8_t)health;
}	
void setStrength(tPlayer *Player, byte strength)
{
	if (strength > 100)
		strength = 100;
	Player->strength = strength;
}
void initPlayer(tPlayer *Player,tRealm *Realm)
{
	// get the player name
	int index=0;
	byte x,y;
	char ch=0;
	// Initialize the player's attributes
	eputs("Enter the player's name: ");
	while ( (index < MAX_NAME_LEN) && (ch != '\n') && (ch != '\r'))
	{
		ch = getUserInput();
		if ( ch > '0' ) // strip conrol characters
		{
			
			Player->name[index++]=ch;
			eputchar(ch);
		}
	}
	Player->name[index]=0; // terminate the name
	setHealth(Player,100);
	Player->strength=(uint8_t)(50+random(50));
	Player->magic=(uint8_t)(50+random(50));	
	Player->wealth=(uint8_t)(10+random(10));
	Player->Weapon1 = 0;
	Player->Weapon2 = 0;
	// Initialize the player's location
	// Make sure the player does not land
	// on an occupied space to begin with
	do {
		x=(uint8_t)random(MAP_WIDTH);
		y=(uint8_t)random(MAP_HEIGHT);
		
	} while(Realm->map[y][x] != '.');
	Player->x=x;
	Player->y=y;
}
void showPlayer(tPlayer *Player)
{
	eputs("\r\nName: ");
	eputs(Player->name);
	eputs("\r\nhealth: ");
	printDecimal(Player->health);
	eputs("\r\nstrength: ");
	printDecimal(Player->strength);
	eputs("\r\nmagic: ");
	printDecimal(Player->magic);
	eputs("\r\nwealth: ");
	printDecimal(Player->wealth);	
	eputs("\r\nLocation : ");
	printDecimal(Player->x);
	eputs(" , ");
	printDecimal(Player->y);	
	eputs("\r\nWeapon1 : ");
	eputs(getWeaponName(Player->Weapon1));
	eputs(" Weapon2 : ");
	eputs(getWeaponName(Player->Weapon2));
}
void initRealm(tRealm *Realm)
{
	unsigned int x,y;
	unsigned int Rnd;
	// clear the map to begin with
	for (y=0;y < MAP_HEIGHT; y++)
	{
		for (x=0; x < MAP_WIDTH; x++)
		{
			Rnd = random(100);
			
			if (Rnd >= 98) // put in some baddies
				Realm->map[y][x]=	Baddies[random(sizeof(Baddies))];
			else if (Rnd >= 95) // put in some good stuff
				Realm->map[y][x]=	FindTypes[random(sizeof(FindTypes))];
			else if (Rnd >= 90) // put in some rocks
				Realm->map[y][x]='*'; 
			else // put in empty space
				Realm->map[y][x] = '.';	
		}
	}
	
	// finally put the exit to the next level in
	x = random(MAP_WIDTH);
	y = random(MAP_HEIGHT);
	Realm->map[y][x]='X';
	playNewRealmSound();
}
void showRealm(tRealm *Realm,tPlayer *Player)
{
	int x,y;
	eputs("\r\nThe Realm:\r\n");	
	for (y=0;y<MAP_HEIGHT;y++)
	{
		for (x=0;x<MAP_WIDTH;x++)
		{
			
			if ( (x==Player->x) && (y==Player->y))
				eputchar('@');
			else
				eputchar(Realm->map[y][x]);
		}
		eputs("\r\n");
	}
	eputs("\r\nLegend\r\n");
	eputs("(T)roll, (O)gre, (D)ragon, (H)ag, e(X)it\r\n");
	eputs("(w)eapon, (g)old), (m)agic, (s)trength\r\n");
	eputs("@=You\r\n");
}
void showHelp()
{

	eputs("Help\r\n");
	eputs("W,S,D,A : go North, South, East, West\r\n");
	eputs("# : show map (cost: 1 gold piece)\r\n");
	eputs("(H)elp\r\n");
	eputs("(P)layer details\r\n");
	
}

void showGameMessage(char *Msg)
{
	eputs(Msg);
	eputs("\r\nReady\r\n");	
}
char getUserInput()
{
	char ch = 0;
	
	while (ch == 0)
		ch = egetchar();
	return ch;
}
unsigned random(unsigned range)
{
	// Implementing my own version of modulus
	// as it is a lot smaller than the library version
	// To prevent very long subtract loops, the
	// size of the value returned from prbs has been
	// restricted to 8 bits.
	unsigned Rvalue = (prbs()&0xff);
	while (Rvalue >= range)
		Rvalue -= range; 
	return Rvalue;
}
void zap()
{

}
uint32_t prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified
	static unsigned long shift_register=0xa5a5a5a5;
	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's
}
