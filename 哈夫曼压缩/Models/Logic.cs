using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace 哈夫曼压缩.Models;
public class Compression
{
    [DllImport("Dll1.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern double compress(string filename, string compressedFilepath);
    [DllImport("Dll1.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void decompress(string filename, string decompressedFilepath);
    public async Task<double> Compress(string filename, string compressedFilepath)
    {
        return await Task.Run(() => compress(filename, compressedFilepath));
    }
    public async Task Decompress(string filename, string decompressedFilepath)
    {
        await Task.Run(() => decompress(filename, decompressedFilepath));
    }
}