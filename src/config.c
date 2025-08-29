#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

#include "config.h"

typedef enum {
    NONE,
    BIRTH,
    SURVIVAL,
} RulesParseState;

Rules config_parse_rules(char *rules_str) {
    Rules rules = {0};
    RulesParseState state = NONE;

    for (size_t i = 0; rules_str[i] != 0; ++i) {
        char c = toupper((unsigned char)rules_str[i]);

        if (c == 'B')
            state = BIRTH;
        else if (c == 'S')
            state = SURVIVAL;
        else if (isdigit(c)) {
            uint8_t n = c - '0';
            if (n > 8)
                continue;

            if (state == BIRTH)
                rules.birth |= (1 << n);
            else if (state == SURVIVAL)
                rules.survival |= (1 << n);
        }
    }

    return rules;
}

uint32_t config_parse_color(char *hex_color) {
    while (*hex_color == '#')
        hex_color++;

    char c;
    uint32_t value, color = 0xFF000000;

    for (int i = 0; i < 6; ++i) {
        if ((c = hex_color[i]) == 0)
            break;

        if (c >= '0' && c <= '9')
            value = c - '0';
        else if (c >= 'A' && c <= 'F')
            value = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f')
            value = c - 'a' + 10;
        else
            return 0; // Black

        color |= value << ((5 - i) * 4);
    }

    return color;
}
