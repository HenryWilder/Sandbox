#pragma once
#include <stdio.h>
#include <stdlib.h>
class Unit;

struct QuadTreeNode
{
public:
    QuadTreeNode() : children{ nullptr, nullptr, nullptr, nullptr, }, count(0), capacity(0), coverage(), contentsAtLevel(nullptr) {};
    QuadTreeNode(Rectangle _coverage) : children{ nullptr, nullptr, nullptr, nullptr, }, count(0), capacity(0), coverage(_coverage), contentsAtLevel(nullptr)
    { printf("Added a qTree node covering { %f, %f, %f, %f }\n", coverage.x, coverage.y, coverage.width, coverage.height); };
    QuadTreeNode(Rectangle _coverage, Unit* elem) : children{ nullptr, nullptr, nullptr, nullptr, }, count(1), capacity(1), coverage(_coverage), contentsAtLevel(new Unit* [1]) { contentsAtLevel[0] = elem; };
    ~QuadTreeNode();

    int MaxDepth() const;
    int TotalChildren() const;

    int Weight() const;
    int WeightOf(int child) const;
    bool WithinCoverage(Vector2 pt);

    QuadTreeNode* GetChild(int i) const;
    QuadTreeNode* TraceChild(Vector2 pos);

    Unit* GetElement(int i) const;
    void Reserve(int _count);
    void Push(Unit* const elem);
    void Pop();
    QuadTreeNode& AddChild(int child);
    void Subdivide();
    void Prune();
    void RemoveChild(int child);

    Unit** begin();
    Unit** end();

#ifdef _DEBUG
    void DrawDebug(int size = 1);
#endif

protected:
    QuadTreeNode* children[4];
    int count, capacity;
    Rectangle coverage;
    Unit** contentsAtLevel; // Vector of T pointers
};

struct Shader;
struct RenderTexture;
typedef RenderTexture RenderTexture2D;

class Board
{
public:
    Board();
    ~Board();

    void Draw();

    static Shader* s_boardShader;
    static Shader* s_blackUnitShdr;
    static Shader* s_whiteUnitShdr;
    static RenderTexture2D* s_unitsBuffer;

    QuadTreeNode* m_qTree;
};
extern Shader* s_boardShader;
extern Shader* s_blackUnitShdr;
extern Shader* s_whiteUnitShdr;
extern RenderTexture2D* s_unitsBuffer;

extern Board g_board;