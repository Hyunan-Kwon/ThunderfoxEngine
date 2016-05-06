#pragma once

#include "TFBuffer.h"

#define PER255F 0.003921568627f	//@ /255.0f

namespace TFFramework {
	extern TFArrayBuffer *vertexBuffer_quad, *uvBuffer_quad;

	extern void init();
	extern void cleanup();
}