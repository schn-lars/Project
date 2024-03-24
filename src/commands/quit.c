/*
 * This file executes the exit command.
 */
#include "quit.h"

void quit()
{
    notify("Have a nice day.");
    shell_running = 0;
}