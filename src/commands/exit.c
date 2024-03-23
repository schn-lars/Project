/*
 * This file executes the exit command.
 */
#include "exit.h"

void exit()
{
    notify("Have a nice day.");
    shell_running = 0;
}