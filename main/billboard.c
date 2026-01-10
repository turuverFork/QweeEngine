//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Billboard System for Particles
//==================================================================

#include "raylib.h"
#include "particles.h"
#include <math.h>

void DrawBillboard(Camera3D camera, Texture2D texture, Vector3 position, 
                   float size, Color tint)
{    
    DrawSphere(position, size * 0.5f, tint);
}