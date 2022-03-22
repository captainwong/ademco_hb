package javademo.jni;

import java.io.*;
import java.net.*;

public class SimpleServer {

    public static String printable_bytes(byte[] b){
        String HEX_STRING = "0123456789ABCDEF";
        String s = "";
        for(int i = 0; i < b.length; i++){
            byte c = b[i];
            if(32 <= c && c <= 127){
                s += (char)c;
            }else{
                s += "\\x" + HEX_STRING.charAt(b[i] >>> 4);
                s += HEX_STRING.charAt(b[i] & 0x0F);
            }
        }
        return s;
    }

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
            System.out.println("using seq=1234, acct=861234567890, ademco_id=666666, event=3400, zone=0, gg=0");
            String data = lib.pack(1234, "861234567890", 666666, 3400, 0, 0);
            System.out.println("data=" + data);
        }

        // test pack2
        {
            System.out.println("testing pack2...");
            System.out.println("using seq=1234, acct=861234567890, ademco_id=666666, event=3400, zone=0, gg=0, xdata=EB BA 3F A1 76");
            byte[] xdata = new byte[5];
            xdata[0] = '\u00EB';
            // xdata[1] = 0xBA;
            // xdata[2] = 0x3F;
            // xdata[3] = 0xA1;
            // xdata[4] = 0x76;
            String sx = new String(xdata);
            String data = lib.pack2(1234, "861234567890", 666666, 3400, 0, 0, sx);
            try{
                byte[] bytes = data.getBytes(data);
                System.out.println("data=" + printable_bytes(bytes));
            }catch(UnsupportedEncodingException e){
                System.out.println("data=" + data);
            }
        }

        // test pack ack
        {
            System.out.println("testing packAck...");
            System.out.println("using seq=1234, acct=861234567890, ");
            String data = lib.packAck(1234, "861234567890");
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