package javademo.jni;
import java.io.*;
import java.net.*;
import java.util.regex.*;


public class SimpleServerThread extends Thread {
    private Socket socket;
    private AdemcoHbLibrary lib;
 
    public SimpleServerThread(Socket socket, AdemcoHbLibrary lib) {
        this.socket = socket;
        this.lib = lib;
    }
 
    public void run() {
        try {
            InputStream input = socket.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
 
            OutputStream output = socket.getOutputStream();
            PrintWriter writer = new PrintWriter(output, true);
 
 
            String line;
            // 创建 Pattern 对象
            String pattern1 = "\"(.+)\"(\\d{4}).+\\#(.+)\\[";
            Pattern r1 = Pattern.compile(pattern1);
            String pattern2 = "\\[\\#([\\d\\|a-fA-F]{6})\\|(\\d{4})\\s(\\d{2})\\s(\\d{3})\\]";
            Pattern r2 = Pattern.compile(pattern2);

            while(true){
                line = reader.readLine();
                if(line==null)continue;
                System.out.println(line);
                
                // 现在创建 matcher 对象
                Matcher m = r1.matcher(line);
                if (m.find()) {
                    String id = m.group(1);
                    int seq = Integer.parseInt(m.group(2));
                    int ademco_id = Integer.parseInt(m.group(3));
                    System.out.println("Found id: " + id);
                    System.out.println("Found seq: " + seq);
                    System.out.println("Found ademco_id: " + ademco_id); 

                    switch(id){
                        case "ACK":break;
                        case "NULL": // reply ack
                        {
                            String ack = lib.packAck(seq, ademco_id);
                            System.out.println("replying ack:" + ack);
                            writer.write(ack);
                            writer.flush();
                            break;
                        }
                        case "HENG-BO":
                        case "ADM-CID":
                        {
                            // reply ack
                            String ack = lib.packAck(seq, ademco_id);
                            System.out.println("replying ack:" + ack);
                            writer.write(ack);
                            writer.flush();

                            // handle event
                            m = r2.matcher(line);
                            if (m.find()) {
                                int ademco_id16 = Integer.parseInt(m.group(1), 16);
                                assert(ademco_id == ademco_id16);
                                int ademco_event = Integer.parseInt(m.group(2));
                                int gg = Integer.parseInt(m.group(3));
                                int zone = Integer.parseInt(m.group(4));
                                System.out.println("Found ademco_event: " + ademco_event);
                                System.out.println("Found gg: " + gg);
                                System.out.println("Found zone: " + zone); 
                            }else{
                                System.out.println("r2 NO MATCH");
                            }
                            
                            break;
                        }
                    
                    }
                } else {
                    System.out.println("r1 NO MATCH");
                }
            }
 
        } catch (IOException ex) {
            System.out.println("Server exception: " + ex.getMessage());
            ex.printStackTrace();
        }
    }
}