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

QuadTreeNode* QuadTreeNode::TraceChild(Vector2 pos)
{
    DrawDebug(1);
    QuadTreeNode* child;
    if (pos.x < coverage.x + (coverage.width * 0.5f)) { // Left
        if (pos.y < coverage.y + (coverage.height * 0.5f)) // Up
            child = children[0];
        else // Right
            child = children[2];
    }
    else { // Right
        if (pos.y < coverage.y + (coverage.height * 0.5f)) // Down
            child = children[1];
        else // Right
            child = children[3];
    }
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
#ifdef _DEBUG
    printf("This contains %i Unit*s\n", Weight());
#endif
    for (int i = 0; i < 4; ++i)
    {
        QuadTreeNode& child = AddChild(i);
#ifdef _DEBUG
        printf("Child coverage: { %f..%f, %f..%f }\n",
            child.coverage.x,
            ((float)child.coverage.x + (float)child.coverage.width),
            child.coverage.y,
            ((float)child.coverage.y + (float)child.coverage.height));
#endif
        for (Unit* unit : *this)
        {
#ifdef _DEBUG
            printf("Unit pos: { %f, %f }, which is %s\n", unit->GetPos().x, unit->GetPos().y, (child.WithinCoverage(unit->GetPos()) ? "within" : "outside"));
#endif
            if (child.WithinCoverage(unit->GetPos()))
            {
                child.Push(unit);
            }
        }
#ifdef _DEBUG
        printf("Child contains %i Unit*s\n", child.Weight());
#endif
    }
}
void QuadTreeNode::Prune()
{
    for (int i = 0; i < 4; ++i)
    {
        if (children[i])
        {
            if (!children[i]->Weight())
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
            unitPos.x = (unitPos.x * spce::scrn::outp::g_otileWidth) + (spce::scrn::outp::g_otileWidth / 2.0f);
            unitPos.y = (unitPos.y * spce::scrn::outp::g_otileWidth) + (spce::scrn::outp::g_otileWidth / 2.0f);
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
#endif

Board::Board()
{
    m_qTree = new QuadTreeNode({ 0,0,8,8 });
    m_qTree->Reserve(16);
    for (int x = 0; x < 8; ++x) // Add pawns
    {
        Unit* unit = new Unit({ (float)x, 1.0f }, Unit::UnitColor::Black);
        m_qTree->Push(unit);
    }
    for (int x = 0; x < 8; ++x) // Add pawns
    {
        Unit* unit = new Unit({ (float)x, 6.0f }, Unit::UnitColor::White);
        m_qTree->Push(unit);
    }

    m_qTree->Subdivide();
    for (int i = 0; i < 4; ++i)
    {
        m_qTree->GetChild(i)->Subdivide();
        for (int j = 0; j < 4; ++j)
        {
            m_qTree->GetChild(i)->GetChild(j)->Subdivide();
        }
    }
    m_qTree->Prune();
}
Board::~Board()
{
    for (Unit* elem : *m_qTree) { delete elem; }
    delete m_qTree;
}

void Board::Draw()
{
    BeginShaderMode(*s_boardShader); {

        DrawTextureEx(*sprite::missing, { 0,0 }, 0.0f, (float)spce::scrn::outp::g_oboardWidth * 0.5f/*missing.png is 2 pixels wide*/, WHITE);

    } EndShaderMode();

    BeginTextureMode(*s_unitsBuffer); {

        BeginShaderMode(*s_blackUnitShdr); {

            for (Unit* unit : *m_qTree) { if (unit->IsBlack()) unit->Draw(); }

        } EndShaderMode();

        BeginShaderMode(*s_whiteUnitShdr); {

            for (Unit* unit : *m_qTree) { if (unit->IsWhite()) unit->Draw(); }

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
RenderTexture2D* Board::s_unitsBuffer = nullptr;

Board g_board;