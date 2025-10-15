/**
 * Class that holds information about a specific event and knows if the event
 * is at the current node or the neighbour.
 * @version 1.0
 * @author Jonatan Westling (c22jwg)
 */
public class EventInfo {
    private Event event ;
    private Node neighbour;
    private int steps;
    private boolean isEvent;
    private int id;

    /**
     * Constructor that creates an EventInfo object that hold an event.
     * @param event the actual event
     * @param id of the event
     */
    public EventInfo(Event event, int id){
        this.event = event;
        this.steps = 0;
        this.isEvent = true;
        this.id = id;

    }

    /**
     * Constructor that creates an EventInfo object that holds a neighbour from which the agent
     * came from with the id.
     * @param neighbour the neighbour where the agent came from with the event id.
     * @param steps amount of steps from the actual event
     * @param id number of the actual event
     */
    public EventInfo(Node neighbour, int steps, int id){
        this.neighbour = neighbour;
        this.steps = steps;
        this.isEvent = false;
        this.id = id;
    }

    /**
     * Method to get id of the event
     * @return the id of the event
     */
    public int getId(){
        return this.id;
    }

    /**
     * Method to get the event
     * @return the event
     */
    public Event getEvent(){
        return this.event;
    }

    /**
     * Method to get the neighbour from where the agent came from with the id
     * @return the neighbour
     */
    public Node getNeighbour(){
        return this.neighbour;
    }

    /**
     * Method to update the neighbour
     * @param neighbour the new neighbour to update with
     */
    public void setNeighbour(Node neighbour) {
        this.neighbour = neighbour;
    }

    /**
     * Method to update the steps
     */
    public void addStep(){
        this.steps++;
    }

    /**
     * Method to get the amount of steps
     * @return the amount of steps
     */
    public int getSteps(){
        return this.steps;
    }

    /**
     * Method to check if the event info contains the event or not.
     * @return true or false whether it is an event or not.
     */
    public boolean isEvent(){
        return this.isEvent;
    }

    /**
     * Method to check if an object equals the EventInfo.
     * @param o the object to compare with
     * @return true or false if it is an event or not.
     */
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof EventInfo)) return false;

        return ((EventInfo) o).getSteps() == this.steps && ((EventInfo) o).getId() == this.id && ((EventInfo) o).getNeighbour() == this.neighbour;
    }
}
