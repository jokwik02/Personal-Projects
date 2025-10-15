/**
 * An Event class to be used in a network that has a position, an id, and the time when it was created
 * @version 1.0
 * @author Jonatan Westling (c22jwg)
 */
public class Event {
    private int id;
    private Position position;
    private int time;

    /**
     * Constructor method that creates an event with a position, and id, and when it was created.
     * @param id the id of the event
     * @param position the position of the node where the event was created
     * @param time the time when the event was created
     */
    public Event(int id, Position position, int time) {
        this.id = id;
        this.time = time;
        this.position = position;
    }

    /**
     *
     * @return the events id
     */
    public int getId(){
        return this.id;
    }

    /**
     *
     * @return the position of the id
     */
    public Position getPosition() {
        return this.position;
    }

    /**
     *
     * @return the time when the event was created
     */
    public int getTime() {
        return this.time;
    }
}
