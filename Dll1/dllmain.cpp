// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <filesystem>
using namespace std;

struct HuffmanNode
{
	unsigned char data;
	unsigned long long weight;
	HuffmanNode* left, * right;
	bool isLeaf;
	string password;
	HuffmanNode(unsigned char data = 0, unsigned long long weight = 0, HuffmanNode* left = nullptr, HuffmanNode* right = nullptr, bool isLeaf = true, string password = "") : data(data), weight(weight), left(left), right(right), isLeaf(isLeaf), password(password) {}
};

class compare
{
public:
	bool operator()(HuffmanNode* a, HuffmanNode* b) // 重载比较运算符,用于优先队列,权值小的优先级高
	{
		return a->weight > b->weight;
	}
};

class Compression
{
public:
	double Compress(string filename, string compressedFilepath);
	void Decompress(string filename, string decompressedFilepath);

protected:
	string source_string;                             // 源字符串
	string binary_string;                             // 二进制字符串
	map<unsigned char, unsigned long long> weightmap; // 权值map
	map<unsigned char, string> passwordmap;           // 密码map
	map<string, int> compresspasswordmap;
	priority_queue<HuffmanNode*, vector<HuffmanNode*>, compare> HuffmanTree; // 哈夫曼树优先队列

	int binaryStringtoInt(const string& binaryString); // 二进制字符串转十进制
	string inttoBinaryString(int n);                   // 十进制转二进制字符串

	void Weightmap_Init(ifstream& in);                           // 初始化权值map
	HuffmanNode* MakeHuffmanNode(long long i);                   // 创建哈夫曼节点
	void HuffmanTreeQueue_Init();                                // 初始化哈夫曼树
	void HuffmanTree_Init();                                     // 初始化哈夫曼树
	void PasswordMap_Init(HuffmanNode*& root, string& password); // 初始化密码map
	void BinaryString_Init();                                    // 初始化二进制字符串
	void DEL(HuffmanNode*& root);                                // 删除哈夫曼树
};

int Compression::binaryStringtoInt(const string& binarystring) // 二进制字符串转十进制码
{
	int decimal = 0;
	for (char c : binarystring)
	{
		decimal = (decimal << 1) + (c - '0'); // 乘以2，然后加上当前位的值
	}
	return decimal;
}

string Compression::inttoBinaryString(int n) // 十进制转二进制字符串
{
	string binaryString;
	while (n > 0) // 除2取余法
	{
		binaryString.insert(binaryString.begin(), '0' + (n & 1));
		n >>= 1;
	}
	while (binaryString.size() < 8) // 补0
	{
		binaryString.insert(binaryString.begin(), '0');
	}
	return binaryString;
}

void Compression::Weightmap_Init(ifstream& in) // 初始化权值map
{
	const int bufferSize = 1024; // 缓冲区大小
	char buffer[bufferSize];
	while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0)
	{
		for (int i = 0; i < in.gcount(); i++)
		{
			unsigned char ch = buffer[i];
			source_string += ch;
			weightmap[ch]++;
		}
		in.clear(); // 清除EOF标志，以便读取最后一部分的数据
	}
}

HuffmanNode* Compression::MakeHuffmanNode(long long i) // 创建哈夫曼节点
{
	HuffmanNode* node = new HuffmanNode(i, weightmap[i]);
	return node;
}

void Compression::HuffmanTreeQueue_Init() // 初始化哈夫曼树队列
{
	for (auto i : weightmap)
	{
		HuffmanNode* node = MakeHuffmanNode(i.first);
		HuffmanTree.push(node);
	}
}

void Compression::HuffmanTree_Init() // 构建哈夫曼树
{
	while (HuffmanTree.size() > 1)
	{
		HuffmanNode* left = HuffmanTree.top();
		HuffmanTree.pop();
		HuffmanNode* right = HuffmanTree.top();
		HuffmanTree.pop();
		HuffmanNode* node = new HuffmanNode(0, left->weight + right->weight, left, right, false); // 非叶子节点
		HuffmanTree.push(node);
	}
}

void Compression::PasswordMap_Init(HuffmanNode*& root, string& password) // 初始化密码map
{
	if (root != nullptr && root->isLeaf) // 叶子节点
	{
		root->password = password;
		passwordmap[root->data] = password;
	}
	if (root->left != nullptr)
	{
		PasswordMap_Init(root->left, password += "0"); // 递归左子树,密码加0
		password.pop_back();// 删除最后一个字符，原因是递归左子树后，password多了一个字符
	}
	if (root->right != nullptr)
	{
		PasswordMap_Init(root->right, password += "1"); // 递归右子树,密码加1
		password.pop_back();
	}
}

void Compression::BinaryString_Init() // 初始化二进制字符串
{
	for (int i = 0; i < source_string.size(); i++)
	{
		binary_string += passwordmap[source_string[i]]; // 把密码map中的值加入二进制字符串
	}
}

void Compression::DEL(HuffmanNode*& root)
{
	if (root == nullptr)
	{
		return;
	}
	if (root->left != nullptr)
	{
		DEL(root->left);
	}
	if (root->right != nullptr)
	{
		DEL(root->right);
	}
	delete root;
	root = nullptr;
}

unsigned long long getFileSize(const filesystem::path& filename) // 获取文件大小
{
	ifstream in(filename, ios::binary);
	if (!in.is_open())
	{
		throw "未找到文件";
	}
	return filesystem::file_size(filename);
}

string getUniqueFileName(const string& filename, const string& path, const string& extension) // 获取唯一的解压文件名
{
	string decompressFileName = path + '\\' + filename + extension;
	int i = 1;
	while (filesystem::exists(decompressFileName))
	{
		decompressFileName = path + '\\' + filename + "(" + to_string(i) + ")" + extension;
		i++;
	}
	return decompressFileName;
}

double Compression::Compress(string filename, string compressedFilepath) // 压缩
{
	ifstream in(filename, ios::binary);
	if (!in.is_open())
	{
		throw "未找到文件";
	}

	unsigned long long size1 = getFileSize(filename); // 获取文件大小
	filesystem::path name = filename;
	string extension = name.extension().string();                                                 // 获取文件扩展名
	int extensionLength = extension.size();                                                       // 获取扩展名长度
	string compressedFile = getUniqueFileName(name.stem().string(), compressedFilepath, ".huff"); // 获取唯一的压缩文件名

	Weightmap_Init(in);      // 初始化权值map
	HuffmanTreeQueue_Init(); // 初始化哈夫曼树队列
	HuffmanTree_Init();      // 构建哈夫曼树
	HuffmanNode* root = HuffmanTree.top();
	HuffmanTree.pop();
	string password = "";
	PasswordMap_Init(root, password); // 初始化密码map
	BinaryString_Init();              // 初始化二进制字符串
	in.close();

	ofstream out(compressedFile, ios::binary);
	if (!out.is_open())
	{
		throw "压缩失败，请检查文件夹是否有写入权限";
	}

	out.write((const char*)&extensionLength, sizeof(extensionLength)); // 写入扩展名长度
	out.write(extension.c_str(), extension.size());                     // 写入扩展名

	int passwordmapsize = passwordmap.size();
	out.write((const char*)&passwordmapsize, sizeof(passwordmapsize)); // 写入map长度

	for (auto i : passwordmap)
	{
		int first = i.first;
		out.write((const char*)&first, sizeof(first)); // 写入权值map键
		int secondsize = i.second.size();
		out.write((const char*)&secondsize, sizeof(secondsize)); // 写入权值map值长度
		int n = 8 - (secondsize % 8);                             // n表示map值补0长度
		if (n)
		{
			i.second.append(n, '0');
		}
		for (int j = 0; j < i.second.size(); j += 8) // 注意secondsize已更新
		{
			string temp = i.second.substr(j, 8); // substr(j, 8)表示从第j个字符开始截取8个字符
			unsigned char tempint = binaryStringtoInt(temp);
			out.write((const char*)&tempint, sizeof(tempint)); // 写入权值map值
		}
	}

	int n = 8 - (binary_string.size() % 8); // n表示二进制字符串补0长度
	if (n)
	{
		binary_string.append(n, '0');
	}
	out.write((const char*)&n, sizeof(n)); // 写入补0长度

	for (long long i = 0; i < binary_string.size(); i += 8)
	{
		string temp = binary_string.substr(i, 8); // substr(j, 8)表示从第j个字符开始截取8个字符
		int tempint = binaryStringtoInt(temp);    // 二进制字符串转十进制
		unsigned char tempchar = (unsigned char)tempint;
		out.write((const char*)&tempchar, sizeof(tempchar)); // 写入压缩后的二进制字符串
	}

	unsigned long long size2 = getFileSize(compressedFile);
	out.close();
	DEL(root);
	return (double)size2 / size1 * 100;
}

void Compression::Decompress(string filename, string decompressedFilepath) // 解压
{
	ifstream in(filename, ios::binary);
	if (!in.is_open())
	{
		throw "未找到文件";
	}
	filesystem::path name = filename;

	int extensionLength;
	in.read((char*)&extensionLength, sizeof(extensionLength)); // 读取扩展名长度
	char* extension = new char[extensionLength + 1];
	in.read(extension, extensionLength); // 读取扩展名
	extension[extensionLength] = '\0';
	string decompressedFile = getUniqueFileName(name.stem().string(), decompressedFilepath, extension); // 获取唯一的解压文件名

	int passwordmapsize;
	in.read((char*)&passwordmapsize, sizeof(passwordmapsize)); // 读取map长度

	for (int i = 0; i < passwordmapsize; i++)
	{
		int key;
		in.read((char*)&key, sizeof(key)); // 读取权值map键
		int length;
		in.read((char*)&length, sizeof(length)); // 读取权值map值长度
		string temp;
		for (int j = 0; j <= length / 8; j++)
		{
			unsigned char tempint; // 读取权值map值
			in.read((char*)&tempint, sizeof(tempint));
			temp += inttoBinaryString((int)tempint);
		}
		temp.erase(length, temp.size() - length + 1); // 删除多余的0
		compresspasswordmap[temp] = key;              // 构建解压密码map
	}

	int zerosize;
	in.read((char*)&zerosize, sizeof(zerosize)); // 读取补0长度

	string compressstring;
	while (in.peek() != EOF) // 读取压缩后的二进制字符串
	{
		unsigned char tempchar;
		in.read((char*)&tempchar, sizeof(tempchar));
		compressstring += inttoBinaryString((int)tempchar);
	}
	compressstring.erase(compressstring.size() - zerosize, zerosize); // 删除多余的0

	in.close();

	ofstream out(decompressedFile, ios::binary);
	if (!out.is_open())
	{
		throw "解压失败，请检查文件夹是否有写入权限";
	}
	string str;
	for (long long i = 0; i < compressstring.size(); i++)
	{
		str += compressstring[i];
		auto it = compresspasswordmap.find(str);
		if (it != compresspasswordmap.end())
		{
			out.write((const char*)&it->second, 1);
			str.clear();
		}
	}
	out.close();
	delete[] extension;
}

bool compareFiles(const string& filePath1, const string& filePath2) // 比较两个文件是否相同
{
	ifstream file1(filePath1, ios::binary);
	ifstream file2(filePath2, ios::binary);

	if (file1.fail())
	{
		throw "未找到文件1";
	}
	if (file2.fail())
	{
		throw "未找到文件2";
	}

	// 检查文件大小
	unsigned long long size1 = getFileSize(filePath1);
	unsigned long long size2 = getFileSize(filePath2);
	if (size1 != size2)
	{
		return false;
	}

	// 逐字节比较
	return equal(istreambuf_iterator<char>(file1.rdbuf()), istreambuf_iterator<char>(), istreambuf_iterator<char>(file2.rdbuf()));
}

extern "C" _declspec(dllexport) double compress(const char* filename, const char* compressedFilepath) // 压缩
{
	Compression compression;
	
	string filenameStr = filename;
	string compressedFilepathStr = compressedFilepath;
	return compression.Compress(filenameStr, compressedFilepathStr);
}

extern "C" _declspec(dllexport) void decompress(const char* filename, const char* decompressedFilepath) // 解压
{
	Compression compression;
	
	string filenameStr = filename;
	string decompressedFilepathStr = decompressedFilepath;
	compression.Decompress(filenameStr, decompressedFilepathStr);
}
