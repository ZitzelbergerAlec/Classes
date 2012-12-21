import java.io.*;
import javax.sound.midi.*;
import javax.swing.*;
import java.awt.event.*;

public class gui1{
	public void paintComponent(Graphics g){
		Image image = new ImageIcon("test.jpg").getImage();
		g.drawImage(image,3,4,this);
	}
}