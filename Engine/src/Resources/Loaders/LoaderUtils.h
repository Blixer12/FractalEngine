#pragma once

#include "Defines.h"

#include "Core/Memory.h"

#include "Resources/ResourceDef.h"

struct ResourceLoader;

Bool8 ResourceUnload( struct ResourceLoader* Self, Resource* Resource, MemoryTag Tag);
