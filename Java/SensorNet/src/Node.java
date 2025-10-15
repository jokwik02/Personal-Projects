
import java.util.*;

/**
 * A Node class to be used in a network that has a position, a list of events it knows about, a list of its neighbours
 * and a queue of messages that wants to interact with the node
 * @version 1.0
 * @author Joakim Wiksten (c22jwn), Arvid Asplund (tfy19aad)
 */
public class Node {
    private Position position;
    private ArrayList<Node> neighbours;

    private ArrayList<EventInfo> eventList;

    private Queue<Message> messageQueue;

    /**
     * Constructor method that creates a node with a position.
     *
     * @param position the position of the node
     */

    public Node(Position position) {
        this.position = position;
        this.eventList = new ArrayList<>();
        this.messageQueue = new LinkedList<>();
    }

    /**
     *
     * @return the nodes position
     */
    public Position getPosition(){
        return this.position;
    }

    /**
     *
     * @return the nodes list of neighbours
     */
    public ArrayList<Node> getNeighbours(){
        return (ArrayList<Node>) this.neighbours.clone();
    }

    /**
     * Sets the nodes neighbours
     * @param l the list to be set
     */
    public void setNeighbours(ArrayList<Node> l){
        this.neighbours = l;
    }

    /**
     * Creates an event and adds it to the map of events.
     *
     * @param time the current iteration of the program
     * @return the id of the created event
     */
    public int createEvent(int time){
        int id  = Objects.hash(time, this.position.getX(), this.position.getY());
        Event e = new Event(id, this.position, time);
        EventInfo i = new EventInfo(e, id);
        this.eventList.add(i);
        return id;
    }

    /**
     * A method used in testing that returns the specified Eventinfo from the nodes eventlist
     * @param id the id of the event to get information from
     * @return the Eventinfo with the correct id, or null if the Eventinfo is not found
     */
    public EventInfo getEventinfo(int id){
        for(int i = 0; i < this.eventList.size(); i++){
            if(eventList.get(i).getId() == id){
                return this.eventList.get(i);
            }
        }
        return null;
    }

    /**
     * @return the nodes eventlist
     */
    public ArrayList<EventInfo> getEventList() {
        return this.eventList;
    }

    /**
     * Sets the nodes eventlist (only used in testing)
     * @param l the new eventlist for the node
     */
    public void setEventList(ArrayList<EventInfo> l){
        this.eventList = l;
    }

    /**
     * Adds an eventinfo to the nodes eventlist (only used in testing)
     * @param e the eventinfo that should be added
     */
    public void addEvent(EventInfo e){
        this.eventList.add(e);
    }

    /**
     * Enqueues a message in the node message queue
     * @param message the message that is enqueued
     */
    public void enqueueMessage(Message message){
        messageQueue.add(message);
    }

    /**
     * @return the nodes queue of messages
     */
    public Queue<Message> getMessageQueue(){
        return this.messageQueue;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Node node = (Node) o;
        return Objects.equals(position, node.position);
    }

    @Override
    public int hashCode() {
        return Objects.hash(position);
    }
}
