#include <iostream>
#include <cstdarg>
#include <vector>

#ifndef __HEAP__
#define __HEAP__

template <typename T>
class MinHeap
{
private:
    std::vector<T> items;

    int left_child(int parent)
    {
        return (parent * 2) + 1;
    };

    int right_child(int parent)
    {
        return this->left_child(parent) + 1;
    };

    int parent(int index)
    {
        return (index - 1) / 2;
    }

    bool has_left_child(int parent)
    {
        return this->left_child(parent) < (int)this->items.size();
    };

    bool has_right_child(int parent)
    {
        return this->right_child(parent) < (int)this->items.size();
    };

    void swap(int a, int b)
    {
        T temp = this->items[a];
        this->items[a] = this->items[b];
        this->items[b] = temp;
    }

    void update()
    {
        T current;
        T parent;
        int current_index = this->items.size() - 1;
        int parent_index;
        bool swapping = true;
        while (current_index > 0 && swapping)
        {
            swapping = false;
            parent_index = this->parent(current_index);
            current = this->items[current_index];
            parent = this->items[parent_index];

            if (*current < *parent)
            {
                swapping = true;
                this->swap(current_index, parent_index);
                current_index = parent_index;
            }
        }
    }

    void update_down()
    {
        T current;
        T left_child;
        T right_child;
        T lesser_child;
        int current_index = 0;
        int child_index;
        bool swapping = true;
        while (this->has_left_child(current_index) && swapping)
        {
            swapping = false;
            current = this->items[current_index];
            child_index = this->left_child(current_index);
            left_child = this->items[child_index];

            if (!this->has_right_child(current_index))
            {
                lesser_child = left_child;
                child_index = this->left_child(current_index);
            }
            else
            {
                child_index = this->right_child(current_index);
                right_child = this->items[child_index];

                if (*left_child < *right_child)
                {
                    lesser_child = left_child;
                    child_index = this->left_child(current_index);
                }
                else
                {
                    lesser_child = right_child;
                    child_index = this->right_child(current_index);
                }
            }

            if (*lesser_child < *current)
            {
                swapping = true;
                this->swap(current_index, child_index);
                current_index = child_index;
            }
        }
    }

public:
    MinHeap(){};

    MinHeap(int num, ...)
    {
        va_list valist;
        va_start(valist, num);
        for (int i = 0; i < num; i++)
        {
            this->push(va_arg(valist, T));
        }
        va_end(valist);
    };

    void push(T item)
    {
        this->items.push_back(item);

        this->update();
    };

    T pop()
    {
        T top = this->top();

        this->swap(0, this->items.size() - 1);
        this->items.pop_back();
        this->update_down();

        return top;
    };

    T top()
    {
        return this->items[0];
    };

    void display()
    {
        std::cout << "{";
        for (auto item : this->items)
        {
            std::cout << item << ", ";
        }
        std::cout << "}\n";
    }

    bool empty()
    {
        return this->items.empty();
    }
};

#endif