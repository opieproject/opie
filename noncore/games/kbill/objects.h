#ifndef OBJECTS_H
#define OBJECTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "Horde.h"
#include "Network.h"
#include "Library.h"
#include "Bucket.h"
#include "Spark.h"
#include "Game.h"
#include "UI.h"

extern Horde bill;
extern Network net;
extern Library OS;
extern Bucket bucket;
extern Spark spark;
extern Game game;
extern UI ui;

#endif
