import javax.swing.*;
import java.awt.*;
import java.net.MalformedURLException;
import java.net.URL;
import java.time.Instant;
/**
 * Class representing an episode of a radio channel
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class Episode {
    private String name;
    private Instant startDate;
    private Instant endDate;
    private String description;
    private ImageIcon image;
    private int id;
    private String errorMessage;
    private int tableRow;
    private int programId;

    //Creates an episode
    public Episode(String name, String startDate, String endDate, int id, int programId) {
        this.name = name;
        this.startDate = Instant.parse(startDate);
        this.endDate = Instant.parse(endDate);
        this.id = id;
        this.programId = programId;
    }
    //Creates a new channel (for copying a channel)
    public Episode(Episode episode){
        this.name = episode.getName();
        this.startDate = episode.getStartDate();
        this.endDate = episode.getEndDate();
        this.id = episode.getId();
        this.programId = episode.getProgramId();
        this.image = episode.getImage();
        this.description = episode.getDescription();
        this.tableRow = episode.getTableRow();
        this.errorMessage = episode.getErrorMessage();
    }
    //Creates image from url
    private ImageIcon createImage(String urlString) throws MalformedURLException {
        URL url = new URL(urlString);
        Image image = Toolkit.getDefaultToolkit().createImage(url);
        Image resizedImage = image.getScaledInstance(200, 200, Image.SCALE_SMOOTH);
        return new ImageIcon(resizedImage);
    }

    public ImageIcon getImage() {
        return image;
    }

    public String getName() {
        return name;
    }

    public Instant getEndDate() {
        return endDate;
    }

    public Instant getStartDate() {
        return startDate;
    }

    public String getDescription() {
        return description;
    }

    public int getId() {
        return id;
    }

    public void setImage(String imageUrl) throws MalformedURLException {
        this.image = createImage(imageUrl);
    }
    public void setImage(ImageIcon image){
        this.image = image;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getErrorMessage() {
        return errorMessage;
    }

    public void setErrorMessage(String errorMessage) {
        this.errorMessage = errorMessage;
    }

    public int getTableRow() {
        return tableRow;
    }

    public void setTableRow(int tableRow) {
        this.tableRow = tableRow;
    }

    public int getProgramId() {
        return programId;
    }
    public Episode copy(){
        return new Episode(this);
    }
}
