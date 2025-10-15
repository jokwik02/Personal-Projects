import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Test class for the Event class.
 * @version 1.0
 * @author Jonatan Westling (c22jwg)
 */
public class EventTest {
    /**
     * Test that checks that getId method is working correctly
     */
    @Test
    void getIdTest(){
        Position pos = new Position(1, 1);
        Event event = new Event(723, pos, 6);
        int id = event.getId();
        assertEquals(723, id);
    }

    /**
     * Test that checks that getPos method is working correctly
     */
    @Test
    void getPosTest(){
        Position pos = new Position(45, 61);
        Event event = new Event(723, pos, 6);
        Position pos1 = event.getPosition();
        assertEquals(pos, pos1);
    }

    /**
     * Test to check that getTime method is working correctly
     */
    @Test
    void getTimeTest(){
        Position pos = new Position(34, 53);
        Event event = new Event(3, pos, 54);
        int time = event.getTime();
        assertEquals(54, time);
    }
}
