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

    public native void testPrint();

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
    public native ParseResult parse(String pack, int pack_len);

    /*
    * @brief 将远程控制命令（不包含xdata）打包成网络传输数据
    * @param[in] seq 序号
    * @param[in] acct 主机账号
    * @param[in] ademco_id 安定宝ID
    * @param[in] ademco_event 安定宝事件码
    * @param[in] zone 防区号
    * @param[in] gg 分防区号
    * @return String
    */
    public native String pack(int seq, String acct, int ademco_id, int ademco_event, int zone, int gg);

    
    /*
    * @brief 将远程控制命令（包含xdata）打包成网络传输数据
    * @param[in] seq 序号
    * @param[in] acct 主机账号
    * @param[in] ademco_id 安定宝ID
    * @param[in] ademco_event 安定宝事件码
    * @param[in] zone 防区号
    * @param[in] gg 分防区号
    * @return hex String，如字符串"123456" 表示为 "313233343536", 需手动转换为byte[] 进行发送，参考SimpleServerThread
    */
    public native String pack2(int seq, String acct, int ademco_id, int ademco_event, int zone, int gg, String xdata);

    /*
    * @brief 打包ACK
    * @param[in] seq 序号
    * @param[in] acct 主机账号
    * @return String
    */
    public native String packAck(int seq, String acct);


    // public void testPrint(){ nativePrint(); }
    // public ParseResult parse(String pack, int pack_len){return nativeParse(pack, pack_len);}
    // public String pack(int seq, int ademco_id, int ademco_event, int zone, int gg){return nativePack(seq, ademco_id, ademco_event, zone, gg);}
    // public String packAck(int seq, int ademco_id){return nativePackAck(seq, ademco_id);}

}
