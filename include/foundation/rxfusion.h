/*

Copyright 2016 Green Mountain Computing Systems, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#ifndef RXFUSION_H
#define RXFUSION_H

#ifdef min
#undef min
#undef max
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <functional>
#include <limits>

#ifndef CONSTRUCTED
#define CONSTRUCTED
#endif

#define SECONDS(x) ((x)*1000)
#define MINUTES(x) ((x)*SECONDS(60))
#define HOURS(x) ((x)*MINUTES(60))

#ifndef DBG_NET
#define DBG_NET 0
#endif

unsigned long millis();

#include "storage.h"
#include "tuple.h"
#include "vec.h"
#include "rxbase.h"
#include "rxcore.h"
#include "generators.h"
#include "originators.h"
#include "operators.h"
#include "consumers.h"
#include "formatters.h"
#include "rxbuilder.h"
#include "gen/rxbuilder.h"

#endif
