#pragma once

//
// CONTAINS DELTA TIME DEFINITION
//

// INCLUDE OBJECT PROVIDING DELTA TIME GETTER
#include "../src/core/time/time.h"

// DEFINE DELTA GETTER AS "delta"
// DELTA CAN ALSO BE VALUE 0, DONT USE SMOOTHING THEN
#define delta Time::deltaf()