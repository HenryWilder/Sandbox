#pragma once

enum ModifierKey
{
    KEY_CONTROL,
    KEY_ALT,
    KEY_SHIFT,
};
bool IsModifierDown(int id);

Rectangle TwoPointRec(Vector2 a, Vector2 b);

enum class ToolType : char
{
    // (V) Selects nodes and components if the start point overlaps none.
    //     Replaces the selection with the clicked object if it is not in the current selection.
    //     Moves the currently selected elements if the element clicked is in the current selection.
    //     Sets all selected elements to match the gate/blueprint picker if the picker changes.
    //     Displays the inputs and outputs of the hovered node, wire, or component
    Select,

    // (P) Draws nodes of the active gate connected by wires where the mouse is pressed.
    //     Connects nodes when a wire is drawn between one another.
    //     Cancels the current wire when right clicking
    Pen,

    // (A) Manually orders the input/output nodes on the hovered component.
    //     Reverses the direction of clicked wires.
    //     Moves dragged nodes.
    Direct,

    // (K) Sets the gate of the clicked node to match the active gate.
    //     Sets the blueprint of clicked components to match the active blueprint.
    Edit,

    // (Z) Moves the camera when dragging.
    //     Zooms when scrolling.
    Camera,
};

class Tool
{
protected: // Persistent members

    // Pallet
    static ComponentBlueprint* s_activeBlueprint;
    static Gate s_activeGate;

protected: // Global members

    // Hovering
    Component* s_hoveredComp;
    Node* s_hoveredNode;
    Wire s_hoveredWire;

public: // Virtuals

    // No event
    virtual void Tick();

    // Left mouse
    virtual void Primary_Press();
    virtual void Primary_Release();
    // Right mouse
    virtual void Secondary_Press();
    virtual void Secondary_Release();
    // Middle mouse 
    virtual void Tertiary_Press();
    virtual void Tertiary_Release();

    // Keyboard
    virtual void TickKeyboardPressCheck(); // Check if any keys being listened for are pressed or released
    virtual void TickKeyboardReleaseCheck(); // Check if any keys being listened for are pressed or released

    // Inputs
    virtual void SimulateTick();
    virtual void DrawTick();
};

class Tool_Select : public Tool
{
private: // Members

    union {
        Vector2 m_selectionStart;
        Vector2 m_draggingStart;
    };
    bool m_selecting = false;
    bool m_dragging = false;
    std::vector<Node*> m_selectedNodes;
    std::vector<Component*> m_selectedComps;

protected: // Helpers

    Rectangle GetSelectionRec() const;
    Vector2 GetDraggingOffset() const;

    void StartSelecting();
    void TickSelecting();
    void FinishSelecting();

    void StartDragging();
    void FinishDragging();

    void DeleteSelected();

public: // Overrides

    // No event
    void Tick() override;

    // Left mouse
    void Primary_Press() override;
    void Primary_Release() override;

    // Keyboard
    void TickKeyboardPressCheck() override;
};

class Tool_Pen : public Tool
{
private: // Members

    Node* m_wireStart = nullptr; // The previous node in the wire

public: // Getters

    Node* GetStartNode() const;

public: // Overrides

    // Left mouse
    void Primary_Press() override;

    // Right mouse
    void Secondary_Press() override;
};

class Tool_Direct : public Tool
{
public: // Overrides

    // Left mouse
    void Primary_Press() override;
    void Primary_Release() override;
};

class Tool_Edit : public Tool
{
public: // Overrides

};

class Tool_Camera : public Tool
{
public: // Overrides

};

class ToolHandler
{
private:
    ToolHandler();
    ToolType m_toolType;
    Tool* m_tool;

public:
    ~ToolHandler();
    static ToolHandler& Global();

    void SetToolType(ToolType type);
    ToolType GetToolType() const;
    const Tool* GetTool() const;

    void SimulateTick();
    void DrawTick();
};