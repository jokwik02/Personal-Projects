import java.awt.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.jar.JarEntry;
import javax.swing.*;

/**
 * GUI that is used to simulate the rumor routing algorithm. Show the agents and the queries position and paths and
 * choose what should happen during each iteration of the simulation.
 * @version 1.0
 * @author Arvid Asplund (tfy19aad)
 */
public class Simulation {
    private JFrame frame;
    private Network network;
    private JTextArea textArea = new JTextArea(1, 10);

    private final static int MAX_RADIUS = 15;
    private final static int NUM_QUERY_NODES = 4;
    private final static double EVENT_PROBABILITY = 0.0001;
    private final static double AGENT_PROBABILITY = 0.5;
    private final static int AGENT_MAX_STEPS = 50;
    private final static int QUERY_MAX_STEPS = 45;
    private final static int QUERY_RESEND_MULTIPLIER = 8;

    /**
     * Loads a network and creates a graphical interface. Then run a given number of iteration, and during each
     * iteration it tries to create an event, moves the queries and agents and every 400 iteration new queries are
     * sent out.
     */
    public Simulation(String filename) {
        loadNetwork(filename);

        NetworkPanel panel = new NetworkPanel(network);

        JPanel textPanel = new JPanel();;
        textArea.setEditable(false);
        textArea.setText("Iteration: " + 0);
        textPanel.add(new JScrollPane(textArea), BorderLayout.LINE_END);

        createFrame(panel, textPanel);

        ArrayList<Integer> idList = new ArrayList<>();
        ArrayList<Agent> agentsToRemove = new ArrayList<>();
        ArrayList<Query> queriesToRemove = new ArrayList<>();

        double eventsFound = 0;
        double eventsSearched = 0;

        for (int i = 1; i < 10000; i++) {
            /*
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }*/
            textArea.setText("Iteration: " + i);

            idList.addAll(network.generateEvent(i));
            if (i % 400 == 0) {
                eventsSearched += network.getQueryNodes().size();
                network.generateQuery(idList);
            }

            ArrayList<Agent> agents = network.getAgents();
            for (int j = 0; j < agents.size(); j++) {
                Agent agent = agents.get(j);
                if (agent.move()) {
                    agents.remove(agent);
                }
            }

            ArrayList<Query> queries = network.getQueries();

            for (int j = 0; j < queries.size(); j++) {
                Query query = queries.get(j);
                if (query.move() || query.isHasReturned()) {
                    queriesToRemove.add(query);
                }
                if (query.hasReturned()) {
                    System.out.println(query.eventToString());
                    eventsFound++;
                }
            }

            agents.removeAll(agentsToRemove);
            queries.removeAll(queriesToRemove);
            agentsToRemove.clear();
            queriesToRemove.clear();

            panel.repaint();
        }

        System.out.println("Events created: " + idList.size());
        System.out.println("Events found: " + eventsFound);
        System.out.println("Queries sent: " + eventsSearched);
        System.out.println("Success rate: " + eventsFound/eventsSearched);
    }

    /**
     * Load the network from a text file consisting of the positions of the nodes.
     */
    private void loadNetwork(String filename) {
        Scanner s;

        try {
            s = new Scanner(new File(filename));
        } catch (FileNotFoundException ex) {
            throw new RuntimeException(ex);
        }

        network = new Network(s, MAX_RADIUS, NUM_QUERY_NODES, EVENT_PROBABILITY, AGENT_PROBABILITY,
                    QUERY_MAX_STEPS, AGENT_MAX_STEPS, QUERY_RESEND_MULTIPLIER);
    }

    /**
     * Creates a JFrame and adds a NetworkPanel to it.
     *
     * @param panel a panel to add to the frame
     * @param textPanel a panel to show iteration of the simulation
     */
    private void createFrame(NetworkPanel panel, JPanel textPanel) {
        panel.setBackground(Color.WHITE);
        frame = new JFrame();
        frame.setSize(975, 900);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(textPanel, BorderLayout.LINE_END);
        frame.add(panel, BorderLayout.CENTER);
        frame.setVisible(true);
    }

    /**
     * Main that runs the simulation
     *
     * @param args command-line arguments
     */
    public static void main(String[] args) {
        String filename;
        if (args.length == 0) {
            throw new IllegalArgumentException("No argument given");
        } else {
            filename = args[0];
        }

        new Simulation(filename);
    }
}