import javax.swing.AbstractAction; //to generate actions
import javax.swing.Action; //to generate actions
import java.io.*;
import javax.sound.midi.*; //for GUI
import javax.swing.*; //for GUI
import java.awt.event.*; //for GUI
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import javazoom.jl.player.Player; //for MP3 Player
import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 
import java.util.Enumeration;

class MP3Controller extends AbstractAction{
	

}


class myGUI implements ActionListener{
	JButton button1;
	int button1state = 0;
	
	void startGui(){
		/// Creates a JFrame object (the window)
		JFrame mainframe = new JFrame("MP3 Player");
		
		///Creates a new button object (labels it Button 1)
		button1 = new JButton("Stopped");
		
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
			button1.setText("Playing");
			button1state = 1;
			}
		else{
			button1.setText("Stopped");
			button1state = 0;
			}
	
	}
}

class MP3 {
    private String filename;
    private Player player; 

    // constructor that takes the name of an MP3 file
    public MP3(String filename) {
        this.filename = filename;
    }

    public void close() { if (player != null) player.close(); }

    // play the MP3 file to the sound card
    public void play() {
        try {
            FileInputStream fis     = new FileInputStream(filename);
            BufferedInputStream bis = new BufferedInputStream(fis);
            player = new Player(bis);
        }
        catch (Exception e) {
            System.out.println("Problem playing file " + filename);
            System.out.println(e);
        }

        // run in new thread to play in background
        new Thread() {
            public void run() {
                try { player.play(); }
                catch (Exception e) { System.out.println(e); }
            }
        }.start();
    } //play
} //MP3


public class lab5 implements SerialPortEventListener {
	SerialPort serialPort;
	
        /** The port we're normally going to use. You need to change these to match your system*/
	private static final String PORT_NAMES[] = { 
			"/dev/tty.usbserial-A9007UX1", // Mac OS X
			"/dev/ttyUSB0", // Linux
			"COM6", // Windows
			};
	
	/** Buffered input stream from the port */
	private InputStream input;
	
	/** The output stream to the port */
	private OutputStream output;
	
	/** Milliseconds to block while waiting for port open */
	private static final int TIME_OUT = 2000;
	
	/** Default bits per second for COM port. */
	private static final int DATA_RATE = 9600;

	public void initialize() {
		CommPortIdentifier portId = null;
		Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

		// iterate through, looking for the port
		while (portEnum.hasMoreElements()) { // Looking through all available ports for one that matches the one we want.
			CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement(); // Make a new port
			for (String portName : PORT_NAMES) {
				if (currPortId.getName().equals(portName)) { // Is the current port name the same as one of the names in our list?
					portId = currPortId; //if so, use this port.
					break;
				}
			}
		}

		if (portId == null) { // We never found the port!
			System.out.println("Could not find COM port.");
			return;
		}

		try { // We think we know the port so lets TRY to open it. It might not work though so we are only TRYing it.
			// open serial port, and use class name for the appName.
			serialPort = (SerialPort) portId.open(this.getClass().getName(),
					TIME_OUT);

			// set port parameters
			serialPort.setSerialPortParams(DATA_RATE,
					SerialPort.DATABITS_8,
					SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);

			// open the streams
			output = serialPort.getOutputStream();
			input = serialPort.getInputStream();
			
		} catch (Exception e) {
			System.err.println(e.toString());
		}
	}

	/**
	 * This should be called when you stop using the port.
	 * This will prevent port locking on platforms like Linux.
	 */
	public synchronized void close() {
		if (serialPort != null) {
			serialPort.removeEventListener();
			serialPort.close();
		}
	}

	public String getUARTLine(){
			String inputLine = null;

			try{
				BufferedReader input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
				inputLine = input.readLine();
				if (inputLine.length() == 0)
					return null;
				} catch (IOException e){
					//System.out.println("IOException: " + e);
					return null;
				}
			return inputLine;
		}
	
	public char readUART(){
			int temp = '.';
			
			try{
				if (input.available() > 0)
					temp = input.read();
				if (temp == -1)
					return 0;
				} catch (IOException e){
					//System.out.println("IOException: " + e);
					return 0;
				}
		return (char)(byte)temp;
		}
	
	public int writeUART(char b){
			try{
				output.write((char)(byte)b);
				} catch (IOException e){
					//System.out.println("IOException: " + e);
					return -1;
				}
		return 1;
		}
	
	/**
	 * Handle an event on the serial port. Read the data and print it.
	 */
	public synchronized void serialEvent(SerialPortEvent oEvent) {
		if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
			try {
				int available = input.available();
				byte chunk[] = new byte[available];
				input.read(chunk, 0, available);

				// Displayed results are codepage dependent
				System.out.print(new String(chunk));
			} catch (Exception e) {
				System.err.println(e.toString());
			}
		}
		// Ignore all the other eventTypes, but you should consider the other ones.
	}

	public static void main(String[] args) throws Exception {
		lab5 main = new lab5();
		main.initialize();
		/** Your code should go here **/
		/*global variables*/
		String uartLine; //container variable for UART lines
		System.out.println("Started");	
		String filename = "Neon Trees-Animal.mp3";
        MP3 mp3 = new MP3(filename);
        mp3.play();
		int stopped = 0;

		//Instantiate GUI
		myGUI gui = new myGUI();
		gui.startGui();
		
        // do whatever computation you like, while music plays
        while(true){
			try{
				uartLine = main.getUARTLine(); //get the line from the UART
				if(uartLine.isEmpty() == false){ //only print it if it's not null
					if(uartLine.equals("stop")){
						//stop MP3 playback
						System.out.println("Stopped");
						mp3.close();
						stopped = 1;
						//myGUI.clickButton();
					} else if(uartLine.equals("play")){
						//start MP3 playback
						System.out.println("Playing");	
						try{
							mp3.play();
							//myGUI.clickButton();
						} catch(Exception e) {}
						
					}
				}
			}
			catch(Exception e) {
				//System.err.println(e.toString());
			}
			
		} //while
	}
}


