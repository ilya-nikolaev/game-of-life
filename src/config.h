#ifndef CONFIG_H
#define CONFIG_H

#include "core.h"

typedef struct Rules {
    RulesBitmap16 birth;
    RulesBitmap16 survival;
} Rules;

Rules config_parse_rules(const char *rules);

#endif
