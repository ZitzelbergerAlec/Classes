import java.io.*;
import javax.sound.midi.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class tGUI implements ActionListener{
	JButton button;
	String whosTurn = "X";
	int button1state = 0;

	JButton A1 = new JButton();
	JButton B1 = new JButton();
	JButton C1 = new JButton();
	JButton A2 = new JButton();
	JButton B2 = new JButton();
	JButton C2 = new JButton();
	JButton A3 = new JButton();
	JButton B3 = new JButton();
	JButton C3 = new JButton();
	
	public static void main(String[] args){
		tGUI gui = new tGUI();
		gui.startGui();
	
	}
	
	void startGui(){
		/// Creates a JFrame object (the window)
		JFrame mainframe = new JFrame("Tic Tac Toe Game");
		JPanel panel = new JPanel();
		panel.setLayout(new GridLayout(3,3));
		
		JButton[] buttonArray = {A1, B1, C1, A2, B2, C2, A3, B3, C3};
		
		for(int i=0; i<9; i++){
			button = buttonArray[i];
			button.addActionListener(this);
			panel.add(button);
		}
		
		/// Set the Program to exit when the window is closed		
		mainframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		
		mainframe.add(panel);
		/// Set the window to 300 by 300 pixels
		mainframe.setSize(500,500);
		
		/// Actually display the window
		mainframe.setVisible(true);
		
	}
	
	public void actionPerformed(ActionEvent event){
		JButton button = (JButton) event.getSource();
		if(event.getSource() == A1){
			System.out.println("A1 was clicked!");
		}
		button.setText(whosTurn);
		if(whosTurn.equals("X")){
			whosTurn = "O";
		} else {
			whosTurn = "X";
		}
		
	
	}
}
