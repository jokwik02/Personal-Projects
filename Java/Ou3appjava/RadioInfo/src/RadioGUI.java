import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
/**
 * GUI for Radio Info program
 * @author Joakim Wiksten (c22jwn)
 * @version 2023-01-03
 */
public class RadioGUI {
    private JMenuBar menuBar;
    private JTextArea tagline;
    private JFrame frame;
    private JLabel imageLabel;
    private JTable table;
    private JButton updateButton;
    private JPanel startPanel;
    private Timer timer;
    public RadioGUI(){
        frame = new JFrame();
        timer = new Timer(3600000, null);
        buildMenu();
        buildStartPanel();
        frame.setJMenuBar(menuBar);
        frame.add(startPanel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setMinimumSize(new Dimension(700,500));
        frame.setVisible(true);

    }
    //Builds a start screen that is shown when starting the program
    private void buildStartPanel(){
        JLabel label = new JLabel("Välkommen till RadioInfo!");
        JLabel label2 = new JLabel("För att se info om kanalerna, tryck på" +
                " kanalknappen längst uppe i vänster hörn.");
        label.setHorizontalAlignment(SwingConstants.CENTER);
        label.setFont(new Font(new JLabel().getFont().getFontName(), Font.PLAIN, 30));
        label2.setHorizontalAlignment(SwingConstants.CENTER);
        startPanel = new JPanel(new GridLayout(3,1));
        startPanel.add(label);
        startPanel.add(label2);
    }
    //removes the startscreen
    public void removeStartPanel(){
        frame.remove(startPanel);
        frame.repaint();
        frame.revalidate();
        frame.add(buildMiddlePanel());
    }
    //Builds a panel containing info about a channel
    private JPanel buildMiddlePanel() {
        JPanel panel = new JPanel(new BorderLayout());
        JPanel upperPanel = new JPanel(new BorderLayout());
        tagline = new JTextArea();
        tagline.setEditable(false);
        tagline.setLineWrap(true);
        tagline.setWrapStyleWord(true);
        imageLabel = new JLabel();
        table = new JTable();
        table.setBorder(BorderFactory.createLineBorder(Color.BLACK,2));
        updateButton = new JButton("Updatera Kanaldata");
        updateButton.setVisible(false);
        JPanel updateButtonPanel = new JPanel(new FlowLayout());
        updateButtonPanel.add(updateButton);
        JScrollPane scrollPane = new JScrollPane(table);
        upperPanel.add(imageLabel, BorderLayout.CENTER);
        upperPanel.add(tagline, BorderLayout.SOUTH);
        panel.add(upperPanel, BorderLayout.NORTH);
        panel.add(scrollPane, BorderLayout.CENTER);
        panel.add(updateButtonPanel, BorderLayout.SOUTH);
        return panel;
    }
    //Builds a menubar
    private void buildMenu(){
        menuBar = new JMenuBar();
        menuBar.setBackground(Color.LIGHT_GRAY);
    }

    public JMenuBar getMenuBar() {
        return menuBar;
    }

    public void setImage(ImageIcon image) {
        imageLabel.setIcon(image);
        frame.repaint();
    }


    public JTextArea getTagline() {
        return tagline;
    }

    public void setTableModel(DefaultTableModel tableModel) {
        this.table.setModel(tableModel);
        frame.repaint();
    }

    public JTable getTable() {
        return table;
    }

    public JFrame getFrame() {
        return frame;
    }

    public JButton getUpdateButton() {
        return updateButton;
    }

    public Timer getTimer() {
        return timer;
    }

    public JPanel getStartPanel() {
        return startPanel;
    }
}
