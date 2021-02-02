#pragma once

class Unit;

template<class T>
struct QuadTreeNode
{
public:
    QuadTreeNode() : children({ nullptr, nullptr, nullptr, nullptr, }), count(0), capacity(0), contentsAtLevel(nullptr) {};
    ~QuadTreeNode() {
        for (int child = 0; child < 4; ++child)
        {
            if (children[child]) delete children[child];
        }
        if (contentsAtLevel) delete[] contentsAtLevel;
    }

    int Weight() const
    {
        return count;
    }
    int WeightOf(int child) const
    {
        QuadTreeNode* childPtr = children[child];
        if (childPtr) return childPtr->Weight();
        else return 0;
    }

    QuadTreeNode* GetChild(int i) const
    {
        return children[i];
    }

    T* operator[](int i) const
    {
        return contentsAtLevel[i];
    }
    void Push(const T& elem)
    {
        if ((count + 1) >= capacity) // Need to resize
        {
            capacity++;
            T* temp = new T*[capacity];
            for (int i = 0; i < count; ++i) { temp[i] = contentsAtLevel[i]; }
            temp[count] = &elem;
            delete[] contentsAtLevel;
            contentsAtLevel = temp;
        }
        else // Grow into existing capacity
        {
            contentsAtLevel[count] = &elem;
        }
        count++;
    }
    void Pop()
    {
        if (count) contentsAtLevel[--count] = nullptr;
    }
    QuadTreeNode* AddChild(int child)
    {
        if (!children[child])
        {
            children[child] = new QuadTreeNode();
        }
        return children[child];
    }
    void RemoveChild(int child)
    {
        if (children[child])
        {
            delete children[child];
            children[child] = nullptr;
        }
    }

protected:
    QuadTreeNode* children[4];
    int count, capacity;
    T* contentsAtLevel;
};

class Board
{
public:
    Board() {
        m_units;
    }

    void Draw();

protected:
    QuadTree<Unit> m_units;
};
extern Board g_board;