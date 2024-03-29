#pragma once
#ifdef _DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

#include <vector>
#include <array>
#pragma message("Linking Board.h")


struct Shader;
struct RenderTexture;
typedef RenderTexture RenderTexture2D;
class Unit;

struct QTNode
{
    #pragma region Internal Structures
    enum class Quadrant : char
    {
        SmallXSmallY = 0b00,
        SmallXLargeY = 0b01,
        LargeXSmallY = 0b10,
        LargeXLargeY = 0b11,
    };
    #pragma endregion

    #pragma region Construction/Destruction
    QTNode() : coverage(), quadrant(), children{ nullptr, nullptr, nullptr, nullptr }, units() {};
    QTNode(Rectangle _coverage, Quadrant _quadrant) : coverage(_coverage), quadrant(_quadrant), children{ nullptr, nullptr, nullptr, nullptr }, units() {};
    ~QTNode();
    #pragma endregion

    #pragma region Get/Set Functions
    QTNode* GetChild(Quadrant where) const;
    std::array<QTNode*, 4>::iterator GetChildIter(Quadrant where);
    std::array<QTNode*, 4>::iterator GetChildIter(QTNode* child);
    Vector2 GetMiddle() const;

    int Weight() const;
    int MaxDepth() const;
    int TotalChildren() const;

    bool WithinCoverage(Vector2 pt);
    #pragma endregion

    #pragma region Quad-Tree Specifics
    QTNode* TraceShallow(Vector2 pos); // Trace only this node
    QTNode* TraceDeep(Vector2 pos); // Trace until the end of a branch is reached

    void Eliminate(Unit* elem); // Erase the element from the ENTIRE tree.

    QTNode& AddChild(Quadrant where);
    void RemoveChild(Quadrant where);

    void Subdivide();
    void Prune();
    #pragma endregion

    #pragma region Debug
#ifdef _DEBUG
    void PrintWeight(Color color);
    void DrawDebug(int size = 1);
#endif
    #pragma endregion

    #pragma region Member Variables
    Rectangle coverage;
    Quadrant quadrant;
    std::array<QTNode*, 4> children;
    std::vector<Unit*> units;
    #pragma endregion
};

class Board
{
public:
    Board();
    ~Board();

    void Draw();

    static Shader s_boardShader;
    static Shader s_blackUnitShdr;
    static Shader s_whiteUnitShdr;
    static Shader s_selectedUnitShdr;
    static RenderTexture2D s_unitsBuffer;

    Unit* m_selected;
    QTNode* m_qTree;
};

extern Shader s_boardShader;
extern Shader s_blackUnitShdr;
extern Shader s_whiteUnitShdr;
extern Shader s_selectedUnitShdr;
extern RenderTexture2D s_unitsBuffer;

extern Board g_board;