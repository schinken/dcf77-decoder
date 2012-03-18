#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/portpins.h>
#include <stdbool.h>
#include <avr/iom32.h>
#include <stdio.h>

extern struct tRawTime tTime;

#include "RS232.h"
#include "DCF77_v2.h"
