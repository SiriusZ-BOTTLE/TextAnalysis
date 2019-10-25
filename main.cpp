#include <iostream>

#include "TextAnalysis.h"
//#include "CodeConvert.h"

using namespace std;

//测试文本
//本来我是不想写这种完全没有意义的文字的,但是由于需要测试一下算法,只能继续这样无聊的工作.我不知道还需要写多少个字才能让"最短编辑距离"算法消耗的时间稍微长一点
//其实有的时候吧,你会发现无聊的工作做得多了也就没什么感觉了,沉浸在繁忙做功中虽然很累,但是至少不空虚
//以下是机翻
std::string str1 =
u8"I didn't want to write something that didn't make any sense,\n but I had to test the algorithm,\n\
and I didn't know how many words I needed to write to make the minimum edit distance algorithm take a little longer";
std::string str10 = u8"本来我是不想写这种完全没有意义的文字的,但是由于需要测试一下算法,只能继续这样无聊的工作.我不知道还需要写多少个字才能让\"最短编辑距离\"算法消耗的时间稍微长一点.";



std::string str2 =
u8"In fact, sometimes, you will find that the boring work to do more is no sense of feeling,\
immersed in the busy work although very tired, but at least not empty";

std::string str20 = u8"其实有的时候吧,你会发现无聊的工作做得多了也就没什么感觉了,沉浸在繁忙做功中虽然很累,但是至少不空虚.";


std::string str3 =
u8"I didn't want to write those pointless word, but I had to test the algorithm,\
and I didn't know how many words I needed to write to make the minimum edit distance algorithm take a little longer time.";

std::string str4 = u8"你可能没有告诉过我你的名字,but我认识你.我想和你讲讲 pencil 的故事.";
std::string str5 = u8"从前有一只笔,它的名字是pencil!";
std::string str6 = u8"嘿嘿嘿哈哈哈嘻嘻嘻.";
std::string str7 = u8"你可能没有告诉过我你的名字.";

std::string str8 = u8"铅笔.我  和你.你和 他...它 和她...铅笔和橡皮.";

std::string str100 = u8"其实一开始要写这个测试文本,我是拒绝的.我不能从早肝到晚上,又是框架设计,又是代码实现,又是找BUG,现在又来写测试文本.但是没有办法,没有其他人来写啊,那我只能顶着极高的自闭值,亲自写这一份测试文本.话说BUG真的挺多的.";
std::string str101 = u8"其实一开始要写这个测试文本,我是不想写的.我不能从早忙到晚上,又是框架设计,又是代码实现,又是找BUG,现在又来写测试文本.但是没有办法,没有其他人来写啊,那我只能压制住难过的情绪,亲自写这一份测试文本.话说BUG真的挺多的.";

std::string str110 = u8"我有一个朋友老王,他很聪明.";
std::string str111 = u8"我还有一个朋友老孙,他很漂亮.";

std::string str120 = u8"陈明安走向收银台,拿出手机在扫描区一挥,像往常一样走出了图书室.\
天色已暗, 城市上空巨大的全息投影穹顶和大楼的墙面循环播放着各种广告.但你若是足够仔细, 就能发现角落里微不足道的一串数字, 显示19点36分.\
每天工作结束, 陈明安总会到\"木制品\"来, 读半个小时书.\"木制品\"是陈明安家楼下的一家沉浸式电子阅读室, 在这里看书体验非常好, 平时也没什么人, 非常安静, 而且有茶水供应.阅读累了还能靠在窗边的木椅子上, 听着舒缓的音乐, 嘬两口盐茶, 一呆就是一下午.\
老惯例, 每次 << 高玩EX >> 出新期刊的时候, 陈明安总是到这里来.陈不会整个读一遍, 不过是看看新游戏介绍, 读一读自己感兴趣的游戏攻略.今天, << 边土 >> 这款游戏再次出现在陈眼前.\
游戏本体售价接近两千, 还需要购买额外的神经连接装置, 重要的是, 加上自己一直也在肝别的游戏, 不然咱也早就入坑了, 陈明安心想.但是看着网上传出的游戏实景视频, 逼真的物理效果, 游戏场景100% 可自定义, 作为一名打了10多年游戏的骨灰瘾君子, 也是很想体验一番.\
凌晨, 陈明安盯着电脑显示屏上的游戏 << DustLand >> 的购买页, 在操控版上划来划去, 不时嘀咕两声.游戏主体买断制1898, 加上连接器898元不到3000, 这差不多将近陈明安两个月的工资.据说连接器是高端货, 带有最新的感官模拟, 兼容很多市面上的其他游戏, 而且支持的游戏时间比达到了惊人的8:1.\
自从2037年第一款神经连接器问世以来, 几年内游戏用神经连接技术的发展越来越迅速, 其中BI(Blueintelligence)公司于2043年发布的BlueHelmetV0更是作为里程碑式的新技术, 该连接器可以让大脑在操控阶段进入类似睡眠状态, 对于游戏应用方面完全是一大福音-- - 持有连接器的玩家可以在晚上边睡觉边打游戏了!";

std::string str121 = u8"陈明安走向收银台,面带微笑,拿出手机在扫描区一扫,走出了木制品图书室.\
天色已经昏暗, 城市上空巨大的全息投影穹顶和大楼的墙面循环播放着各色广告.但你若是仔细观察, 就能发现角落里微不足道的一串数字, 显示19点36分.\
每天工作结束, 陈明安总会到\"木制品\"来, 读半个小时书.\"木制品\"是陈明安家楼下的一家沉浸式电子阅读室, 在这里看书体验非常好, 平时也没什么人, 非常安静, 而且有茶水供应.阅读累了还能靠在窗边的木椅子上, 听着舒缓的音乐, 嘬两口红茶或者大麦茶, 一呆就是一整天.\
老惯例, 每次 << 高玩EX >> 出新期刊的时候, 陈明安总是到这里.陈不会把杂志整个读一遍, 无非是看看新游戏介绍, 读一读自己感兴趣的游戏攻略.今天, << 边土 >> 这款游戏再次出现在陈眼前.\
游戏本体售价接近两千, 还需要购买额外的神经连接装置, 重要的是, 加上自己一直也在玩别的游戏, 不然咱也早就入坑了, 陈明安心想.但是看着网上传出的游戏实景视频, 逼真的物理效果, 游戏场景100% 可自定义, 作为一名打了10多年游戏的骨灰瘾君子, 也是很想体验一番.\
凌晨, 陈明安盯着电脑显示屏上的游戏 << DustLand >> 的购买页, 在操控版上划来划去, 不时嘀咕两声.游戏主体买断制1898, 带上连接器898元不到3000, 这差不多将近陈明安两个月的工资.据说连接器是同类别中最新的高端货, 带有新一代的感官模拟, 兼容很多市面上的其他游戏, 而且支持的游戏时间比达到了令人吃惊的8:1.\
自从2037年第一款神经连接器出现以来, 几年内游戏用神经连接技术的发展越来越迅速, 其中BI(Blueintelligence)公司于2043年发布的BlueHelmetV0更是作为里程碑式的新技术, 该连接器可以让大脑在操控阶段进入类似睡眠状态, 对于游戏应用方面完全是一大福音-- - 持有连接器的玩家可以在晚上边睡觉边打游戏了!";

std::string str130 = u8"陈明安走向收银台,拿出手机在扫描区一挥,像往常一样走出了图书室,\
天色已暗, 城市上空巨大的全息投影穹顶和大楼的墙面循环播放着各种广告, 但你若是足够仔细, 就能发现角落里微不足道的一串数字, 显示19点36分,\
每天工作结束, 陈明安总会到\"木制品\"来, 读半个小时书, \"木制品\"是陈明安家楼下的一家沉浸式电子阅读室, 在这里看书体验非常好, 平时也没什么人, 非常安静, 而且有茶水供应, 阅读累了还能靠在窗边的木椅子上, 听着舒缓的音乐, 嘬两口盐茶, 一呆就是一下午,\
老惯例, 每次 << 高玩EX >> 出新期刊的时候, 陈明安总是到这里来, 陈不会整个读一遍, 不过是看看新游戏介绍, 读一读自己感兴趣的游戏攻略, 今天, << 边土 >> 这款游戏再次出现在陈眼前,\
游戏本体售价接近两千, 还需要购买额外的神经连接装置, 重要的是, 加上自己一直也在肝别的游戏, 不然咱也早就入坑了, 陈明安心想, 但是看着网上传出的游戏实景视频, 逼真的物理效果, 游戏场景100% 可自定义, 作为一名打了10多年游戏的骨灰瘾君子, 也是很想体验一番,\
凌晨, 陈明安盯着电脑显示屏上的游戏 << DustLand >> 的购买页, 在操控版上划来划去, 不时嘀咕两声, 游戏主体买断制1898, 加上连接器898元不到3000, 这差不多将近陈明安两个月的工资, 据说连接器是高端货, 带有最新的感官模拟, 兼容很多市面上的其他游戏, 而且支持的游戏时间比达到了惊人的8:1,\
自从2037年第一款神经连接器问世以来, 几年内游戏用神经连接技术的发展越来越迅速, 其中BI(Blueintelligence)公司于2043年发布的BlueHelmetV0更是作为里程碑式的新技术, 该连接器可以让大脑在操控阶段进入类似睡眠状态, 对于游戏应用方面完全是一大福音-- - 持有连接器的玩家可以在晚上边睡觉边打游戏了!";

std::string str131 = u8"陈明安走向收银台,面带微笑,拿出手机在扫描区一扫,走出了木制品图书室,\
天色已经昏暗, 城市上空巨大的全息投影穹顶和大楼的墙面循环播放着各色广告, 但你若是仔细观察, 就能发现角落里微不足道的一串数字, 显示19点36分,\
每天工作结束, 陈明安总会到\"木制品\"来, 读半个小时书, \"木制品\"是陈明安家楼下的一家沉浸式电子阅读室, 在这里看书体验非常好, 平时也没什么人, 非常安静, 而且有茶水供应, 阅读累了还能靠在窗边的木椅子上, 听着舒缓的音乐, 嘬两口红茶或者大麦茶, 一呆就是一整天,\
老惯例, 每次 << 高玩EX >> 出新期刊的时候, 陈明安总是到这里, 陈不会把杂志整个读一遍, 无非是看看新游戏介绍, 读一读自己感兴趣的游戏攻略, 今天, << 边土 >> 这款游戏再次出现在陈眼前,\
游戏本体售价接近两千, 还需要购买额外的神经连接装置, 重要的是, 加上自己一直也在玩别的游戏, 不然咱也早就入坑了, 陈明安心想, 但是看着网上传出的游戏实景视频, 逼真的物理效果, 游戏场景100% 可自定义, 作为一名打了10多年游戏的骨灰瘾君子, 也是很想体验一番,\
凌晨, 陈明安盯着电脑显示屏上的游戏 << DustLand >> 的购买页, 在操控版上划来划去, 不时嘀咕两声, 游戏主体买断制1898, 带上连接器898元不到3000, 这差不多将近陈明安两个月的工资, 据说连接器是同类别中最新的高端货, 带有新一代的感官模拟, 兼容很多市面上的其他游戏, 而且支持的游戏时间比达到了令人吃惊的8:1,\
自从2037年第一款神经连接器出现以来, 几年内游戏用神经连接技术的发展越来越迅速, 其中BI(Blueintelligence)公司于2043年发布的BlueHelmetV0更是作为里程碑式的新技术, 该连接器可以让大脑在操控阶段进入类似睡眠状态, 对于游戏应用方面完全是一大福音-- - 持有连接器的玩家可以在晚上边睡觉边打游戏了!";





std::string str140 = u8"这里是模板文本段零.这里是模板文本段一.你好啊我是瓶子君.这里是模板文本端二";
std::string str141 = u8"这里是模板文本段零.乐于助人的好人.这里是模板文本段一.这里是模板文本端二";
std::string str142 = u8"这里是模板文本段零.这里是模板文本段一.这里是模板文本端二";//模板

std::string str150 = u8"一二三四五六七  九十";
std::string str151 = u8"一二三四五六七八九十";
std::string str152 = u8"一二三四五六七八九十";//模板

std::string str160 = u8"以下直到句点之前,都是模板文本的内容了,模板必须稍微长一点,比较混杂一点,这样就能看出来程序的问题.以下是常规文本内容,这里的文本不应该被忽略";
std::string str161 = u8"以下直到句点之前,都是模板文本的内容了,模板必须长一点,混杂一点,这样才能看出来程序的问题.以下是常规文本内容,这里的文本不应该被忽略";
std::string str162 = u8"以下直到句点之前,都是模板文本的内容了,模板必须长一点,混杂一点,这样才能看出来程序的问题.";//模板

std::string str170 = u8"这是一个BUG检测语句,需要携带English";
std::string str171 = u8"这是一个BUG检测语句,需要包含English";
std::string str172 = u8"这是一个检测语句,需要";


void test_NLPIR(void)
{
	//using namespace std;

	//if (NLPIR_Init(DataPath.c_str(),GBK_CODE, ""))
	//{
	//	printf("初始化成功");
	//	printf("\n\n");
	//}
	//else
	//{
	//	printf("初始化失败\n");
	//	printf("按任意键退出...\n");
	//	getchar();
	//	return 0;
	//}

	//string sSrc = "我可能还没跟你讲过 pencil 的故事";

	//printf("原文: \n%s", sSrc.c_str());
	//printf("\n");

	//// 分词测试
	//string cutWord = NLPIR_ParagraphProcess(sSrc.c_str(), 1);
	//printf("\n");
	//printf("分词结果：%s", cutWord.c_str());
	//printf("\n\n");

	//int count = 0;
	//const result_t* result = NLPIR_ParagraphProcessA(sSrc.c_str(), &count);
	//for (size_t i = 0; i < count; i++)
	//{
	//	printf("<STA>%3d <LEN>%3d <WTS>%3s <WTN>%3d <WID>%3d <UDir>%3d <WEI>%3d\n", result[i].start, result[i].length, result[i].sPOS,
	//		result[i].iPOS, result[i].word_ID, result[i].word_type, result[i].weight);
	//}

	//// 获取新词测试
	///*string newWord = NLPIR_GetNewWords(sSrc.c_str(), 10, false);
	//printf("获取新词：%s", newWord.c_str());
	//printf("\n\n");*/

	//// 词频统计
	//string wordFreq = NLPIR_WordFreqStat(sSrc.c_str());
	//printf("词频结果如下：%s", wordFreq.c_str());
	//printf("\n\n");

	//// 关键词
	///*string keyWords = NLPIR_GetKeyWords(sSrc.c_str(), 10, true);
	//printf("关键词：%s", keyWords.c_str());
	//printf("\n\n");*/

	//// 添加用户词典测试
	////int addDictNum = NLPIR_ImportUserDict("./test/dict.txt", true);
	////printf("导入用户词典单词个数：%d", addDictNum);
	////printf("\n\n");

	//// 删除单词测试
	///*string sDelWord = "中蒙";
	//NLPIR_DelUsrWord(sDelWord.c_str());
	//printf("该词已经被删除：%s", sDelWord.c_str());
	//printf("\n\n");*/


	//NLPIR_Exit();
	//getchar();
}


void test_JieBa()
{
	using namespace std;
	using namespace Paths_JieBa;
	cppjieba::Jieba jieba(DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH);
	vector<string> words;
	vector<cppjieba::Word> jiebawords;
	string s;
	string result;

	s = u8"他来到了网易杭研大厦";
	cout << s << endl;
	cout << "[demo] Cut With HMM" << endl;
	jieba.Cut(s, words, true);
	cout << limonp::Join(words.begin(), words.end(), "/") << endl;

	cout << "[demo] Cut Without HMM " << endl;
	jieba.Cut(s, words, false);
	cout << limonp::Join(words.begin(), words.end(), "/") << endl;

	s = u8"我来到北京清华大学";
	cout << s << endl;
	cout << "[demo] CutAll" << endl;
	jieba.CutAll(s, words);
	cout << limonp::Join(words.begin(), words.end(), "/") << endl;

	s = u8"小明硕士毕业于中国科学院计算所，后在日本京都大学深造";
	cout << s << endl;
	cout << "[demo] CutForSearch" << endl;
	jieba.CutForSearch(s, words);
	cout << limonp::Join(words.begin(), words.end(), "/") << endl;

	cout << "[demo] Insert User Word" << endl;
	jieba.Cut(u8"男默女泪", words);
	cout << limonp::Join(words.begin(), words.end(), "/") << endl;
	jieba.InsertUserWord(u8"男默女泪");
	jieba.Cut(u8"男默女泪", words);
	cout << limonp::Join(words.begin(), words.end(), "/") << endl;

	cout << "[demo] CutForSearch Word With Offset" << endl;
	jieba.CutForSearch(s, jiebawords, true);
	cout << jiebawords << endl;

	cout << "[demo] Lookup Tag for Single Token" << endl;
	const int DemoTokenMaxLen = 32;
	char DemoTokens[][DemoTokenMaxLen] = { u8"拖拉机", "CEO", "123", "�?" };
	vector<pair<string, string> > LookupTagres(sizeof(DemoTokens) / DemoTokenMaxLen);
	vector<pair<string, string> >::iterator it;
	for (it = LookupTagres.begin(); it != LookupTagres.end(); it++) {
		it->first = DemoTokens[it - LookupTagres.begin()];
		it->second = jieba.LookupTag(it->first);
	}
	cout << LookupTagres << endl;

	cout << "[demo] Tagging" << endl;
	vector<pair<string, string> > tagres;
	s = u8"我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰�?";
	jieba.Tag(s, tagres);
	cout << s << endl;
	cout << tagres << endl;

	cout << "[demo] Keyword Extraction" << endl;
	const size_t topk = 5;
	vector<cppjieba::KeywordExtractor::Word> keywordres;
	jieba.extractor.Extract(s, keywordres, topk);
	cout << s << endl;
	cout << keywordres << endl;
	return;
}


int test_string()
{
	using namespace std;

	string str0 = "0123456789abcdef";
	string str1 = "23";


	//printf("\n\n%d\n", str0.find(str1, 2));
	//printf("%d", sizeof(int));

	return 0;
}


int test_TA()
{
	using namespace TextAnalysis;
	using namespace std;
	using namespace std::chrono;
	
	//u8".铅笔.是.非常好的工具.你和他.她和他,然而我不会用.."

	string str = readTextFile("import/test.txt");//读取文本

	//cout << str.size() << endl;
	//cout << str << endl;
	
	TextStream ts1(str);//默认UTF-8
	TextStream ts2(str);
	double res = 0;

	try
	{
		//Settings::seqSim_imp_customized = DefinitionsSet::Implements_NumberSequenceSimilarity::Jaccard;
		//Template::setTemplate(str130);
		//Settings::templateOn = true;

		//Template::setTemplate(str162);
		//Settings::template_enable = true;

		ts1.init();//初始化
		ts2.init();//初始化
		TextAnalysor::setTextStream0(ts1);
		TextAnalysor::setTextStream1(ts2);

		
		//cout << endl << ts1.getUnitNumOutsideTemplate() << " " << ts2.getUnitNumOutsideTemplate() << endl;
		//cout << endl << ts1 << endl << ts2<<endl;
		//cout << ts1.vector_segmentsInfo;

		auto start =steady_clock::now();
		res = TextAnalysor::getSimilarity();//获得相似度
		auto end = steady_clock::now();

		auto tt = duration_cast<milliseconds>(end - start);

		printf("\n%.2lf%%\n",res*100);

		cout << "\nTime:\n" << tt.count()<<"ms";

	}
	catch (exception &e)
	{
		cout << e.what() << endl;
	}
	
	return 0;
}

void test_TA2()
{
	using namespace TextAnalysis;//导入命名空间
	

	TextStream ts0;
	ts0.setText(str151);

	ts0.init();

	cout << ts0 << endl;


	ts0.reset();

	ts0.setText(str101);

	ts0.init();

	cout << ts0;

}

void test_TA3()
{
	using namespace TextAnalysis;//导入命名空间


	try
	{
		string str="1\n\n\n2\n34\n\n5";
		

		str = readTextFile("import/test.txt");
		//cout << str;

		TextStream ts0(str), ts1(str);
		//ts0.setText(str);
		ts0.init();
		//ts1.setText(str);
		ts1.init();

		cout << ts0 << endl;
		//cout << ts1 << endl;

		TextAnalysor::setTextStream0(ts0);
		TextAnalysor::setTextStream1(ts1);


		//cout << ts0.vector_segmentsInfo.size();

		cout << TextAnalysor::getSimilarity() << endl;


		//cout << ts0.vector_segmentsInfo.max_size()<<endl;
		//cout << ts0.vector_segmentsInfo.size() << endl;

	}
	catch (exception &e)
	{
		cout << "exception occ" << endl;
		cout << e.what();
	}


}


void test_E()
{
	using namespace TextAnalysis;//导入命名空间


	try
	{
		vector<int > v_i{1,2,3};
		cout << v_i[100] << endl;

		int array_i[5] = { 1,2,3,4,5 };

		cout << array_i[10];
	}
	catch (exception & e)
	{	
		cout << e.what();
	}
	catch (out_of_range & oor)
	{
		cout << oor.what();
	}
	catch (...)
	{
		cout << "exception occ" << endl;
	}
}


void test_CodeConvert()
{
	//string str = "你好,这里是GBK编码的中文字符,但是经程序转换为utf-8编码";
	//string str_utf8;
	//str_utf8 = GbkToUtf8(str.c_str());
	//system("chcp 65001");//切换控制台编码
	//cout << str_utf8 << endl;
	//cout << "这里是GBK编码abcdefg";
}


void test_self()
{
	using namespace cppjieba;
	using namespace Paths_JieBa;//导入结巴命名空间
	//分词对象
	Jieba jieba1
	(
		DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH
	);
	vector<pair<string, string> > tagres;
	vector<cppjieba::Word>vec;

	string str;
	//str = u8"我说你啊      哈哈哈哈,    thank you!";
	str = str101;

	jieba1.Tag(str, tagres);

	//cout<<jieba1.LookupTag(u8"他");

	//cout <<"Tag"<< tagres<<endl;
	//jieba1.CutForSearch(str, vec, true);

	//cout <<"Cut For Search"<< str << endl;


	/*for (int i = 0; i < vec.size(); i++)
	{
		cout
			<< "<word> " << vec[i].word
			<< " <offset> " << vec[i].offset
			<< " <unicode_offset> " << vec[i].unicode_offset
			<< " <unicode_length> " << vec[i].unicode_length
			<< " <WT> " << tagres[i].second << endl;
	}*/

}


void test_DS()
{
	using namespace TextAnalysis;
	using namespace TextAnalysis::DependenciesSet;

	try
	{


		//NumberSequence ns0 = {1,1,2,3,4};
		//NumberSequence ns1 = { 1,2,3,3,5 };

		////比较三种序列相似度算法的结果
		//cout
		//	<< cal_sequenceSimilarity_BinaryScan(ns0, ns1) << endl
		//	<< cal_sequenceSimilarity_Levenshtein(ns0, ns1) << endl//最小编辑距离相似度
		//	<< cal_sequenceSimilarity_Jaccard(ns0, ns1) << endl;//杰卡德距离相似度

		/*NumberSequence ns;
		int size;

		string text = u8"你好哈哈 Hallo guys";

		size = getCount_codeUnit_in_utf8_text(text);
		cout << size << endl;
		ns.assign(size, 0);

		transformText_to_NumberSequence(text, ns);

		cout << ns;*/


		SimilaritySequence ss0{ 1,-1 };
		SimilaritySequence ss1{ 1,-1,-1 };

		cout << cal_result_SimilaritySequence_summary_IGN(ss0, ss1);

	}
	catch (exception &e)
	{
		cout << e.what();
	}

}


void test_time()
{
	using namespace std;
	using namespace std::chrono;

	vector<string> v0;
	vector<string> v1;

	auto start = steady_clock::now();

	//经过时间测试发现,assign和resize效率差异不大(都挺慢的,涉及到内存申请)
	v0.assign(1000, "x");
	//v0.resize(1000, "x");

	auto end = steady_clock::now();
	auto tt = duration_cast<microseconds>(end - start);

	cout << "\nTime:" << tt.count() << "ms";

}


void test_template()
{
	using namespace TextAnalysis;


	Template::setTemplate(u8"(0) I don't know what to say. But i want to help you, do you trust me?");

	const string& str = Template::getTemplate();
	cout << str<<endl;

	cout<< Template::getNumberSequence()<<endl;

}


void test_seprators()
{
	using namespace TextAnalysis;

	auto& seps_c = separators_customized;
	auto& seps_d = separators_default;

	//cout << int(separators_default[6].str_separator[1]);

	string str{ "\x01\x1f" };

	cout << (int)(str[1]);


}


/**********************************<main>***************************************/
int main(void)
{
	system("chcp 65001");//切换控制台编码至UTF-8
	std::ios::sync_with_stdio(false);//解除绑定,加速输出
	//test_NLPIR();
	test_TA();
	//test_TA2();
	//test_TA3();
	//test_string();
	//test_CodeConvert();
	//test_JieBa();
	//test_self();
	//test_DS();
	//test_template();
	//test_time();

	//test_E();
	//test_seprators();
	return 0;
}


