import java.awt.Color;

import javax.swing.JButton;

public class Board {
    class BoardSpace {
        BoardSpace(int x, int y) {
            this.button = new JButton();
            this.team = ((x % 2 == (y % 2)) ? Unit.Team.White : Unit.Team.Black);
            this.x = x;
            this.y = y;
            this.button.setOpaque(true);
            this.button.setBackground(this.team == Unit.Team.White ? Color.WHITE : Color.BLACK);
            this.button.setForeground(this.team == Unit.Team.White ? Color.WHITE : Color.BLACK);
            this.button.setBounds(x * 48, y * 48, 48, 48);
        }
        
        JButton button;
        Unit.Team team;
        int x, y;

        public JButton GetButtonComp() {
            return button;
        }
    }

    Board() {
        units = new Unit[8 * 4];

        // Black pawns
        for (int i = 0; i < 8; ++i) {
            units[i] = new Unit(Unit.Type.Pawn, Unit.Team.Black, i, 1);
        }

        // Black units
        units[8] = new Unit(Unit.Type.Rook, Unit.Team.Black, 0, 0);
        units[9] = new Unit(Unit.Type.Knight, Unit.Team.Black, 1, 0);
        units[10] = new Unit(Unit.Type.Bishop, Unit.Team.Black, 2, 0);
        units[11] = new Unit(Unit.Type.King, Unit.Team.Black, 3, 0);
        units[12] = new Unit(Unit.Type.Queen, Unit.Team.Black, 4, 0);
        units[13] = new Unit(Unit.Type.Bishop, Unit.Team.Black, 5, 0);
        units[14] = new Unit(Unit.Type.Knight, Unit.Team.Black, 6, 0);
        units[15] = new Unit(Unit.Type.Rook, Unit.Team.Black, 7, 0);

        // White pawns
        for (int i = 0; i < 8; ++i) {
            units[16 + i] = new Unit(Unit.Type.Pawn, Unit.Team.White, i, 6);
        }

        // White units
        units[24] = new Unit(Unit.Type.Rook, Unit.Team.White, 0, 7);
        units[25] = new Unit(Unit.Type.Knight, Unit.Team.White, 1, 7);
        units[26] = new Unit(Unit.Type.Bishop, Unit.Team.White, 2, 7);
        units[27] = new Unit(Unit.Type.King, Unit.Team.White, 3, 7);
        units[28] = new Unit(Unit.Type.Queen, Unit.Team.White, 4, 7);
        units[29] = new Unit(Unit.Type.Bishop, Unit.Team.White, 5, 7);
        units[30] = new Unit(Unit.Type.Knight, Unit.Team.White, 6, 7);
        units[31] = new Unit(Unit.Type.Rook, Unit.Team.White, 7, 7);

        count = 32;

        // Spaces
        spaces = new BoardSpace[8 * 8];
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                spaces[y * 8 + x] = new BoardSpace(x, y);
            }
        }
    }

    int count;
    Unit[] units;
    BoardSpace[] spaces;

    int GetUnitAtSpace(int x, int y) {
        for (int i = 0; i < count; ++i) {
            if (units[i].x == x && units[i].y == y)
                return i;
        }
        return -1;
    }
    Unit GetUnit(int id) {
        return units[id];
    }
}
