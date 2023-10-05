
#include "TCDataFactory.h"

const unit TCDataFactory::known_units[NUMUNITS] = {
    {"feet", "ft", LENGTH, 0.3048},
    {"meters", "m", LENGTH, 1.0},
    {"knots", "kt", VELOCITY, 1.0},
    {"knots^2", "kt^2", BOGUS, 1.0},
};

/* Find a unit; returns -1 if not found. */
int TCDataFactory::findunit(const char *unit) {
  for (int a = 0; a < NUMUNITS; a++) {
    if (!strcmp(unit, known_units[a].name) ||
        !strcmp(unit, known_units[a].abbrv))
      return a;
  }
  return -1;
}
