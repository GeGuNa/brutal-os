#pragma once

#include <locale/local_t.h>

locale_t duplocale(locale_t);

void freelocale(locale_t);

locale_t newlocale(int, const char *, locale_t);

locale_t uselocale(locale_t);
