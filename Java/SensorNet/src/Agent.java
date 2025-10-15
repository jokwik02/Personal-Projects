import java.util.ArrayList;
import java.util.Random;
import java.util.Stack;
import java.util.HashSet;

/**
 * An Agent class to be used in a network that has a position and a list of events it knows about.
 * It randomly moves around a network spreading and gaining information about events
 * @version 1.0
 * @author Arvid Asplund (tfy19aad), Karl Furberg Schwartz (tfy18kfz), Joakim Wiksten (c22jwn)
 */

public class Agent implements Message{
    private Node currentNode;
    private Node nextNode;
    private HashSet<Node> visitedNodes = new HashSet<Node>();
    private Stack<Node> moveMemory = new Stack<Node>();
    private int iteration;
    private int maxSteps;
    private ArrayList<EventInfo> eventList;
    private Stack<Node> path = new Stack<Node>();

    /**
     * Constructor that creates an agent with a known position, the number of steps it has taken and a list which it
     * uses to store events
     * @param node
     * @param maxSteps
     */
    public Agent(Node node, int maxSteps) {
        this.currentNode = node;
        this.nextNode = node;
        iteration = 0;
        //moveMemory.push(node);
        path.push(node);
        //visitedNodes.add(node);
        eventList = new ArrayList<>();
        this.currentNode.enqueueMessage(this);
        this.maxSteps = maxSteps;
    }

    /**
     * method that moves the agent to the next node if it is first in the queue. It also merges the lists of events for
     * the node and the agent. If not first in queue it will put the agent in the queue for the next node.
     * @return
     */
    public boolean move() {
        if(iteration < maxSteps) {
            if (currentNode.getMessageQueue().peek() == this) {
                currentNode.getMessageQueue().remove();
                mergeLists();
                updateList();

                visitedNodes.add(currentNode);

                ArrayList<Node> neighbours = currentNode.getNeighbours();

                Random rand = new Random();

                while (!neighbours.isEmpty()) {
                    int randomIndex = rand.nextInt(neighbours.size());

                    nextNode = neighbours.remove(randomIndex);

                    if (!visitedNodes.contains(nextNode)) {
                        moveMemory.push(currentNode);
                        moveMemory.push(nextNode);
                        break;
                    }
                }

                if (!moveMemory.empty()) {
                    currentNode = moveMemory.pop();
                    currentNode.enqueueMessage(this);
                    path.push(currentNode);
                    iteration++;
                } else {
                    throw new IllegalArgumentException("Node has no neighbours");
                }
            }
        }else {
            if (currentNode.getMessageQueue().contains(this)) {
                currentNode.getMessageQueue().remove(this);
            }
            return true;
        }

        return false;
    }

    /**
     * method that merges the lists of events for the agent and the node. It prioritizes fewer steps to an event when
     * merging
     */
    public void mergeLists(){
        ArrayList<EventInfo> nodeList = currentNode.getEventList();
        for (int i = 0; i < this.eventList.size(); i++){
            EventInfo currentAgentEventinfo = this.eventList.get(i);
            boolean idFound = false;
            for (int j = 0; j < nodeList.size(); j++){
                EventInfo currentNodeEventInfo = nodeList.get(j);
                if(currentAgentEventinfo.getId() == currentNodeEventInfo.getId()){
                    idFound = true;
                    if(currentAgentEventinfo.getSteps() > currentNodeEventInfo.getSteps()){
                        if(currentNodeEventInfo.isEvent()){
                            EventInfo eventInfo = new EventInfo(currentNode, currentNodeEventInfo.getSteps(), currentNodeEventInfo.getId());
                            eventList.remove(i);
                            eventList.add(i, eventInfo);
                        } else {
                            EventInfo eventInfo = new EventInfo(currentNodeEventInfo.getNeighbour(), currentNodeEventInfo.getSteps(), currentNodeEventInfo.getId());
                            eventList.remove(i);
                            eventList.add(i, eventInfo);
                        }
                    } else if (currentAgentEventinfo.getSteps() < currentNodeEventInfo.getSteps()) {
                        EventInfo eventInfo = new EventInfo(currentAgentEventinfo.getNeighbour(), currentAgentEventinfo.getSteps(), currentAgentEventinfo.getId());
                        nodeList.remove(j);
                        nodeList.add(j, eventInfo);
                    }
                    j = nodeList.size() + 1;
                }
            }
            if (idFound == false){
                EventInfo eventInfo = new EventInfo(currentAgentEventinfo.getNeighbour(), currentAgentEventinfo.getSteps(), currentAgentEventinfo.getId());
                nodeList.add(eventInfo);
            }
        }
        for (int i = 0; i < nodeList.size(); i++){
            EventInfo currentNodeEventInfo = nodeList.get(i);
            boolean idFound = false;
            for (int j = 0; j < eventList.size(); j++){
                EventInfo currentAgentEventInfo = eventList.get(j);
                if(currentAgentEventInfo.getId() == currentNodeEventInfo.getId()){
                    idFound = true;
                }
            }
            if (idFound == false){
                if(currentNodeEventInfo.isEvent()){
                    EventInfo eventInfo = new EventInfo(currentNode, currentNodeEventInfo.getSteps(), currentNodeEventInfo.getId());
                    eventList.add(eventInfo);
                } else {
                    EventInfo eventInfo = new EventInfo(currentNodeEventInfo.getNeighbour(), currentNodeEventInfo.getSteps(), currentNodeEventInfo.getId());
                    eventList.add(eventInfo);
                }
            }
        }
    }

    /**
     * updates the agents event list and sets all the neighbours for the events to the current node and adds that the agent
     * has taken one step.
     */
    public void updateList(){
        for(int i = 0; i < eventList.size(); i++){
            eventList.get(i).setNeighbour(currentNode);
            eventList.get(i).addStep();
        }
    }

    /**
     * Returns the node that the agent resides in.
     * @return
     */
    public Node getCurrentNode() {
        return currentNode;
    }

    /**
     * Used for testing, sets the Arraylist containing EventLists as the eventlist for the agent.
     * @param eventList
     */
    public void setEventList(ArrayList<EventInfo> eventList){
        this.eventList = eventList;
    }

    /**
     * Returns the event list for the agent (only for testing).
     * @return
     */
    public ArrayList<EventInfo> getEventList(){
        return this.eventList;
    }

    /**
     * returns a copy of the path for the agent, used for simulation.
     * @return
     */
    public Stack<Node> getPath() {
        return (Stack<Node>) path.clone();
    }
}
