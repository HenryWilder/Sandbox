public class Unit {
    Unit(Type type, Team team, int x, int y) {
        this.type = type;
        this.team = team;
        this.x = x;
        this.y = y;
    }

    enum Type {
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
        King
    }

    enum Team {
        Black,
        White
    }

    Type type;
    Team team;
    int x, y;
}
