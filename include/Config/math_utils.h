#pragma once
#include <cmath>
#include "raylib.h"
namespace Math3D {
inline Vector3 Add(Vector3 a,Vector3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Vector3 Subtract(Vector3 a,Vector3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Vector3 Scale(Vector3 a,float s){return {a.x*s,a.y*s,a.z*s};}
inline Vector3 Cross(Vector3 a,Vector3 b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline float Length(Vector3 a){return std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);}
inline Vector3 Normalize(Vector3 a){float l=Length(a);return l>0?Scale(a,1.0F/l):Vector3{};}
inline float Distance(Vector3 a,Vector3 b){return Length(Subtract(a,b));}
}
