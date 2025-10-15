/**
 * An interface for messages that are sent between nodes
 * @author Joakim Wiksten (c22jwn)
 */
public interface Message {
    boolean move();
    Node getCurrentNode();

}
