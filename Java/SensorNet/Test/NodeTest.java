
import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Class for testing the Node Class
 * @version 1.0
 * @author Joakim Wiksten (c22jwn)
 */
public class NodeTest {
    private final static int MAX_RADIUS = 15;
    private final static int NUM_QUERY_NODES = 4;
    private final static double EVENT_PROBABILITY = 0.0001;
    private final static double AGENT_PROBABILITY = 0.5;
    private final static int AGENT_MAX_STEPS = 50;
    private final static int QUERY_MAX_STEPS = 45;
    private final static int QUERY_RESEND_MULTIPLIER = 8;

    private Node getNode(){
        Position p = new Position(1,1);
        Node n = new Node(p);
        return n;
    }

    /**
     * Tests if getPosition() returns the correct position
     */
    @Test
    public void testGetPosition(){
        Position p = new Position(1,1);
        Node n = new Node(p);
        assertEquals(p, n.getPosition());
    }

    /**
     * Tests if set- and getNeighbours() sets and returns the correct neighbourlist
     */
    @Test
    public void testSetGetNeighbours(){
        ArrayList l = new ArrayList<>();
        for(int i = 0; i < 12; i++){
            Node node = getNode();
            l.add(node);
        }
        Node n = getNode();
        n.setNeighbours(l);
        assertEquals(l,n.getNeighbours());
    }

    /**
     * Tests if createEvent() returns the correct Id
     */
    @Test
    public void testCreateEventReturnsCorrectId(){
        Node n = getNode();
        int id = Objects.hash(10,n.getPosition().getX(), n.getPosition().getY());
        assertEquals(id,n.createEvent(10));
    }

    /**
     * Tests if getEventInfo() returns the correct eventinfo
     */
    @Test
    public void testGetEventInfo(){
        Node n = getNode();
        Event e = new Event(1000, n.getPosition(),10);
        EventInfo i = new EventInfo(e,e.getId());
        n.addEvent(i);
        assertEquals(i, n.getEventinfo(e.getId()));
    }

    /**
     * Tests if createEvent() creates the correct event
     */
    @Test
    public void testCreateEvent(){
        Node n = getNode();
        int id = n.createEvent(10);
        Event event = new Event(id, n.getPosition(),10);
        assertEquals(n.getEventinfo(id).getEvent().getId(), event.getId());
        assertEquals(n.getEventinfo(id).getEvent().getTime(), event.getTime());
        assertEquals(n.getEventinfo(id).getEvent().getPosition(), event.getPosition());
    }

    /**
     * Tests if set- and getEventList() sets and returns the correct Eventlist
     */
    @Test
    public void testGetSetEventList(){
        Node n = getNode();
        ArrayList<EventInfo> list = new ArrayList<>();
        for(int i = 0; i < 8; i++){
            Event e = new Event(10, n.getPosition(), 5);
            EventInfo eventInfo = new EventInfo(e, 10);
            list.add(i,eventInfo);
        }
        n.setEventList(list);
        assertEquals(n.getEventList(), list);
    }

    /**
     * tests that enqueueing an agent results in the correct agent being enqueued
     */
    @Test
    public void testEnqueueAgent(){
        Agent a = new Agent(new Node(new Position(0,0)), AGENT_MAX_STEPS);
        Node n = new Node(new Position(0,0));
        n.enqueueMessage(a);
        assertEquals(n.getMessageQueue().peek(), a);
    }

    /**
     * tests that enqueueing a query results in the correct query being enqueued
     */
    @Test
    public void testEnqueueQuery(){
        Node n = new Node(new Position(0,0));
        Query q = new Query(n,10,QUERY_MAX_STEPS,QUERY_RESEND_MULTIPLIER);
        n.enqueueMessage(q);
        assertEquals(n.getMessageQueue().peek(), q);
    }
}
