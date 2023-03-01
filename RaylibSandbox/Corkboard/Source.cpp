#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <variant>
#include <optional>
#include "Thread.h"
#include "Notecard.h"
#include "CBButton.h"

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

#pragma endregion

#define CORKBOARD CLITERAL(Color){202,164,120,255}

using Element = std::variant<Notecard*, Thread*, CBButton*>;
using OptElement = std::optional<Element>;

std::vector<CBButton> g_buttons = {
    CBButton({0,0})
};

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    CreateCard(Vector2Zero(), cardstockBlue);
    g_cards.back()->title = "Title";
    g_cards.back()->content = "Content";
    CreateCard(Vector2Zero(), cardstockGreen);

    OptElement optDraggedElement = {};
    bool draggingPin = false; // If optDraggedElement is a card, this says whether the card is being dragged or the pin (making a thread).
    Vector2 clickOffset = Vector2Zero();

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Vector2 cursor = GetMousePosition();

        OptElement optHoveredElement = {};
        bool hoveringPin = false; // If optHoveredElement is a card, this says whether the card is being hovered or the pin.

        // Hover checks
        {
            for (CBButton& button : g_buttons)
            {
                if (button.IsHovered(cursor))
                {
                    optHoveredElement = &button;
                    goto FinishedHoverChecks;
                }
            }

            for (Thread* thread : g_threads)
            {
                if (thread->IsHovered(cursor))
                {
                    optHoveredElement = thread;

                    // Pins have priority
                    for (Notecard* card : g_cards)
                    {
                        if (card->IsPinHovered(cursor))
                        {
                            optHoveredElement = card;
                            hoveringPin = true;
                            break;
                        }
                    }

                    goto FinishedHoverChecks;
                }
            }

            for (Notecard* card : g_cards)
            {
                if (card->IsCardHovered(cursor))
                {
                    optHoveredElement = card;
                    hoveringPin = card->IsPinHovered(cursor);
                    goto FinishedHoverChecks;
                }
            }
        }
    FinishedHoverChecks:

        if (optDraggedElement.has_value())
        {
            Element draggedElement = *optDraggedElement;

            // Always lose target on release
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                // Create pin
                if (draggingPin && // Wants to create thread
                    optHoveredElement.has_value() && // Hovering something
                    std::holds_alternative<Notecard*>(*optHoveredElement) && // Hovering a notecard or pin
                    *optHoveredElement != draggedElement) // Not hovering the same notecard/pin that we originated from
                {
                    _ASSERT_EXPR(std::holds_alternative<Notecard*>(draggedElement), L"Can only drag threads from pins.");

                    Notecard* startCard = std::get<Notecard*>(draggedElement);
                    Notecard* endCard = std::get<Notecard*>(*optHoveredElement);

                    // Don't create duplicate connections
                    bool shouldCreateThread = true;
                    {
                        Notecard* cardWithFewerConnections =
                            startCard->threads.size() <= endCard->threads.size()
                            ? startCard
                            : endCard;

                        for (Thread* thread : cardWithFewerConnections->threads)
                        {
                            bool connectionExists =
                                (thread->start == startCard && thread->end == endCard) ||
                                (thread->end == startCard && thread->start == endCard);

                            if (connectionExists)
                            {
                                shouldCreateThread = false;
                                break;
                            }
                        }
                    }

                    if (shouldCreateThread)
                        CreateThread(RED, startCard, endCard);
                }

                // Cleanup
                optDraggedElement = {};
                draggingPin = false;
            }
            else
            {
                // Notecard
                if (std::holds_alternative<Notecard*>(draggedElement))
                {
                    if (!draggingPin)
                    {
                        std::get<Notecard*>(draggedElement)->position = cursor + clickOffset;
                    }
                }
                // Thread
                else // if (std::holds_alternative<Thread*>(draggedElement))
                {
                    // Todo
                }
            }
        }

        if (optHoveredElement.has_value())
        {
            Element hoveredElement = *optHoveredElement;

            // Notecard
            if (std::holds_alternative<Notecard*>(hoveredElement))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    optDraggedElement = hoveredElement;
                    if (hoveringPin)
                    {
                        draggingPin = true;
                    }
                    else
                    {
                        Notecard* card = std::get<Notecard*>(hoveredElement);
                        clickOffset = card->position - cursor;
                        auto it = std::find(g_cards.begin(), g_cards.end(), card);
                        g_cards.erase(it);
                        g_cards.push_front(card);
                    }
                }
            }
            // Thread
            else // if (std::holds_alternative<Thread*>(draggedElement))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    // Todo
                }
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(CORKBOARD);

            // Cards are drawn in reverse order
            for (int i = g_cards.size() - 1; i >= 0; --i)
            {
                g_cards[i]->DrawCard();
            }

            // Hovered card/pin
            if (optHoveredElement.has_value() && std::holds_alternative<Notecard*>(*optHoveredElement))
            {
                Notecard* card = std::get<Notecard*>(*optHoveredElement);

                // Hovering pin
                if (hoveringPin)
                {
                    DrawRing(card->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + 3, 0, 360, 100, YELLOW);
                }
                // Hovering card
                else
                {
                    DrawRectangleLinesEx(card->GetCardRectangle(), 3, YELLOW);
                    DrawText(TextFormat("%i", card->threads.size()), card->position.x, card->position.y - 10, 8, RED);
                }
            }

            // Dragged card/pin
            if (optDraggedElement.has_value() && std::holds_alternative<Notecard*>(*optDraggedElement))
            {
                Notecard* card = std::get<Notecard*>(*optDraggedElement);

                // Hovering pin
                if (draggingPin)
                {
                    DrawRing(card->PinPosition(), Notecard::pinRadius, Notecard::pinRadius + 3, 0, 360, 100, BLUE);
                }
                // Hovering card
                else
                {
                    DrawRectangleLinesEx(card->GetCardRectangle(), 3, BLUE);
                }
            }

            for (Thread* thread : g_threads)
            {
                thread->Draw();
            }

            // Hovering thread
            if (optHoveredElement.has_value() && std::holds_alternative<Thread*>(*optHoveredElement))
            {
                Thread* thread = std::get<Thread*>(*optHoveredElement);

                DrawLineEx(thread->StartPosition(), thread->EndPosition(), thread->thickness, YELLOW);
            }
            
            // Creating thread
            if (draggingPin)
            {
                // draggingPin implies that we have already hovered and clicked a pin on a notecard and have not yet released.
                _ASSERT_EXPR(optDraggedElement.has_value() && std::holds_alternative<Notecard*>(*optDraggedElement), L"Dragging a pin without having a notecard selected");

                Notecard* startCard = std::get<Notecard*>(*optDraggedElement);
                DrawLineEx(startCard->PinPosition(), cursor, 2.0f, LIGHTGRAY);
            }

            for (Notecard* card : g_cards)
            {
                card->DrawPin();
            }

            for (CBButton button : g_buttons)
            {
                button.Draw();
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    while (!g_cards.empty())
    {
        delete g_cards.back();
        g_cards.pop_back();
    }

    while (!g_threads.empty())
    {
        delete g_threads.back();
        g_threads.pop_back();
    }

    CloseWindow();

	return 0;
}