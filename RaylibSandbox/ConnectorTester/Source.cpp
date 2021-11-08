// This program is made to experiment with creating connector nodes for practicing before making a full thing in Electron Architect.

#include <raylib.h>
#include <raymath.h>
#include <vector>

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

using Vector1 = float;

#pragma region Vector math

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

bool operator==(Vector2 a, Vector2 b) { return ((a.x == b.x) && (a.y == b.y));   }
bool operator!=(Vector2 a, Vector2 b) { return ((a.x != b.x) || (a.y != b.y));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

bool operator==(Vector3 a, Vector3 b) { return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z)); }
bool operator!=(Vector3 a, Vector3 b) { return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z)); }

#pragma endregion


class Connectable;

// The visual/interactable icon showing the point where the connection is made
struct Node
{
    Node* wired; // What node/port this node is wired to
    Connectable* attached; // The Connectable from which this node is borne of
    Vector2 attachedOffset; // The center position of this node relative to the pivot
    float radius; // The size of the node to display/calulate mouse collision against
    uint8_t interchange_id; // Group ID for identifying what other nodes in the array could be exchanged for this one without issue (same number = interchangeable; 0 = ungrouped (non-interchangeable))
    enum class TransferMode {
        Input,
        Output,
    } type;

    bool IsInput() const
    {
        return type == TransferMode::Input;
    }
    bool IsOutput() const
    {
        return type == TransferMode::Output;
    }
};


// Base class for all objects that can connect 
class Connectable
{
private:
    Vector2 pivot; // The point which snaps to the mouse when being moved, and which all relative vectors are offset from
    std::vector<Rectangle> collision; // All rectangles making up the collision box (triangles are harder to calculate than rectangles and use more memory). Use coordinates relative to the pivot
    std::vector<Node> nodes;

public:
    // Returns the node the position collides with, if any
    Node* GetNodeAtPosition(Vector2 pt)
    {
        pt -= pivot;
        for (Node& node : nodes)
        {
            if (CheckCollisionPointCircle(pt, node.attachedOffset, node.radius))
                return &node;
        }
        return nullptr;
    }

    // Fills the output vector will all the nodes that the target can be replaced with
    void GetInterchangeableNodes(std::vector<Node*>& output, const Node* target)
    {
        output.clear();

        if (target->interchange_id == 0)
            return;

        size_t count = 0;
        for (Node& node : nodes)
        {
            if (node.interchange_id == target->interchange_id && node.type == target->type)
                ++count;
        }

        if (!count)
            return;

        output.reserve(count);
        for (Node& node : nodes)
        {
            if (node.interchange_id == target->interchange_id && node.type == target->type)
                output.push_back(&node);
        }
    }

    // Fills the output vector will all the nodes that the target can be replaced with, and exclusively nodes which are available for connection
    void GetAvailableInterchangeableNodes(std::vector<Node*>& output, const Node* target)
    {
        output.clear();

        if (target->interchange_id == 0)
            return;

        size_t count = 0;
        for (Node& node : nodes)
        {
            if (!node.wired && node.interchange_id == target->interchange_id && node.type == target->type)
                ++count;
        }

        if (!count)
            return;

        output.reserve(count);
        for (Node& node : nodes)
        {
            if (!node.wired && node.interchange_id == target->interchange_id && node.type == target->type)
                output.push_back(&node);
        }
    }

    // Returns the first node that the target can be replaced with, exclusively a node which is available for connection
    Node* GetFirstAvailableInterchangeableNode(const Node* target)
    {
        if (target->interchange_id == 0)
            return nullptr;

        for (Node& node : nodes)
        {
            if (!node.wired && node.interchange_id == target->interchange_id && node.type == target->type)
                return &node;
        }
    }

    // Draws a node as a basic circle
    void DrawNodeFlat(Node* node, Color color) const
    {
        DrawCircleV(pivot + node->attachedOffset, node->radius, color);
    }

    // Draws a node with an outline
    void DrawNodeOutlined(Node* node, Color color, Color outline, float outlineThickness) const
    {
        float halfThickness = outlineThickness * 0.5f;
        DrawCircleV(pivot + node->attachedOffset, node->radius + halfThickness, outline);
        DrawCircleV(pivot + node->attachedOffset, node->radius - halfThickness, color);
    }
    // Draws a node with an outline which insets from the edge
    void DrawNodeOutlined_Inside(Node* node, Color color, Color outline, float outlineThickness) const
    {
        DrawCircleV(pivot + node->attachedOffset, node->radius, outline);
        DrawCircleV(pivot + node->attachedOffset, node->radius - outlineThickness, color);
    }
    // Draws a node with an outline which offsets from the edge
    void DrawNodeOutlined_Outside(Node* node, Color color, Color outline, float outlineThickness) const
    {
        DrawCircleV(pivot + node->attachedOffset, node->radius + outlineThickness, outline);
        DrawCircleV(pivot + node->attachedOffset, node->radius, color);
    }
};

enum class RequestMessage
{
    // Connection granted
    Success = 0b00000000,
    Success_Noflags = Success, // No additional information is necessary, request was granted with no changes.
    Success_Swapped = Success | 1, // The specific node connected was changed to another node which is equally valid, but connection was still successful
    Success_Redundant = Success | 2, // The nodes which were requested to be connected were already connected, so no changes were made/no changes are needed
    Success_Repaired_Widowed = Success | 3, // There was a Retry_Widowed on one or both of the nodes which was successfully resolved and a connection made
    Success_Repaired_Unrequited = Success | 4, // There was a Retry_Malformed_Wire on one or both of the nodes which was successfully resolved and a connection made

    // Connection possible, but requires modifications which must be granted to be performed safely.
    Retry = 0b01000000,
    // Literally repeat the request.
    // A change in data may have occurred which must be accounted for, and a repeat can be attempted.
    Retry_TryAgain = Retry,
    // One or both nodes were connected to other nodes which no longer recognized them as connections.
    // Please choose whether to sever or repair the connection and try again.
    Retry_Widowed = Retry | 1,
    // One of the nodes was connected to the other, but the other was not connected to the former.
    // Please choose whether to repair the connection between the two, or to disregard the connection and clean it.
    Retry_Unrequited = Retry | 2,
    // One or both nodes is already connecd to another node and cannot connect
    // Please choose a different wire or wires to connect instead
    Retry_Taken = Retry | 3,

    // Connection not possible according to connection rules
    Failed = 0b10000000,
    Failed_Unknown = Failed, // Failed for undocumented reasons
    Failed_Misc = Failed | 1, // Failed for reasons not deemed important enough for a specific code
    Failed_Null = Failed | 2, // Failed because one or both nodes were nullptr and could not be accessed
    Failed_Homo = Failed | 3, // Failed because either both nodes were inputs or both were outputs and the connector would not fit
    Failed_Adulterer = Failed | 4, // Failed because one or both nodes were already committed, and the specific method did not support replacement
    Failed_NonFungible = Failed | 5, // Failed because one or both nodes were already connected, and no replacement could be found
    Failed_Orphan = Failed | 6, // One or both nodes were detatched from any connectible object and were either corrupted, uninitialized, or otherwise divergent from proper behavior.
};

enum ResolutionFlags
{
    Resolve_Widowed = 0b0001,
    Resolve_Unrequited = 0b0010,
    Resolve_Taken = 0b0100,
    // Doesn't actually allow for the connection to be made, but still fixes the problem of there being an orphan (by *deleting* it)
    // DO NOT USE THIS FLAG IF YOU HAVE COPIES OF EITHER NODE'S POINTER, AS THE MEMORY WILL BE FREED
    Resolve_Orphan = 0b1000,
};

bool IsSuccess(RequestMessage msg)
{
    return (unsigned int)msg & (unsigned int)RequestMessage::Success;
}
bool IsRetry(RequestMessage msg)
{
    return (unsigned int)msg & (unsigned int)RequestMessage::Retry;
}
bool IsFailed(RequestMessage msg)
{
    return (unsigned int)msg & (unsigned int)RequestMessage::Failed;
}
// 0 = success
// 1 = retry
// 2 = failed
int MessageAction(RequestMessage msg)
{
    return IsRetry(msg) | (IsFailed(msg) << 1);
}

// Ask nicely to connect two nodes safely; if anything goes wrong, a message will be returned giving details on what went wrong
RequestMessage RequestConnection(Node* a, Node* b, unsigned int resolutionFlags)
{
    // One or both nodes is null
    if (!a || !b)
        return RequestMessage::Failed_Null;

    // Both nodes are of same "sex"
    if ((a->type == Node::TransferMode::Input && b->type == Node::TransferMode::Input) ||
        (a->type == Node::TransferMode::Output && b->type == Node::TransferMode::Output))
        return RequestMessage::Failed_Homo;

    bool neededToChange = false;

    // Node-a is already connected to something
    if (a->wired)
    {
        // Connected to the other node parameter (Half the work is already done!)
        if (a->wired == b)
        {
            // Both nodes already recognize each other as connected (Job done!!)
            if (b->wired == a)
                return RequestMessage::Success_Redundant;
            // Unrequited love...
            else
            {
                // We have permission to resolve the problem automatically
                if (resolutionFlags & Resolve_Unrequited)
                {
                    // Node-b either is not connected to ANYTHING (so she's available?)
                    // OR the node node-b is connected to does not recognize the connection (so she's available...)
                    // OR Node-b's significant other is connected to a node OTHER THAN node-b (SCANDALOUS!! ...So she's available...)
                    if (!b->wired || !b->wired->wired || b->wired->wired != b)
                    {
                        b->wired = a;
                        return RequestMessage::Success_Repaired_Unrequited;
                    }
                    // Wire-b is in an existing, loving connection and is unavailable. Sorry, a.
                    else
                    {
                        a->wired = nullptr;
                        return RequestMessage::Failed_Adulterer;
                    }
                }
                // We do not have permission to resolve the problem automatically, and nodes a and b will have to be resolved externally.
                else
                    return RequestMessage::Retry_Unrequited;
            }
        }

        // If a is connected to b, the function will have exited by this point.
        // Beyond this point the assumption is made that the initial node-a passed in is already connected, and must either be fungibly replaced, or fail the connection.

        neededToChange = true;

        // Node-a has his mom's phone number so we can call her up and ask if any of a's brothers are interested
        if (a->attached)
        {
            // We have permission to call node-a's mom
            if (resolutionFlags & Resolve_Taken)
            {
                // Try looking for a suitable replacement for node-a
                a = a->attached->GetFirstAvailableInterchangeableNode(a);

                // Node-a doesn't have any suitable replacements! They're all nonexistent or unavailable!
                if (!a)
                    return RequestMessage::Failed_NonFungible;
            }
            // We do not have permission to resolve the problem automatically, and it should be handeled externally.
            else
                return RequestMessage::Failed_NonFungible;
        }
        // Node-a doesn't have a mother! OH NO!!
        else
        {
            // We have permission to resolve the orphan, but we still can't make the connection.
            // I sincerely hope you didn't keep any copy of node-a's pointer when you gave me this permission.
            if (resolutionFlags & Resolve_Orphan)
                delete a;

            return RequestMessage::Failed_Orphan;
        }
    }

    // Node-b is already connected to something
    if (b->wired)
    {
        // Connected to the other node parameter (Half the work is already done!)
        if (b->wired == a)
        {
            // Both nodes already recognize each other as connected (Job done!!)
            if (a->wired == b)
                return RequestMessage::Success_Redundant;
            // Unrequited love...
            else
            {
                // We have permission to resolve the problem automatically
                if (resolutionFlags & Resolve_Unrequited)
                {
                    // Node-a either is not connected to ANYTHING (so he's available?)
                    // OR the node node-a is connected to does not recognize the connection (so he's available...)
                    // OR Node-a's significant other is connected to a node OTHER THAN node-a (SCANDALOUS!! ...So he's available...)
                    if (!a->wired || !a->wired->wired || a->wired->wired != a)
                    {
                        a->wired = b;
                        return RequestMessage::Success_Repaired_Unrequited;
                    }
                    // Wire-a is in an existing, loving connection and is unavailable. Sorry, b.
                    else
                    {
                        b->wired = nullptr;
                        return RequestMessage::Failed_Adulterer;
                    }
                }
                // We do not have permission to resolve the problem automatically, and nodes b and a will have to be resolved externally.
                else
                    return RequestMessage::Retry_Unrequited;
            }
        }

        // If b is connected to a, the function will have exited by this point.
        // Beyond this point the assumption is made that the initial node-a passed in is already connected, and must either be fungibly replaced, or fail the connection.

        neededToChange = true;

        // Node-b has her mom's phone number so we can call her up and ask if any of b's sisters are interested
        if (b->attached)
        {
            // We have permission to call node-b's mom
            if (resolutionFlags & Resolve_Taken)
            {
                // Try looking for a suitable replacement for node-b
                b = b->attached->GetFirstAvailableInterchangeableNode(b);

                // Node-b doesn't have any suitable replacements! They're all nonexistent or unavailable!
                if (!b)
                    return RequestMessage::Failed_NonFungible;
            }
            // We do not have permission to resolve the problem automatically, and it should be handeled externally.
            else
                return RequestMessage::Failed_NonFungible;
        }
        // Node-b doesn't have a mother! OH NO!!
        else
        {
            // We have permission to resolve the orphan, but we still can't make the connection.
            // I sincerely hope you didn't keep any copy of node-b's pointer when you gave me this permission.
            if (resolutionFlags & Resolve_Orphan)
                delete b;

            return RequestMessage::Failed_Orphan;
        }
    }

    // We get to establish a connection!! :D
    a->wired = b;
    b->wired = a;

    if (neededToChange)
        return RequestMessage::Success_Swapped;
    else
        return RequestMessage::Success_Noflags;
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    // @TODO: Load persistent assets & variables

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // @TODO: simulate frame

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            // TODO: Draw frame

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}