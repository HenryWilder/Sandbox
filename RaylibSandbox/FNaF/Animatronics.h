#pragma once
#include <map>
#include <vector>
#include <string>
#include <raylib.h>

#if _DEBUG
    #define DEBUG_ONLY(code) code
    #define RELEASE_ONLY(code)
    #define DEBUG_ELSE(debug, release) debug
#else
    #define DEBUG_ONLY(code)
    #define RELEASE_ONLY(code) code
    #define DEBUG_ELSE(debug, release) release
#endif

enum class Cam : int
{
    ERR,    // Error has ocurred (not a Cam)

    Cam1A, /* Stage */ Cam1B, /* Dining */ Cam1C, /* Pirate cove */

    Cam2A, /* West hall */ Cam2B,  /* West corner */

    Cam3, /* Supply closet */

    Cam4A, /* East hall */ Cam4B, /* East corner */

    Cam5,  /* Backstage */ Cam6,  /* Kitchen */ Cam7,  /* Bathroom */

    // technically not cameras
    wDoor, /* West doorway */ eDoor, /* East doorway */
};

enum class Place : int
{
    ERR, // Error has ocurred (not a Place)

    ShowStage, /* 1A */ DiningHall, /* 1B */
    PirateCove_0, /* 1C */ PirateCove_1, /* 1C */ PirateCove_2, /* 1C */ // All three of these share a cam

    WestHall, /* 2A */ WestCorner, /* 2B */

    SupplyCloset, /* 3 */

    EastHall, /* 4A */ EastCorner, /* 4B */

    Restrooms, /* 5 */ Kitchen, /* 6 */ Backstage, /* 7 */

    EastDoor, /* eDoor */ WestDoor, /* wDoor */
};

enum class Character : char
{
    Bonnie  = 'B',
    Chica   = 'C',
    Freddy  = 'F',
    Foxy    = 'f',
};

Cam GetPlaceCam(Place); // Conversion function

typedef std::vector<Texture2D> Sprite;

struct PlaceSprite
{
    PlaceSprite() : loc(), spr() {};
    PlaceSprite(Place _loc, Sprite _spr) : loc(_loc), spr(_spr) {};

    Place loc;
    Sprite spr;
};

// Base class
class Animatronic
{
private:
    PlaceSprite* spRoute;               // Pointer to stack array
    int pos;                            // Index of what position in the "route" array the animatronic is current at
    int level;                          // Animatronic's "AI level" (likelihood of taking a movement opprotunity) 0..20
    int cooldownTime;
    DEBUG_ONLY(public:) int cooldown;

public:
    // Animatronic base constructor
    template<int size> Animatronic(PlaceSprite (&_spRoute)[size], int _cooldownTime)
        : spRoute(_spRoute), pos(0), level(0), cooldownTime(_cooldownTime), cooldown(_cooldownTime) {};

    int GetLevel();                     // Returns the value of the animatronic's AI level
    void SetLevel(int _level);          // Sets the animatronic's AI level to a new value

    void Move();                        // Move the animatronic forward 1 position in their route
    void Teleport(int to);              // Instantly set the animatronic's position in their route to a specific index
    void Reset();                       // Teleports the animatronic back to the start of their route
    Place GetLoc();                     // Returns the Place the animatronic is currently located based on their route & position

    virtual void TryMove();             // When called by a "scheduler", attempt a movement opprotunity. If passed 

    bool IsOnCam(Cam cam);              // Returns a boolean for whether the animatronic should be rendered on the camera passed as a parameter
    Texture2D GetTexture(int f);        // Get the texture to draw this frame

    void Tick();
};

// Class interface
class Stunable
{
private:
    bool b_stunned;                     // Whether the animatronic is stunned

public:
    Stunable()                          // Stunable animatronics always start un-stunned
        : b_stunned(false) {};

    bool IsStunned();                   // Tells whether the animatronic is in a stunned/locked state

    void Stun();                        // Would be called by the camera, when the monitor is opened
    virtual void Release();             // Would be called by the camera, when the monitor is closed
};

// Chica
class Chica : public Animatronic
{
private:
    PlaceSprite chicaRoute[7] = {   // The (stack) array of the location's *** will pass through on her route
       PlaceSprite(Place::ShowStage,        Sprite{ LoadTexture("Chica_ShowStage.PNG") }),
       PlaceSprite(Place::DiningHall,       Sprite{ LoadTexture("Chica_ShowStage.PNG") }),
       PlaceSprite(Place::Restrooms,        Sprite{ LoadTexture("Chica_ShowStage.PNG") }),
       PlaceSprite(Place::Kitchen,          Sprite{ LoadTexture("Chica_ShowStage.PNG") }),
       PlaceSprite(Place::EastHall,         Sprite{ LoadTexture("Chica_ShowStage.PNG") }),
       PlaceSprite(Place::EastCorner,       Sprite{ LoadTexture("Chica_ShowStage.PNG") }),
       PlaceSprite(Place::EastDoor,         Sprite{ LoadTexture("Chica_ShowStage.PNG") })
    };

public:
    Chica() : Animatronic(chicaRoute, 300) {};
};

// Bonnie
class Bonnie : public Animatronic
{
private:
    PlaceSprite bonnieRoute[7] = {   // The (stack) array of the location's *** will pass through on their route
        PlaceSprite(Place::ShowStage,       Sprite{ LoadTexture("Bonnie_ShowStage.PNG") }),
        PlaceSprite(Place::Backstage,       Sprite{ LoadTexture("Bonnie_ShowStage.PNG") }),
        PlaceSprite(Place::DiningHall,      Sprite{ LoadTexture("Bonnie_ShowStage.PNG") }),
        PlaceSprite(Place::WestHall,        Sprite{ LoadTexture("Bonnie_ShowStage.PNG") }),
        PlaceSprite(Place::WestCorner,      Sprite{ LoadTexture("Bonnie_ShowStage.PNG") }),
        PlaceSprite(Place::SupplyCloset,    Sprite{ LoadTexture("Bonnie_ShowStage.PNG") }),
        PlaceSprite(Place::WestDoor,        Sprite{ LoadTexture("Bonnie_ShowStage.PNG") })
    };        

public:
    Bonnie() :                          // Constructor for Bonnie
        Animatronic(bonnieRoute, 200) {};    // Set the route
};

// Freddy
class Freddy : public Animatronic, public Stunable
{
private:
    PlaceSprite freddyRoute[7] = {   // The (stack) array of the location's *** will pass through on his route
        PlaceSprite(Place::ShowStage,       Sprite{ LoadTexture("Freddy_ShowStage.PNG") }),
        PlaceSprite(Place::DiningHall,      Sprite{ LoadTexture("Freddy_ShowStage.PNG") }),
        PlaceSprite(Place::Restrooms,       Sprite{ LoadTexture("Freddy_ShowStage.PNG") }),
        PlaceSprite(Place::Kitchen,         Sprite{ LoadTexture("Freddy_ShowStage.PNG") }),
        PlaceSprite(Place::EastHall,        Sprite{ LoadTexture("Freddy_ShowStage.PNG") }),
        PlaceSprite(Place::EastCorner,      Sprite{ LoadTexture("Freddy_ShowStage.PNG") }),
        PlaceSprite(Place::EastDoor,        Sprite{ LoadTexture("Freddy_ShowStage.PNG") })
    };
    DEBUG_ONLY(public:) int storedCrits;

public:
    Freddy() :                          // Constructor for Freddy
        Animatronic(freddyRoute, 400), storedCrits(0) {};

    void TryMove() override;
    void Release() override;
};

// Foxy
class Foxy : public Animatronic, public Stunable
{
private:
    PlaceSprite foxyRoute[5] = {    // The (stack) array of the location's Foxy will pass through on his route
        PlaceSprite(Place::PirateCove_0,    Sprite{ LoadTexture("Foxy_PirateCove_0.PNG") }),
        PlaceSprite(Place::PirateCove_1,    Sprite{ LoadTexture("Foxy_PirateCove_1.PNG") }),
        PlaceSprite(Place::PirateCove_2,    Sprite{ LoadTexture("Foxy_PirateCove_2.PNG") }),
        PlaceSprite(Place::WestHall,        Sprite{ LoadTexture("Foxy_WestHall.PNG") }),
        PlaceSprite(Place::WestDoor,        Sprite{ LoadTexture("Foxy_WestHall.PNG") })
    };

public:
    Foxy() :                            // Constructor for Foxy
        Animatronic(foxyRoute, 700) {};

    void TryMove() override;
};
