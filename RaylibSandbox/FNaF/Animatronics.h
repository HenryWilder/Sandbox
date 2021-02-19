#pragma once
#include <map>
#include <vector>
#include <string>
#include <raylib.h>

#if _DEBUG
#define PUBLICIZE_IN_DEBUG public:
#else
#define PUBLICIZE_IN_DEBUG
#endif

enum class Cam : int
{
    ERR, // Error has ocurred (not a Cam)

    Cam1A, // Stage
    Cam1B, // Dining
    Cam1C, // Pirate cove

    Cam2A, // West hall
    Cam2B, // West corner

    Cam3, // Supply closet

    Cam4A, // East hall
    Cam4B, // East corner

    Cam5, // Backstage
    Cam6, // Kitchen
    Cam7, // Bathroom

    wDoor,
    eDoor,
};

enum class Place : int
{
    ERR, // Error has ocurred (not a Place)

    PirateCove_0,
    PirateCove_1,
    PirateCove_2,

    ShowStage,
    DiningHall,
    SupplyCloset,
    Restrooms,
    Kitchen,
    Backstage,

    EastHall,
    EastCorner,
    EastDoor,

    WestHall,
    WestCorner,
    WestDoor,
};

enum class Character : char
{
    Bonnie  = 'B',
    Chica   = 'C',
    Freddy  = 'F',
    Foxy    = 'f',
};

Cam GetPlaceCam(Place); // Conversion function

struct Sprite
{
    Sprite() : set() {};

    Sprite(std::vector<Texture2D> _set) : set(_set) {};

    std::vector<Texture2D> set; // An array of textures

    Texture2D& operator[](int i);

    std::vector<Texture2D>::iterator begin();
    std::vector<Texture2D>::iterator end();
    std::vector<Texture2D>::const_iterator begin() const;
    std::vector<Texture2D>::const_iterator end() const;
};

// Base class
class Animatronic
{
private:
    Place* route;                       // Pointer to stack array
    std::map<Place, Sprite> sprites;    // Set of sprites for each room
    int pos;                            // Index of what position in the "route" array the animatronic is current at
    int level;                          // Animatronic's "AI level" (likelihood of taking a movement opprotunity)
    int cooldownTime;
    PUBLICIZE_IN_DEBUG int cooldown;

public:
    // Animatronic base constructor
    template<int size> Animatronic(Place (&_route)[size], int _cooldownTime, std::vector<Sprite> _sprites)
        : route(_route), sprites(), pos(0), level(20), cooldownTime(_cooldownTime), cooldown(_cooldownTime) {
        for (int i = 0; i < _sprites.size(); ++i) {
            sprites.insert(std::pair<Place, Sprite>(_route[i], _sprites[i])); // Insert sprite at route position
        }
    };
    ~Animatronic() {
        for (std::pair<Place, Sprite> spr : sprites) {
            for (Texture& tex : spr.second) {
                UnloadTexture(tex);
            }
        }
    }

    int GetLevel();                     // Returns the value of the animatronic's AI level
    void SetLevel(int newLevel);        // Sets the animatronic's AI level to a new value

    void Move();                        // Move the animatronic forward 1 position in their route
    void Teleport(int to);              // Instantly set the animatronic's position in their route to a specific index
    void Reset();                       // Teleports the animatronic back to the start of their route
    Place GetLoc();                     // Returns the Place the animatronic is currently located based on their route & position

    virtual void TryMove();             // When called by a "scheduler", attempt a movement opprotunity. If passed 

    bool IsOnCam(Cam cam);              // Returns a boolean for whether the animatronic should be rendered on the camera passed as a parameter
    Texture* GetSprite(int f);              // Draws the animatronic's sprite on the camera using the sprite relevant to that Cam/Place
    void DrawSprite(int f);             // Draws the animatronic's sprite on the camera using the sprite relevant to that Cam/Place

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
    static Place chicaRoute[7];         // The (stack) array of the location's *** will pass through on their route

public:
    Chica()                             // Constructor for Chica
        : Animatronic(chicaRoute, 300, {
        Sprite({ LoadTexture("Chica_ShowStage.PNG") }),
        Sprite({ LoadTexture("Chica_ShowStage.PNG") }),
        Sprite({ LoadTexture("Chica_ShowStage.PNG") }),
        Sprite({ LoadTexture("Chica_ShowStage.PNG") }),
        Sprite({ LoadTexture("Chica_ShowStage.PNG") }),
        Sprite({ LoadTexture("Chica_ShowStage.PNG") }),
        Sprite({ LoadTexture("Chica_ShowStage.PNG") }),
        }) {};
};
extern Place chicaRoute[7];             // External route declaration

// Bonnie
class Bonnie : public Animatronic
{
private:
    static Place bonnieRoute[7];        // The (stack) array of the location's *** will pass through on their route

public:
    Bonnie() :                          // Constructor for Bonnie
        Animatronic(bonnieRoute, 200, {
        Sprite({ LoadTexture("Bonnie_ShowStage.PNG") }),
        Sprite({ LoadTexture("Bonnie_ShowStage.PNG") }),
        Sprite({ LoadTexture("Bonnie_ShowStage.PNG") }),
        Sprite({ LoadTexture("Bonnie_ShowStage.PNG") }),
        Sprite({ LoadTexture("Bonnie_ShowStage.PNG") }),
        Sprite({ LoadTexture("Bonnie_ShowStage.PNG") }),
        Sprite({ LoadTexture("Bonnie_ShowStage.PNG") }),
        }) {};    // Set the route
};
extern Place bonnieRoute[7];            // External route declaration

// Freddy
class Freddy : public Animatronic, public Stunable
{
private:
    static Place freddyRoute[7];        // The (stack) array of the location's *** will pass through on their route
    PUBLICIZE_IN_DEBUG int storedCrits;

public:
    Freddy() :                          // Constructor for Freddy
        Animatronic(freddyRoute, 400, {
        Sprite({ LoadTexture("Freddy_ShowStage.PNG") }),
        Sprite({ LoadTexture("Freddy_ShowStage.PNG") }),
        Sprite({ LoadTexture("Freddy_ShowStage.PNG") }),
        Sprite({ LoadTexture("Freddy_ShowStage.PNG") }),
        Sprite({ LoadTexture("Freddy_ShowStage.PNG") }),
        Sprite({ LoadTexture("Freddy_ShowStage.PNG") }),
        Sprite({ LoadTexture("Freddy_ShowStage.PNG") }),
        }), storedCrits(0) {};

    void TryMove() override;
    void Release() override;
};
extern Place freddyRoute[7];            // External route declaration

// Foxy
class Foxy : public Animatronic, public Stunable
{
private:
    static Place foxyRoute[5];          // The (stack) array of the location's *** will pass through on their route

public:
    Foxy() :                            // Constructor for Foxy
        Animatronic(foxyRoute, 700, {
            Sprite({ LoadTexture("Foxy_PirateCove_0.PNG") }),
            Sprite({ LoadTexture("Foxy_PirateCove_1.PNG") }),
            Sprite({ LoadTexture("Foxy_PirateCove_2.PNG") }),
            Sprite({ LoadTexture("Foxy_WestHall.PNG") }),
            Sprite({ LoadTexture("Foxy_WestHall.PNG") }),
        }) {};

    void TryMove() override;
};
extern Place foxyRoute[5];              // External route declaration
