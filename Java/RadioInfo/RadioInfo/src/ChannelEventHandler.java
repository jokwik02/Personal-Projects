import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;

import java.net.MalformedURLException;
import java.util.HashMap;
import java.util.Map;
/**
 * Class for handling events while parsing channels
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class ChannelEventHandler extends DefaultHandler {
    private Map<String,Channel> channelMap;
    private String currentName;
    private String currentTagline;
    private String currentImageURL;
    private String currentType;
    private int currentID;
    private String elementValue;
    private String errorMessage;
    public ChannelEventHandler(){
        channelMap = new HashMap<>();
    }

    @Override
    public void characters(char[] ch, int start, int length){
        elementValue = new String(ch,start,length);
    }
    @Override
    public void startElement(String namespace, String localName,
                             String qName, Attributes attr) {
        //Set channel name and id
        if (qName.equals("channel")) {
            currentName = attr.getValue("name");
            currentID = Integer.parseInt(attr.getValue("id"));
        }
    }
    @Override
    public void endElement(String uri, String localName, String qName) {
        switch (qName) {
            //Sets image url, tagline, and channeltype
            case "image" -> currentImageURL = elementValue;
            case "tagline" -> currentTagline = elementValue;
            case "channeltype" -> currentType = elementValue;
            case "channel" -> {
                //Creates a channel and adds it to the eventhandlers map of channels
                try {
                    channelMap.put(currentName, new Channel(currentName, currentImageURL, currentTagline, currentID, currentType));
                } catch (MalformedURLException e) {
                    errorMessage = e.getMessage();
                }
            }
        }
    }

    public Map<String, Channel> getChannelMap() {
        return channelMap;
    }

    public String getErrorMessage() {
        return errorMessage;
    }
}
