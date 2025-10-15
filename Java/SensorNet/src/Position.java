/**
 * Handles positions in the network.
 * @version 1.0
 * @author Arvid Asplund (tfy19aad)
 */
public class Position {
    private final int x;
    private final int y;

    /**
     * Constructor method that initializes the x and y coordinates of the Position object.
     *
     * @param x The x coordinate of the position.
     * @param y The y coordinate of the position.
     */
    public Position(int x, int y) {
        this.x = x;
        this.y = y;
    }

    /**
     * Getter method for the x coordinate.
     *
     * @return The x coordinate of the position.
     */
    public int getX() {
        return x;
    }

    /**
     * Getter method for the y coordinate.
     *
     * @return The y coordinate of the position.
     */
    public int getY() {
        return y;
    }

    /**
     * Override of the equals() method that compares two Position objects for equality based on their x and y coordinates.
     *
     * @param o The object to compare to the Position object.
     * @return true if the two positions are equal, false otherwise.
     */
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Position position)) return false;
        return x == position.x && y == position.y;
    }
}