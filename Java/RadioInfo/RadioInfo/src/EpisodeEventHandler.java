import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;
import java.net.MalformedURLException;
/**
 * Class for handling events while parsing information about Episodes
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class EpisodeEventHandler extends DefaultHandler {
    private String elementValue;
    private Episode episode;
    private boolean correctEpisode;
    private String imageURL;
    private String description;

    public EpisodeEventHandler(Episode episode){
        this.episode = episode;
    }
    @Override
    public void characters(char[] ch, int start, int length){
        elementValue = new String(ch,start,length);
    }
    @Override
    public void startElement(String namespace, String localName,
                             String qName, Attributes attr) {
        //Checks if correct episode has been found and resets variables when a new episode is encountered
        switch (qName){
            case "program" -> {
                if(episode.getProgramId() == Integer.parseInt(attr.getValue("id"))){
                    correctEpisode = true;
                }
            }
            case "scheduledepisode" -> {
                imageURL = "" ;
                description = "";
            }
        }
    }
    @Override
    public void endElement(String uri, String localName, String qName) throws StopParsingException {
        switch (qName) {
            //Sets image url and description
            case "imageurl" -> imageURL = elementValue;
            case "description" -> description = elementValue;
            case "scheduledepisode" -> {
                if(correctEpisode) {
                    try {
                        //Adds info to an episode
                        if(!imageURL.isEmpty()) {
                            episode.setImage(imageURL);
                        }
                        episode.setDescription(description);
                        correctEpisode = false;
                        //Tells the parser that we have found our episode and
                        //that there is no need to keep parsing
                        throw new StopParsingException();
                    } catch (MalformedURLException e) {
                        episode.setErrorMessage(e.getMessage());
                    }
                }
            }
        }
    }
}
