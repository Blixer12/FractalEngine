#pragma once

#include "CrystalDef.inl"

struct PlatformState;

Bool8 CrystalBackendCreate(CrystalBackendType Type, struct PlatformState* Platform, CrystalBackend* Backend);
void CrystalBackendDestroy(CrystalBackend* Backend);