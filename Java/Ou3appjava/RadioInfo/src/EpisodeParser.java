import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParserFactory;
import java.io.IOException;
import java.net.URL;
import java.time.Instant;
import java.time.ZoneId;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

/**
 * Class for parsing information about episodes
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class EpisodeParser implements Parser<Void>{
    private Channel channel;
    private Episode episode;
    public EpisodeParser(Channel channel, Episode episode){
        this.channel = channel;
        this.episode = episode;
    }
    //Runs the parser
    @Override
    public Void run() throws IOException, ParserConfigurationException, SAXException {
        URL url = new URL("http://api.sr.se/api/v2/scheduledepisodes?channelid=" + channel.getId()
                + "&pagination=false&date=" + formatDate(episode.getStartDate()));
        SAXParserFactory factory = SAXParserFactory.newInstance();
        factory.setNamespaceAware( true );
        javax.xml.parsers.SAXParser parser = factory.newSAXParser();
        try {
            //Parses the information using an episode event handler
            parser.parse(new InputSource(url.openStream()), new EpisodeEventHandler(episode));
        }catch (StopParsingException ignored){} //Just used to stop the parsing when we don't need to anymore
        return null;
    }
    //Formats the date to the format used in the url
    private String formatDate(Instant date){
        ZonedDateTime zonedDateTime = date.atZone(ZoneId.of("CET"));
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");
        return zonedDateTime.format(formatter);
    }
}
