import org.junit.jupiter.api.Test;

import javax.swing.plaf.basic.BasicInternalFrameTitlePane;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;

import static org.junit.jupiter.api.Assertions.*;


/**
 * Test the Network class and all of its methods.
 * @version 1.0
 * @author Arvid Asplund (tfy19aad)
 */
public class NetworkTest {
    private final static int MAX_RADIUS = 15;
    private final static int NUM_QUERY_NODES = 4;
    private final static double EVENT_PROBABILITY = 0.0001;
    private final static double AGENT_PROBABILITY = 0.5;
    private final static int AGENT_MAX_STEPS = 50;
    private final static int QUERY_MAX_STEPS = 45;
    private final static int QUERY_RESEND_MULTIPLIER = 8;

    /**
     * Test files without initial int.
     */
    @Test
    public void testFileWithNoInitialInt() {
        // Test file with no int
        String file1 = """
                1,1
                2,2
                3,3
                """;
        IllegalArgumentException exception1 = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file1), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. First line must be an integer.", exception1.getMessage());

        // Test file where the first number is a not an int
        String file2 = """
                2.2
                1,1
                2,2
                3,3
                """;
        IllegalArgumentException exception2 = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file2), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. First line must be an integer.", exception2.getMessage());
    }

    /**
     * Test files where number of nodes and positions does not match.
     */
    @Test
    public void testFileWithWrongNumberOfPosition() {
        // Test when the file has more nodes than positions
        String file1 = """
                4
                1,1
                2,2
                3,3
                """;
        IllegalArgumentException exception1 = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file1), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. Number of nodes doesn't match the number of given positions.", exception1.getMessage());

        // Test when the file has more positions than nodes
        String file2 = """
                2
                1,1
                2,2
                3,3
                """;
        IllegalArgumentException exception2 = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file2), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. Number of nodes doesn't match the number of given positions.", exception2.getMessage());
    }

    /**
     * Test that error is thrown if two nodes has the same position.
     */
    @Test
    public void testFileWithTwoEqualNode() {
    String file = """
                2
                1,1
                1,1
                """;

        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. Two nodes with the same position.", exception.getMessage());
    }


    /**
     * Test that error is thrown if a node has no neighbours.
     */
    @Test
    public void testFileWithNodeWithoutNeighbour() {
        String file = """
                3
                0,0
                1,1
                100,100
                """;

        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. Node without neighbours.", exception.getMessage());
    }

    /**
     * Test files where the positions are given with the wrong format.
     */
    @Test
    public void testFileWithWrongFormatOnPosition() {
        // Test when the position is not divided by ","
        String file1 = """
                3
                1;1
                2;2
                3;3
                """;
        IllegalArgumentException exception1 = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file1), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. Coordinates are not divided by ','", exception1.getMessage());

        // Test when one coordinate is missing
        String file2 = """
                3
                 ,1
                2, 
                3, 
                """;
        IllegalArgumentException exception2 = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file2), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. Coordinates not given as integers.", exception2.getMessage());

        // Test when coordinate is not given as int
        String file3 = """
                3
                1,1.0
                2,2
                3,3
                """;
        IllegalArgumentException exception3 = assertThrows(IllegalArgumentException.class, () -> {
            new Network(new Scanner(file3), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        });
        assertEquals("Invalid file format. Coordinates not given as integers.", exception3.getMessage());
    }


    /**
     * Test so that the findNeighbours() finds the rights neighbours. Want to fins neighbours to a node on
     * position (20,20). Node 2-9 should be neighbours, the rest not
     */
    @Test
    public void testFindNeighbours() {
        String file = """
                17
                20,20
                5,20
                35,20
                20,5
                20,35
                10,10
                30,30
                10,30
                30,10
                35,35
                5,5
                5,35
                35,5
                0,20
                40,20
                20,0
                20,40
                """;

        Network network = new Network(new Scanner(file), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);

        network.findNeighbours();

        ArrayList<Node> neighbours = network.getNodes().get(0).getNeighbours();

        assertEquals(8, neighbours.size());

        assertEquals(new Position(5, 20), neighbours.get(0).getPosition());
        assertEquals(new Position(35, 20), neighbours.get(1).getPosition());
        assertEquals(new Position(20, 5), neighbours.get(2).getPosition());
        assertEquals(new Position(20, 35), neighbours.get(3).getPosition());
        assertEquals(new Position(10, 10), neighbours.get(4).getPosition());
        assertEquals(new Position(30, 30), neighbours.get(5).getPosition());
        assertEquals(new Position(10, 30), neighbours.get(6).getPosition());
        assertEquals(new Position(30, 10), neighbours.get(7).getPosition());
    }

    /**
     * Test so that generateQuery does not fail when the idList is null, i.e. no events has been created yet.
     */
    @Test
    public void testGenerateQueryIdListNull(){
        String filename = "testNetwork.txt";
        Scanner s;
        try {
            s = new Scanner(new File(filename));
        } catch (FileNotFoundException ex) {
            throw new RuntimeException(ex);
        }

        Network network = new Network(s, MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);

        ArrayList<Integer> idList = new ArrayList<>();

        network.generateQuery(idList);
        assertEquals(0, network.getQueries().size());
    }

    /**
     * Test so that 4 queries are created and check so that they are created at the right nodes.
     */
    @Test
    public void testGenerateQuery(){
        String filename = "testNetwork.txt";
        Scanner s;
        try {
            s = new Scanner(new File(filename));
        } catch (FileNotFoundException ex) {
            throw new RuntimeException(ex);
        }

        Network network = new Network(s, MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);

        ArrayList<Integer> idList = new ArrayList<>();
        idList.add(1);

        network.generateQuery(idList);

        ArrayList<Node> queryNodes = network.getQueryNodes();
        ArrayList<Node> nodes = network.getNodes();

        assertEquals(4, queryNodes.size());

        int queriesInQueue = 0;
        for (int i = 0; i < nodes.size(); i++) {
            Node node = nodes.get(i);
            Queue<Message> queue = node.getMessageQueue();
            if (queue.size() != 0) {
                queriesInQueue += 1;
                queryNodes.remove(node);
            }
        }

        assertEquals(0, queryNodes.size());
        assertEquals(4, queriesInQueue);
    }


    /**
     * Test so that the probability of an event happening works. 0 events should be created when the probability is 0,
     * when the probability is 1 it should be one event on each node and if the probability is between we check so that
     * the number of events are about what we expect with some confidence interval.
     */
    @Test
    public void testGenerateEventEventProbability() {
        String filename = "testNetwork.txt";
        Scanner s;

        double[] eventProbability = new double[3];
        eventProbability[0] = 0;
        eventProbability[1] = 0.01;
        eventProbability[2] = 1;

        int n = 50;
        int size = 50*50;

        for (int i = 0; i < 3; i++) {
            int totalEvents = 0;
            for (int j = 0; j < n; j++) {
                try {
                    s = new Scanner(new File(filename));
                } catch (FileNotFoundException ex) {
                    throw new RuntimeException(ex);
                }

                Network network = new Network(s, MAX_RADIUS, NUM_QUERY_NODES, eventProbability[i], AGENT_PROBABILITY,
                        AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);

                size = network.getNodes().size();

                network.generateEvent(0);

                for (int k = 0; k < size; k++) {
                    totalEvents += network.getNodes().get(k).getEventList().size();
                }
            }

            if (i == 0) {
                assertEquals(0, totalEvents);
            } else if (i == 1) {
                boolean inConfInterval = (n*size*eventProbability[i]*0.95 < totalEvents && totalEvents < n*size*eventProbability[i]*1.05);
                assertTrue(inConfInterval);
            } else {
                assertEquals(n*size, totalEvents);
            }
        }
    }

    /**
     * Test so that the probability of an agent happening works. By setting the probability of an event to 1, we now
     * that one event will be created at each node. 0 agents should be created when the probability is 0,
     * when the probability is 1 it should be one agent on each node and if the probability is between we check so that
     * the number of agents are about what we expect with some confidence interval.
     */
    @Test
    public void testGenerateEventAgentProbability() {
        String filename = "testNetwork.txt";
        Scanner s;

        double[] agentProbability = new double[3];
        agentProbability[0] = 0;
        agentProbability[1] = 0.5;
        agentProbability[2] = 1;

        int eventProbability = 1;

        int n = 50;
        int size = 50*50;

        for (int i = 0; i < 3; i++) {
            int totalAgents = 0;
            for (int j = 0; j < n; j++) {
                try {
                    s = new Scanner(new File(filename));
                } catch (FileNotFoundException ex) {
                    throw new RuntimeException(ex);
                }

                Network network = new Network(s, MAX_RADIUS, NUM_QUERY_NODES, eventProbability, agentProbability[i],
                        AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);

                network.generateEvent(0);

                for (int k = 0; k < size; k++) {
                    totalAgents += network.getNodes().get(k).getMessageQueue().size();
                }
            }

            if (i == 0) {
                assertEquals(0, totalAgents);
            } else if (i == 1) {
                boolean inConfInterval = (n*size*agentProbability[i]*0.95 < totalAgents && totalAgents < n*size*agentProbability[i]*1.05);
                assertTrue(inConfInterval);
            } else {
                assertEquals(n*size, totalAgents);
            }
        }
    }

    /**
     * Test that getNodes returns all the nodes in the network.
     */
    @Test
    public void testGetNodes() {
        String file = """
                3
                1,1
                2,2
                3,3
                """;

        Network network = new Network(new Scanner(file), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);

        ArrayList<Node> nodes = network.getNodes();

        assertEquals(3, nodes.size());
        for(int i = 0; i < nodes.size(); i++) {
            assertEquals(new Node(new Position(i+1,i+1)), nodes.get(i));
        }
    }

    /**
     * Test that getAgents returns all the agents in the network.
     */
    @Test
    public void testGetAgents() {
        String file1 = """
                3
                1,1
                2,2
                3,3
                """;
        Network network = new Network(new Scanner(file1), MAX_RADIUS, NUM_QUERY_NODES, 1, 1,
                AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        assertEquals(0, network.getAgents().size());
        network.generateEvent(1);
        assertEquals(3, network.getAgents().size());
    }

    /**
     * Test that getQueries returns all the queries in the network.
     */
    @Test
    public void testGetQueries() {
        String file = """
                4
                1,1
                2,2
                3,3
                4,4
                """;
        Network network = new Network(new Scanner(file), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
        ArrayList<Integer> idList = new ArrayList<>();
        idList.add(1);
        assertEquals(0, network.getQueries().size());
        network.generateQuery(idList);
        assertEquals(NUM_QUERY_NODES, network.getQueries().size());
    }

    /**
     * Test that getQueryNodes returns 4 nodes and that the there is one node left in nodes if we remove all the
     * queryNodes.
     */
    @Test
    public void testGetQueryNodes() {
        String file = """
                5
                1,1
                2,2
                3,3
                4,4
                5,5
                """;
        Network network = new Network(new Scanner(file), MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                AGENT_MAX_STEPS, QUERY_MAX_STEPS, QUERY_RESEND_MULTIPLIER);

        ArrayList<Node> nodes = network.getNodes();
        ArrayList<Node> queryNodes = network.getQueryNodes();

        assertEquals(NUM_QUERY_NODES, queryNodes.size());

        nodes.removeAll(queryNodes);

        assertEquals(1, nodes.size());
    }
}

