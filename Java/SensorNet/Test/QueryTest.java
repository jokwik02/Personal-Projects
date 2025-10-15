import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.util.ArrayList;
import java.util.Scanner;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Class for testing the Query class
 * @version 1.0
 * @author Jonatan Westling (c22jwg), Joakim Wiksten (c22jwn)
 */
public class QueryTest {
    Query q;
    Node n;
    private final static int MAX_RADIUS = 15;
    private final static int NUM_QUERY_NODES = 4;
    private final static double EVENT_PROBABILITY = 0.0001;
    private final static double AGENT_PROBABILITY = 0.5;
    private final static int AGENT_MAX_STEPS = 50;
    private final static int QUERY_MAX_STEPS = 45;
    private final static int QUERY_RESEND_MULTIPLIER = 8;

    /**
     * Create a query q and a node n before each test
     */
    @BeforeEach
    void createQuery(){
        n = new Node(new Position(1, 1));
        q = new Query(n, 34, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
    }

    /**
     *  Check that query is not null
     */
    @Test
    void queryNotNull(){
        assertNotNull(q);
    }

    /**
     * Check that the node has correct position
     */
    @Test
    void correctPos(){
        Node current = q.getCurrentNode();
        Position pos =new Position(1, 1);
        assertEquals(pos, current.getPosition());
    }

    /**
     * Check that the robot moves to its neighbour
     */
    @Test
    void moveRandom(){
        ArrayList<Node> neighbours = new ArrayList<>();
        Position pos = new Position(1, 2);
        Node n1 = new Node(pos);
        neighbours.add(n1);
        n.setNeighbours(neighbours);
        q.move();
        assertEquals(n1, q.getCurrentNode());
    }

    /**
     * Tests that a query can find an event and then follow the path back to the starting node. It is both tested by
     * assertequals() and by checking the query's position that is printed out, to confirm the correct path was taken
     */
    @Test
    public void testFindEvent(){
        Node n1 = new Node(new Position(0,0));
        Node n2 = new Node(new Position(1,0));
        Node n3 = new Node(new Position(2,0));
        Node n4 = new Node(new Position(3,0));
        ArrayList<Node> n1neigh = new ArrayList<>();
        ArrayList<Node> n2neigh = new ArrayList<>();
        ArrayList<Node> n3neigh = new ArrayList<>();
        ArrayList<Node> n4neigh = new ArrayList<>();
        n1neigh.add(n2);
        n1.setNeighbours(n1neigh);
        n2neigh.add(n1);
        n2neigh.add(n3);
        n2.setNeighbours(n2neigh);
        n3neigh.add(n2);
        n3neigh.add(n4);
        n3.setNeighbours(n3neigh);
        n4neigh.add(n3);
        n4.setNeighbours(n4neigh);

        int id = n3.createEvent(10);

        Query q = new Query(n1, id, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        while (!q.isHasReturned()){
            System.out.println("X:" + q.getCurrentNode().getPosition().getX() + " Y: " + q.getCurrentNode().getPosition().getY());
            q.move();
        }
        assertEquals(q.getCurrentNode(), n1);
    }
    /**
     * Tests that a query can find a path to an event and the event and then follow the path back to the starting node.
     * It is both tested by assertequals() and by checking the query's position that is printed out,
     * to confirm the correct path was taken
     */
    @Test
    public void testFindPathAndEvent(){
        Node n1 = new Node(new Position(0,0));
        Node n2 = new Node(new Position(1,0));
        Node n3 = new Node(new Position(2,0));
        Node n4 = new Node(new Position(3,0));
        Node n5 = new Node(new Position(2,1));
        ArrayList<Node> n1neigh = new ArrayList<>();
        ArrayList<Node> n2neigh = new ArrayList<>();
        ArrayList<Node> n3neigh = new ArrayList<>();
        ArrayList<Node> n4neigh = new ArrayList<>();
        ArrayList<Node> n5neigh = new ArrayList<>();
        n1neigh.add(n2);
        n1.setNeighbours(n1neigh);
        n2neigh.add(n1);
        n2neigh.add(n3);
        n2.setNeighbours(n2neigh);
        n3neigh.add(n2);
        n3neigh.add(n4);
        n3neigh.add(n5);
        n3.setNeighbours(n3neigh);
        n4neigh.add(n3);
        n4.setNeighbours(n4neigh);
        n5neigh.add(n3);
        n5.setNeighbours(n5neigh);

        int id = n4.createEvent(10);

        Agent a = new Agent(n4, AGENT_MAX_STEPS);



        Query q = new Query(n1, id, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        while(!q.isHasReturned()){
            System.out.println("Query: X:" + q.getCurrentNode().getPosition().getX() + " Y: " + q.getCurrentNode().getPosition().getY());
            q.move();
            if(!q.isHasReturned()) {
                a.move();
            }

        }
        assertEquals(q.getCurrentNode(), n1);
    }

    /**
     * Test to check that the find path method is working correctly
     */
    @Test
    void findPathTest(){
        //create a network where all nodes are neighbours
        String file = """
                10
                100,10
                200,10
                100,20
                200,20
                100,30
                200,30
                100,40
                200,40
                100,50
                200,50
                """;

        Network network = new Network(new Scanner(file), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY, QUERY_MAX_STEPS, AGENT_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        //find neighbours in network
        network.findNeighbours();
        ArrayList<Node> nodes = network.getNodes();
        //create agent
        int id = nodes.get(9).createEvent(10);
        Agent a = new Agent(nodes.get(9), AGENT_MAX_STEPS);
        //move agent three times and create nodes for each
        a.move();
        Node node1 = a.getCurrentNode();
        a.move();
        Node node2 = a.getCurrentNode();
        a.move();
        Node node3 = a.getCurrentNode();
        Node qStart = a.getCurrentNode();
        //move tha agent away so query do not get stuck in queue
        a.move();

        Query q = new Query(qStart, id, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        //check so query does have correct pos each move
        assertEquals(node3.getPosition().getX(), q.getCurrentNode().getPosition().getX());
        assertEquals(node3.getPosition().getY(), q.getCurrentNode().getPosition().getY());
        q.move();

        assertEquals(node2.getPosition().getX(), q.getCurrentNode().getPosition().getX());
        assertEquals(node2.getPosition().getY(), q.getCurrentNode().getPosition().getY());
        q.move();

        assertEquals(node1.getPosition().getX(), q.getCurrentNode().getPosition().getX());
        assertEquals(node1.getPosition().getY(), q.getCurrentNode().getPosition().getY());
    }
}
