import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;
import java.time.Duration;
import java.time.Instant;
/**
 * Class for handling events while parsing a channels schedule
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class ScheduleEventHandler extends DefaultHandler {
    private String elementValue;
    private String currentName;
    private String currentStartDate;
    private String currentEndDate;
    private Channel currentChannel;
    private Instant startTime;
    private int currentId;
    private int currentProgramId;
    public ScheduleEventHandler(){
        startTime = Instant.now();
    }
    @Override
    public void characters(char[] ch, int start, int length){
        elementValue = new String(ch,start,length);
    }

    @Override
    public void startElement(String namespace, String localName,
                             String qName, Attributes attr) {
        //Resets variables and gets the episodes program id
        switch (qName){
            case "scheduledepisode" -> {
                currentName  = "";
                currentId = 0;
                currentStartDate = "";
                currentEndDate = "";
            }
            case "program" -> currentProgramId = Integer.parseInt(attr.getValue("id"));
        }
    }
    @Override
    public void endElement(String uri, String localName, String qName) throws StopParsingException {
        switch (qName) {
            //Sets the title, start time, end time, and id of the episode
            case "title" -> currentName= elementValue;
            case "starttimeutc" -> currentStartDate = elementValue;
            case "endtimeutc" -> currentEndDate = elementValue;
            case "episodeid" -> currentId = Integer.parseInt(elementValue);
            case "scheduledepisode" -> {
                //Adds new episode to the channels episode list
                Episode episode = new Episode(currentName, currentStartDate,currentEndDate, currentId, currentProgramId);
                if(episode.getStartDate().isAfter(startTime.plus(Duration.ofHours(12)))){
                    //Tells the parser to stop parsing if we have reached more than
                    //twelve ours ahead in the schedule
                    throw new StopParsingException();
                }
                currentChannel.addEpisode(episode);
            }
        }
    }

    public void setCurrentChannel(Channel currentChannel) {
        this.currentChannel = currentChannel;
    }

}
