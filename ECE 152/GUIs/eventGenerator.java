import java.util.Observable;
import java.io.*;
import javax.swing.*;
import java.awt.*;

public class eventGenerator implements Observer
{
	JButton button; 
	JFrame frame; 
	
	
	public void go(){
		frame = new JFrame("Main Frame");
	
	}
	
    public static void main(String[] args) 
    {
		Chapter12b gui = new Chapter12b();
		gui.startGui();
        ReadFile rf = new ReadFile();
        eventGenerator eg = new eventGenerator();
        rf.addObserver(eg);
        rf.readFile();
		button = new JButton("Playing");
    }

    @Override
    public void update(Observable o, Object arg)
    {
        // This method will be called after the File is finished reading.
		button.setText("Stopped");
    }
	
	public class ReadFile extends Observable 
{
    public void readFile()
    {
        Thread.sleep(1000);
        notifyObservers();
    }
	
	public class Chapter12b implements ActionListener{
		JButton button1;
		int button1state = 0;
		
		void startGui(){
			/// Creates a JFrame object (the window)
			JFrame mainframe = new JFrame("Main Frame");
			
			///Creates a new button object (labels it Button 1)
			button1 = new JButton("Button 1");
			
			/// Put a listener on the button so the code can respond to clicks
			button1.addActionListener(this);
			
			/// Set the Program to exit when the window is closed		
			mainframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			
			///Put the button on the window
			mainframe.getContentPane().add(button1);
			
			/// Set the window to 300 by 300 pixels
			mainframe.setSize(300,300);
			
			/// Actually display the window
			mainframe.setVisible(true);
			
		}
		
		public void actionPerformed(ActionEvent event){
			if (button1state == 0){
				button1.setText("Clicked");
				button1state = 1;
				}
			else{
				button1.setText("Not Clicked");
				button1state = 0;
				}
		
		}
	}// close chapter12b class

}
}

