import java.util.*;

/**
 * Represents a query in a network of nodes. Used to search for a specific event using the event ID.
 * Implements three different move methods:
 * -Randomly moves through the network to search for nodes previously visited by the agent.
 * -Uses the followPath method to navigate to the node where the desired event is located.
 * -Utilizes the goBack method to return to the starting position and prints out the event information.
 * @version 1.0
 * @author Jonatan Westling, Arvid Asplund (tfy19aad), Karl Furberg Schwartz (tfy18kfz), Joakim Wiksten (c22jwn)
 */
public class Query implements Message{
    private Stack<Node> moveMemory;
    private Stack<Node> path = new Stack<>();
    private HashSet<Node> visitedNodes = new HashSet<Node>();
    private Node currentNode;
    private Node startingNode;
    private boolean hasReturned;
    private boolean foundPath;
    private boolean foundEvent;
    private int iteration;
    private int id;
    private int maxSteps;
    private int iteration_to_resend;
    private boolean hasBeenReset;
    private boolean hasBeenResend;
    private Event event;

    /**
     * Constructor that creates a query object
     * @param node The node where the query will start at.
     * @param id The id of the event that the query will search after
     */
    public Query(Node node, int id, int maxSteps, int multiplier) {
        this.currentNode = node;
        this.startingNode = node;
        this.id = id;
        this.moveMemory = new Stack<>();
        this.iteration = 0;
        this.foundEvent = false;
        this.foundPath = false;
        this.hasBeenReset = false;
        this.hasBeenResend = false;
        this.hasReturned = false;
        this.maxSteps = maxSteps;
        this.iteration_to_resend = maxSteps * multiplier;
        currentNode.enqueueMessage(this);
        moveMemory.push(node);
        visitedNodes.add(node);
    }

    /**
     * A private method that compares the query's wanted id to the id's of the events in the current node
     * eventlist. If a matching id is found, it will start moving the query accordingly
     * @return true if a matching id is found, otherwise false
     */
    private boolean compareId(){

        for (int i = 0; i < currentNode.getEventList().size(); i++) {
            EventInfo eventInfo = currentNode.getEventList().get(i);
            if(eventInfo.getId() == this.id) {
                if (eventInfo.isEvent()) {
                    this.foundEvent = true;
                    this.event = eventInfo.getEvent();
                    goBack();
                    return true;
                } else if (!this.foundPath) {
                    this.foundPath = true;
                    followPath();
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * This method will reset the query so the query starts over again from
     * its starting node and all info is reset.
     */
    public void resetQuery(){
        this.currentNode = this.startingNode;
        this.visitedNodes.clear();
        this.visitedNodes.add(startingNode);
        this.moveMemory.clear();
        this.path.clear();
        this.moveMemory.push(startingNode);
        this.foundEvent = false;
        this.foundPath = false;
        this.hasBeenReset = true;
    }

    /**
     * Method that checks if query is first in queue, then decides which type of the
     * move methods it will use. If event is found the method goBack will be called, otherwise
     * if a path is found the move method will call the method follow path. Lastly the randomly method will
     * be called. If query isn't first in the queue noting will happen this iteration.
     * @return true will delete query
     */
    public boolean move() {
        if(currentNode.getMessageQueue().peek() == this) {
            currentNode.getMessageQueue().remove();
            //if event already found go back
            if (foundEvent) {
                goBack();
                return false;
                //if path already found follow path
            } else if (foundPath) {
                followPath();
                return false;
            }
            //no path or event found, search randomly
            //check so iteration does not surpass 45
            if ((iteration < maxSteps)) {
                if (!this.compareId()) {
                    //continue to search randomly
                    randomly();
                    return false;
                }
            } else {
                //maximum steps reached do something
                currentNode.getMessageQueue().remove(this);
                if (!hasBeenReset){
                    resetQuery();
                    return false;
                } else {
                    return true;
                }
            }
        } else {
            // Do not update iteration if the query is in a queue
            if (!currentNode.getMessageQueue().contains(this)) {
                iteration++;
            }
            // Resend query
            if (iteration == iteration_to_resend && !hasBeenResend) {
                iteration = 0;
                currentNode.enqueueMessage(this);
                hasBeenResend = true;
            }
        }
        return false;
    }

    /**
     * Method that randomly selects the next node to move to within the network.
     * It adds current node to the visited ones. Then retrieves the neighbours of
     * the current node, then randomly selects an unvisited neighbour to move to.
     */
    private void randomly() {
        visitedNodes.add(currentNode);

        ArrayList<Node> neighbours = currentNode.getNeighbours();

        Random rand = new Random();

        while (!neighbours.isEmpty()) {
            int randomIndex = rand.nextInt(neighbours.size());

            Node nextNode = neighbours.remove(randomIndex);

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

    /**
     * Method that follows the path when query found a node that have information
     * about the specific if.
     */
    private void followPath() {
        this.compareId();
        //check if we found the node with the event
        if(!this.foundEvent) {
         //   ArrayList<EventInfo> tempList = currentNode.getEventList();
            EventInfo info = null;
            //iterate over arraylist until matching id is found
            for(int i = 0; i < currentNode.getEventList().size(); i++){
                if (currentNode.getEventList().get(i).getId()==id) {
                    info = currentNode.getEventList().get(i);
                }
            }
            if(info == null){
                throw new IllegalArgumentException("cannot follow path");
            }
            Node nextNode = info.getNeighbour();
            moveMemory.push(nextNode);
            visitedNodes.add(nextNode);
            this.currentNode = nextNode;
            currentNode.enqueueMessage(this);
        }
    }

    /**
     * Method that is used when query found the node where the event happened. The method
     * follow the path that the query has taken back to the starting position of the query.
     */
    private void goBack(){

        //get previous position
        Node nextNode = moveMemory.peek();
        moveMemory.pop();
        //set previous position to current
        //update current to the next one
        this.currentNode = nextNode;
        this.currentNode.enqueueMessage(this);
        if (moveMemory.isEmpty() && currentNode.equals(startingNode)){
            hasReturned = true;
            currentNode.getMessageQueue().remove(this);
        }
    }

    /**
     * Method to check if query has returned.
     * @return true or false depending on the state.
     */
    public boolean isHasReturned(){
        return this.hasReturned;
    }

    /**
     * Method to get current node
     * @return the current node
     */
    public Node getCurrentNode() {
        return currentNode;
    }

    /**
     * Method to get the path of the query.
     * @return a stack of a cloned path.
     */
    public Stack<Node> getPath() {
        return (Stack<Node>) path.clone();
    }

    /**
     * Method to get if the query has returned or not.
     * @return true or false depending on the state.
     */
    public boolean hasReturned() {
        return this.hasReturned;
    }

    /**
     * Method that is used to print out the information about the event.
     * @return the string to print out.
     */
    public String eventToString() {
        event.getTime();
        event.getPosition();

        return String.format("Pos: (%s, %s), Time: %s, Id: %s", event.getPosition().getX(), event.getPosition().getY(),
                event.getTime(), event.getId());
    }
}