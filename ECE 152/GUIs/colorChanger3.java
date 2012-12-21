import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class colorChanger3 {
	JFrame frame;
	JButton colorButton;
	JButton labelButton;
	JLabel label;
	
	public static void main(String args[]) {
		colorChanger3 gui = new colorChanger3();
		gui.go();
    }
	
	public void go(){
		//build gui
		frame = new JFrame();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		colorButton = new JButton("Change Colors");
		labelButton = new JButton("Change label");
		label = new JLabel("I'm a Label");
		
		colorButton.addActionListener(new ColorListener());
		labelButton.addActionListener(new LabelListener());
		MyDrawPanel drawPanel = new MyDrawPanel();
		
		frame.getContentPane().add(BorderLayout.EAST, labelButton);
		frame.getContentPane().add(BorderLayout.SOUTH, colorButton);
		frame.getContentPane().add(BorderLayout.CENTER, drawPanel);
		frame.getContentPane().add(BorderLayout.WEST, label);
		frame.setSize(500, 500);
		frame.setVisible(true);
	}
	
	class LabelListener implements ActionListener {
		public void actionPerformed(ActionEvent event){
			label.setText("Ouch!");
		}
	} //close inner class
	
	class ColorListener implements ActionListener {
		public void actionPerformed(ActionEvent event){
			frame.repaint();
		}
	} //close inner class
}

class MyDrawPanel extends JPanel {
	public void paintComponent(Graphics g){
		//fill the oval with a random color
		Graphics2D g2d = (Graphics2D) g;
		int red = (int) (Math.random() * 256);
		int green = (int) (Math.random() * 256);
		int blue = (int) (Math.random() * 256);
		Color startColor = new Color(red, green, blue);
		
		red = (int) (Math.random() * 256);
		green = (int) (Math.random() * 256);
		blue = (int) (Math.random() * 256);
		Color endColor = new Color(red, green, blue);
		
		GradientPaint gradient = new GradientPaint(70,70,startColor,150,150,endColor);
		g2d.setPaint(gradient);
		g2d.fillOval(70,70,100,100);
	}
}