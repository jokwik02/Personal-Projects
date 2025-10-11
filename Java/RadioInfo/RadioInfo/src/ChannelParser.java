import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParserFactory;
import java.io.IOException;
import java.net.URL;

/**
 * Class for parsing information about channels
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class ChannelParser implements Parser<Void>{
    private ChannelEventHandler eventHandler;

    public ChannelParser(ChannelEventHandler eventHandler) {
        this.eventHandler = eventHandler;
    }
    @Override
    public Void run() throws IOException, ParserConfigurationException, SAXException {
        URL url = new URL("http://api.sr.se/api/v2/channels?pagination=false");
        SAXParserFactory factory = SAXParserFactory.newInstance();
        factory.setNamespaceAware( true );
        javax.xml.parsers.SAXParser parser = factory.newSAXParser();
        //Parses the information using a channel event handler and a SAX parser
        parser.parse(new InputSource(url.openStream()), eventHandler);
        return null;
    }
}
