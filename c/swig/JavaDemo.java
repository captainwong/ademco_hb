

import java.io.*;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.*;
import com.hb3344.ademco.*;

public class JavaDemo {

    

    private Selector selector;
    private Map<SocketChannel, Client> clients;
    private InetSocketAddress listenAddr;

    public JavaDemo(String addr, int port) throws IOException{
        listenAddr = new InetSocketAddress(addr, port);
        clients = new HashMap<SocketChannel, Client>();
    }

    private void start() throws IOException{
        selector = Selector.open();
        ServerSocketChannel channel = ServerSocketChannel.open();
        channel.configureBlocking(false);
        channel.socket().bind(listenAddr);
        channel.register(selector, SelectionKey.OP_ACCEPT);
        System.out.println("Server started...");

        while(true){
            selector.select();
            Iterator keys = selector.selectedKeys().iterator();
            while(keys.hasNext()){
                SelectionKey key = (SelectionKey)keys.next();
                keys.remove();
                if(!key.isValid()) continue;
                if(key.isAcceptable()) accept(key);
                else if(key.isReadable()) read(key); 
            }
        }
    }

    private void accept(SelectionKey key) throws IOException{
        ServerSocketChannel sChannel = (ServerSocketChannel)key.channel();
        SocketChannel channel = sChannel.accept();
        channel.configureBlocking(false);
        Socket socket = channel.socket();
        SocketAddress addr = socket.getRemoteSocketAddress();
        System.out.println("Accepted new client from:" + addr);
        clients.put(channel, new Client(channel));
        channel.register(selector, SelectionKey.OP_READ);
    }

    private void read(SelectionKey key) throws IOException{
        SocketChannel channel = (SocketChannel)key.channel();
        ByteBuffer buf = ByteBuffer.allocate(1024);
        int n = channel.read(buf);
        if(n==-1){
            clients.get(channel).offline();
            clients.remove(channel);
            channel.close();
            key.cancel();
            return;
        }

        byte[] data = new byte[n];
        System.arraycopy(buf.array(), 0, data, 0, n);
        clients.get(channel).onMsg(data);
    }

    static {
        try {
            System.loadLibrary("ademco");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
            System.exit(1);
        }
    }

    public static String printable_bytes(byte[] b){
        return printable_bytes(b, b.length);
    }

    public static String printable_bytes(byte[] b, int length){
        String HEX_STRING = "0123456789ABCDEF";
        String s = "";
        for(int i = 0; i < length; i++){
            byte c = b[i];
            if(32 <= c && c <= 127){
                s += (char)c;
            }else{
                s += "\\x" + HEX_STRING.charAt((b[i] >>> 4) & 0x0F);
                s += HEX_STRING.charAt(b[i] & 0x0F);
            }
        }
        return s;
    }

    public static String printable_bytes(char[] b){
        String HEX_STRING = "0123456789ABCDEF";
        String s = "";
        for(int i = 0; i < b.length; i++){
            int c = b[i] & 0xFF;
            if(32 <= c && c <= 127){
                s += (char)c;
            }else{
                s += "\\x" + HEX_STRING.charAt(c >>> 4);
                s += HEX_STRING.charAt(c & 0x0F);
            }
        }
        return s;
    }

    public static String printable_bytes(String b){
        String HEX_STRING = "0123456789ABCDEF";
        String s = "";
        for(int i = 0; i < b.length(); i++){
            char c = b.charAt(i);
            if(32 <= c && c <= 127){
                s += (char)c;
            }else{
                s += "\\x" + HEX_STRING.charAt(c >>> 4);
                s += HEX_STRING.charAt(c & 0x0F);
            }
        }
        return s;
    }

    public static void test_libademco(){
        // test parse
        {
            System.out.println("testing parse...");
            String data = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
            System.out.println("using data=" + data);
            AdemcoPacket pkt = new AdemcoPacket();
            SWIGTYPE_p_int cb = libademco.new_intp();
            AdemcoParseResult res = libademco.ademcoPacketParse(data.getBytes(), data.length(), pkt, cb);
            assert(res == AdemcoParseResult.RESULT_OK);
            assert(libademco.intp_value(cb) == data.length());
            System.out.println("parse result=" + res + ",cb_commited=" + libademco.intp_value(cb));
            libademco.delete_intp(cb);
        }

        // test pack
        {            
            System.out.println("testing pack...");
            System.out.println("using seq=1234, acct=861234567890, ademco_id=666666, event=3400, zone=0, gg=0");
            AdemcoPacket pkt = new AdemcoPacket();
            libademco.ademcoMakeHbPacket2(pkt, 1234, "861234567890", 666666l, AdemcoEvent.EVENT_ARM, 0l, 0l, null);
            System.out.println("data=" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

        // test pack with xdata
        {
            System.out.println("testing pack with xdata...");
            System.out.println("using seq=1234, acct=861234567890, ademco_id=666666, event=3400, zone=0, gg=0, xdata=123456");
            AdemcoXDataSegment xdata = new AdemcoXDataSegment();
            libademco.ademcoMakeXData(xdata, AdemcoXDataLengthFormat.TWO_HEX, AdemcoXDataTransform.AdemcoXDataTransform_as_is, "123456".getBytes(), 6);
            AdemcoPacket pkt = new AdemcoPacket();
            libademco.ademcoMakeHbPacket2(pkt, 1234, "861234567890", 666666l, AdemcoEvent.EVENT_ARM, 0l, 0l, xdata);
            System.out.println("data=" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

        // test with hex xdata
        {
            System.out.println("testing pack with hex xdata...");
            System.out.println("using seq=1234, acct=861234567890, ademco_id=666666, event=3400, zone=0, gg=0, xdata=EB BA 3F A1 76");
            byte[] xdatacontent = new byte[5];
            xdatacontent[0] = (byte)0xEB;
            xdatacontent[1] = (byte)0xBA;
            xdatacontent[2] = (byte)0x3F;
            xdatacontent[3] = (byte)0xA1;
            xdatacontent[4] = (byte)0x76;
            AdemcoXDataSegment xdata = new AdemcoXDataSegment();
            libademco.ademcoMakeXData(xdata, AdemcoXDataLengthFormat.TWO_HEX, AdemcoXDataTransform.AdemcoXDataTransform_as_is, xdatacontent, 5);
            AdemcoPacket pkt = new AdemcoPacket();
            libademco.ademcoMakeHbPacket2(pkt, 1234, "861234567890", 666666l, AdemcoEvent.EVENT_ARM, 0l, 0l, xdata);
            System.out.println("data=" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

        // test pack ack
        {
            System.out.println("testing pack ack...");
            System.out.println("using seq=1234, acct=861234567890");
            AdemcoPacket pkt = new AdemcoPacket();
            libademco.ademcoMakeAckPacket2(pkt, 1234, "861234567890", 0l);
            System.out.println("data=" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

        // test pack ack without acct
        {
            System.out.println("testing pack ack without acct...");
            System.out.println("using seq=1234, acct=\"\" ademco_id=666666");
            AdemcoPacket pkt = new AdemcoPacket();
            libademco.ademcoMakeAckPacket2(pkt, 1234, "", 666666l);
            System.out.println("data=" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

    }

    public static void main(String[] args) throws Exception {
        
        JavaDemo.test_libademco();
        int port = 12345;
        if (args.length > 0){
            port = Integer.parseInt(args[0]);
        }

        new JavaDemo("localhost", port).start();
    }

    public class Client {
        SocketChannel channel;
        byte[] buf = new byte[0];
        AdemcoPacket pkt = new AdemcoPacket();
        SWIGTYPE_p_int cb = libademco.new_intp();
        HbMachineType type = HbMachineType.HMT_INVALID;
        HbMachineStatus status = HbMachineStatus.HMS_INVALID;
        int seq = 0;
        long ademco_id = 0;
        String acct = "";
        long lastTimeStatusChange = 0;

        public Client(SocketChannel channel){
            this.channel = channel;
        }

        public void offline(){
            System.out.println(id() + " is offline");
            libademco.delete_intp(cb);
        }

        public void onMsg(byte[] data) throws IOException {
            buf = append(buf, data);
            //System.out.println(printable_bytes(buf));
            AdemcoParseResult res = libademco.ademcoPacketParse(buf, buf.length, pkt, cb);
            System.out.println(res + ", " + libademco.intp_value(cb));
            switch(res) {
                case RESULT_OK:
                    buf = trim(buf, libademco.intp_value(cb));
                    handleMsg();
                    break;
                case RESULT_NOT_ENOUGH:
                    break;
                case RESULT_ERROR:
                    buf = new byte[0];
                    break;
            }
        }

        private byte[] getRawWithLen(){
            byte[] b = new byte[pkt.getRaw_len()];
            System.arraycopy(pkt.getRaw(), 0, b, 0, pkt.getRaw_len());
            return b;
        }

        private String id(){
            String str = "Client " + channel.socket().getRemoteSocketAddress();
            if(!acct.isEmpty()){
                str += " acct=" + acct;
            }
            if(ademco_id != 0){
                str += " ademco_id=" + ademco_id;
            }
            return str;
        }

        private int nextSeq(){
            seq++;
            if(seq == 10000){
                seq = 1;
            }
            return seq;
        }

        private void handleMsg() throws IOException {
            System.out.println(id() + " R:" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
            switch(pkt.getId()){
                case AID_NULL:
                    acct = pkt.getAcct();
                    replyAck();
                    break;
    
                case AID_ACK:
                    break;
    
                case AID_HB:
                case AID_ADM_CID:
                    acct = pkt.getAcct();
                    ademco_id = pkt.getData().getAdemcoId();
                    AdemcoEvent event = pkt.getData().getAdemcoEvent();
                    System.out.println(id() + " event=" + libademco.ademcoEventToString(event) + " zone=" + pkt.getData().getZone());
                    if(libademco.ademcoIsMachineStatusEvent(event) != 0){
                        status = libademco.hbMachineStatusFromAdemcoEvent(event);
                        // test arm/disarm
                        long now = System.currentTimeMillis();
                        if(now - lastTimeStatusChange > 5000){
                            lastTimeStatusChange = now;
                            if(status == HbMachineStatus.HMS_ARM){
                                disarm("123456");
                            }else{
                                arm();
                            }
                        }
                    }
                    if(libademco.ademcoIsMachineTypeEvent(event) != 0){
                        type = libademco.hbMachineTypeFromAdemcoEvent(event);
                    }
                    replyAck();
                    if(type == HbMachineType.HMT_INVALID && !acct.isEmpty() && ademco_id != 0){
                        askType();
                    }
                    break;    
            }
        }

        private void replyAck() throws IOException {
            libademco.ademcoMakeAckPacket2(pkt, pkt.getSeq(), pkt.getAcct(), 0l);
            channel.write(ByteBuffer.wrap(getRawWithLen()));
            System.out.println(id() + " S:" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

        private void askType() throws IOException {
            libademco.ademcoMakeHbPacket2(pkt, nextSeq(), acct, ademco_id, AdemcoEvent.EVENT_WHAT_IS_YOUR_TYPE, 0L, 0L, null);
            channel.write(ByteBuffer.wrap(getRawWithLen()));
            System.out.println(id() + " S:" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

        private void arm() throws IOException{
            libademco.ademcoMakeHbPacket2(pkt, nextSeq(), acct, ademco_id, AdemcoEvent.EVENT_ARM, 0L, 0L, null);
            channel.write(ByteBuffer.wrap(getRawWithLen()));
            System.out.println(id() + " S:" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }

        private void disarm(String pwd) throws IOException {
            AdemcoXDataSegment xdata = null;
            if(pwd != null && !pwd.isEmpty()){
                xdata = new AdemcoXDataSegment();
                libademco.ademcoMakeXData(xdata, AdemcoXDataLengthFormat.TWO_HEX, AdemcoXDataTransform.AdemcoXDataTransform_as_is, pwd.getBytes(), pwd.length());
            }
            libademco.ademcoMakeHbPacket2(pkt, nextSeq(), acct, ademco_id, AdemcoEvent.EVENT_DISARM, 0L, 0L, xdata);
            channel.write(ByteBuffer.wrap(getRawWithLen()));
            System.out.println(id() + " S:" + printable_bytes(pkt.getRaw(), pkt.getRaw_len()));
        }
    }

    public static char[] append(char[]origin, char[] buf){
        char[] dst = new char[origin.length + buf.length];
        System.arraycopy(origin, 0, dst, 0, origin.length);
        System.arraycopy(buf , 0, dst, origin.length, buf.length);
        return dst;
    }

    public static byte[] append(byte[]origin, byte[] buf){
        byte[] dst = new byte[origin.length + buf.length];
        System.arraycopy(origin, 0, dst, 0, origin.length);
        System.arraycopy(buf , 0, dst, origin.length, buf.length);
        return dst;
    }

    public byte[] trim(byte[] origin, int pos){
        byte[] dst = new byte[origin.length - pos];
        System.arraycopy(origin, pos, dst, 0, origin.length - pos);
        return dst;
    }

    public static byte[] c2b(char[] arr){
        byte[] b = new byte[arr.length];
        for(int i = 0; i < arr.length; i++){
            b[i] = (byte)arr[i];
        }
        return b;
    }

    public static char[] b2c(byte[] arr){
        char[] b = new char[arr.length];
        for(int i = 0; i < arr.length; i++){
            b[i] = (char)arr[i];
        }
        return b;
    }
}