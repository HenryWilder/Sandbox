#include "CustomMath.h"
#include "CustomTypes.h"
//#include "Global.h"

//bool Square::IsDiagonal(const Square b) const { // Is b diagonal of this?
//    Square compare = Square(*this - b);
//    return (cstm::Abs(compare.x) == cstm::Abs(compare.y));
//}
//
//bool Square::IsPosXDiagonal(const Square b) const { // Is b (y = x) diagonal of this?
//    return (IsDiagonal(b) &&
//        ((x > b.x) && (y > b.y)) ||
//        ((x < b.x) && (y < b.y)));
//}
//
//bool Square::IsNegXDiagonal(const Square b) const { // Is b (y = -x) diagonal of this?
//    return (IsDiagonal(b) &&
//        ((x > b.x) && (y < b.y)) ||
//        ((x < b.x) && (y > b.y)));
//}
//
//bool Square::IsValid() const
//{
//    return (
//        ((x >= 0) && (x < spce::game::g_sideTileCount)) &&
//        ((y >= 0) && (y < spce::game::g_sideTileCount)));
//}
