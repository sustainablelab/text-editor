#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

typedef uint8_t u8;
typedef u8 bool;
#define true 1
#define false 0

// Default terminal attributes
struct termios default_termios;
// Modified terminal attributes
struct termios mod_termios;

void restore_default_mode(void)
{ // Restore the default terminal attributes.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_termios);
}

void canonical_off(void)
{
    /* *************DOC***************
     * Read each key as it is typed. Do not wait for Enter.
     * ****************************/

    mod_termios.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod_termios);
}

void hide_keystrokes(void)
{
    /* *************DOC***************
     * Make keystrokes invisible.
     * ****************************/

    mod_termios.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod_termios);
}

void turn_off_signals(void)
{
    /* *************DOC***************
     * Turn off Ctrl-C (terminate) and Ctrl-Z (suspend)
     * ****************************/

    mod_termios.c_lflag &= ~(ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod_termios);
}

void turn_off_sw_flow_ctrl(void)
{
    /* *************DOC***************
     * No software flow control -- Ctrl-S: XOFF (pause), Ctrl-Q: XON (resume)
     * ****************************/

    mod_termios.c_iflag &= ~(IXON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod_termios);
}

void turn_off_ctrl_v(void)
{
    /* *************DOC***************
     * On some systems Ctrl-V is the esc sequence for a literal character.
     * ****************************/

    mod_termios.c_lflag &= ~(IEXTEN);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod_termios);
}

void turn_off_ctrl_m(void)
{
    /* *************DOC***************
     * Do not convert Ctrl-M (13 `\r`) carriage return to `\n` (10)
     * ****************************/

    mod_termios.c_iflag &= ~(ICRNL);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod_termios);
}

void turn_off_output_processing(void)
{
    /* *************DOC***************
     * Do not convert `\n` to `\r\n`.
     * ****************************/

    mod_termios.c_oflag &= ~(OPOST);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod_termios);
}

int main()
{
    // Read the terminal attributes.
    tcgetattr(STDIN_FILENO, &default_termios);
    // Make a copy for modifying.
    mod_termios = default_termios;

    // Restore default terminal attributes when the program exits.
    atexit(restore_default_mode);

    // Process each byte as it comes in (do not wait for Enter)
    canonical_off();

    // Start with keystroke visibility off.
    hide_keystrokes();

    // Turn off Ctrl-C (terminate) and Ctrl-Z (suspend) signals
    turn_off_signals();

    // Turn off software flow control Ctrl-S (pause) Ctrl-Q (resume)
    turn_off_sw_flow_ctrl();

    // Turn off Ctrl-V escape sequence for literal characters
    turn_off_ctrl_v();

    // Leave Ctrl-M as `\r`, do not convert to `\n`.
    turn_off_ctrl_m();

    // Do not convert `\n` to `\r\n`.
    // NOTE: Now I must use `\r\n` for newline in `printf()` statements.
    turn_off_output_processing();

    /* =====[ READ STDIN ]===== */
    char c;
    bool quit = false;
    while ( (!quit) && (read(STDIN_FILENO, &c, 1) == 1) )
    {

        switch(c)
        {
            // q to quit
            case 'q': quit = true; break;
            // Detect SPACE         (placeholder to do something).
            case ' ': printf("--SPACE--");  fflush(stdout); break;
            // Detect TAB           (placeholder to do something).
            case 9: printf("--TAB--");      fflush(stdout); break;
            // Detect ESC           (placeholder to do something).
            case 27: printf("--ESC--");     fflush(stdout); break;
            // Detect BACKSPACE     (placeholder to do something).
            case 127: printf("--BACK--");   fflush(stdout); break;
            //
            default: break;
        }

        // Print the character.
        // My environment requires I flush stdout for immediate print.
        if (iscntrl(c))
        {
            printf("%d\r\n", c); fflush(stdout);
        }
        else
        {
            printf("%d ('%c')\r\n", c, c); fflush(stdout);
        }
    }
    printf("Quit\r\n");
    return 0;
}
