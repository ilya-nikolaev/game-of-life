#ifndef CONFIG_H
#define CONFIG_H

#include "core.h"

typedef struct
{
    bool b[16];
    bool s[16];
} Rules;

void config_parse_rules(const char *rules_str, Rules* rules);

#endif
