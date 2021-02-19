#include <raylib.h>
#include "random.h"
#include "Animatronics.h"

Cam GetPlaceCam(Place p)
{
    switch (p)
    {
    default: return Cam::ERR; break;

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

    case Place::WestDoor:     return Cam::wDoor; break;
    case Place::EastDoor:     return Cam::eDoor; break;
    }
}

int Animatronic::GetLevel() { return level; }

void Animatronic::SetLevel(int newLevel) { level = newLevel; }

void Animatronic::Move() { ++pos; }

void Animatronic::Teleport(int to) { pos = to; }

void Animatronic::Reset() { Teleport(0); }

Place Animatronic::GetLoc() { return route[pos]; }

void Animatronic::TryMove() {
    if ((Random() % 20) <= level) Move();
}

bool Animatronic::IsOnCam(Cam cam) { return (GetPlaceCam(GetLoc()) == cam); }

Texture* Animatronic::GetSprite(int f)
{
    if (!sprites.empty())
    {
        auto it = sprites.find(GetLoc());
        if (it != sprites.end()) return &(it->second[f]);
    }
    return nullptr;
}

void Animatronic::DrawSprite(int f)
{
    Texture* tex = GetSprite(f);
    if (tex) DrawTexture(*tex, 0, 0, WHITE);
}

void Animatronic::Tick()
{
    if (!(--cooldown)) {
        TryMove();
        cooldown = cooldownTime;
    }
}

Place Chica::chicaRoute[7] = {
       Place::ShowStage,
       Place::DiningHall,
       Place::Restrooms,
       Place::Kitchen,
       Place::EastHall,
       Place::EastCorner,
       Place::EastDoor,
};

Place Bonnie::bonnieRoute[7] = {
        Place::ShowStage,
        Place::Backstage,
        Place::DiningHall,
        Place::WestHall,
        Place::WestCorner,
        Place::SupplyCloset,
        Place::WestDoor,
};

Place Freddy::freddyRoute[7] = {
        Place::ShowStage,
        Place::DiningHall,
        Place::Restrooms,
        Place::Kitchen,
        Place::EastHall,
        Place::EastCorner,
        Place::EastDoor,
};

Place Foxy::foxyRoute[5] = {
        Place::PirateCove_0,
        Place::PirateCove_1,
        Place::PirateCove_2,
        Place::WestHall,
        Place::WestDoor,
};

void Stunable::Stun() { b_stunned = true; }

void Stunable::Release() { b_stunned = false; }

bool Stunable::IsStunned() { return b_stunned; }

Texture2D& Sprite::operator[](int i)
{
    return set[i % set.size()];
}

std::vector<Texture2D>::iterator Sprite::begin()
{
    return set.begin();
}

std::vector<Texture2D>::iterator Sprite::end()
{
    return set.end();
}

std::vector<Texture2D>::const_iterator Sprite::begin() const
{
    return set.begin();
}

std::vector<Texture2D>::const_iterator Sprite::end() const
{
    return set.end();
}

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
