#pragma once
#include <algorithm>

template<typename T>
class Table
{
    friend class Col;
public:
    struct Ref
    {
        Ref(T* ref)
            : ref(ref)
        {
        }

        void operator=(const T value)
        {
            *ref = value;
        }

        operator T()
        {
            return *ref;
        }

    private:
        T* ref;
    };

    struct RefCol
    {
        RefCol(int x, const Table<T>* table)
            : x(x), table(table)
        {}

        Ref operator[](int y)
        {
            return Ref(&table->table[x + y * table->cols]);
        }

        int x;
        const Table<T>* table;
    };

    Table()
    {}

    Table(int rows, int cols)
        : rows(rows), cols(cols)
    {
        table = new T[rows * cols];
    }

    Table(int rows, int cols, T init)
        : Table(rows, cols)
    {
        std::fill_n(table, rows * cols, init);
    }

    ~Table()
    {
        delete[] table;
    }

    RefCol operator[](int x)
    {
        return RefCol(x, this);
    }

private:
    int rows;
    int cols;
    T* table = nullptr;
};