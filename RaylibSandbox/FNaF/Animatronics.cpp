#include <raylib.h>
#include "random.h"
#include "Animatronics.h"

Cam GetPlaceCam(Place p)
{
    switch (p)
    {
    default:                    return Cam::ERR; break;

    case Place::PirateCove_0:   return Cam::Cam1C; break;
    case Place::PirateCove_1:   return Cam::Cam1C; break;
    case Place::PirateCove_2:   return Cam::Cam1C; break;

    case Place::ShowStage:      return Cam::Cam1A; break;
    case Place::DiningHall:     return Cam::Cam1B; break;
    case Place::SupplyCloset:   return Cam::Cam3; break;
    case Place::Restrooms:      return Cam::Cam7; break;
    case Place::Kitchen:        return Cam::Cam6; break;
    case Place::Backstage:      return Cam::Cam5; break;

    case Place::EastHall:       return Cam::Cam4A; break;
    case Place::EastCorner:     return Cam::Cam4B; break;

    case Place::WestHall:       return Cam::Cam2B; break;
    case Place::WestCorner:     return Cam::Cam2B; break;

    case Place::WestDoor:       return Cam::wDoor; break;
    case Place::EastDoor:       return Cam::eDoor; break;
    }
}

int Animatronic::GetLevel() { return level; }

void Animatronic::SetLevel(int _level) { level = _level; }

void Animatronic::Move() { ++pos; }

void Animatronic::Teleport(int to) { pos = to; }

void Animatronic::Reset() { Teleport(0); }

Place Animatronic::GetLoc() { return spRoute[pos].loc; }

void Animatronic::TryMove() {
    if ((Random() % 20) <= level) Move();
}

bool Animatronic::IsOnCam(Cam cam) { return (GetPlaceCam(GetLoc()) == cam); }

Texture2D Animatronic::GetTexture(int f)
{
    Sprite& spr = spRoute[pos].spr;
    return spr[f % spr.size()];
}

void Animatronic::Tick()
{
    if (!(--cooldown)) {
        TryMove();
        cooldown = cooldownTime;
    }
}

void Stunable::Stun() { b_stunned = true; }

void Stunable::Release() { b_stunned = false; }

bool Stunable::IsStunned() { return b_stunned; }

void Freddy::Release()
{
    Stunable::Release();
    TryMove();
}

void Freddy::TryMove()
{
    if (!IsStunned()) {
        if (storedCrits > 0) {
            while (storedCrits--) {
                Animatronic::TryMove();
            }
            storedCrits = 0;
        }
        else {
            Animatronic::TryMove();
        }
    }
    else {
        storedCrits++;
    }
}

void Foxy::TryMove()
{
    if (!IsStunned())
        Animatronic::TryMove();
}
