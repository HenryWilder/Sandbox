#include <raylib.h>
#include "CustomMath.h"
#include "Global.h"
#include "Board.h"
#include "Unit.h"

QuadTreeNode::~QuadTreeNode() {
    for (int child = 0; child < 4; ++child)
    {
        if (children[child]) delete children[child];
    }
    if (contentsAtLevel) delete[] contentsAtLevel;
}

int QuadTreeNode::MaxDepth() const
{
    int max = 0;
    bool hasChild = false;
    for (int i = 0; i < 4; ++i)
    {
        if (children[i])
        {
            hasChild = true;
            max = cstm::Max(max, children[i]->MaxDepth());
        }
    }
    return max + (hasChild ? 1 : 0);
}

int QuadTreeNode::TotalChildren() const
{
    int total = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (children[i])
        {
            total++;
            total += children[i]->TotalChildren();
        }
    }
    return total;
}

int QuadTreeNode::Weight() const
{
    return count;
}
int QuadTreeNode::WeightOf(int child) const
{
    QuadTreeNode* childPtr = children[child];
    if (childPtr) return childPtr->Weight();
    else return 0;
}

QuadTreeNode* QuadTreeNode::GetChild(int i) const
{
    return children[i];
}

QuadTreeNode* QuadTreeNode::GetQuadrant(Vector2 pos)
{
    if (pos.x < coverage.x + (coverage.width * 0.5f)) { // Left
        if (pos.y < coverage.y + (coverage.height * 0.5f)) // Up
            return children[0];
        else // Right
            return children[2];
    }
    else { // Right
        if (pos.y < coverage.y + (coverage.height * 0.5f)) // Down
            return children[1];
        else // Right
            return children[3];
    }
}

QuadTreeNode* QuadTreeNode::TraceChild(Vector2 pos)
{
    DrawDebug(1);
    QuadTreeNode* child = GetQuadrant(pos);
    if (child) return child->TraceChild(pos);
    else return this; // this is the closest we're getting
}

Unit* QuadTreeNode::GetElement(int i) const
{
    return contentsAtLevel[i];
}
void QuadTreeNode::Pop()
{
    if (count) contentsAtLevel[--count] = nullptr;
}
void QuadTreeNode::Erase(Unit* elem)
{
    int i = 0;
    for (; i < count - 1; ++i) { if (contentsAtLevel[i] == elem) break; }
    for (; i < count - 1; ++i) { if (contentsAtLevel[i] == elem) contentsAtLevel[i] = contentsAtLevel[i + 1]; }
    Pop();
}
void QuadTreeNode::Eliminate(Unit* elem)
{
    QuadTreeNode* next = GetQuadrant(elem->GetPos());
    if (next)
    {
        next->Erase(elem);
        next->Eliminate(elem);
    }
}
void QuadTreeNode::Push(Unit* const elem)
{
    if ((count + 1) >= capacity) Reserve(count + 1);

    contentsAtLevel[count++] = elem;
}
QuadTreeNode& QuadTreeNode::AddChild(int child)
{
    if (children[child] == nullptr)
    {
        Vector2 subdivDim = { coverage.width * 0.5f, coverage.height * 0.5f };
        Vector2 offset = { coverage.x, coverage.y };
        switch (child)
        {
        default:
        case 0:
            break;
        case 1:
            offset.x += subdivDim.x;
            break;
        case 2:
            offset.y += subdivDim.y;
            break;
        case 3:
            offset.x += subdivDim.x;
            offset.y += subdivDim.y;
            break;
        }
        children[child] = new QuadTreeNode({ offset.x, offset.y, subdivDim.x, subdivDim.y });
    }
    return *children[child];
}
void QuadTreeNode::Subdivide()
{
    if (count > 1)
    {
        for (int i = 0; i < 4; ++i)
        {
            QuadTreeNode& child = AddChild(i);
            for (Unit* unit : *this)
            {
                if (child.WithinCoverage(unit->GetPos()))
                {
                    child.Push(unit);
                }
            }
            if (child.Weight() > 1)
            {
                child.Subdivide();
            }
            else if (child.Weight() < 0)
            {
                RemoveChild(i);
            }
        }
    }
}
void QuadTreeNode::Prune()
{
    for (int i = 0; i < 4; ++i)
    {
        if (children[i])
        {
            if ((children[i]->Weight() == 0) || (children[i]->Weight() == Weight()))
            {
                delete children[i];
                children[i] = nullptr;
            }
            else
            {
                children[i]->Prune();
            }
        }
    }
}
bool QuadTreeNode::WithinCoverage(Vector2 pt)
{
    return (
        ((pt.x >= coverage.x) && (pt.x < (coverage.x + coverage.width ))) &&
        ((pt.y >= coverage.y) && (pt.y < (coverage.y + coverage.height)))
        );
}
void QuadTreeNode::RemoveChild(int child)
{
    if (children[child])
    {
        delete children[child];
        children[child] = nullptr;
    }
}

void QuadTreeNode::Reserve(int _count)
{
    if (_count == capacity) return;

    capacity = _count;
    Unit** temp = new Unit * [capacity];
    for (int i = 0; i < cstm::Min(count, capacity); ++i) { temp[i] = contentsAtLevel[i]; }
    delete[] contentsAtLevel;
    contentsAtLevel = temp;
}

Unit** QuadTreeNode::begin()
{
    return contentsAtLevel;
}
Unit** QuadTreeNode::end()
{
    return contentsAtLevel + count;
}

#ifdef _DEBUG
void QuadTreeNode::DrawDebug(int size)
{
    Rectangle scaledRec = coverage;
    scaledRec.x *= spce::scrn::outp::g_otileWidth;
    scaledRec.y *= spce::scrn::outp::g_otileWidth;
    scaledRec.width *= spce::scrn::outp::g_otileWidth;
    scaledRec.height *= spce::scrn::outp::g_otileWidth;
    Color thisColor = { rand(), rand(), rand(), 255 };
    DrawRectangleLinesEx(scaledRec, size * spce::scrn::g_gameScale, thisColor);
    if (begin())
    {
        for (Unit* unit : *this)
        {
            Vector2 unitPos = unit->GetPos();
            unitPos.x = (unitPos.x * spce::scrn::outp::g_otileWidth);
            unitPos.y = (unitPos.y * spce::scrn::outp::g_otileWidth);
            DrawCircleV(unitPos, size * spce::scrn::g_gameScale, thisColor);
        }
    }
    if (size > 1)
    {
        for (int i = 0; i < 4; ++i)
        {
            QuadTreeNode* child = GetChild(i);
            if (child) child->DrawDebug(size - 1);
        }
    }
}
void QuadTreeNode::PrintDebug(int depth)
{
    printf("{ %f..%f, %f..%f }", coverage.x, coverage.x + coverage.width, coverage.y, coverage.y + coverage.height);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < depth; ++j) { printf(" "); }
        printf("{\n");
        if (children[i])
        {
            children[i]->PrintDebug(depth + 1);
        }
        else
        {
            for (Unit* unit : *this)
            {
                for (int j = 0; j <= depth; ++j) { printf(" "); }
                printf("{ %f, %f }\n", unit->GetPos().x, unit->GetPos().y);
            }
        }
        for (int j = 0; j < depth; ++j) { printf(" "); }
        printf("}\n");
    }
}
#endif

Board::Board()
{
    m_qTree = new QuadTreeNode({ 0,0,8,8 });
    m_qTree->Reserve(16);
    for (int x = 0; x < 8; ++x) // Add pawns
    {
        Unit* unit = new Unit({ (float)x + 0.5f, 1.5f }, Unit::UnitColor::Black);
        m_qTree->Push(unit);
    }
    for (int x = 0; x < 8; ++x) // Add pawns
    {
        Unit* unit = new Unit({ (float)x + 0.5f, 6.5f }, Unit::UnitColor::White);
        m_qTree->Push(unit);
    }
    m_qTree->Subdivide();
    m_qTree->Prune();
    //m_qTree->PrintDebug();
}
Board::~Board()
{
    for (Unit*& elem : *m_qTree) {
        delete elem;
    }
    delete m_qTree;
}

void Board::Draw()
{
    BeginShaderMode(*s_boardShader); {

        DrawTextureEx(*sprite::missing, { 0,0 }, 0.0f, (float)spce::scrn::outp::g_oboardWidth * 0.5f/*missing.png is 2 pixels wide*/, WHITE);

    } EndShaderMode();

    BeginTextureMode(*s_unitsBuffer); {

        ClearBackground({0,0,0,0});

        BeginShaderMode(*s_blackUnitShdr); {

            for (Unit* unit : *m_qTree) { if (unit->IsBlack()) unit->Draw(); }

        } EndShaderMode();

        BeginShaderMode(*s_whiteUnitShdr); {

            for (Unit* unit : *m_qTree) { if (unit->IsWhite()) unit->Draw(); }

        } EndShaderMode();

        BeginShaderMode(*s_selectedUnitShdr); {

            s_selected->Draw();

        } EndShaderMode();

    } EndTextureMode();

    DrawTextureEx(s_unitsBuffer->texture, { 0,0 }, 0.0f, spce::scrn::g_gameScale, WHITE);

#ifdef _DEBUG
    srand(0);
    //m_qTree->DrawDebug(4);
#endif
}

Shader* Board::s_boardShader = nullptr;
Shader* Board::s_blackUnitShdr = nullptr;
Shader* Board::s_whiteUnitShdr = nullptr;
Shader* Board::s_selectedUnitShdr = nullptr;
RenderTexture2D* Board::s_unitsBuffer = nullptr;
Unit* Board::s_selected = nullptr;

Board g_board;