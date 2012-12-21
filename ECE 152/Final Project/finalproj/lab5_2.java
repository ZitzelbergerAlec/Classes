import javazoom.jl.player.Player;
import java.io.FileInputStream;
import java.util.concurrent.Executors;
import java.io.*;
import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 
import java.util.Enumeration;

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
		System.out.println("Started");	
		lab5 main = new lab5();
		main.initialize();
		/** Your code should go here **/
		/*global variables*/
		
		String uartLine; //container variable for UART lines
		
		//multithreading? play music while doing other stuff
		Executor executor = Executors.newSingleThreadExecutor();  
		executor.execute(new Runnable() { public void run() {  
			//if(player != null) player.close();  
			try{  
				player = new Player(new FileInputStream("Neon Trees-Animal.mp3"));  
				player.play();  
			}catch(Exception e){} 
		} });
		
		/*
		try{
			FileInputStream fis=new FileInputStream("Neon Trees-Animal.mp3");
			Player playMp3=new Player(fis);
			playMp3.play();
		} 
		catch(Exception e){
		}
		*/
		
		while(true){
			try{
				uartLine = main.getUARTLine(); //get the line from the UART
				if(uartLine.isEmpty() == false){ //only print it if it's not null
					if(uartLine.equals("stop")){
						//stop MP3 playback
						System.out.println("Stopped");	
					} else if(uartLine.equals("play")){
						//start MP3 playback
						System.out.println("Playing");	
						/*
						try{
							//playMp3.play();
						} catch(Exception e) {}
						*/
					}
				}
			}
			catch(Exception e) {
				//System.err.println(e.toString());
			}
		} //while
	}
}


