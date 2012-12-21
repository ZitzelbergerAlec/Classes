import javax.sound.midi.*;
import java.io.*;
import javax.swing.*;
import java.awt.*;

public class musicVideo2 {
	static JFrame f = new JFrame("Event Generation Test");
	static MyDrawPanel ml;
	JButton button;
	int playState = 1;
	
	public static void main(String[] args){
		musicVideo2 gui = new musicVideo2();
		gui.go();
	} //close method
	
	public void setUpGui(){
		button = new JButton("Playing");
		f.getContentPane().add(button1);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setContentPane(ml);
		f.setSize(300,300);
		f.setVisible(true);
	} //close method
		
	public void go(){
		setUpGui();
		MyClass c = new MyClass();

		// Register for MyEvents from c
		c.addMyEventListener(new MyEventListener() {
			public void myEventOccurred(MyEvent evt) {
				if(playState == 1){
					button.setText("Stopped");
					playState = 0;
				} else if(playState == 0){
					button.setText("Playing");
					playState = 1;
				}
			}
		});
		try{
			for(int i=5; i<120; i++){
				if(i == 60){
					c.fireMyEvent();
				} else if(i == 100){
					c.fireMyEvent();
				}
				Thread.sleep(100); //add a delay so we can see changes
			} //end loop
		} catch(Exception ex) {ex.printStackTrace();}
			
	} //close go method	
	
	//Start borrowed event code
	// Declare the event. It must extend EventObject.
	public class MyEvent extends EventObject {
		public MyEvent(Object source) {
			super(source);
		}
	}

	// Declare the listener class. It must extend EventListener.
	// A class must implement this interface to get MyEvents.
	public interface MyEventListener extends EventListener {
		public void myEventOccurred(MyEvent evt);
	}

	// Add the event registration and notification code to a class.
	public class MyClass {
		// Create the listener list
		protected javax.swing.event.EventListenerList listenerList =
			new javax.swing.event.EventListenerList();

		// This methods allows classes to register for MyEvents
		public void addMyEventListener(MyEventListener listener) {
			listenerList.add(MyEventListener.class, listener);
		}

		// This methods allows classes to unregister for MyEvents
		public void removeMyEventListener(MyEventListener listener) {
			listenerList.remove(MyEventListener.class, listener);
		}

		// This private class is used to fire MyEvents
		void fireMyEvent(MyEvent evt) {
			Object[] listeners = listenerList.getListenerList();
			// Each listener occupies two elements - the first is the listener class
			// and the second is the listener instance
			for (int i=0; i<listeners.length; i+=2) {
				if (listeners[i]==MyEventListener.class) {
					((MyEventListener)listeners[i+1]).myEventOccurred(evt);
				}
			}
		}
	}
	//End borrowed event code
	
} //close class