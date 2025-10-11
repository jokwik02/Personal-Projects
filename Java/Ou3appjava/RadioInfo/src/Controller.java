import org.xml.sax.SAXException;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import javax.xml.parsers.ParserConfigurationException;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.time.Instant;
import java.time.ZoneId;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutionException;

/**
 * Controller class for Radio Info program
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */

public class Controller {
    private RadioGUI radioGUI;
    private Map<String,Channel> channelMap;
    private Channel selectedChannel;
    private boolean hasStarted;
    SwingWorker<String,Void> scheduleWorker;
    private List<Channel> updatedChannels;
    private CountDownLatch latch;
    public Controller() {
        radioGUI = new RadioGUI();
        hasStarted = false;
        parseChannels();
        updatedChannels = new ArrayList<>();

    }
    //Loads all the channels in a swing worker
    private void parseChannels(){
        SwingWorker<ChannelEventHandler,String> worker = new SwingWorker<>() {
            @Override
            protected ChannelEventHandler doInBackground() {
                ChannelEventHandler eventHandler = new ChannelEventHandler();
                try {
                    ChannelParser channelParser = new ChannelParser(eventHandler);
                    channelParser.run();
                } catch (IOException | ParserConfigurationException | SAXException exception) {
                    publish(exception.getMessage());
                }
                return eventHandler;
            }

            //For error messages
            @Override
            protected void process(List<String> chunks) {
                for (String chunk : chunks) {
                    radioGUI.getStartPanel().add(new JLabel(chunk));
                }
            }

            //Builds the menu bar when the parsing of channels is done
            @Override
            protected void done() {
                try {
                    ChannelEventHandler eventHandler = get();
                    channelMap = eventHandler.getChannelMap();
                    if (eventHandler.getErrorMessage() == null) {
                        buildMenuBar();
                    } else {
                        JLabel errorLabel = new JLabel(eventHandler.getErrorMessage());
                        errorLabel.setForeground(Color.RED);
                        radioGUI.getStartPanel().add(errorLabel);
                    }
                } catch (InterruptedException | ExecutionException exception) {
                    JLabel errorLabel = new JLabel(exception.getMessage());
                    errorLabel.setForeground(Color.RED);
                    radioGUI.getStartPanel().add(errorLabel);
                }
            }
        };
        worker.execute();
    }
    //Builds the menu bar
    private void buildMenuBar(){
        JMenuBar menuBar = radioGUI.getMenuBar();
        JMenu mainMenu = new JMenu("Kanaler");
        List<JMenu> menuList = new ArrayList<>();
        for (Channel channel : channelMap.values()){
            boolean menuExists = false;
            JMenuItem menuItem = new JMenuItem(channel.getName());
            menuItem.addActionListener(new JMenuItemActionListener());
            for (JMenu menu : menuList){
                if(menu.getText().equals(channel.getType())){
                    menu.add(menuItem);
                    menuExists = true;
                    break;
                }
            }
            if(!menuExists){
                JMenu menu = new JMenu(channel.getType());
                menuList.add(menu);
                menu.add(menuItem);
            }
        }
        for (JMenu menu : menuList){
            mainMenu.add(menu);
        }
        menuBar.add(mainMenu);
        radioGUI.getFrame().repaint();
        radioGUI.getFrame().revalidate();
    }
    //Parses the Schedule for a channel
    private void parseSchedule(Channel channelCopy, Channel channel, boolean updateAnyway, boolean calledFromUpdate){
        DefaultTableModel defaultTableModel = new DefaultTableModel();
        //Loading message
        defaultTableModel.addColumn("Laddar in tablå...");
        radioGUI.setTableModel(defaultTableModel);
        //Parses the schedule using a swing worker
        scheduleWorker = new SwingWorker<>() {
            @Override
            protected String doInBackground() throws InterruptedException {
                String errorMessage = null;
                try {
                    ScheduleParser parser = new ScheduleParser(channelCopy);
                    channelCopy.setEpisodeList(parser.run());
                } catch (IOException | ParserConfigurationException | SAXException ex) {
                    errorMessage = ex.getMessage();
                }
                if(calledFromUpdate){
                    latch.countDown();
                }
                return errorMessage;
            }

            protected void done(){
                if(!isCancelled()) {
                    try {
                        if(get() != null){
                            radioGUI.getFrame().add(new JLabel(get()));
                        }
                    } catch (InterruptedException | ExecutionException e) {
                        radioGUI.getFrame().add(new JLabel(e.getMessage()));
                    }
                    channel.setEpisodeList(channelCopy.getEpisodeList());
                    if (selectedChannel == channel) {
                        setTable(channelCopy);
                    }
                }
            }
        };
        //Only parses the Schedule if it hasn't already been parsed
        if(!updatedChannels.contains(channel) || updateAnyway) {
            scheduleWorker.execute();
        } else {
            setTable(channel);
        }
    }
    //Sets the table containing the schedule
    private void setTable(Channel channel){
        selectedChannel.setEpisodeList(channel.getEpisodeList());
        List<Episode> episodeList = channel.getEpisodeList();
        String[] columnNames = {"Program", "Starttid", "Sluttid"};
        DefaultTableModel model = new DefaultTableModel(new Object[][]{}, columnNames){
            @Override
            public boolean isCellEditable(int row, int column){
                return false;
            }
        };
        for (Episode episode : episodeList) {
            String startTime = getTime(episode.getStartDate());
            String endTime = getTime(episode.getEndDate());
            Object[] row = {episode.getName(), startTime, endTime};
            model.addRow(row);
            episode.setTableRow(model.getRowCount() - 1);
        }
        radioGUI.setTableModel(model);
        radioGUI.getTimer().restart();
    }
    //Returns the time in the wanted format
    private String getTime(Instant date){
        ZonedDateTime zonedDateTime = ZonedDateTime.ofInstant(date, ZoneId.of("CET"));
        return zonedDateTime.format(DateTimeFormatter.ofPattern("dd/MM:HH:mm"));
    }
    //Builds a mouse listener that listener for episodes in the table being clicked
    private void buildTableMouseListener(){
        JTable table = radioGUI.getTable();
        table.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                int row = table.getSelectedRow();
                int column = table.getSelectedColumn();
                if(column == 0){
                    for (Episode episode: selectedChannel.getEpisodeList()){
                        if(episode.getTableRow() == row){
                            buildPopUpWindow(episode);
                            break;
                        }
                    }
                }

            }
        });
    }
    //Builds a pop-up window showing information about an episode
    private void buildPopUpWindow(Episode episode) {
        JDialog popUpWindow = new JDialog(radioGUI.getFrame(), episode.getName(),true);
        popUpWindow.setMinimumSize(new Dimension(400,300));
        popUpWindow.setResizable(false);
        popUpWindow.setLayout(new BorderLayout());
        JLabel textLabel = new JLabel("Laddar Avsnittsinfo...");
        popUpWindow.add(textLabel,BorderLayout.SOUTH);
        popUpWindow.setLocationRelativeTo(radioGUI.getFrame());
        parseEpisodeInfo(episode, popUpWindow, textLabel);
        popUpWindow.setVisible(true);
    }
    //Parses info about an episode using a Swing worker
    private void parseEpisodeInfo(Episode episode, JDialog popUpWindow, JLabel textLabel) {
        Episode episodeCopy = episode.copy();
        Channel channelCopy = selectedChannel.copy();
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            protected Void doInBackground() throws Exception {
                EpisodeParser episodeParser = new EpisodeParser(channelCopy, episodeCopy);
                episodeParser.run();
                return null;
            }
            @Override
            protected void done(){
                //Adds the info to the pop-up window
                episode.setDescription(episodeCopy.getDescription());
                episode.setImage(episodeCopy.getImage());
                popUpWindow.remove(textLabel);
                JTextArea textArea = new JTextArea(episode.getDescription());
                textArea.setEditable(false);
                textArea.setLineWrap(true);
                textArea.setWrapStyleWord(true);
                if(episode.getDescription() == null||episode.getDescription().isEmpty()){
                    textArea.setText("Ingen beskrivning hittades");
                }
                if(episode.getErrorMessage() != null){
                    textArea.setText(episode.getErrorMessage());
                }
                popUpWindow.add(textArea,BorderLayout.SOUTH);
                if(episode.getImage() != null) {
                    popUpWindow.add(new JLabel(episode.getImage()), BorderLayout.CENTER);
                } else {
                    popUpWindow.add(new JLabel("Ingen bild hittades"), BorderLayout.CENTER);
                }
                popUpWindow.repaint();
                popUpWindow.revalidate();
            }
        };
        worker.execute();
    }
    //Builds a button that updates the schedule when clicked
    private void buildUpdateButton(){
        radioGUI.getUpdateButton().addActionListener(new UpdateActionListener());
    }
    //Builds a timer that updates the schedule every hour
    private void buildTimer(){
        radioGUI.getTimer().addActionListener(new UpdateActionListener());
        radioGUI.getTimer().start();
    }

    //Action listener for the items in the menu
    private class JMenuItemActionListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            JMenuItem selected = (JMenuItem) e.getSource();
            //Only happens the first time an item is clicked
            if(!hasStarted){
                radioGUI.removeStartPanel();
                buildTableMouseListener();
                buildUpdateButton();
                buildTimer();
                hasStarted = true;
            }
            //If a scheduleParsing is currently happening, cancel it
            try {
                scheduleWorker.cancel(true);
            } catch (NullPointerException ignored){
            } //Ignores it if the schedule worker hasn't been initialized yet.
            selectedChannel = channelMap.get(selected.getText());
            radioGUI.getUpdateButton().setVisible(true);
            radioGUI.setImage(selectedChannel.getImage());
            radioGUI.getTagline().setText(selectedChannel.getTagline());
            parseSchedule(selectedChannel.copy(), selectedChannel, false, false);
            if(!updatedChannels.contains(selectedChannel)) {
                updatedChannels.add(selectedChannel);
            }
        }
    }
    //Action listener for when data need to be updated
    private class UpdateActionListener implements ActionListener{
        @Override
        public void actionPerformed(ActionEvent e) {
            selectedChannel.setEpisodeList(new ArrayList<>());
            radioGUI.getMenuBar().getMenu(0).setEnabled(false);
            latch = new CountDownLatch(updatedChannels.size());
            SwingWorker<Void,Void> worker = new SwingWorker<>() {
                @Override
                protected Void doInBackground() throws Exception {
                    for (Channel channel : updatedChannels) {
                        parseSchedule(channel.copy(), channel, true, true);
                    }
                    latch.await();
                    return null;
                }
                @Override
                protected void done(){
                    radioGUI.getMenuBar().getMenu(0).setEnabled(true);
                }
            };
            worker.execute();
        }
    }
}
