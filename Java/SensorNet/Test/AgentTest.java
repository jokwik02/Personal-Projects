import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.HashSet;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Class for testing the Agent Class
 * @version 1.0
 * @author Joakim Wiksten (c22jwn)
 */
public class AgentTest {
    private final static int MAX_RADIUS = 15;
    private final static int NUM_QUERY_NODES = 4;
    private final static double EVENT_PROBABILITY = 0.0001;
    private final static double AGENT_PROBABILITY = 0.5;
    private final static int AGENT_MAX_STEPS = 50;
    private final static int QUERY_MAX_STEPS = 45;
    private final static int QUERY_RESEND_MULTIPLIER = 8;

    /**
     * Tests if the getCurrentNode() method returns the correct node
     */
    @Test
    public void testGetCurrentNode(){
        Node n = new Node(new Position(0,0));
        Agent a = new Agent(n, AGENT_MAX_STEPS);
        assertEquals(a.getCurrentNode(), n);
    }

    /**
     * Tests if the agent moves to one of its neighbours when move() is called
     */
    @Test
    public void testAgentMoves(){
        ArrayList<Node> neighbours = new ArrayList<>();
        Node n2 = new Node( new Position(0, 0));
        Node n = new Node(new Position(2,2));

        neighbours.add(n2);
        n.setNeighbours(neighbours);

        Agent a  = new Agent(n, AGENT_MAX_STEPS);
        a.move();

        assertNotEquals(a.getCurrentNode(), n);
        assertEquals(a.getCurrentNode(), n2);
    }

    /**
     * Private method that checks if two Lists of Eventinfo are equal
     * @param l1 List to compare
     * @param l2 List to compare
     * @return true if the lists are equal, otherwise false
     */
    private boolean eventListsAreEqual(ArrayList<EventInfo> l1, ArrayList<EventInfo> l2) {
        if (l1.size() != l2.size()) {
            System.out.println("Sizes are not equal");
            return false;
        }

        int amountOfEquals = 0;
        for (EventInfo element1 : l1) {
            for(EventInfo element2: l2) {
                if(element1.equals(element2)){
                    amountOfEquals++;
                }
            }

        }
        if(amountOfEquals != l1.size()){
            System.out.println("Elements are not equal");
            return false;
        }

        return true;
    }

    /**
     * Tests if a merge of two Eventinfo lists only containing neighbours, results in the correct lists
     */
    @Test
    public void testMergeListsOfNeighbours(){
        Node n = new Node(new Position(2,2));
        EventInfo i1 = new EventInfo(new Node(new Position(0,0)), 10,100);
        EventInfo i2 = new EventInfo(new Node(new Position(1,1)), 3, 101);
        EventInfo i3 = new EventInfo(new Node(new Position(2,1)), 2, 102);
        EventInfo i4 = new EventInfo(new Node(new Position(0,0)), 10,99);
        EventInfo i5 = new EventInfo(new Node(new Position(1,1)), 7, 101);
        EventInfo i6 = new EventInfo(new Node(new Position(2,1)), 1, 102);
        n.addEvent(i1);
        n.addEvent(i2);
        n.addEvent(i3);
        Agent agent = new Agent(n, AGENT_MAX_STEPS);

        ArrayList<EventInfo> agentList = new ArrayList<>();
        agentList.add(i4);
        agentList.add(i5);
        agentList.add(i6);

        agent.setEventList(agentList);

        agent.mergeLists();

        ArrayList<EventInfo> completeTable = new ArrayList<>();
        completeTable.add(i4);
        completeTable.add(i1);
        completeTable.add(i2);
        completeTable.add(i6);

        assertTrue(eventListsAreEqual(completeTable, agent.getCurrentNode().getEventList()));
        assertTrue(eventListsAreEqual(completeTable, agent.getEventList()));

    }

    /**
     * Tests if a merge of two Eventinfo lists containing both neighbours and Events, results in the correct lists
     */
    @Test
    public void testMergeListsOfEvents(){
        Node n = new Node(new Position(2,2));
        EventInfo i1 = new EventInfo(new Node(new Position(0,0)), 10,100);
        EventInfo i2 = new EventInfo(new Node(new Position(1,1)), 3, 101);
        EventInfo i3 = new EventInfo(new Node(new Position(2,1)), 2, 102);
        EventInfo i4 = new EventInfo(new Event(103, new Position(0,0), 12), 103);
        EventInfo i5 = new EventInfo(new Event(100, new Position(0,0), 12), 100);
        EventInfo i6 = new EventInfo(new Event(101, new Position(0,0), 12), 101);
        n.addEvent(i4);
        n.addEvent(i5);
        n.addEvent(i6);
        Agent agent = new Agent(n, AGENT_MAX_STEPS);

        ArrayList<EventInfo> agentList = new ArrayList<>();
        agentList.add(i1);
        agentList.add(i2);
        agentList.add(i3);

        agent.setEventList(agentList);

        agent.mergeLists();

        ArrayList<EventInfo> completeNodeTable = new ArrayList<>();
        completeNodeTable.add(i3);
        completeNodeTable.add(i4);
        completeNodeTable.add(i5);
        completeNodeTable.add(i6);

        ArrayList<EventInfo> completeAgentTable = new ArrayList<>();
        completeAgentTable.add(new EventInfo(n, 0, 101));
        completeAgentTable.add(new EventInfo(n, 0, 100));
        completeAgentTable.add(new EventInfo(n, 0, 103));
        completeAgentTable.add(i3);

        assertTrue(eventListsAreEqual(completeNodeTable, agent.getCurrentNode().getEventList()));
        assertTrue(eventListsAreEqual(completeAgentTable, agent.getEventList()));

    }

    /**
     * Tests if a merge of two Eventinfo lists where one is empty, results in the correct lists
     */
    @Test
    public void testMergeListsEmptyList(){
        Node n = new Node(new Position(2,2));
        EventInfo i1 = new EventInfo(new Node(new Position(0,0)), 10,100);
        EventInfo i5 = new EventInfo(new Event(100, new Position(0,0), 12), 99);
        EventInfo i6 = new EventInfo(new Event(101, new Position(0,0), 12), 101);
        n.addEvent(i1);
        n.addEvent(i5);
        n.addEvent(i6);

        Agent agent = new Agent(n, AGENT_MAX_STEPS);

        agent.mergeLists();

        ArrayList<EventInfo> completeNodeTable = new ArrayList<>();
        completeNodeTable.add(i1);
        completeNodeTable.add(i5);
        completeNodeTable.add(i6);

        ArrayList<EventInfo> completeAgentTable = new ArrayList<>();
        completeAgentTable.add(i1);
        completeAgentTable.add(new EventInfo(n, 0 ,99));
        completeAgentTable.add(new EventInfo(n, 0 ,101));


        assertTrue(eventListsAreEqual(completeNodeTable, agent.getCurrentNode().getEventList()));
        assertTrue(eventListsAreEqual(completeAgentTable, agent.getEventList()));

    }

    /**
     * Tests that the updateList() method updates the agents eventlist correctly
     */
    @Test
    public void testUpdateList(){
        Node n = new Node(new Position(2,2));
        EventInfo i1 = new EventInfo(new Node(new Position(0,0)), 10,100);
        EventInfo i2 = new EventInfo(new Node(new Position(1,1)), 3, 101);
        EventInfo i3 = new EventInfo(new Node(new Position(2,1)), 2, 102);

        ArrayList<EventInfo> agentList = new ArrayList<>();
        agentList.add(i1);
        agentList.add(i2);
        agentList.add(i3);

        Agent a = new Agent(n, AGENT_MAX_STEPS);
        a.setEventList(agentList);
        a.updateList();

        EventInfo i4 = new EventInfo(n, 11,100);
        EventInfo i5 = new EventInfo(n, 4, 101);
        EventInfo i6 = new EventInfo(n, 3, 102);

        ArrayList<EventInfo> correctList = new ArrayList<>();
        correctList.add(i4);
        correctList.add(i5);
        correctList.add(i6);

        assertTrue(eventListsAreEqual(correctList, a.getEventList()));
    }
}
