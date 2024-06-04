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
                ademco_packet_t pkt = new ademco_packet_t();
                ademco_parse_result_t res = libademco.ademco_parse_packet(raw, (uint)raw.Length, pkt, cb, null);
                Debug.Assert(res == ademco_parse_result_t.ADEMCO_PARSE_RESULT_OK);
                Debug.Assert(libademco.size_tp_value(cb) == (uint)str.Length);
                Debug.Assert(pkt.crc == 0xC5C3);
                Debug.Assert(pkt.len == 0x0053);
                Debug.Assert(pkt.id == ademco_packet_id_t.AID_HB);
                Debug.Assert(pkt.seq == 0);
                Debug.Assert(pkt.acct == "90219125916578");
                Debug.Assert(pkt.dat.ademco_id == 0);
                Debug.Assert(pkt.dat.ademco_event == ademco_event_t.EVENT_I_AM_WIRE);
                Debug.Assert(pkt.dat.gg == 0);
                Debug.Assert(pkt.dat.zone == 0);
                Console.WriteLine("res={0:D}, commited={1:D}", res, libademco.size_tp_value(cb));
            }

            // test pack
            {
                Console.WriteLine("test pack");
                byte[] buff = new byte[1024];
                uint len = libademco.ademco_make_hb_packet(buff, 1024, 1, "861234567890", 666666, ademco_event_t.EVENT_ARM_AWAY, 0, 0, null);
                Debug.Assert(len > 0);
                Console.WriteLine("len={0:D}", len);
                libademco.ademco_print(buff, len);

                Console.WriteLine("test parse packed data");
                ademco_packet_t pkt = new ademco_packet_t();
                SWIGTYPE_p_size_t cb = libademco.new_size_tp();
                ademco_parse_result_t res = libademco.ademco_parse_packet(buff, len, pkt, cb, null);
                Debug.Assert(res == ademco_parse_result_t.ADEMCO_PARSE_RESULT_OK);
                Debug.Assert(libademco.size_tp_value(cb) == len);
                Debug.Assert(pkt.id == ademco_packet_id_t.AID_HB);
                Debug.Assert(pkt.seq == 1);
                Debug.Assert(pkt.acct == "861234567890");
                Debug.Assert(pkt.dat.ademco_id == 666666);
                Debug.Assert(pkt.dat.ademco_event == ademco_event_t.EVENT_ARM_AWAY);
                Debug.Assert(pkt.dat.gg == 0);
                Debug.Assert(pkt.dat.zone == 0);
            }

            // test pack
            // buff not enough
            {
                Console.WriteLine("test pack, buff not enough");
                Byte[] buff = new Byte[10];
                uint len = libademco.ademco_make_hb_packet(buff, 10, 1, "861234567890", 666666, ademco_event_t.EVENT_ARM_AWAY, 0, 0, null);
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
            ademco_packet_t pkt = new ademco_packet_t();
            SWIGTYPE_p_size_t cb = libademco.new_size_tp();
            byte[] buff = new byte[0];
            int nread = 0;
            uint ademco_id = 0;
            string acct = "";
            hb_machine_type_t type = hb_machine_type_t.HMT_INVALID;
            hb_machine_status_t status = hb_machine_status_t.HMS_INVALID;
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

                ademco_parse_result_t res = ademco_parse_result_t.ADEMCO_PARSE_RESULT_OK;
                while (res == ademco_parse_result_t.ADEMCO_PARSE_RESULT_OK)
                {
                    res = libademco.ademco_parse_packet(buff, (uint)buff.Length, pkt, cb, null);
                    switch (res)
                    {
                        case ademco_parse_result_t.ADEMCO_PARSE_RESULT_OK:
                            Console.Write("C:"); libademco.ademco_print(buff, libademco.size_tp_value(cb));
                            buff = eat(buff, (int)libademco.size_tp_value(cb));
                            switch (pkt.id)
                            {
                                case ademco_packet_id_t.AID_NULL:
                                    replyAck(stream, pkt.seq, pkt.acct);
                                    break;
                                case ademco_packet_id_t.AID_HB:
                                case ademco_packet_id_t.AID_ADM_CID:
                                    replyAck(stream, pkt.seq, pkt.acct);
                                    acct = pkt.acct;
                                    ademco_id = pkt.dat.ademco_id;
                                    if (libademco.ademco_is_machine_type_event(pkt.dat.ademco_event) != 0)
                                    {
                                        type = libademco.hb_machine_type_from_ademco_event(pkt.dat.ademco_event);
                                    }
                                    if (libademco.ademco_is_machine_status_event(pkt.dat.ademco_event) != 0)
                                    {
                                        status = libademco.hb_machine_status_from_ademco_event(pkt.dat.ademco_event);
                                    }

                                    // 演示如何进行布撤防，真实项目里可以删改本段                             
                                    if (++counter % 5 == 0)
                                    {
                                        if (status == hb_machine_status_t.HMS_ARM_STAY)
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
                        case ademco_parse_result_t.ADEMCO_PARSE_RESULT_ERROR:
                            buff = new byte[0];
                            break;
                    }
                }
            }
        }

        static void replyAck(NetworkStream stream, ushort seq, string acct)
        {
            byte[] buff = new byte[1024];
            uint len = libademco.ademco_make_ack_packet(buff, 1024, seq, acct, 0);
            Console.Write("S:");
            libademco.ademco_print(buff, len);
            stream.Write(buff, 0, (int)len);
        }

        static void sendArm(NetworkStream stream, ushort seq, string acct, uint ademco_id)
        {
            byte[] buff = new byte[1024];
            uint len = libademco.ademco_make_hb_packet(buff, 1024, seq, acct, ademco_id, ademco_event_t.EVENT_ARM_AWAY, 0, 0, null);
            Console.Write("S:");
            libademco.ademco_print(buff, len);
            stream.Write(buff, 0, (int)len);
        }

        static void sendDisarm(NetworkStream stream, ushort seq, string acct, uint ademco_id, string pwd)
        {
            byte[] buff = new byte[1024];
            ademco_xdata_t xdata = null;
            if (!string.IsNullOrEmpty(pwd))
            {
                xdata = new ademco_xdata_t();
                libademco.ademco_make_xdata(xdata,
                    ademco_xdata_length_format_t.ADEMCO_XDATA_LENGTH_FMT_TWO_HEX,
                    ademco_xdata_transform_t.ADEMCO_XDATA_TRANSFORM_AS_IS,
                    Encoding.ASCII.GetBytes(pwd),
                    (uint)pwd.Length);
            }
            uint len = libademco.ademco_make_hb_packet(buff, 1024, seq, acct, ademco_id, ademco_event_t.EVENT_DISARM, 0, 0, xdata);
            Console.Write("S:");
            libademco.ademco_print(buff, len);
            stream.Write(buff, 0, (int)len);
        }
    }
}
