#include "quit.h"

/**
 * This method gets called when the user enters 'quit'.
 * Breaks loop in davis.c->davis()
 */
void quit()
{
    notify("Have a nice day.");
    shell_running = 0;
}