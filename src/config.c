#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

#include "config.h"

typedef enum {
    NONE,
    BIRTH,
    SURVIVAL,
} RulesParseState;


void config_parse_rules(const char *rules_str, Rules* rules) {
    RulesParseState state = NONE;

    for (size_t i = 0; rules_str[i] != 0; ++i) {
        char c = toupper((unsigned char)rules_str[i]);

        if (c == 'B')
            state = BIRTH;
        else if (c == 'S')
            state = SURVIVAL;
        else if (isdigit(c)) {
            uint8_t n = c - '0';
            if (n > 9)
                continue;

            if (state == BIRTH)
                rules->b[n] = true;
            else if (state == SURVIVAL)
                rules->s[n] = true;
        }
    }
}
