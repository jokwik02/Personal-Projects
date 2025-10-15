import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Class that test the EventInfo class
 * @version 1.0
 * @author Joakim Wiksten (c22jwn)
 */
public class EventInfoTest {

    /**
     * Test getEvent method to get the event from EventInfo
     */
    @Test
    public void testGetEventFromEvent(){
        Event e = new Event(100, new Position(1,2), 9);
        EventInfo info = new EventInfo(e,10);
        //check that the event info contains the event we added
        assertEquals(e, info.getEvent());
    }

    /**
     * Test getNeighbour from a event info that is holding the actual event,
     * should return null.
     */
    @Test
    public void testGetNeighbourFromEvent(){
        Event e = new Event(100, new Position(1,2), 9);
        EventInfo info = new EventInfo(e,10);
        assertNull(info.getNeighbour());
    }

    /**
     * Test that getNeighbour returns the neighbour actual neighbour
     */
    @Test
    public void testGetNeighbourFromNeighbour(){
        Node neighbour = new Node(new Position(0,0));
        EventInfo info = new EventInfo(neighbour,10,10);
        assertEquals(neighbour, info.getNeighbour());
    }

    /**
     *Test that the method getEvent returns null when the event info contains a neighbour.
     * This should return null.
     */
    @Test
    public void testGetEventFromNeighbour(){
        Node neighbour = new Node(new Position(0,0));
        EventInfo info = new EventInfo(neighbour,10,10);
        assertNull(info.getEvent());
    }

    /**
     * Test that getSteps from event return correct amount of steps.
     */
    @Test
    public void testGetStepsFromEvent(){
        Event e = new Event(100, new Position(1,2), 9);
        EventInfo info = new EventInfo(e,10);
        assertEquals(info.getSteps(), 0);
    }

    /**
     * Test that getSteps from event return correct amount of steps.
     */
    @Test
    public void testGetStepsFromNeighbour(){
        Node n = new Node(new Position(0,0));
        EventInfo info = new EventInfo(n, 10,10);
        assertEquals(info.getSteps(), 10);
    }

    /**
     * Test that the method isEvent works, should return true because the
     * created eventinfo contains the event.
     */
    @Test
    public void testIsEvent(){
        Event e = new Event(100, new Position(1,2), 9);
        EventInfo info = new EventInfo(e,10);
        assertTrue(info.isEvent());
    }

    /**
     * Test that the method isEvent works, should return false because the
     * created eventinfo contains a neighbour and not an event.
     */
    @Test
    public void testIsNeighbour(){
        Node n = new Node(new Position(0,0));
        EventInfo info = new EventInfo(n,1000,10);
        assertFalse(info.isEvent());
    }

    /**
     * Test that checks that the getId works correctly
     */
    @Test
    public void testGetId(){
        Node n = new Node(new Position(0,0));
        Event e = new Event(100, new Position(1,2), 9);
        EventInfo info1 = new EventInfo(e, 10);
        EventInfo info2 = new EventInfo(n, 10, 100);
        assertEquals(info1.getId(), 10);
        assertNotEquals(info2.getId(), 1000);
    }

    /**
     * Tests that two identical events are the same
     */
    @Test
    public void testEquals(){
        Node n = new Node(new Position(0,0));
        EventInfo i1 = new EventInfo(n, 10, 1);
        EventInfo i2 = new EventInfo(n, 10, 1);
        assertEquals(i1, i2);
        i1 = new EventInfo(new Event(0,new Position(0,0),0), 0);
        i2 = new EventInfo(new Event(0,new Position(0,0),0), 0);
        assertEquals(i1, i2);
    }
}
