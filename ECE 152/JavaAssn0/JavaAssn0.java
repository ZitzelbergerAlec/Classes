import java.io.*;          // Access System.out

class javaAssn0 {
    public static void main (String[] args) { //main function
		//the data that will be entered by the user
        String answer = null;
		
		//an instance of the BufferedReader class
        //will be used to read the data
        BufferedReader reader;

        //specify the reader variable 
        //to be a standard input buffer
        reader = new BufferedReader(new InputStreamReader(System.in));
		
		while(true){	
			System.out.println("Question 1: Why did the chicken cross the road?");
			System.out.println("A: To see what he could see.");
			System.out.println("B: To get to the other side.");
			System.out.println("C: To run away from the fox.");
			System.out.println("D: None of the above.\n");
			
			
			try {
				answer = reader.readLine();
			} catch (IOException ioe) {
				System.out.println("IO error trying to read your answer!");
				System.exit(1);
			}
	
			if(answer.equalsIgnoreCase("B")){
				System.out.println("Congrats! You got the right answer!\n");
				break;
			} else if(((answer.equalsIgnoreCase("A")) | (answer.equalsIgnoreCase("C")) | (answer.equalsIgnoreCase("D"))) == false){
				System.out.println("'" + answer + "' is not a valid answer! Try again.\n");
			} else {
				System.out.println("Wrong! The chicken wanted to get to the other side because that is the punch line of this lame joke.\n");
				break;
			}
		}
		
		while(true){
			System.out.println("Question 2: How much wood would a woodchuck chuck if a woodchuck could chuck wood?");
			System.out.println("A: 1 lbs.");
			System.out.println("B: 1 WUP (Woodchuck Unit of Power).");
			System.out.println("C: Enough to keep a small woodchuck home warm for the winter.");
			System.out.println("D: I have no idea and this is a silly question.\n");
			
			
			try {
				answer = reader.readLine();
			} catch (IOException ioe) {
				System.out.println("IO error trying to read your answer!");
				System.exit(1);
			}
	
			if(answer.equalsIgnoreCase("B")){
				System.out.println("Correct! A woodchuck would be able to exert 1 WUP!\n");
				break;
			} else if(((answer.equalsIgnoreCase("A")) | (answer.equalsIgnoreCase("C")) | (answer.equalsIgnoreCase("D"))) == false){
				System.out.println("'" + answer + "' is not a valid answer! Try again.\n");
			} else {
				System.out.println("Sorry, wrong answer! Try again!\n");
			}
		}
    }
}
