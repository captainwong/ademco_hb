package javademo.jni;

import java.io.*;
import java.net.*;

public class SimpleServer {
    public static void main(String[] args) {

        AdemcoHbLibrary lib = new AdemcoHbLibrary();
        lib.testPrint();

        // test parse
        {
            System.out.println("testing parse...");
            String data = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
            System.out.println("using data=" + data);
            AdemcoHbLibrary.ParseResult res = lib.parse(data, data.length());
            assert(res.result == 0);
            assert(res.cb_commited == data.length());
            System.out.println("parse result=" + res.result + ",cb_commited=" + res.cb_commited);
        }

        // test pack
        {            
            System.out.println("testing pack...");
            System.out.println("using seq=1234, ademco_id=666666, event=3400, zone=0, gg=0");
            String data = lib.pack(1234, 666666, 3400, 0, 0);
            System.out.println("data=" + data);
        }

        // test pack ack
        {
            System.out.println("testing packAck...");
            System.out.println("using seq=1234, ademco_id=666666");
            String data = lib.packAck(1234, 666666);
            System.out.println("data=" + data);
        }

        int port = 12345;
        if (args.length > 0){
            port = Integer.parseInt(args[0]);
        }
 
        try (ServerSocket serverSocket = new ServerSocket(port)) { 
            System.out.println("Server is listening on port " + port); 
            while (true) {
                Socket socket = serverSocket.accept();
                System.out.println("New client connected"); 
                new SimpleServerThread(socket, lib).start();
            }
 
        } catch (IOException ex) {
            System.out.println("Server exception: " + ex.getMessage());
            ex.printStackTrace();
        }
    }
}