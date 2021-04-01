//#include "AI.h"
//
//float AI::MoveValue(Move move)
//{
//    return 0.0f;
//}
//
//AI::Move AI::EvaluatePossibilities()
//{
//    m_possibleMoves.clear();
//    for (Unit* unit : *m_board->GetTeamArray(m_team))
//    {
//        PieceMoves moves;
//        unit->AvailableMoves(&moves);
//        m_possibleMoves[unit] = moves;
//    }
//    float bestValue = 0.0f;
//    Move bestMove;
//    for (auto[unit, moveset] : m_possibleMoves)
//    {
//        for (Coord move : moveset.m_available)
//        {
//            Move testMove = Move{ unit, move };
//            float test = MoveValue(testMove);
//            if (test > bestValue)
//            {
//                bestValue = test;
//                bestMove = testMove;
//            }
//        }
//    }
//    return bestMove;
//}
