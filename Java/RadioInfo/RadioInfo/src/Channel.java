import javax.swing.*;
import java.awt.*;
import java.net.MalformedURLException;
import java.net.URL;
import java.time.Duration;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
/**
 * Class representing a radio channel
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class Channel {
    private String name;
    private ImageIcon image;
    private String tagline;
    private String type;
    private int id;
    private List<Episode> episodeList;

    //Creates a new channel
    public Channel(String name, String url, String tagline, int id, String type) throws MalformedURLException {
        episodeList= new ArrayList<>();
        this.name = name;
        image = createImage(url);
        this.tagline = tagline;
        this.id = id;
        this.type = type;
    }
    //Creates a new channel (for copying a channel)
    public Channel(Channel channel){
        this.name = channel.getName();
        this.id = channel.getId();
        this.image = channel.getImage();
        this.tagline = channel.getTagline();
        this.episodeList = new ArrayList<>();
        this.type = channel.getType();
    }
    //Creates an image from an url
    private ImageIcon createImage(String urlString) throws MalformedURLException {
        URL url = new URL(urlString);
        Image image = Toolkit.getDefaultToolkit().createImage(url);
        Image resizedImage = image.getScaledInstance(200, 200, Image.SCALE_SMOOTH);
        return new ImageIcon(resizedImage);
    }
    //Adds an episode to the channels list of episodes
    public void addEpisode(Episode episode){
        if (isIn12HourRange(episode.getStartDate())){
            episodeList.add(episode);
        }
    }
    //Checks if an episode is in the range of 12 hours before or after current time
    private boolean isIn12HourRange(Instant date){
        Instant startOfTimeRange = Instant.now().minus(Duration.ofHours(12));
        Instant endOfTimeRange = Instant.now().plus(Duration.ofHours(12));
        return !date.isBefore(startOfTimeRange) && !date.isAfter(endOfTimeRange);
    }

    public String getName() {
        return name;
    }

    public ImageIcon getImage() {
        return image;
    }

    public String getTagline() {
        return tagline;
    }

    public int getId() {
        return id;
    }

    public String getType() {
        return type;
    }

    public List<Episode> getEpisodeList() {
        return episodeList;
    }

    public void setEpisodeList(List<Episode> episodeList) {
        this.episodeList = episodeList;
    }
    public Channel copy(){
        return new Channel(this);
    }
}
