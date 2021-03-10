/**
*  Created for YASCON CTF 2020
*
*  Challenge name: RetroArch
*  Category: Reverse engineering
*  Author: @odinshell
*  Date: 12-10-2020
*/

// include psp stuff
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// include commons stuff
#include "../common/callback.h"
#include "../common/ui.h"

// configure PSP stuff
#define VERS 1
#define REVS 0

PSP_MODULE_INFO("RetroArch", PSP_MODULE_USER, VERS, REVS);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

// easier printing
#define printf pspDebugScreenPrintf

// cheatcode stack
int n, top = -1, top2 = -1, *stack, *stack2;

void push(int x)
{
	if (top == n)
		return;
	stack[++top] = x;
}

void push2(int x)
{
	if (top2 == n)
		return;
	stack2[++top2] = x;
}

int pop()
{
	if (top == -1)
		return -1;
	return stack[top--];
}

int main(int argc, char **argv)
{
	// basic init
	setupExitCallback();
	pspDebugScreenInit();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	char *banner =
		"\n/$$$$$$$$                     /$$           /$$                 "
		"\n|_____ $$                     | $$          | $$                "
		"\n     /$$/   /$$$$$$   /$$$$$$ | $$  /$$$$$$ | $$$$$$$   /$$$$$$$"
		"\n    /$$/   /$$__  $$ |____  $$| $$ |____  $$| $$__  $$ /$$_____/"
		"\n   /$$/   | $$$$$$$$  /$$$$$$$| $$  /$$$$$$$| $$  \ $$|  $$$$$$ "
		"\n  /$$/    | $$_____/ /$$__  $$| $$ /$$__  $$| $$  | $$ \____  $$"
		"\n /$$$$$$$$|  $$$$$$$|  $$$$$$$| $$|  $$$$$$$| $$$$$$$/ /$$$$$$$/"
		"\n|________/ \_______/ \_______/|__/ \_______/|_______/ |_______/ "
		"\n\n\n\n Usable keys:"
		"\n\n SQUARE, CROSS, CIRCLE, TRIANGLE, LTRIGGER, RTRIGGER";

	n = 10;
	stack = (int *)malloc(n * sizeof(int));
	stack2 = (int *)malloc(n * sizeof(int));

	int check[10] = {1337, 1337, 1339, 1339, 1336, 1338, 1336, 1338, 1341, 1340};
	bool cleared = false;

	printf("%s\n", banner);

	// Cheat code controls:
	// tri tri x  x sq cir sq cir left right
	//  0  0   2  2  1  3  1   3   4   5
	// XOR-key: 1337
	// resultant array: [1337 1337 1339 1339 1336 1338 1336 1338 1341 1340]

	while (isRunning())
	{
		sceDisplayWaitVblankStart();
		pspDebugScreenSetXY(0, 0);
		pollPad();
		pollLatch();

		if (top > 9)
		{
			// recursive pop
			for (int j = 0; j <= 10; j++)
			{
				pop();
			}
			printf("\n Here we go again!");
		}
		else if (top == 9)
		{
			int count = 0;
			for (int i = 0; i < 9; i++)
			{
				if ((stack[i] ^ 1337) == check[i])
				{
					count += 1;
				}
				if (count == 9)
				{
					if (cleared == false)
					{
						pspDebugScreenClear();
						printf("\nCheat activated!\n\n");
						for (int p = 0; p < 9; p++)
						{
							push2((stack[p] ^ (1337 - 320)) >> p);
						}
						printf("YASCON{");
						for (int k = 0; k < 9; k++)
						{
							printf("%d", stack2[k]);
						}
						printf("} \n");
						cleared = true;
					}
				}
			}
		}

		// Key listerners
		if (isKeyDown(PSP_CTRL_TRIANGLE))
		{
			pspDebugScreenClear();
			printf("\nTriangle is down!\n");
			push(0);
		}
		if (isKeyDown(PSP_CTRL_SQUARE))
		{
			pspDebugScreenClear();
			printf("\nSquare is down!\n");
			push(1);
		}
		if (isKeyDown(PSP_CTRL_CROSS))
		{
			pspDebugScreenClear();
			printf("\nCross is down!\n");
			push(2);
		}
		if (isKeyDown(PSP_CTRL_CIRCLE))
		{
			pspDebugScreenClear();
			printf("\nCircle is down!\n");
			push(3);
		}
		if (isKeyDown(PSP_CTRL_LTRIGGER))
		{
			pspDebugScreenClear();
			printf("\nLTRIGGER is down!\n");
			push(4);
		}
		if (isKeyDown(PSP_CTRL_RTRIGGER))
		{
			pspDebugScreenClear();
			printf("\nRTRIGGER is down!\n");
			push(5);
		}
	}

	sceKernelExitGame();
	return 0;
}