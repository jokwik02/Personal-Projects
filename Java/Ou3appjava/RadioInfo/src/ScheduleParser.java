import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParserFactory;
import java.io.IOException;
import java.net.URL;
import java.time.Duration;
import java.time.Instant;
import java.time.ZoneId;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;
import java.time.temporal.ChronoUnit;
import java.util.List;

/**
 * Class for parsing information about episodes
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class ScheduleParser implements Parser<List<Episode>>{
    private Channel channel;
    public ScheduleParser(Channel channel) {
        this.channel = channel;
    }

    //Runs the parser
    @Override
    public List<Episode> run() throws IOException, ParserConfigurationException, SAXException {
        parseYesterdaysDate();
        parse(Instant.now());
        parseTomorrowsDate();
        return channel.getEpisodeList();
    }
    //If we need to parse yesterday's schedule, since 12 hours before might be yesterday
    private void parseYesterdaysDate() throws IOException, ParserConfigurationException, SAXException {
        Instant currentDate = Instant.now();
        Instant twelveHoursBefore = currentDate.minus(Duration.ofHours(12));
        if(currentDate.truncatedTo(ChronoUnit.DAYS).isAfter(twelveHoursBefore.truncatedTo(ChronoUnit.DAYS))){
            parse(twelveHoursBefore);
        }
    }
    //If we need to parse tomorrow's schedule, since 12 hours after might be tomorrow
    private void parseTomorrowsDate() throws IOException, ParserConfigurationException, SAXException {
        Instant currentDate = Instant.now();
        Instant twelveHoursAfter = currentDate.plus(Duration.ofHours(12));
        if(currentDate.truncatedTo(ChronoUnit.DAYS).isBefore(twelveHoursAfter.truncatedTo(ChronoUnit.DAYS))){
            parse(twelveHoursAfter);
        }
    }
    private void parse(Instant date) throws ParserConfigurationException, SAXException, IOException {
        SAXParserFactory factory = SAXParserFactory.newInstance();
        factory.setNamespaceAware( true );
        javax.xml.parsers.SAXParser parser = factory.newSAXParser();
        ScheduleEventHandler eventHandler = new ScheduleEventHandler();
        String urlString = "http://api.sr.se/api/v2/scheduledepisodes?channelid=" + channel.getId()
                + "&pagination=false&date=" + formatDate(date);
        URL url = new URL(urlString);
        eventHandler.setCurrentChannel(channel);
        try {
            //Parses the information using a SAX parser and a ScheduleEventHandler
            parser.parse(new InputSource(url.openStream()), eventHandler);
        } catch (StopParsingException ignored){} //Just used to stop the parsing when we don't need to parse anymore
    }
    private String formatDate(Instant date){
        ZonedDateTime zonedDateTime = date.atZone(ZoneId.of("CET"));
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");
        return zonedDateTime.format(formatter);
    }
}
