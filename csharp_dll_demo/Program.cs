using System;

using System.Runtime.InteropServices;

namespace csharp_dll_demo
{
    class Program
    {
        [DllImport(@"G:\dev_libs\ademco_hb\x64\Release\ademco_hb.dll",
            EntryPoint = "parse",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 parse(string buf, Int32 buf_len, ref Int32 commited);

        static void Main(string[] args) 
        {
            Console.WriteLine("Hello World!");

            //IntPtr commited = new IntPtr();
            string raw = "\nC5C30053\"HENG-BO\"0000R000000L000000#90219125916578[#000000|1737 00 000]_09:11:19,08-05-2019\r";
            Int32 commited = 0;
            byte[] data = System.Text.Encoding.Default.GetBytes(raw);
            Int32 res = parse(raw, raw.Length, ref commited);

            Console.WriteLine("res={0:D}, commited={1:D}", res, commited);
        }
    }
}
