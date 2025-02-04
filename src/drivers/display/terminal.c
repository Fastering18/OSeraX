#include <limine.h>
#include <kernel.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define DECLARE_LOCK(name) volatile int name##Locked;
#define LOCK(name)                                             \
    while (!__sync_bool_compare_and_swap(&name##Locked, 0, 1)) \
        ;                                                      \
    __sync_synchronize();
#define UNLOCK(name)      \
    __sync_synchronize(); \
    name##Locked = 0;

char *resetcolour = "\033[0m";

struct limine_terminal **terminals;
struct limine_terminal *default_trm;
uint64_t term_count;

void _terminal_printc(char c, struct limine_terminal *term);
void terminal_printc(char c);

void terminal_init()
{
    terminals = terminal_request.response->terminals;
    default_trm = terminals[0];
    term_count = terminal_request.response->terminal_count;
}

void _terminal_print(const char *str, struct limine_terminal *term)
{
    if (terminal_request.response == NULL || term == NULL)
        return;
    terminal_request.response->write(term, str, strlen(str));
}
void terminal_print(const char *str)
{
    _terminal_print(str, default_trm);
}

void _terminal_printi(int num, struct limine_terminal *term)
{
    if (num != 0)
    {
        char temp[10];
        int i = 0;
        if (num < 0)
        {
            _terminal_printc('-', term);
            num = -num;
        }
        if (num <= 0)
        {
            temp[i++] = '8';
            num = -(num / 10);
        }
        while (num > 0)
        {
            temp[i++] = num % 10 + '0';
            num /= 10;
        }
        while (--i >= 0)
            _terminal_printc(temp[i], term);
    }
    else
        _terminal_printc('0', term);
}
void terminal_printi(int num)
{
    _terminal_printi(num, default_trm);
}

void _terminal_printc(char c, struct limine_terminal *term)
{
    char str[] = {c, 0};
    _terminal_print(str, term);
}
void terminal_printc(char c)
{
    _terminal_printc(c, default_trm);
}

void terminal_println(const char *str)
{
    char destnl[sizeof(str) + 1]; // "<str>\n\0"
    strcat(destnl, str);
    strcat(destnl, "\n");
    // terminal_printi(strlen(str));
    _terminal_print(destnl, default_trm);
}

void terminal_print_info(const char *str)
{
    char sos[sizeof(str) + sizeof("\033[0;34m[INFO]:\033[0m ") + 1];
    strcat(sos, "\033[0;34m[INFO]:\033[0m ");
    strcat(sos, str);
    terminal_println(sos);
}

void _terminal_reset(struct limine_terminal *term)
{
    // lockit(term_lock);
    if (terminal_request.response == NULL || term == NULL)
        return;
    terminal_request.response->write(term, "", LIMINE_TERMINAL_FULL_REFRESH);
}
void terminal_reset()
{
    terminal_reset(default_trm);
}

void _terminal_clear(const char *ansii_colour, struct limine_terminal *term)
{
    _terminal_print(ansii_colour, term);
    _terminal_print("\033[H\033[2J", term);
}
void terminal_clear(const char *ansii_colour)
{
    _terminal_clear(ansii_colour, default_trm);
}

DECLARE_LOCK(printf)
void printf(const char *format, ...)
{
    LOCK(printf)
    char *str = format;

    char ite;
    int numargs = 0;
    for (int z = 0; z < strlen(str); z++)
    {
        if (str[z + 1] != '%')
        {
            numargs++;
        }
    }

    va_list listPointer;
    va_start(listPointer, numargs);

    for (int z = 0; z < strlen(str); z++)
    {
        ite = str[z];
        if (ite == '%')
        {
            switch (str[z + 1])
            {
            case 'c':
            {
                char arg = va_arg(listPointer, char);
                terminal_printc(arg);
                break;
            }
            case 's':
            {
                char *arg = va_arg(listPointer, char *);
                terminal_print(arg);

                break;
            }
            case 'd':
            {
                int arg = va_arg(listPointer, int);
                char conv[10];
                itoa(arg, conv, 10);

                terminal_print(conv);
                break;
            }
            case 'l':
            {
                if (str[z + 2] != 'l')
                    break;
                z += 2;

                switch (str[z + 1])
                {
                case 'd':
                {
                    uint64_t arg = va_arg(listPointer, uint64_t);
                    char conv[33];
                    ltoa(arg, conv, 10);

                    terminal_print(conv);
                    break;
                }
                }
                break;
            }
            case 'x':
            {
                int arg = va_arg(listPointer, int);
                char conv[10];
                itoa(arg, conv, 16);
                terminal_print(conv);
                break;
            }
            }
            z++;
        }
        else
        {
            // drawchar(ite, 0xffffffff, 0x00000000);
            terminal_printc(ite);
        }
    }
    // drawstring(current , 0xffffffff, 0x00000000);
    UNLOCK(printf)
    va_end(listPointer);
}


void terminal_cursor_up(int lines)
{
    printf("\033[%dA", lines);
}

void terminal_cursor_down(int lines)
{
    printf("\033[%dB", lines);
}

void terminal_cursor_right(int lines)
{
    printf("\033[%dC", lines);
}

void terminal_cursor_left(int lines)
{
    printf("\033[%dD", lines);
}


void *terminal_callback(struct limine_terminal *term, uint64_t type, uint64_t first, uint64_t second, uint64_t third)
{
    terminal_printi(type);
    terminal_print("kbd cb called");
    switch (type)
    {
    case LIMINE_TERMINAL_CB_KBD_LEDS:
        terminal_print("\nled");
        break;
    default:
        break;
    }
}