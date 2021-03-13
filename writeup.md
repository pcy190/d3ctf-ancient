### ancient
这道题的主要思路来自`算术编码`，论文出处:
Said, Amir. "Introduction to arithmetic coding-theory and practice." Hewlett Packard Laboratories Report (2004)
https://www.hpl.hp.com/techreports/2004/HPL-2004-76.pdf

具体实现参考
https://github.com/nibrunie/ArithmeticCoding

可以通过下面的链接了解算术编码
https://zhuanlan.zhihu.com/p/23834589

程序首先验证56位的flag长度，然后将前五个字符通过fnv1a哈希与`d3ctf`的哈希对比，作为最基础的判断。

而后将输入的字符串append到一个固定字符串`This is an ancient string, it represents the origin of all binary characters, isn't it. Let me see, it says 0,1,2,3,4,5,6,7...`后面，然后调用`encode_value_with_update`函数。因为此处的算术编码采用动态编码表，即编码表分布与传入字符串有关。但是这里的分布编码表实际是通过这个固定字符串的得到的，所以对我们输入flag进行编码的时候，每次使用的都是同一个编码表，可以视为静态编码表。即通过这个126个字符长的固定字符串确定分布表，而后对输入的字符串进行分配。

因此一种可行的思路是通过爆破每一个输入字符的方式来得到flag。

题目中所有用到字符串的地方都进行了编译期的字符串保护。通过模板类，将字符串异或同一个key来存储。包括最后要对比的数据，也进行了这样的编译期保护。这个可以通过静态分析得到key然后手动xor还原，也可以动态调试dump出来。

程序最后逐位对比数据，若178个字符正确(即固定的126个字符+56个输入字符编码后的字符182个去除末尾4个无用的编码0)，则正确。

其中，在对比的时候，通过loop_for_junk(loc_401820)来略微增加难度。loop_for_junk函数其实没有实际效果，其间加了花指令并循环多次，用以抵抗符号执行的爆破。loop_for_junk传入的index指针在执行后，其index值实际上并没有被修改，可以直接patch掉。

如果能够发现是算术编码的实现，则可以通过dump出对比表，然后再次传入解码
动态dump可知最后的对比数据为
```
char target[]="\x54\x67\x69\x73\x20\x69\x73\x20\x61\x6e\x20\x61\x6e\x63\x69\x65\x6e\x74\x20\x73\x74\x72\x69\x6e\x67\x2c\x20\x69\x74\x20\x72\x65\x70\x72\x65\x73\x65\x6e\x74\x73\x20\x74\x68\x65\x20\x6f\x72\x69\x67\x69\x6e\x20\x6f\x66\x20\x61\x6c\x6c\x20\x62\x69\x6e\x61\x72\x79\x20\x63\x68\x61\x72\x61\x63\x74\x65\x72\x73\x2c\x20\x69\x73\x6e\x27\x74\x20\x69\x74\x2e\x20\x4c\x65\x74\x20\x6d\x65\x20\x73\x65\x65\x2c\x20\x69\x74\x20\x73\x61\x79\x73\x20\x30\x2c\x31\x2c\x32\x2c\x33\x2c\x34\x2c\x35\x2c\x36\x2c\x37\x2e\x2e\x2e\x67\xf3\xa3\xca\x23\x58\xa3\xd1\xf8\xc1\x96\xe3\xd7\x85\x85\xfe\xbe\x7b\xd2\x82\x59\xf4\xd8\xf0\x5f\xf5\xe2\x55\xe5\x2c\x14\xdc\xd6\xf4\x60\xf9\x89\x84\x0c\x70\x50\xb8\xf5\xde\x7f\xff\x5a\xc8\x8d\x61\xf0\x02\x00\x00\x00\x00\x00\x00";
```
```
void writeup(){
   unsigned char *decomp = static_cast<unsigned char *>(malloc(sizeof(unsigned char) * local_size * 2));
   cout<<endl;
   ac_state_t encoder_state;
   init_state(&encoder_state, 16);
   const int update_range = strlen(MAGIC_STRING), range_clear = 0;
   reset_uniform_probability(&encoder_state);
   decode_value_with_update(decomp, target, &encoder_state, local_size, update_range, range_clear);
   cout << decomp << endl;
}
```

另一方面，预期解也可以是爆破，每次传入新的一位，而后在0x401C83下断点，然后比较index(rbp+var_14)的变化，如果index比上一次增加了，那么这次的字符就是正确的。以此类推。