using System;

using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.IO;
using System.Collections.Concurrent;
using com.hb3344.ademco;

namespace CSharpDemo
{
    class Program
    {
        static void Main(string[] args)
        {
            // test parse
            {
                Console.WriteLine("test parse");
                string str = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
                byte[] raw = Encoding.ASCII.GetBytes(str);
                SWIGTYPE_p_size_t cb = libademco.new_size_tp();
                AdemcoPacket pkt = new AdemcoPacket();
                AdemcoParseResult res = libademco.ademcoPacketParse(raw, (uint)raw.Length, pkt, cb, null);
                Debug.Assert(res == AdemcoParseResult.RESULT_OK);
                Debug.Assert(libademco.size_tp_value(cb) == (uint)str.Length);
                Debug.Assert(pkt.crc == 0xC5C3);
                Debug.Assert(pkt.len == 0x0053);
                Debug.Assert(pkt.id == AdemcoPacketId.AID_HB);
                Debug.Assert(pkt.seq == 0);
                Debug.Assert(pkt.acct == "90219125916578");
                Debug.Assert(pkt.data.ademcoId == 0);
                Debug.Assert(pkt.data.ademcoEvent == AdemcoEvent.EVENT_I_AM_WIRE_MACHINE);
                Debug.Assert(pkt.data.gg == 0);
                Debug.Assert(pkt.data.zone == 0);
                Console.WriteLine("res={0:D}, commited={1:D}", res, libademco.size_tp_value(cb));
            }

            // test pack
            {
                Console.WriteLine("test pack");
                byte[] buff = new byte[1024];
                uint len = libademco.ademcoMakeHbPacket(buff, 1024, 1, "861234567890", 666666, AdemcoEvent.EVENT_ARM, 0, 0, null);
                Debug.Assert(len > 0);
                Console.WriteLine("len={0:D}", len);
                libademco.ademcoPrint(buff, len);

                Console.WriteLine("test parse packed data");
                AdemcoPacket pkt = new AdemcoPacket();
                SWIGTYPE_p_size_t cb = libademco.new_size_tp();
                AdemcoParseResult res = libademco.ademcoPacketParse(buff, len, pkt, cb, null);
                Debug.Assert(res == AdemcoParseResult.RESULT_OK);
                Debug.Assert(libademco.size_tp_value(cb) == len);
                Debug.Assert(pkt.id == AdemcoPacketId.AID_HB);
                Debug.Assert(pkt.seq == 1);
                Debug.Assert(pkt.acct == "861234567890");
                Debug.Assert(pkt.data.ademcoId == 666666);
                Debug.Assert(pkt.data.ademcoEvent == AdemcoEvent.EVENT_ARM);
                Debug.Assert(pkt.data.gg == 0);
                Debug.Assert(pkt.data.zone == 0);
            }

            // test pack
            // buff not enough
            {
                Console.WriteLine("test pack, buff not enough");
                Byte[] buff = new Byte[10];
                uint len = libademco.ademcoMakeHbPacket(buff, 10, 1, "861234567890", 666666, AdemcoEvent.EVENT_ARM, 0, 0, null);
                Debug.Assert(len == 0);
            }

            Console.WriteLine("");
            simpleServer(12345);
        }

        static void simpleServer(int port)
        {
            Console.WriteLine("running simpleServer {0:D}", port);

            try
            {
                var listener = new TcpListener(IPAddress.Any, port);
                listener.Start();
                while (true)
                {
                    TcpClient client = listener.AcceptTcpClient();
                    Thread thread = new Thread(new ParameterizedThreadStart(clientHandler));
                    thread.Start(client);
                }

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        static byte[] append(byte[] origin, byte[] buf, int len)
        {
            byte[] dst = new byte[origin.Length + len];
            Array.ConstrainedCopy(origin, 0, dst, 0, origin.Length);
            Array.ConstrainedCopy(buf, 0, dst, origin.Length, len);
            return dst;
        }

        static byte[] eat(byte[] origin, int len)
        {
            if (origin.Length == len) { return new byte[0]; }
            byte[] dst = new byte[origin.Length - len];
            Array.ConstrainedCopy(origin, len, dst, 0, origin.Length - len);
            return dst;
        }

        static ushort nextSeq(ushort seq)
        {
            if (++seq == 10000) { seq = 1; }
            return seq;
        }

        static void clientHandler(object cli)
        {
            TcpClient client = (TcpClient)cli;
            NetworkStream stream = client.GetStream();
            AdemcoPacket pkt = new AdemcoPacket();
            SWIGTYPE_p_size_t cb = libademco.new_size_tp();
            byte[] buff = new byte[0];
            int nread = 0;
            uint ademco_id = 0;
            string acct = "";
            HbMachineType type = HbMachineType.HMT_INVALID;
            HbMachineStatus status = HbMachineStatus.HMS_INVALID;
            ushort seq = 0;
            int counter = 1;

            while (true)
            {
                byte[] msg = new byte[1024];
                try
                {
                    nread = stream.Read(msg, 0, 1024);
                }
                catch
                {
                    break;
                }
                if (nread == 0) { break; }
                buff = append(buff, msg, nread);

                AdemcoParseResult res = AdemcoParseResult.RESULT_OK;
                while (res == AdemcoParseResult.RESULT_OK)
                {
                    res = libademco.ademcoPacketParse(buff, (uint)buff.Length, pkt, cb, null);
                    switch (res)
                    {
                        case AdemcoParseResult.RESULT_OK:
                            Console.Write("C:"); libademco.ademcoPrint(buff, libademco.size_tp_value(cb));
                            buff = eat(buff, (int)libademco.size_tp_value(cb));
                            switch (pkt.id)
                            {
                                case AdemcoPacketId.AID_NULL:
                                    replyAck(stream, pkt.seq, pkt.acct);
                                    break;
                                case AdemcoPacketId.AID_HB:
                                case AdemcoPacketId.AID_ADM_CID:
                                    replyAck(stream, pkt.seq, pkt.acct);
                                    acct = pkt.acct;
                                    ademco_id = pkt.data.ademcoId;
                                    if (libademco.ademcoIsMachineTypeEvent(pkt.data.ademcoEvent) != 0)
                                    {
                                        type = libademco.hbMachineTypeFromAdemcoEvent(pkt.data.ademcoEvent);
                                    }
                                    if (libademco.ademcoIsMachineStatusEvent(pkt.data.ademcoEvent) != 0)
                                    {
                                        status = libademco.hbMachineStatusFromAdemcoEvent(pkt.data.ademcoEvent);
                                    }

                                    // 演示如何进行布撤防，真实项目里可以删改本段                             
                                    if (++counter % 5 == 0)
                                    {
                                        if (status == HbMachineStatus.HMS_ARM)
                                        {
                                            sendDisarm(stream, nextSeq(seq), acct, ademco_id, "123456");
                                        }
                                        else
                                        {
                                            sendArm(stream, nextSeq(seq), acct, ademco_id);
                                        }
                                    }

                                    break;
                                default:
                                    break;
                            }
                            break;
                        case AdemcoParseResult.RESULT_ERROR:
                            buff = new byte[0];
                            break;
                    }
                }
            }
        }

        static void replyAck(NetworkStream stream, ushort seq, string acct)
        {
            byte[] buff = new byte[1024];
            uint len = libademco.ademcoMakeAckPacket(buff, 1024, seq, acct, 0);
            Console.Write("S:");
            libademco.ademcoPrint(buff, len);
            stream.Write(buff, 0, (int)len);
        }

        static void sendArm(NetworkStream stream, ushort seq, string acct, uint ademcoId)
        {
            byte[] buff = new byte[1024];
            uint len = libademco.ademcoMakeHbPacket(buff, 1024, seq, acct, ademcoId, AdemcoEvent.EVENT_ARM, 0, 0, null);
            Console.Write("S:");
            libademco.ademcoPrint(buff, len);
            stream.Write(buff, 0, (int)len);
        }

        static void sendDisarm(NetworkStream stream, ushort seq, string acct, uint ademcoId, string pwd)
        {
            byte[] buff = new byte[1024];
            AdemcoXDataSegment xdata = null;
            if (!string.IsNullOrEmpty(pwd))
            {
                xdata = new AdemcoXDataSegment();
                libademco.ademcoMakeXData(xdata, AdemcoXDataLengthFormat.TWO_HEX, AdemcoXDataTransform.AdemcoXDataTransform_as_is, Encoding.ASCII.GetBytes(pwd), (uint)pwd.Length);
            }
            uint len = libademco.ademcoMakeHbPacket(buff, 1024, seq, acct, ademcoId, AdemcoEvent.EVENT_DISARM, 0, 0, xdata);
            Console.Write("S:");
            libademco.ademcoPrint(buff, len);
            stream.Write(buff, 0, (int)len);
        }
    }
}
