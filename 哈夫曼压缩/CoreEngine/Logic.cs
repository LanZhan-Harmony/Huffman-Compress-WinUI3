using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace 哈夫曼压缩.CoreEngine;
public class Compression
{
    [DllImport("Dll1.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern double compress(string filename, string compressedFilepath);
    [DllImport("Dll1.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void decompress(string filename, string decompressedFilepath);
    public double Compress(string filename, string compressedFilepath)
    {
        return compress(filename, compressedFilepath);
    }
    public void Decompress(string filename, string decompressedFilepath)
    {
        decompress(filename, decompressedFilepath);
    }
}