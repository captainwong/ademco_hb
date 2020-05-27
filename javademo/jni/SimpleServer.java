package javademo.jni;

import java.io.*;
import java.net.*;

public class SimpleServer {
    public static void main(String[] args) {
        int port = 12345;
        if (args.length > 0){
            port = Integer.parseInt(args[0]);
        }
 
        // try (ServerSocket serverSocket = new ServerSocket(port)) { 
        //     System.out.println("Server is listening on port " + port); 
        //     while (true) {
        //         Socket socket = serverSocket.accept();
        //         System.out.println("New client connected"); 
        //         new SimpleServerThread(socket).start();
        //     }
 
        // } catch (IOException ex) {
        //     System.out.println("Server exception: " + ex.getMessage());
        //     ex.printStackTrace();
        // }
    }
}