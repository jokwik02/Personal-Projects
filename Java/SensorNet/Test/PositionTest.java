import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Test the Position class and all of its methods.
 * @version 1.0
 * @author Arvid Asplund (tfy19aad)
 */
public class PositionTest {
    private Position position1 = new Position(2, 5);
    private Position position2 = new Position(1, 6);

    /**
     * Test that getX returns what we expect.
     */
    @Test
    public void testGetX() {
        // Test so that it returns the x-coordinate
        assertEquals(position1.getX(), 2);

        // Test so it does not return the same each time
        assertNotEquals(position2.getX(), 2);
    }

    /**
     * Test that getY returns what we expect.
     */
    @Test
    public void testGetY() {
        // Test so that it returns the y-coordinate
        assertEquals(position1.getY(), 5);

        // Test so it does not return the same each time
        assertNotEquals(position2.getY(), 5);
    }

    /**
     * Test that equals works.
     */
    @Test
    public void testEquals() {
        // Same object
        assertEquals(position1, position1);

        // Same coordinate, different object
        assertEquals(position1, new Position(position1.getX(), position1.getY()));

        // Not equal to other position
        assertNotEquals(position1, position2);

        // Not equal if position is not a Position object
        assertNotEquals(position1, "(2,5)");
    }
}