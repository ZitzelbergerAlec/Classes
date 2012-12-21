import java.awt.*;
import javax.swing.*;

public class myGradient extends JPanel {

  public myGradient() {
  }

  public void paintComponent(Graphics g) {
    Graphics2D g2d = (Graphics2D) g;
	GradientPaint gradient = new GradientPaint(70,70,Color.blue, 150, 150, Color.orange); //starting point, starting color, ending point, ending color.
	g2d.setPaint(gradient);
	g2d.fillOval(70,70,100,100);
  }

  public static void main(String args[]) {
    JFrame frame = new JFrame("JPEG Sample");
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    frame.setLayout(new GridLayout(2, 2));

    myGradient panel = new myGradient();
	frame.add(panel);
    frame.setSize(500, 500);
    frame.setVisible(true);
  }
}