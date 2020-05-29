package javademo.jni;

import java.nio.file.FileSystems;

public class AdemcoHbLibrary
{
    static
    {        
        if (System.getProperty("os.name").startsWith("Windows")) { // Windows
            System.load(
                FileSystems.getDefault()
                    .getPath("./x64/Release/ademco_java.dll")  // Dynamic link
                    .normalize().toAbsolutePath().toString());
        }else{ // Unix based 
            System.load(
                FileSystems.getDefault()
                    .getPath("./build/libademcojava.so")  // Dynamic link
                    .normalize().toAbsolutePath().toString());
        }
    }

    private native void nativePrint();

    public class ParseResult {
        // 0 解析到正确、完整的一包；
        // 1 解析到正确、不完整的一包，需要缓存数据，下次继续处理；
        // 2 解析到错误的数据，需要清空缓冲区
        public int result; 
        // 可以正确解析的包长（单次函数调用最多解析一包）
        public int cb_commited;
    }

    /*
    * @brief 解析网络数据
    * @param[in] pack 网络数据流
    * @param[in] pack_len 网络数据流有效长度
    * @return ParseResult
    */
    private static native ParseResult nativeParse(String pack, int pack_len);

    /*
    * @brief 将远程控制命令打包成网络传输数据
    * @param[in] seq 序号
    * @param[in] ademco_id 安定宝ID
    * @param[in] ademco_event 安定宝事件码
    * @param[in] zone 防区号
    * @param[in] gg 分防区号
    * @return String
    */
    private static native String nativePack(int seq, int ademco_id, int ademco_event, int zone, int gg);

    /*
    * @brief 打包ACK
    * @param[in] seq 序号
    * @param[in] ademco_id 安定宝ID
    * @return String
    */
    private static native String nativePackAck(int seq, int ademco_id);


    public void testPprint(){ nativePrint(); }
    public ParseResult parse(String pack, int pack_len){return nativeParse(pack, pack_len);}
    public String pack(int seq, int ademco_id, int ademco_event, int zone, int gg){return nativePack(seq, ademco_id, ademco_event, zone, gg);}
    public String packAck(int seq, int ademco_id){return nativePackAck(seq, ademco_id);}

    public static void main(String[] args){
        AdemcoHbLibrary lib = new AdemcoHbLibrary();
        lib.testPprint();

        String data = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
        ParseResult res = lib.parse(data, data.length());
        assert(res.result == 0);
        assert(res.cb_commited == data.length());
        System.out.println("parse result=" + res.result + ",cb_commited=" + res.cb_commited);
    }
    

}
