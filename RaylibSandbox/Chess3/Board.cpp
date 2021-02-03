#include <raylib.h>
#include "CustomMath.h"
#include "Global.h"
#include "Board.h"
#include "Unit.h"

#pragma region QuadTree

QTNode::~QTNode()
{
    for (int i = 0; i < 4; ++i)
    {
        if (children[i])
        {
            delete children[i];
            children[i] = nullptr;
        }
    }
}

int QTNode::MaxDepth() const
{
    int max = 0;
    bool hasChild = false;
    for (QTNode* child : children)
    {
        if (child)
        {
            hasChild = true;
            max = cstm::Max(max, child->MaxDepth());
        }
    }
    return max + (hasChild ? 1 : 0);
}

int QTNode::TotalChildren() const
{
    int total = 0;
    for (QTNode* child : children)
    {
        if (child)
        {
            total++;
            total += child->TotalChildren();
        }
    }
    return total;
}

int QTNode::Weight() const
{
    return (int)units.size();
}

QTNode* QTNode::GetChild(Quadrant where) const
{
    return children[(int)where];
}

std::array<QTNode*, 4>::iterator QTNode::GetChildIter(Quadrant where)
{
    std::array<QTNode*, 4>::iterator it = children.begin();
    for (int i = 0; i < (char)where; ++i) { ++it; }
    return it;
}
std::array<QTNode*, 4>::iterator QTNode::GetChildIter(QTNode* child)
{
    return GetChildIter(child->quadrant);
}

Vector2 QTNode::GetMiddle() const
{
    return { (coverage.x + (coverage.width * 0.5f)), (coverage.y + (coverage.height * 0.5f)) };
}

QTNode* QTNode::TraceShallow(Vector2 pos)
{
    char quad = 0b00;
    Vector2 mid = GetMiddle();
    quad = ((char)((pos.x > mid.x) << 1) | (char)(pos.y > mid.y));
    return GetChild(Quadrant(quad));
}

QTNode* QTNode::TraceDeep(Vector2 pos)
{
    DrawDebug(1);
    QTNode* child = TraceShallow(pos);
    if (child) return child->TraceDeep(pos);
    else return this; // this is the closest we're getting
}
void QTNode::Eliminate(Unit* elem)
{
    auto it = std::find(units.begin(), units.end(), elem);
    units.erase(it, it);
    QTNode* next = TraceShallow(elem->GetPos()); // The element won't be in more than 1 immediate child
    if (next) next->Eliminate(elem);
}
QTNode& QTNode::AddChild(Quadrant where)
{
    std::array<QTNode*, 4>::iterator childIter = GetChildIter(where);

    if (!*childIter) // If the child does not already exist
    {
        Rectangle subdiv = { coverage.x, coverage.y, coverage.width * 0.5f, coverage.height * 0.5f };

        if ((char)where & 0b10) subdiv.x += subdiv.width;
        if ((char)where & 0b01) subdiv.y += subdiv.height;

        *childIter = new QTNode(subdiv, where);
    }

    return *(*childIter); // Dereference the pointer at the iterator
}
void QTNode::Subdivide()
{
    if (Weight() > 3) // I have multiple children
    {
        for (int i = 0; i < 4; ++i)
        {
            Quadrant quad = Quadrant(i);
            QTNode* child = GetChild(quad);

            if (!child) child = &AddChild(quad);

            for (Unit* unit : units) { if (child->WithinCoverage(unit->GetPos())) child->units.push_back(unit); } // Populate child node with units

            if (!(child->Weight()) || (child->Weight() == Weight()))
            {
                RemoveChild(quad); // Child has NO elements, needs to be removed
                continue;
            }
            else if (child->Weight() > 1) child->Subdivide(); // Child has multiple elements, it can be subdivided
        }
    }
}
void QTNode::Prune()
{
    auto ShouldPruneChild = [](QTNode* child) {
        return (child->units.empty());
    };
    for (QTNode* child : children)
    {
        if (child)
        {
            if (ShouldPruneChild(child)) RemoveChild(child->quadrant);
            else child->Prune();
        }
    }
}
bool QTNode::WithinCoverage(Vector2 pt)
{
    return (
        ((pt.x >= coverage.x) && (pt.x < (coverage.x + coverage.width))) &&
        ((pt.y >= coverage.y) && (pt.y < (coverage.y + coverage.height)))
        );
}
void QTNode::RemoveChild(Quadrant where)
{
    auto it = GetChildIter(where);
    if (*it) { // Do not delete nullptr
        delete* it;
        *it = nullptr;
    }
}

#ifdef _DEBUG
void QTNode::DrawDebug(int size)
{
    Rectangle scaledRec = coverage;
    scaledRec.x *= spce::s::o::g_oTileWidth;
    scaledRec.y *= spce::s::o::g_oTileWidth;
    scaledRec.width *= spce::s::o::g_oTileWidth;
    scaledRec.height *= spce::s::o::g_oTileWidth;
    Color thisColor = { (unsigned char)(rand() % 255), (unsigned char)(rand() % 255), (unsigned char)(rand() % 255), (unsigned char)(255) };
    DrawRectangleLinesEx(scaledRec, size * spce::s::g_gameScale, thisColor);
    PrintWeight(thisColor);
    for (Unit* unit : units)
    {
        Vector2 unitPos = BoardToScreen(unit->GetPos());
        unitPos.x = (unitPos.x * spce::s::o::g_oTileWidth);
        unitPos.y = (unitPos.y * spce::s::o::g_oTileWidth);
        //DrawCircleV(unitPos, (float)(size * spce::s::g_gameScale), thisColor);
    }
    if (size > 1)
    {
        for (QTNode* child : children)
        {
            if (child) child->DrawDebug(size - 1);
        }
    }
}
void QTNode::PrintWeight(Color color)
{
    DrawText(TextFormat("%i", Weight()), (int)(coverage.x) * spce::s::o::g_oTileWidth, (int)(coverage.y) * spce::s::o::g_oTileWidth, 8, color);
}
#endif
#pragma endregion

#pragma region Board

Board::Board()
{
    m_selected = nullptr;
    m_qTree = new QTNode({ 0,0,8,8 }, QTNode::Quadrant::SmallXSmallY);
    m_qTree->units.reserve(16);

    Unit::UnitColor color = Unit::UnitColor::Black;
    for (int y = 1; y <= 6; y += 5, color = Unit::UnitColor::White)
    {
        for (int x = 0; x < 8; ++x) // Add pawns
        {
            Unit* unit = new Unit({ (float)x + 0.5f, (float)y + 0.5f }, color);
            m_qTree->units.push_back(unit);
        }
    }

    m_qTree->Subdivide();
    m_qTree->Prune();
}
Board::~Board()
{
    std::vector<Unit*> consolidated;
    consolidated.reserve(m_qTree->units.size());
    for (Unit* elem : m_qTree->units)
    {
        if (elem) consolidated.push_back(elem);
    }

    delete m_qTree;

    if (m_selected)
    {
        for (Unit* unit : consolidated)
        {
            if (unit == m_selected)
            {
                m_selected = nullptr;
                break;
            }
        }
        if (m_selected)
        {
            delete m_selected;
            m_selected = nullptr;
        }
    }

    for (int i = 0; i < consolidated.size(); ++i)
    {
        if (consolidated[i])
        {
            delete consolidated[i];
            consolidated[i] = nullptr;
        }
    }
}

void Board::Draw()
{
    // Background
    BeginShaderMode(s_boardShader); {

        DrawTextureEx(sprite::missing, { 0,0 }, 0.0f, (float)spce::s::o::g_oBoardWidth * 0.5f/*missing.png is 2 pixels wide*/, WHITE);

    } EndShaderMode();

    // Units
    BeginTextureMode(s_unitsBuffer); {

        // Clean the buffer
        ClearBackground({ 0,0,0,0 });

        // Black units
        BeginShaderMode(s_blackUnitShdr); {

            for (Unit* unit : m_qTree->units) { if (unit->IsBlack()) unit->Draw(); }

        } EndShaderMode();

        // White units
        BeginShaderMode(s_whiteUnitShdr); {

            for (Unit* unit : m_qTree->units) { if (unit->IsWhite()) unit->Draw(); }

        } EndShaderMode();

        // Selected unit
        BeginShaderMode(s_selectedUnitShdr); {

            m_selected->Draw();

        } EndShaderMode();

    } EndTextureMode();

    DrawTextureEx(s_unitsBuffer.texture, { 0,0 }, 0.0f, spce::s::g_gameScale, WHITE);

#ifdef _DEBUG
    srand(0);
    //m_qTree->DrawDebug(4);
#endif
}

Shader Board::s_boardShader;
Shader Board::s_blackUnitShdr;
Shader Board::s_whiteUnitShdr;
Shader Board::s_selectedUnitShdr;
RenderTexture2D Board::s_unitsBuffer;

Board g_board;
#pragma endregion