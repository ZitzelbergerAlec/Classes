import java.io.*;          // Access System.out
import javax.swing.*; //for the GUI
import java.awt.*; //for the GUI
import java.awt.event.*; //for the GUI



class JavaAssign2 implements ActionListener{			
		//for the GUI
		JButton button;
		JLabel label = new JLabel("Welcome to Tic Tac Toe! X goes first.");
		
		//Game variables
		String whosTurn = "X";
		int gameOver = 0;
		char winner;
		
		//set up our distinct color variables for X and O
		Color XColor = Color.red;
		Color OColor = Color.blue;
		Color whosTurnColor = XColor;
		
		//Set up all of our button variables
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
		
		void toggleXO(){
			if(whosTurn.equals("X")){
				whosTurn = "O";
				whosTurnColor = OColor;
			} else {
				whosTurn = "X";
				whosTurnColor = XColor;
			}
			label.setText(whosTurn + "'s turn.");
		}
		
		void startGui(){
			/// Creates a JFrame object (the window)
			JFrame mainframe = new JFrame("Tic Tac Toe Game");
			mainframe.setSize(500,600);
			JPanel panel = new JPanel();
			JPanel outerPanel = new JPanel();//panel to hold the label in the top box, and the board in the bottom one
			outerPanel.setLayout(new GridLayout(2,1));
			
			
			panel.setLayout(new GridLayout(3,3));
			panel.setPreferredSize(new Dimension(500, 500));
			
			//Put our buttons into an array, instantiate them, add action listeners, and add them to the panel
			JButton[] buttonArray = {A1, B1, C1, A2, B2, C2, A3, B3, C3};
			
			for(int i=0; i<9; i++){
				button = buttonArray[i];
				button.addActionListener(this);
				panel.add(button);
			}
			
			/// Set the Program to exit when the window is closed		
			mainframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			
			//add our label and game panels to the outer panel, then add that to the frame
			outerPanel.add(label);
			outerPanel.add(panel);
			mainframe.add(outerPanel);
			
			mainframe.setVisible(true); //Display the frame
			GameHelper helper = new GameHelper();
		}
		
		public void actionPerformed(ActionEvent event){
			JButton button = (JButton) event.getSource(); //Cast the event source as a JButton so we can work with it
			if(gameOver == 1){ //the game is over
				if(winner != 'C'){
					label.setText("The game is over! " + winner + " won! Stop playing!");
				} else { //it was a cat's game
					label.setText("The game is over! It was a cat's game! Stop playing!");
				}
			} else { //game is not over. Find out which button was pressed and act accordingly
				if(button == A1){
					if(myboard.modifyboard("A1", whosTurn) == 0){ //A1 was pressed. Make sure there isn't someone already there.
						button.setText(whosTurn); //change the button text to X or O to indicate it's taken
						button.setBackground(whosTurnColor); //change the color of the button to match the preset color for x or o
						toggleXO(); //change players
					} else { //Someone's already in that spot
						label.setText("Someone's already in that spot. Please try again."); //don't change players. Give them infinite tries to pick a spot that's not taken
					}
				} else if(button == B1){
					if(myboard.modifyboard("B1", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				} else if(button == C1){
					if(myboard.modifyboard("C1", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				} else if(button == A2){
					if(myboard.modifyboard("A2", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				} else if(button == B2){
					if(myboard.modifyboard("B2", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				} else if(button == C2){
					if(myboard.modifyboard("C2", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				} else if(button == A3){
					if(myboard.modifyboard("A3", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				} else if(button == B3){
					if(myboard.modifyboard("B3", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				} else if(button == C3){
					if(myboard.modifyboard("C3", whosTurn) == 0){
						button.setText(whosTurn);
						button.setBackground(whosTurnColor);
						toggleXO();
					} else {
						label.setText("Someone's already in that spot. Please try again.");
					}
				}
				winner = myboard.checkboard();
				if((winner != 'N') && (winner != 'C')){
						label.setText(winner + " is the winner!");
						gameOver = 1; //Stop the game if there's a winner
				} else if(winner == 'C'){
					label.setText("Cat's game!");
					gameOver = 1; //Stop the game if there's a cat's game
				}
			}
		}

	public static void main ( String[] args )
    {
		//Instantiate the GUI, start it up. It'll take things from here, thanks.
		JavaAssign2 gui = new JavaAssign2(); 
		gui.startGui();
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
			if(move.equalsIgnoreCase("A1")){
				if(!(board[0].equals("-"))){ //Make sure that spot isn't set already
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
			//Test all ways of winning for X.
			if((board[0].equals("X") && board[1].equals("X") && board[2].equals("X")) || (board[3].equals("X") && board[4].equals("X") && board[5].equals("X")) || (board[6].equals("X") && board[7].equals("X") && board[8].equals("X")) || (board[0].equals("X") && board[4].equals("X") && board[8].equals("X")) || (board[2].equals("X") && board[4].equals("X") && board[6].equals("X")) || (board[0].equals("X") && board[3].equals("X") && board[6].equals("X")) || (board[1].equals("X") && board[4].equals("X") && board[7].equals("X")) || (board[2].equals("X") && board[5].equals("X") && board[8].equals("X"))){
				return 'X';
			}
			//Test all ways of winning for O.
			else if((board[0].equals("O") && board[1].equals("O") && board[2].equals("O")) || (board[3].equals("O") && board[4].equals("O") && board[5].equals("O")) || (board[6].equals("O") && board[7].equals("O") && board[8].equals("O")) || (board[0].equals("O") && board[4].equals("O") && board[8].equals("O")) || (board[2].equals("O") && board[4].equals("O") && board[6].equals("O")) || (board[0].equals("O") && board[3].equals("O") && board[6].equals("O")) || (board[1].equals("O") && board[4].equals("O") && board[7].equals("O")) || (board[2].equals("O") && board[5].equals("O") && board[8].equals("O"))){
				return 'O';
			} 
			//Check for cat's game
			else if((!(board[0].equals("-")) && !(board[1].equals("-")) && !(board[2].equals("-"))) && (!(board[3].equals("-")) && !(board[4].equals("-")) && !(board[5].equals("-"))) && (!(board[6].equals("-")) && !(board[7].equals("-")) && !(board[8].equals("-"))) && (!(board[0].equals("-")) && !(board[4].equals("-")) && !(board[8].equals("-"))) && (!(board[2].equals("-")) && !(board[4].equals("-")) && !(board[6].equals("-"))) && (!(board[0].equals("-")) && !(board[3].equals("-")) && !(board[6].equals("-"))) && (!(board[1].equals("-")) && !(board[4].equals("-")) && !(board[7].equals("-"))) && (!(board[2].equals("-")) && !(board[5].equals("-")) && !(board[8].equals("-")))){
				return 'C';
			}	
			return 'N'; //No winner or cat's game...yet
		}
	}
}