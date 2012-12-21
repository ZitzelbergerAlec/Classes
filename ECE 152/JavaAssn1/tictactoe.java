import java.util.Scanner;
import java.io.*;          // Access System.out

class tictactoe{
    public static void main ( String[] args )
    {
		tictactoeboard myboard = new tictactoeboard();
		GameHelper helper = new GameHelper();
		String location;
		char winner;
		int result = 0;
		char XOToggle = 'X';
		
		myboard.printboard(); // Print out the board for the first time
		location = helper.getUserInput("X goes first. Where?"); // Here is an example of taking in some data 
		myboard.modifyboard(location, 'X');
		myboard.printboard(); // Print out the board for the first time
		
		/**** You need to write this ********/
		do {
			if(XOToggle == 'X'){
				XOToggle = 'O';
			} else if(XOToggle == 'O'){
				XOToggle = 'X';
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
		private char[] board = {'-', '-', '-', '-', '-', '-', '-', '-', '-'};
		
		char[] getboard(){
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
		
		int modifyboard(String move, char XorO){ // This function takes the user input trys to place the move. If the grid spot is already filled, it returns a 1, else returns 0. The grid should be updated with 'X' or 'O' as appropriate.
	
		/**** You need to write this ********/
			if(move.equalsIgnoreCase("A1")){
				if(board[0] != '-'){ //Make sure that spot isn't set already
					return 1;
				}
				board[0] = XorO;
			} else if(move.equalsIgnoreCase("B1")){
				if(board[1] != '-'){
					return 1;
				}
				board[1] = XorO;
			} else if(move.equalsIgnoreCase("C1")){
				if(board[2] != '-'){
					return 1;
				}
				board[2] = XorO;
			} else if(move.equalsIgnoreCase("A2")){
				if(board[3] != '-'){
					return 1;
				}
				board[3] = XorO;
			} else if(move.equalsIgnoreCase("B2")){
				if(board[4] != '-'){
					return 1;
				}
				board[4] = XorO;
			} else if(move.equalsIgnoreCase("C2")){
				if(board[5] != '-'){
					return 1;
				}
				board[5] = XorO;
			} else if(move.equalsIgnoreCase("A3")){
				if(board[6] != '-'){
					return 1;
				}
				board[6] = XorO;
			} else if(move.equalsIgnoreCase("B3")){
				if(board[7] != '-'){
					return 1;
				}
				board[7] = XorO;
			} else if(move.equalsIgnoreCase("C3")){
				if(board[8] != '-'){
					return 1;
				}
				board[8] = XorO;
			}
		return 0;
		}
		
		char checkboard(){ // This function checks if there is a winner on the board. If so, it should return 'X' or 'O' based on the winner. it should return 'N' if no winner found
	
		/**** You need to write this ********/
			//Test all ways of winning for X.
			if((board[0] == 'X' && board[1] == 'X' && board[2] == 'X') || (board[3] == 'X' && board[4] == 'X' && board[5] == 'X') || (board[6] == 'X' && board[7] == 'X' && board[8] == 'X') || (board[0] == 'X' && board[4] == 'X' && board[8] == 'X') || (board[2] == 'X' && board[4] == 'X' && board[6] == 'X') || (board[0] == 'X' && board[3] == 'X' && board[6] == 'X') || (board[1] == 'X' && board[4] == 'X' && board[7] == 'X') || (board[2] == 'X' && board[5] == 'X' && board[8] == 'X')){
				return 'X';
			}
			//Test all ways of winning for O.
			else if((board[0] == 'O' && board[1] == 'O' && board[2] == 'O') || (board[3] == 'O' && board[4] == 'O' && board[5] == 'O') || (board[6] == 'O' && board[7] == 'O' && board[8] == 'O') || (board[0] == 'O' && board[4] == 'O' && board[8] == 'O') || (board[2] == 'O' && board[4] == 'O' && board[6] == 'O') || (board[0] == 'O' && board[3] == 'O' && board[6] == 'O') || (board[1] == 'O' && board[4] == 'O' && board[7] == 'O') || (board[2] == 'O' && board[5] == 'O' && board[8] == 'O')){
				return 'O';
			}
			return 'N';
		}
	}	