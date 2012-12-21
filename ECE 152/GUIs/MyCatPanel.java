import java.awt.*;
import javax.swing.*;

public class MyCatPanel extends JPanel {

  public MyCatPanel() {
  }

  public void paintComponent(Graphics g) {
    int width = getWidth();
    int height = getHeight();
    Image image = new ImageIcon("warrior-cat-icon-35.jpg").getImage();
	g.drawImage(image,3,4,this);
  }

  public static void main(String args[]) {
    JFrame frame = new JFrame("JPEG Sample");
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    frame.setLayout(new GridLayout(2, 2));

    MyCatPanel panel = new MyCatPanel();
	frame.add(panel);
    frame.setSize(300, 200);
    frame.setVisible(true);
  }
}