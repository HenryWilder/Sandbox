#pragma once
//struct Square
//{
//    Square() : x(), y() {};
//    Square(unsigned int _xy) : x(_xy), y(_xy) {};
//    Square(unsigned int _x, unsigned int _y) : x(_x), y(_y) {};
//    Square(const Square& src) : x(src.x), y(src.y) {};
//
//    int x;
//    int y;
//
//    Square& operator=(const Square b) {
//        x = b.x;
//        y = b.y;
//        return *this;
//    }
//
//    Square operator+(const Square b) const { return { x + b.x, y + b.y }; }
//    Square operator-(const Square b) const { return { x - b.x, y - b.y }; }
//    Square operator*(const Square b) const { return { x * b.x, y * b.y }; }
//    Square operator*(const int scale) const { return { x * scale, y * scale }; }
//    Square operator*(const float scale) const { return { (int)(((float)x * scale) + 0.5f), (unsigned char)(((float)y * scale) + 0.5f) }; }
//    Square operator/(const Square b) const { return { x / b.x, y / b.y }; }
//    Square operator/(const int scale) const { return { x / scale, y / scale }; }
//    Square operator/(const float scale) const { return { (int)(((float)x / scale) + 0.5f), (unsigned char)(((float)y / scale) + 0.5f) }; }
//
//    Square& operator+=(const Square b) {
//        *this = (*this + b);
//        return *this;
//    }
//    Square& operator-=(const Square b) {
//        *this = (*this - b);
//        return *this;
//    }
//    Square& operator*=(const Square b) {
//        *this = (*this * b);
//        return *this;
//    }
//    Square& operator*=(const int scale) {
//        *this = (*this * scale);
//        return *this;
//    }
//    Square& operator*=(const float scale) {
//        *this = (*this * scale);
//        return *this;
//    }
//    Square& operator/=(const Square b) {
//        *this = (*this / b);
//        return *this;
//    }
//    Square& operator/=(const int scale) {
//        *this = (*this / scale);
//        return *this;
//    }
//    Square& operator/=(const float scale) {
//        *this = (*this / scale);
//        return *this;
//    }
//
//    bool operator==(const Square b) const
//    {
//        return ((x == b.x) && (y == b.y));
//    }
//    bool operator!=(const Square b) const
//    {
//        return ((x != b.x) || (y != b.y));
//    }
//    bool IsDiagonal(const Square b) const;
//    bool IsPosXDiagonal(const Square b) const;
//    bool IsNegXDiagonal(const Square b) const;
//
//    bool IsValid() const;
//};