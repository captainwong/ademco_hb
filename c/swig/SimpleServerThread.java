import java.io.*;
import java.net.*;
import java.util.Arrays;
import java.util.regex.*;
import com.hb3344.ademco.AdemcoPacket;
import com.hb3344.ademco.AdemcoPacketId;
import com.hb3344.ademco.AdemcoParseResult;
import com.hb3344.ademco.SWIGTYPE_p_int;
import com.hb3344.ademco.libademco;


public class SimpleServerThread extends Thread {
    private Socket socket;
    private long lastTimeStatusChange = 0;
 
    public SimpleServerThread(Socket socket) {
        this.socket = socket;
    }

    /**
	 * 16进制表示的字符串转换为字节数组
	 *
	 * @param s 16进制表示的字符串
	 * @return byte[] 字节数组
	 */
	public static byte[] hexStringToByteArray(String s) {
	    int len = s.length();
	    byte[] b = new byte[len / 2];
	    for (int i = 0; i < len; i += 2) {
	        // 两位一组，表示一个字节,把这样表示的16进制字符串，还原成一个字节
	        b[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character
	                .digit(s.charAt(i + 1), 16));
	    }
	    return b;
    }
    

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

    
 
    public void run() {
        try {
            InputStream input = socket.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
 
            OutputStream output = socket.getOutputStream();
            PrintWriter writer = new PrintWriter(output, true);
 
 
            // String line;
            // // 创建 Pattern 对象
            // String pattern1 = "\"(.+)\"(\\d{4}).+\\#(.+)\\[";
            // Pattern r1 = Pattern.compile(pattern1);
            // String pattern2 = "\\[\\#([\\d\\|a-fA-F]{6})\\|(\\d{4})\\s(\\d{2})\\s(\\d{3})\\]";
            // Pattern r2 = Pattern.compile(pattern2);

            //char[] cache = new char[0];

            while(true){
                String line = reader.readLine();
                
                System.out.println(line);
                //cache = append(cache, b2c(line.getBytes()));
                SWIGTYPE_p_int cb = libademco.new_intp();
                AdemcoPacket pkt = new AdemcoPacket();
                AdemcoParseResult res = libademco.ademcoPacketParse(line.getBytes(), line.length(), pkt, cb);
                System.out.println(res);
                switch(res) {
                    case RESULT_OK:

                        //cache = Arrays.copyOfRange(cache, libademco.intp_value(cb), cache.length - libademco.intp_value(cb));
                        handle(pkt, writer);
                        break;
                    case RESULT_NOT_ENOUGH:
                        break;
                    case RESULT_ERROR:
                        //cache = new char[0];
                        break;
                }













                //line = reader.readLine();
                //if(line==null)continue;
                //System.out.println(line);


                
                // // 现在创建 matcher 对象
                // Matcher m = r1.matcher(line);
                // if (m.find()) {
                //     String id = m.group(1);
                //     int seq = Integer.parseInt(m.group(2));
                //     String acct = m.group(3);
                //     System.out.println("Found id: " + id);
                //     System.out.println("Found seq: " + seq);
                //     System.out.println("Found acct: " + acct); 

                //     switch(id){
                //         case "ACK":break;
                //         case "NULL": // reply ack
                //         {
                //             String ack = lib.packAck(seq, acct);
                //             System.out.println("replying ack:" + ack);
                //             writer.write(ack);
                //             writer.flush();
                //             break;
                //         }
                //         case "HENG-BO":
                //         case "ADM-CID":
                //         {
                //             // reply ack
                //             String ack = lib.packAck(seq, acct);
                //             System.out.println("replying ack:" + ack);
                //             writer.write(ack);
                //             writer.flush();

                //             // handle event
                //             m = r2.matcher(line);
                //             if (m.find()) {
                //                 int ademco_id16 = Integer.parseInt(m.group(1), 16);
                //                 int ademco_event = Integer.parseInt(m.group(2));
                //                 int gg = Integer.parseInt(m.group(3));
                //                 int zone = Integer.parseInt(m.group(4));
                //                 System.out.println("Found ademco_event: " + ademco_event);
                //                 System.out.println("Found gg: " + gg);
                //                 System.out.println("Found zone: " + zone); 

                //                 // 主机状态改变间隔超过5秒，则触发一次远程控制命令发送给主机
                //                 if(ademco_event == 3400 || ademco_event == 1400){
                //                     long now = System.currentTimeMillis();
                //                     if(now - lastTimeStatusChange > 5000){
                //                         lastTimeStatusChange = now;
                //                         if(ademco_event == 3400){ // 布防则发撤防命令
                //                             String cmd = lib.pack2(seq+1, acct, ademco_id16, 1400, 0, 0, "123456");
                //                             byte[] data = hexStringToByteArray(cmd);
                //                             System.out.println("sending 1400 command:" + printable_bytes(data));
                //                             output.write(data);
                //                             output.flush();
                //                         }else{ // 撤防就发布防命令
                //                             String cmd = lib.pack(seq+1, acct, ademco_id16, 3400, 0, 0);
                //                             System.out.println("sending 3400 command:" + cmd);
                //                             writer.write(ack);
                //                             writer.flush();
                //                         }
                                        
                                        
                //                     }
                //                 }
                //             }else{
                //                 System.out.println("r2 NO MATCH");
                //             }
                            
                //             break;
                //         }
                    
                //     }
                // } else {
                //     System.out.println("r1 NO MATCH");
                // }
            }
 
        } catch (IOException ex) {
            System.out.println("Server exception: " + ex.getMessage());
            ex.printStackTrace();
        }
    }

    public void handle(AdemcoPacket pkt, PrintWriter writer){
        switch(pkt.getId()){
            case AID_NULL:
                libademco.ademcoMakeAckPacket2(pkt, pkt.getSeq(), pkt.getAcct(), 0l);
                writer.write(b2c(pkt.getRaw()), pkt.getRaw_len(), 0);
                break;

            case AID_ACK:
                break;

            case AID_HB:
            case AID_ADM_CID:

        }
    }
}