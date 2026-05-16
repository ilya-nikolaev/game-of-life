#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

#include "config.h"

typedef enum {
    NONE,
    BIRTH,
    SURVIVAL,
} RulesParseState;

Rules config_parse_rules(const char *rules_str) {
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
                rules.birth |= (1u << n);
            else if (state == SURVIVAL)
                rules.survival |= (1u << n);
        }
    }

    return rules;
}
