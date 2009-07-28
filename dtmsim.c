/*
	DETERMINISTIC TURING MACHINE SIMULATOR
	COPYRIGHT (C) ZEE ZUO 2009

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	ALONG WITH this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DIRECTION int
#define LEFT -1
#define RIGHT 1
#define TSTART 5000
#define BLANK 'B'

struct Rule
{
	int forstate;
	char fortape;
	int tostate;
	char totape;
	DIRECTION direction;
};

int nrules;
struct Rule rules[10001];
char tape[10001];

int sbys;
int leftmost, rightmost;

void pressenter()
{
	char buffer[10001];
	printf("Press enter to continue.");
	fgets(buffer, 10000, stdin);
}

int runTM(int head, int state)
{
	int i, j, thb, nh;
	
	for(i=1; i<=nrules; ++i)
	{
		if(rules[i].forstate != state) continue;
		if(rules[i].fortape != tape[head]) continue;
		thb = tape[head];
		tape[head] = rules[i].totape;
		nh = head + rules[i].direction;
		if(nh<leftmost) leftmost = nh;
		if(nh>rightmost) rightmost = nh;
		if(sbys)
		{
			printf("\nFor state %d and tape symbol %c, write %c, go to state %d, and move ",
				state, thb, rules[i].totape, rules[i].tostate);
			if(nh > head) printf("right"); else printf("left"); printf(".\n");
			if(nh != leftmost) for(j=1; j<=(nh-leftmost)*2; ++j) printf(" ");
			printf("H\n");
			for(j=leftmost; j<=rightmost; ++j) printf("%c ", tape[j]);
			printf("\n");
			pressenter();
			printf("\n");
		}

		return runTM(nh, rules[i].tostate);
	}

	return state;
}

void unknowndie()
{
	printf("Unknown command: Type -h for help.\n");
	exit(1);
}

void nofiledie(char* filename)
{
	printf("Cannot find specified file: %s.\n", filename);
	exit(1);
}

void badargdie()
{
	printf("Too few arguments. Type dtmsim -h for help.\n");
	exit(1);
}

void printhelp()
{
	printf("Usage: dtmsim [rulefile] [input_tape]\n\n");
	printf("rulefile: A file representing the simulated Turing machine. See <http://dtmsim.googlecode.com/> for more information.\n");
	printf("input_tape: The input tape. This should not include the character \'B\', which is used for blank symbol.");
	exit(0);
}

void invalidstatedie(char* filename, int nthrule, int wrongstate, int correctmaxstate)
{
	printf("In input file %s: \n", filename);
	printf("\tRule #%d: %d is not a valid state; should be between 0 and %d.\n", nthrule, wrongstate, correctmaxstate);
	exit(1);
}

int main(int argc, char** argv)
{
	FILE* frule;
	int intbuf, i;
	char charbuf;
	int nstates, naccp, ntlen;
	int accp[10001];
	int finalstate;
	char buffer[1001];

	for(i=1; i<=10000; ++i) tape[i] = BLANK;

	if((!strcmp(argv[1], "-h"))||(!strcmp(argv[1], "--help"))) printhelp();
	if(argc < 2) badargdie();
	frule = fopen(argv[1], "r");
	if(frule == NULL) nofiledie(argv[1]);
	
	sbys = 0;
	if(argc >= 4) for(i=3; i<argc; ++i)
	{
		if(!strcmp(argv[i], "-s")) sbys = 1;
	}

	fscanf(frule, "%d %d", &nstates, &naccp);
	for(i=1; i<=naccp; ++i) fscanf(frule, "%d", accp+i);
	
	fscanf(frule, "%d", &nrules);
	for(i=1; i<=nrules; ++i)
	{
		fscanf(frule, "%d", &rules[i].forstate);
		if(rules[i].forstate > nstates || rules[i].forstate < 0) invalidstatedie(argv[1], i, rules[i].forstate, nstates);
		fscanf(frule, "%s", &buffer); rules[i].fortape = buffer[0];
		fscanf(frule, "%d", &rules[i].tostate);
		if(rules[i].tostate > nstates || rules[i].tostate < 0) invalidstatedie(argv[1], i, rules[i].tostate, nstates);
		fscanf(frule, "%s", &buffer); rules[i].totape = buffer[0];
		fscanf(frule, "%s", &buffer); charbuf = buffer[0];
		if(charbuf == 'L') rules[i].direction = LEFT;
		else rules[i].direction = RIGHT;
	}

	for(i=0; i<strlen(argv[2]); ++i)
	{
		if(argv[2][i] == 'B')
		{
			printf("Error: Character %d of the input tape is a blank symbol.\n", i+1);
			exit(1);
		}
		tape[TSTART+i] = argv[2][i];
	}

	leftmost = TSTART;
	rightmost = TSTART + strlen(argv[2]) - 1;
	if(sbys)
	{
		printf("Start in state 0 and tape symbol %c.\n", tape[TSTART]);
		printf("H\n");
		for(i=leftmost; i<=rightmost; ++i) printf("%c ", tape[i]); printf("\n");
		pressenter();
	}

	finalstate = runTM(TSTART, 0);
	printf("Final tape (omitting blank squares):\n");
	for(i=1; i<=10000; ++i)
	{
		if(tape[i] != BLANK) printf("%c ", tape[i]);
	}
	printf("\n");
	for(i=1; i<=naccp; ++i) 
		if(finalstate == accp[i])
			printf("Turing machine died accepting the input tape (%d).\n", finalstate); 
		else
			printf("Turing machine died NOT accepting the state (%d).\n", finalstate);

	return 0;
}
