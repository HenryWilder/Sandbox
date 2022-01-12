import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.awt.image.BufferedImage;

public class App {
    public static void CreateAndShowGUI()
    {
        try {
            UIManager.setLookAndFeel(
                UIManager.getCrossPlatformLookAndFeelClassName());
        } catch (Exception e) { }

        JFrame.setDefaultLookAndFeelDecorated(false);
        JDialog.setDefaultLookAndFeelDecorated(false);

        JFrame frame = new JFrame("Test");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // Menu bar
        JMenuBar menuBar = new JMenuBar();
        JMenu menu = new JMenu("File");
        menu.add(new JMenuItem("New game"));
        menu.add(new JMenuItem("Load"));
        menu.add(new JMenuItem("Save"));
        menuBar.add(menu);
        frame.setJMenuBar(menuBar);

        Board gameBoard = new Board();
        
        // UI elements
        Container contentPane = frame.getContentPane();

        for (int i = 0; i < 8 * 8; ++i) {
            // int unitID = gameBoard.GetUnitAtSpace(i % 8, i / 8);
            // if (unitID != 0)
            // gameBoard.spaces[i].GetButtonComp().setName(switch (gameBoard.GetUnit(unitID).type) {
            //         case Pawn: "Pawn"; break;
            //         case Rook: "Rook"; break;
            //         case Knight: "Knight"; break;
            //         case Bishop: "Bishop"; break;
            //         case Queen: "Queen"; break;
            //         case King: "King"; break;
            //     });
            contentPane.add(gameBoard.spaces[i].GetButtonComp());
        }

        frame.setPreferredSize(new Dimension(8 * (48 + 2), 8 * (48 + 2) + 46 + menuBar.getHeight()));
        frame.getRootPane().setDefaultButton(null);
        frame.pack();
        frame.setLocationRelativeTo(null); // Center on screen
        frame.setVisible(true);
    }
    public static void main(String[] args) throws Exception {
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                CreateAndShowGUI();
            }
        });
    }
}
