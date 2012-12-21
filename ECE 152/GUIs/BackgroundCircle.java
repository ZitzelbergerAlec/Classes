import java.awt.*;
import javax.swing.*;

public class BackgroundCircle extends JPanel {

  public BackgroundCircle() {
  }

  public void paintComponent(Graphics g) {
    g.fillRect(0,0,this.getWidth(), this.getHeight());
	int red = (int) (Math.random() * 256);
    int green = (int) (Math.random() * 256);
	int blue = (int) (Math.random() * 256);
	Color randomColor = new Color(red, green, blue);
	g.setColor(randomColor);
	g.fillOval(70,70,100,100); //start 70 pixels from the left, 70 from the top, make it 100 px wide, and 100 px tall
  }

  public static void main(String args[]) {
    JFrame frame = new JFrame("JPEG Sample");
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    frame.setLayout(new GridLayout(2, 2));

    BackgroundCircle panel = new BackgroundCircle();
	frame.add(panel);
    frame.setSize(500, 500);
    frame.setVisible(true);
  }
}