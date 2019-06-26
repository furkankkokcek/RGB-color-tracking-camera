
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

public class Server extends Thread {
 
    private DatagramSocket socket;
    private boolean running;
    private InetAddress address;
    int port;
    private byte[] buf = new byte[256];
    public boolean isFirst = true;
 
    public Server() throws SocketException {
        socket = new DatagramSocket(4445);
    }
    
 
    public void run() {
    	
        running = true;
 
        while (running) {
        	
        	
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            try {
				socket.receive(packet);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
             
            String received = new String(packet.getData(), 0, packet.getLength());
            System.out.println(received);
            if (received.equals("hello")) {
            	
            	address = packet.getAddress();
                port = packet.getPort();
                
            	sendUDPtoMCU(MainWindow.tempResult);            	
            }
        }
        //socket.close();
    
    }
    
    public void sendUDPtoMCU(String row) {
        DatagramPacket packet = new DatagramPacket(row.getBytes(), row.length(), this.address, this.port);
		try {
			socket.send(packet);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
}