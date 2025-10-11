/**
 * Interface for parsers for parsing Radio information
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public interface Parser<T> {
    T run() throws Exception;
}
