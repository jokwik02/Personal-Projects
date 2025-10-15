import java.awt.*;
import java.awt.geom.AffineTransform;
import java.util.ArrayList;
import javax.swing.*;
import java.util.Stack;

/**
 * An extension of JPanel that is used to be able to show how the agents and queries moves in the network.
 * @version 1.0
 * @author Arvid Asplund (tfy19aad)
 */
public class NetworkPanel extends JPanel {
    private Network network;
    private Position[] positions;
    private final static int SIZE = 6;

    /**
     * Initialize a panel that consist of the position of the nodes in a network.
     *
     * @param network the network that is being visualized.
     */
    public NetworkPanel(Network network) {
        this.network = network;
        ArrayList<Node> nodes = network.getNodes();

        positions = new Position[nodes.size()];

        for (int i = 0; i < network.getNodes().size(); i++) {
                positions[i] = nodes.get(i).getPosition();
        }
    }

    /**
     * Chooses which colors each node in the network should have. If it is an agent on the node it should be blue, if it
     * is a query it should be red. The lines represent the path the agent and queries has moved.
     *
     * @param g the <code>Graphics</code> object to protect
     */
    @Override
    public void paintComponent(Graphics g) {
        Graphics2D g2d = (Graphics2D) g.create();
        AffineTransform tx = new AffineTransform();
        tx.scale(2.5, 2.5);
        g2d.setTransform(tx);

        ArrayList<Agent> agents = network.getAgents();
        ArrayList<Query> queries = network.getQueries();

        g2d.setColor(Color.GRAY);
        for (Position position : positions) {
            g2d.fillOval(position.getX(), position.getY(), SIZE, SIZE);
        }

        for (int i = 0; i < agents.size(); i++) {
            Agent currentAgent = agents.get(i);
            Node currentNode = currentAgent.getCurrentNode();

            g2d.setColor(Color.BLUE);
            g2d.fillOval(currentNode.getPosition().getX(), currentNode.getPosition().getY(), SIZE, SIZE);

            g2d.setColor(Color.BLACK);
            Stack<Node> path = currentAgent.getPath();
            while (!path.isEmpty()) {
                Node tempNode = path.pop();
                if (path.isEmpty()) {
                    break;
                }
                Node nextNode = path.peek();
                g2d.drawLine(tempNode.getPosition().getX() + SIZE/2, tempNode.getPosition().getY() + SIZE/2,
                        nextNode.getPosition().getX() + SIZE/2, nextNode.getPosition().getY() + SIZE/2);
            }
        }

        for (int i = 0; i < queries.size(); i++) {
            Query currentQuery = queries.get(i);
            Node currentNode = currentQuery.getCurrentNode();

            g2d.setColor(Color.RED);
            g2d.fillOval(currentNode.getPosition().getX(), currentNode.getPosition().getY(), SIZE, SIZE);

            g2d.setColor(Color.BLACK);
            Stack<Node> path = currentQuery.getPath();
            while (!path.isEmpty()) {
                Node tempNode = path.pop();
                if (path.isEmpty()) {
                    break;
                }
                Node nextNode = path.peek();
                g2d.drawLine(tempNode.getPosition().getX() + SIZE/2, tempNode.getPosition().getY() + SIZE/2,
                        nextNode.getPosition().getX() + SIZE/2, nextNode.getPosition().getY() + SIZE/2);
            }
        }
        g2d.dispose();
    }
}