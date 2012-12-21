import javazoom.jl.player.Player;
import java.io.FileInputStream;

public class TestPlay
{
    public static void main(String[]args)
    {
        try
        {
            FileInputStream fis=new FileInputStream("C:\\Users\\David\\Desktop\\14_Waka_Flocka_Flame_-_Hard_in_the_Paint_Remix_Feat-Djleak.mp3");
            Player playMp3=new Player(fis);

            playMp3.play();
			//Pause for 4 seconds
            Thread.sleep(4000);
			playMp3.stop();
			//Pause for 4 seconds
            Thread.sleep(4000);
			playMp3.play();
        }
        catch(Exception e)
        {
            System.out.println(e);
        }
    }
}

