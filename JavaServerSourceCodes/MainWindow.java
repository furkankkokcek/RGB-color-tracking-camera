
import javax.imageio.ImageIO;
import javax.swing.*;

import com.sun.xml.internal.ws.util.StringUtils;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.net.SocketException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import java.util.stream.Collectors;

/**
 * Created by indrek on 1.05.2016.
 */
public class MainWindow {

  private static Integer MAX_IMAGE_W = 640;
  private static Integer MAX_IMAGE_H = 480;

  private static final String WINDOW_TITLE = "Arduino Image Capture";
  private static final String BUTTON_NAME_LISTEN = "Listen";
  private static final String BUTTON_NAME_SELECT_SAVE_FOLDER = "Select save folder";
  private static final String SELECT_SAVE_FOLDER_TILE = "Save images to";

  private static final String DEFAULT_IMAGE_DIRECTORY = "/img";


  private JFrame windowFrame;
  private JPanel mainPanel;
  private File selectedFolder;
  private JLabel saveCountLabel = new JLabel();
  private Integer saveCounter = 0;
  private BufferedImage imageBuffer;
  private JLabel imageContainer;
  private JComboBox<String> comPortSelection;
  private JComboBox<Integer> baudRateSelection;
  public static Server server;
  static int rArray[] = new int[160];
  static int sArray[] = new int[10];
  static String tempResult = "";
  
  

  private SerialReader serialReader;
  private ImageCapture imageCapture;



  public MainWindow(JFrame frame) {
    windowFrame = frame;
    imageCapture = new ImageCapture(this::drawImage);
    serialReader = new SerialReader(imageCapture::addReceivedByte);

    mainPanel = new JPanel(new BorderLayout());
    mainPanel.add(createToolbar(), BorderLayout.PAGE_START);
    mainPanel.add(createImagePanel(), BorderLayout.CENTER);
    mainPanel.add(createSavePanel(), BorderLayout.PAGE_END);
  }


  private JComponent createSavePanel() {
    JPanel  saveBar = new JPanel ();
    saveBar.setLayout(new BoxLayout(saveBar, BoxLayout.X_AXIS));
    JLabel filePathLabel = new JLabel();

    saveBar.add(createSelectFolderButton(filePathLabel));
    saveBar.add(Box.createHorizontalStrut(10));
    saveBar.add(filePathLabel);
    saveBar.add(saveCountLabel);

    return saveBar;
  }

  private JButton createSelectFolderButton(JLabel filePathLabel) {
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    fileChooser.setDialogTitle(SELECT_SAVE_FOLDER_TILE);

    JButton listenButton = new JButton(BUTTON_NAME_SELECT_SAVE_FOLDER);
    listenButton.addActionListener((event)->{
      fileChooser.setCurrentDirectory(selectedFolder == null ? getDefaultSaveDirectory() : selectedFolder);

      if (fileChooser.showOpenDialog(listenButton) == JFileChooser.APPROVE_OPTION) {
        selectedFolder = fileChooser.getSelectedFile();
        filePathLabel.setText(selectedFolder.getAbsolutePath());
      }
    });
    return listenButton;
  }

  private File getDefaultSaveDirectory() {
      String currentDir = System.getProperty("user.dir");
      File dir = new File(currentDir + DEFAULT_IMAGE_DIRECTORY);
      return dir.exists() ? dir : new File(currentDir);
  }

  private JComponent createImagePanel() {
    imageBuffer = new BufferedImage(MAX_IMAGE_W,MAX_IMAGE_H,BufferedImage.TYPE_INT_ARGB);
    imageContainer = new JLabel(new ImageIcon(imageBuffer));

    JPanel imagePanel = new JPanel(new GridBagLayout());
    imagePanel.setPreferredSize(new Dimension(MAX_IMAGE_W, MAX_IMAGE_H));
    imagePanel.add(imageContainer);

    return new JScrollPane(imagePanel);
  }


  private JToolBar createToolbar() {
    JToolBar toolBar = new JToolBar();
    toolBar.setFloatable(false);
    toolBar.add(createComPortOption());
    toolBar.add(createBaudRateOption());
    toolBar.add(createStartListeningButton());
    return toolBar;
  }


  private JComboBox createComPortOption() {
    comPortSelection = new JComboBox<>();
    serialReader.getAvailablePorts().forEach(comPortSelection::addItem);
    return comPortSelection;
  }


  private JComboBox createBaudRateOption() {
    baudRateSelection = new JComboBox<>();
    serialReader.getAvailableBaudRates().forEach(baudRateSelection::addItem);
    baudRateSelection.setSelectedItem(serialReader.getDefaultBaudRate());
    return baudRateSelection;
  }


  private JButton createStartListeningButton() {
    JButton listenButton = new JButton(BUTTON_NAME_LISTEN);
    listenButton.addActionListener((event)->{
      this.startListening(listenButton, event);
    });
    return listenButton;
  }


  private void startListening(JButton listenButton, ActionEvent event) {
    try {
      String selectedComPort = (String)comPortSelection.getSelectedItem();
      Integer baudRate = (Integer)baudRateSelection.getSelectedItem();
      serialReader.startListening(selectedComPort, baudRate);
      listenButton.setEnabled(false);
    } catch (SerialReaderException e) {
      JOptionPane.showMessageDialog(windowFrame, e.getMessage());
    }
  }



  private void drawImage(Frame frame, Integer lineIndex) {
    JLabel imageContainer = this.imageContainer;
    
    new Thread(() -> {
    	
		for(int x=0; x<160;x++) {
			int red = frame.getPixelColor(x,lineIndex).getRed();
			int green = frame.getPixelColor(x,lineIndex).getGreen();
			int blue = frame.getPixelColor(x,lineIndex).getBlue();
			rArray[x] += isRed(red, green, blue);
		}
    	
    	if(lineIndex == 119) {
    		for(int j=0;j<160;j++) {
    			int index = j / 16;
    			sArray[index] += rArray[j];
    		}
    		
    		String result = "";
    	
    		for(int i=0;i<10;i++) {
    			String temp = String.format("%04d" , sArray[i]);
    			result += temp + "-";
    		}
    		
  		  	System.out.println(result);
  		  	tempResult = result;
  		  	
  		  	sArray = new int[10];
  		  	rArray = new int[160];
    	}	  	
    }).start();

    // update image in a separate thread so it would not block reading data
    new Thread(() -> {
      synchronized (imageContainer) {
        Graphics2D g = imageBuffer.createGraphics();
        int fromLine = lineIndex != null ? lineIndex : 0;
        int toLine = lineIndex != null ? lineIndex : frame.getLineCount() - 1;
        
        for (int y = fromLine; y <= toLine; y++) {
        	String temp = "";
        	for (int x = 0; x < frame.getLineLength(); x++) {
        		if (x < MAX_IMAGE_W && y < MAX_IMAGE_H) {
        			g.setColor(frame.getPixelColor(x, y)); // gets r, g, b values of the pixel			
        			g.drawLine(x, y, x, y);  
        		}
        	}
        }      
        imageContainer.repaint();
      }
    }).start();
  }
  
  private int isRed(int r, int g, int b) {
	  ArrayList<Integer> tempList = new ArrayList<>();
	  tempList.add(r);
	  tempList.add(g);
	  tempList.add(b);
	  int threshold = Collections.max(tempList);
	  if(threshold > 8 && r == threshold && g < (threshold*0.5) && b < (threshold*0.5)) {
		  return 1;
	  }
	  return 0;
  }


  private void saveImageToFile(BufferedImage image, File toFolder) {
    try {
      // save image to png file
      File newFile = new File(toFolder.getAbsolutePath(), getNextFileName());
      ImageIO.write(image, "png", newFile);
      saveCountLabel.setText(" (" + (++saveCounter) + ")");
    } catch (Exception e) {
      System.out.println("Saving file failed: " + e.getMessage());
    }
  }

  private String getNextFileName() {
    return (new SimpleDateFormat("yyyy-MM-dd_HH.mm.ss.SSS")).format(new Date()) + ".png";
  }


  public static void main(String[] args) throws SocketException {
	server = new Server();
	server.start();
	
    JFrame frame = new JFrame(WINDOW_TITLE);
    MainWindow window = new MainWindow(frame);
    frame.setContentPane(window.mainPanel);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.pack();
    frame.setVisible(true);
  }

}