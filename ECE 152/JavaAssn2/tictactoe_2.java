import java.util.Scanner;
import java.io.*;          // Access System.out
import javax.sound.midi.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;



class tictactoe implements ActionListener{			
		JButton button;
		String whosTurn = "X";
		
		JButton A1 = new JButton();
		JButton B1 = new JButton();
		JButton C1 = new JButton();
		JButton A2 = new JButton();
		JButton B2 = new JButton();
		JButton C2 = new JButton();
		JButton A3 = new JButton();
		JButton B3 = new JButton();
		JButton C3 = new JButton();
		tictactoeboard myboard = new tictactoeboard();
		
		public void setTurn(String turn){
			whosTurn = turn;
		}
		
		public String getTurn(){
			return whosTurn;
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

	public static void main ( String[] args )
    {
		tictactoe gui = new tictactoe();
		gui.startGui();
		
		tictactoeboard myboard = new tictactoeboard();
		GameHelper helper = new GameHelper();
		String location;
		char winner;
		int result = 0;
		String XOToggle = "X";
		
		myboard.printboard(); // Print out the board for the first time
		location = helper.getUserInput("X goes first. Where?"); // Here is an example of taking in some data 
		myboard.modifyboard(location, "X");
		myboard.printboard(); // Print out the board for the first time
		
		/**** You need to write this ********/
		do {
			if(XOToggle.equals("X")){
				XOToggle = "O";
			} else if(XOToggle.equals("X")){
				XOToggle = "X";
			}
			do {
				if(result == 1){
					System.out.println("Someone is already in that spot. Please try again.");
				}
				location = helper.getUserInput(XOToggle + " goes next. Where?"); 
				result = myboard.modifyboard(location, XOToggle);
				myboard.printboard();
			} while(result == 1);
			winner = myboard.checkboard();
		} while(winner == 'N');
		System.out.println(winner + " is the winner!");
    }
}

class GameHelper{
		public String getUserInput(String prompt){
			String inputLine = null;
			System.out.print(prompt + "> ");
			try{
				BufferedReader is = new BufferedReader(new InputStreamReader(System.in));
				inputLine = is.readLine();
				if (inputLine.length() == 0)
					return null;
				} catch (IOException e){
					System.out.println("IOException: " + e);
				}
			return inputLine;
		}
	
	}

	class tictactoeboard{
		private String[] board = {"-", "-", "-", "-", "-", "-", "-", "-", "-"};
		
		String[] getboard(){
			return board;
		}
		
		void printboard(){
			System.out.println("  " + " A   B   C\n");
			System.out.println("1: " + board[0] + " | " + board[1] + " | " + board[2]);
			System.out.println("  -----------");
			System.out.println("2: " + board[3] + " | " + board[4] + " | " + board[5]);
			System.out.println("  -----------");
			System.out.println("3: " + board[6] + " | " + board[7] + " | " + board[8]);
		}
		
		int modifyboard(String move, String XorO){ // This function takes the user input trys to place the move. If the grid spot is already filled, it returns a 1, else returns 0. The grid should be updated with 'X' or 'O' as appropriate.
	
		/**** You need to write this ********/
			if(move.equalsIgnoreCase("A1")){
				if(!board[0].equals("-")){ //Make sure that spot isn't set already
					return 1;
				}
				board[0] = XorO;
			} else if(move.equalsIgnoreCase("B1")){
				if(!(board[1].equals("-"))){
					return 1;
				}
				board[1] = XorO;
			} else if(move.equalsIgnoreCase("C1")){
				if(!(board[2].equals("-"))){
					return 1;
				}
				board[2] = XorO;
			} else if(move.equalsIgnoreCase("A2")){
				if(!(board[3].equals("-"))){
					return 1;
				}
				board[3] = XorO;
			} else if(move.equalsIgnoreCase("B2")){
				if(!(board[4].equals("-"))){
					return 1;
				}
				board[4] = XorO;
			} else if(move.equalsIgnoreCase("C2")){
				if(!(board[5].equals("-"))){
					return 1;
				}
				board[5] = XorO;
			} else if(move.equalsIgnoreCase("A3")){
				if(!(board[6].equals("-"))){
					return 1;
				}
				board[6] = XorO;
			} else if(move.equalsIgnoreCase("B3")){
				if(!(board[7].equals("-"))){
					return 1;
				}
				board[7] = XorO;
			} else if(move.equalsIgnoreCase("C3")){
				if(!(board[8].equals("-"))){
					return 1;
				}
				board[8] = XorO;
			}
		return 0;
		}
		
		char checkboard(){ // This function checks if there is a winner on the board. If so, it should return 'X' or 'O' based on the winner. it should return 'N' if no winner found
	
		/**** You need to write this ********/
			//Test all ways of winning for X.
			if((board[0].equals("X") && board[1].equals("X") && board[2].equals("X")) || (board[3].equals("X") && board[4].equals("X") && board[5].equals("X")) || (board[6].equals("X") && board[7].equals("X") && board[8].equals("X")) || (board[0].equals("X") && board[4].equals("X") && board[8].equals("X")) || (board[2].equals("X") && board[4].equals("X") && board[6].equals("X")) || (board[0].equals("X") && board[3].equals("X") && board[6].equals("X")) || (board[1].equals("X") && board[4].equals("X") && board[7].equals("X")) || (board[2].equals("X") && board[5].equals("X") && board[8].equals("X"))){
				return 'X';
			}
			//Test all ways of winning for O.
			else if((board[0].equals("O") && board[1].equals("O") && board[2].equals("O")) || (board[3].equals("O") && board[4].equals("O") && board[5].equals("O")) || (board[6].equals("O") && board[7].equals("O") && board[8].equals("O")) || (board[0].equals("O") && board[4].equals("O") && board[8].equals("O")) || (board[2].equals("O") && board[4].equals("O") && board[6].equals("O")) || (board[0].equals("O") && board[3].equals("O") && board[6].equals("O")) || (board[1].equals("O") && board[4].equals("O") && board[7].equals("O")) || (board[2].equals("O") && board[5].equals("O") && board[8].equals("O"))){
				return 'O';
			} 
			//Check for cat's game
			//else if((board[0] == 'O' && board[1] == 'O' && board[2] == 'O') || (board[3] == 'O' && board[4] == 'O' && board[5] == 'O') || (board[6] == 'O' && board[7] == 'O' && board[8] == 'O') || (board[0] == 'O' && board[4] == 'O' && board[8] == 'O') || (board[2] == 'O' && board[4] == 'O' && board[6] == 'O') || (board[0] == 'O' && board[3] == 'O' && board[6] == 'O') || (board[1] == 'O' && board[4] == 'O' && board[7] == 'O') || (board[2] == 'O' && board[5] == 'O' && board[8] == 'O')){
			//	return 'C';
			//}
				
			return 'N';
		}
	}	