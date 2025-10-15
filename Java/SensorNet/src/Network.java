import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;
import java.util.Scanner;
import java.lang.Math.*;

/**
 * Network that have information about the nodes, queries and agents in the network. Used by the simulation class
 * to be able to control the other classes and what they should do in each iteration.
 * @version 1.0
 * @author Arvid Asplund (tfy19aad)
 */
public class Network {
    private ArrayList<Node> nodes = new ArrayList<>();;
    private ArrayList<Agent> agents = new ArrayList<>();
    private ArrayList<Query> queries = new ArrayList<>();
    private ArrayList<Node> queryNodes=  new ArrayList<>();
    private int numNodes;

    private final int max_radius;
    private final int num_query_nodes;
    private final double event_probability;
    private final double agent_probability;
    private final int query_max_steps;
    private final int agent_max_steps;
    private final int query_resend_multiplier;

    /**
     * Constructor method that initializes a Network by reading in the nodes positions from a Scanner object. r Also,
     * randomly chooses which nodes that should be able to generate queries.
     *
     * @param s a Scanner object representing the maze
     * @param max_radius the radius that is used to find neighbours
     * @param num_query_nodes the number of nodes that should be able to create queries
     * @param event_probability the probability of an event happening in each iteration
     * @param agent_probability the probability of an agent being created when a event happens
     * @param query_max_steps number of moves the queries can make without finding the event
     * @param agent_max_steps number of moves an agent can make
     * @param query_resend_multiplier constant that is used to know when to resend queries
     * @throws IllegalArgumentException if the file has the wrong format
     */
    public Network(Scanner s, int max_radius, int num_query_nodes, double event_probability, double agent_probability
    ,int query_max_steps, int agent_max_steps, int query_resend_multiplier) {
        this.max_radius = max_radius;
        this.num_query_nodes = num_query_nodes;
        this.event_probability = event_probability;
        this.agent_probability = agent_probability;
        this.query_max_steps = query_max_steps;
        this.agent_max_steps = agent_max_steps;
        this.query_resend_multiplier = query_resend_multiplier;

        try {
            numNodes = Integer.parseInt(s.nextLine());
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Invalid file format. First line must be an integer.");
        }

        for (int i = 0; i < numNodes; i++) {
            if (!s.hasNextLine()) {
                throw new IllegalArgumentException("Invalid file format. Number of nodes doesn't match the number of given positions.");
            }
            String line = s.nextLine();
            String[] parts = line.split(",");

            if (parts.length != 2) {
                throw new IllegalArgumentException("Invalid file format. Coordinates are not divided by ','");
            }

            try {
                  nodes.add(new Node(new Position(Integer.parseInt(parts[0]), Integer.parseInt(parts[1]))));
            } catch (NumberFormatException e) {
                throw new IllegalArgumentException("Invalid file format. Coordinates not given as integers.");
            }
        }

        if (s.hasNextLine()) {
            throw new IllegalArgumentException("Invalid file format. Number of nodes doesn't match the number of given positions.");
        }

        for (Node node1 : nodes) {
            for (Node node2 : nodes) {
                if (node1.equals(node2) && node1 != node2) {
                    throw new IllegalArgumentException("Invalid file format. Two nodes with the same position.");
                }
            }
        }

        // Choose nodes that creates queries
        Random rand = new Random();
        ArrayList<Node> nodesCopy = (ArrayList<Node>) nodes.clone();
        for (int i = 0; i < Math.min(numNodes, num_query_nodes); i++) {
            int randomIndex = rand.nextInt(nodesCopy.size());
            queryNodes.add(nodesCopy.remove(randomIndex));
        }

        findNeighbours();

        for (Node node : nodes) {
            if (node.getNeighbours().size() == 0) {
                throw new IllegalArgumentException("Invalid file format. Node without neighbours.");
            }
        }
    }

    /**
     * Find the neighbours to all nodes. A neighbour is defined as a node with position inside a circle with radius
     * max_radius.
     */
    public void findNeighbours() {
        for (int i = 0; i < numNodes; i++) {
            Node node = nodes.get(i);
            ArrayList<Node> neighbours = new ArrayList<>();
            int x = node.getPosition().getX();
            int y = node.getPosition().getY();

            for (int j = 0; j < numNodes; j++) {
                Node currentNode = nodes.get(j);
                int deltaX = x - currentNode.getPosition().getX();
                int deltaY = y - currentNode.getPosition().getY();

                int dist = deltaX * deltaX + deltaY * deltaY;

                if (0 < dist && dist <= max_radius * max_radius) {
                    neighbours.add(currentNode);
                }
            }

        node.setNeighbours(neighbours);
        }
    }

    /**
     * Generate queries at the nodes that have the right to create queries.
     *
     * @param idList list with id:s of all events that have happened.
     */
    public void generateQuery(ArrayList<Integer> idList) {
        Random random = new Random();
        if (idList.size() != 0) {
            for (int i = 0; i < queryNodes.size(); i++) {
                int randomIndex = random.nextInt(idList.size());

                int id = idList.get(randomIndex);

                queries.add(new Query(queryNodes.get(i), id, query_max_steps, query_resend_multiplier));
            }
        }
    }

    /**
     * Creates a number of events depending on the probability of an event happening at each time step.
     * Also creates agent with probability AGENT_PROBABILITY at the same nodes as the event.
     *
     * @param time the current iteration of the program
     * @return a list with all id:s that have been created
     */
    public ArrayList<Integer> generateEvent(int time) {
        Random random = new Random();

        ArrayList<Integer> idList = new ArrayList<>();
        HashSet<Node> hasCreatedEvent = new HashSet<Node>();

        for (int i = 0; i < nodes.size(); i++) {
            if (random.nextDouble() < event_probability) {
                int randomIndex = random.nextInt(nodes.size());
                Node node = nodes.get(randomIndex);

                while (hasCreatedEvent.contains(node)) {
                    randomIndex = random.nextInt(nodes.size());
                    node = nodes.get(randomIndex);
                }

                idList.add(node.createEvent(time));
                hasCreatedEvent.add(node);

                if (random.nextDouble() < agent_probability) {
                    agents.add(new Agent(node, agent_max_steps));
                }
            }
        }

        return idList;
    }

    /**
     * Returns a list with all the nodes
     *
     * @return list with all the nodes
     */
    public ArrayList<Node> getNodes() {
        return nodes;
    }

    /**
     * Returns a list with all the agents
     *
     * @return list with all the agents
     */
    public ArrayList<Agent> getAgents() {
        return agents;
    }

    /**
     * Returns a list with all the queries
     *
     * @return list with all the queries
     */
    public ArrayList<Query> getQueries() {
        return queries;
    }

    /**
     * Returns a list with all the nodes which can create queries
     *
     * @return list with all the nodes which can create queries
     */
    public ArrayList<Node> getQueryNodes() {
        return queryNodes;
    }
}