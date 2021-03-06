#include <ckeyboard.h>

char scan_code_to_char(const KeyEvent* code) {
    if (!code) return 0;
    if (code->name == KEY_UNKNOWN) return 0;

    if (!code->is_shifted && code->action == KEY_PRESS && code->name >= KEY_0 && code->name <= KEY_9)
        return (code->name - KEY_0) + '0';
    else if (code->action == KEY_PRESS && code->name >= KEY_A && code->name <= KEY_Z)
        return (code->name - KEY_A) + ((code->is_caps_locked != code->is_shifted)? 'A':'a');
    else if (code->action == KEY_PRESS && code->name == KEY_SPACE) return ' ';
    else if (code->action == KEY_PRESS && code->name == KEY_ENTER) return '\n';
    else if (code->action == KEY_PRESS && code->name == KEY_TAB) return '\t';
    
    if (code->is_shifted && code->action == KEY_PRESS) {
        switch (code->name) {
            case KEY_1: return '!';
            case KEY_2: return '@';
            case KEY_3: return 0;
            case KEY_4: return '$';
            case KEY_5: return '%';
            case KEY_6: return '^';
            case KEY_7: return '&';
            case KEY_8: return '*';
            case KEY_9: return '(';
            case KEY_0: return ')';
            case KEY_LEFT_BRACKET: return '{';
            case KEY_RIGHT_BRACKET: return '}';
            case KEY_DASH: return '_';
            case KEY_EQUALS: return '+';
            case KEY_SEMI_COLON: return ':';
            case KEY_SINGLE_QUOTE: return '"';
            case KEY_BACK_SLASH: return '|';
            case KEY_BACK_TICK: return '~';
            case KEY_COMMA: return '<';
            case KEY_DOT: return '>';
            case KEY_SLASH: return '?';
            default: {}
        }
    } else if (code->action == KEY_PRESS) {
        switch (code->name) {
            case KEY_DASH: return '-';
            case KEY_EQUALS: return '=';
            case KEY_LEFT_BRACKET: return '[';
            case KEY_RIGHT_BRACKET: return ']';
            case KEY_SEMI_COLON: return ';';
            case KEY_SINGLE_QUOTE: return '\'';
            case KEY_BACK_SLASH: return '\\';
            case KEY_BACK_TICK: return '`';
            case KEY_COMMA: return ',';
            case KEY_DOT: return '.';
            case KEY_SLASH: return '/';
            default: {}
        }
    }

    return 0;
}