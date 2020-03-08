using System;

using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;

namespace csharp_dll_demo
{
    class Program
    {
        [DllImport(@"G:\dev_libs\ademco_hb\x64\Release\ademco_hb.dll",
            EntryPoint = "parse",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 parse(string buf, Int32 buf_len, ref Int32 commited);

        [DllImport(@"G:\dev_libs\ademco_hb\x64\Release\ademco_hb.dll",
            EntryPoint = "pack",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 pack(ref byte buf, Int32 buf_len, Int32 seq, Int32 ademco_id, Int32 ademco_event, Int32 zone, Int32 gg);

        [DllImport(@"G:\dev_libs\ademco_hb\x64\Release\ademco_hb.dll",
            EntryPoint = "pack_ack",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 pack_ack(ref byte buf, Int32 buf_len, Int32 seq, Int32 ademco_id);

        static void Main(string[] args) 
        {
            Console.WriteLine("Hello World!");

            // test parse
            {
                Console.WriteLine("test parse");
                string raw = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
                Int32 commited = 0;
                Int32 res = parse(raw, raw.Length, ref commited);
                Console.WriteLine("res={0:D}, commited={1:D}", res, commited);
                Console.WriteLine("parse ademco_id/event, etc.");
                string pattern = @"\[\#(?<ademco_id>\d{6})\|(?<ademco_event>\d{4})\s(?<gg>\d{2})\s(?<zone>\d{3})\]";
                foreach (Match match in Regex.Matches(raw, pattern))
                {
                    GroupCollection groups = match.Groups;
                    foreach(Group group in groups)
                    {
                        Console.WriteLine(group.Name + " " + group.Value);
                    }
                }
                Console.WriteLine("");
            }

            // test pack
            {
                Console.WriteLine("test pack");
                Byte[] buff = new Byte[1024];
                Int32 res = pack(ref buff[0], 1024, 1, 666666, 1400, 0, 0);
                Console.WriteLine("res={0:D}", res);
                string str = System.Text.Encoding.Default.GetString(buff, 0, res);
                Console.WriteLine(str);

                Console.WriteLine("test parse packed data");
                Int32 commited = 0;
                res = parse(str, str.Length, ref commited);
                Console.WriteLine("res={0:D}, commited={1:D}", res, commited);
                Console.WriteLine(str);
                Console.WriteLine("");
            }

            // test pack
            // buff not enough
            {
                Console.WriteLine("test pack, buff not enough");
                Byte[] buff = new Byte[10];
                Int32 res = pack(ref buff[0], 10, 1, 666666, 1400, 0, 0);
                Console.WriteLine("res={0:D}", res);
            }

            Console.WriteLine("");
            simpleServer(12345);
        }

        public static void simpleServer(int port)
        {
            Console.WriteLine("running simpleServer {0:D}", port);

            try
            {
                Socket serverSocekt = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                serverSocekt.Bind(new IPEndPoint(IPAddress.Any, port));
                serverSocekt.Listen(5);

                Socket clientSocket = serverSocekt.Accept();
                Console.WriteLine("client " + clientSocket.RemoteEndPoint.ToString());

                while (true)
                {
                    byte[] buff = new byte[4096];
                    int len = clientSocket.Receive(buff);
                    Int32 commited = 0;
                    string data = System.Text.Encoding.Default.GetString(buff, 0, len);
                    Int32 res = parse(data, len, ref commited);
                    if(res == 0)
                    {
                        string pattern = "\"(?<id>.+)\"(?<seq>\\d{4}).+\\#(?<ademco_id>.+)\\[";
                        Match match = Regex.Matches(data, pattern)[0];
                        string id = match.Groups["id"].Value;
                        int seq = Int32.Parse(match.Groups["seq"].Value);
                        int ademco_id = Int32.Parse(match.Groups["ademco_id"].Value, System.Globalization.NumberStyles.HexNumber);

                        // reply ack
                        {
                            Byte[] sendBuff = new Byte[1024];
                            res = pack_ack(ref sendBuff[0], 1024, seq, ademco_id);                            
                            clientSocket.Send(sendBuff, 0, res, SocketFlags.None);                            
                        }

                        // handle event
                        if (id == "HENG-BO")
                        {
                            pattern = @"\[\#(?<ademco_id>\d{6})\|(?<ademco_event>\d{4})\s(?<gg>\d{2})\s(?<zone>\d{3})\]";
                            Console.WriteLine(Regex.Matches(data, pattern)[0].ToString());
                        }
                    }
                }

            }catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

    }
}
