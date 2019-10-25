#pragma once//仅加载一遍
#pragma warning(disable:4996)
//标准C++库
#include <vector>//向量
#include <string>//字符串
#include <iostream>//输入输出流
#include <sstream>
#include <chrono>//万能的C++11啊,强大的时钟库,精确到纳秒级靓仔有没有!
#include <ratio>




//宏定义
#define DLL_EXPORTS//导出DLL
#define INSIDE_DLL//DLL内部

#ifdef __linux__
	#ifdef DLL_EXPORTS
		#undef DLL_EXPORTS
	#endif
#endif

#ifdef DLL_EXPORTS
	#ifdef INSIDE_DLL
		#define TEXT_ANALYSIS_EXPORT __declspec(dllexport)
	#else
		#define TEXT_ANALYSIS_EXPORT __declspec(dllimport)
	#endif
#else
	#define TEXT_ANALYSIS_EXPORT
#endif


//结巴分词
#include "include/cppjieba/Jieba.hpp"






//结巴分词库依赖文件路径
namespace Paths_JieBa
{
	using std::string;

	extern const string dirPath_dict;

	extern const string DICT_PATH;
	extern const string HMM_PATH;
	extern const string USER_DICT_PATH;
	extern const string IDF_PATH;
	extern const string STOP_WORD_PATH;

	//const char* const DICT_PATH = "./lib/cppjieba/dict/jieba.dict.utf8";
	//const char* const HMM_PATH = "./lib/cppjieba/dict/hmm_model.utf8";
	//const char* const USER_DICT_PATH = "./lib/cppjieba/dict/user.dict.utf8";
	//const char* const IDF_PATH = "./lib/cppjieba/dict/idf.utf8";
	//const char* const STOP_WORD_PATH = "./lib/cppjieba/dict/stop_words.utf8";
}


/********************************************************************************
DocumentInfo

Copyright:
Author:瓶子
Date:2019.07.13-2019.09.16
Description:
	文本处理工具
********************************************************************************/

/********************************************************************************
Optimization(优化):
	[1]	初始化文本流的时候先生成文本流对应的数字序列,避免比较时从重复生成
	[2]	所有vecotr的数值录入都先开空间后赋值,提升速度
********************************************************************************/

/********************************************************************************
*TextAnalysis:文本处理工具总命名空间
	尽量遵循模块化设计以取得较好的可扩展性和可维护性(实现得还不错)
********************************************************************************/
namespace TextAnalysis
{
	using namespace std;
	using namespace cppjieba;
	using namespace Paths_JieBa;
	
	extern cppjieba::Jieba jieba;//结巴分词工具对象

	/********************************************************************************
	*定义集合
	*	该命名空间内定义一些基本数据结构和别名
	********************************************************************************/
	namespace DefinitionsSet
	{
		/****************************************
		*编码类型
		*	列举一些基本分类.
		*
		****************************************/
		enum EncodingType
		{
			GBK = 0,//GBK编码
			UTF_8 = 1,//UTF-8编码(默认)
			ASCII//ASCII编码
		};

		/****************************************
		*词类
		*	列举一些基本分类.
		*	考虑到中文有很多种词性,这里选用一些主要类型,
		*	其他类型将被忽略.
		*	该枚举类提供了相对简洁的功能,
		*	分段表中的数组大小依赖End_WorldClass成员,
		*	在这里添加一个新的项不会影响程序完整性
		*	最好更新identifier_worldClass和weight_default
		****************************************/
		enum WorldClass
		{
			Noun,//0名词n
			Verb,//1动词v
			Adj,//2形容词a
			Pronoun,//3代词r
			Adverb,//4副词d
			Prep,//5介词p
			Conj,//6连词c
			Particle,//7助词u
			Unclassified,//8未分类x
			End_WorldClass//哨兵
		};

		/****************************************
		*一些数量界限
		****************************************/
		enum Limits
		{
			Limit_Levenshtein_MAX,//编辑距离算法最大计算长度(两个文本都超过长度则抛出异常)
			//Limit_Segment_MIN,//分段时认作段落的最小词元数(小于这个值会被忽略)
			End_Limits//哨兵
		};

		/****************************************
		*相似度成分
		****************************************/
		enum SimilarityComposition
		{
			SC_Component,//成分
			//SC_Sequence,//序列(被舍弃)
			SC_Structure,//结构

			SC_main_Segments,//段落
			SC_main_KeyWords,//关键词
			End_SimilarityComposition//哨兵
		};

		/****************************************
		*序列相似度算法实现
		****************************************/
		enum Implements_NumberSequenceSimilarity
		{
			BinaryScan,//二元扫描算法(最慢)
			Levenshtein,//编辑距离算法(最快)
			Jaccard,//杰卡德算法(内存消耗少)
			End_Implements_NumberSequenceSimilarity//哨兵
		};


		constexpr pair<char,char> startChars_UTF8[5] =
		{
			{0xE0,0xC0},
			{0xF0,0xE0},
			{0xF8,0xF0},
			{0xFC,0xE8},
			{0xFE,0xEC}
		};

		typedef Implements_NumberSequenceSimilarity I_NSS;

		typedef std::vector<long long> NumberSequence;//数字序列
		typedef NumberSequence IndexSequence;//索引序列
		typedef std::vector<double> FloatSequence;//浮点序列
		typedef FloatSequence SimilaritySequence;
		typedef FloatSequence FloatVector;

		struct WordUnit
		{
			bool ignored{false};
			string word{};
			size_t offset;//开始索引
			size_t offset_codeUnit{0};//u8索引
			size_t num_codeUnit{0};//码元(UTF-8编码单元)数量
			string type{};//词性标识符

			NumberSequence numberSequence{};//数字序列

			friend ostream& operator<<(ostream& os, const WordUnit& wu);
		};

		typedef vector<WordUnit> WordUnitsVector;

		//分隔符
		struct Separator
		{
			size_t num_unitMin{0};//最小词元数
			string str_separator{};//分段符
			bool asciiRange{false};//分段符是否是一个ASCII码范围的标记
		};


		/****************************************
		*文本段信息结构
		****************************************/
		struct SegmentInfo
		{
			bool ignored;//标记该段落是否会被忽略
			size_t index_begin;//段开头索引a
			size_t index_end;//段结尾索引

			//以下是基于分词结果数组的索引
			IndexSequence set_indexList[End_WorldClass];

			NumberSequence sequence_structure;//语法成分结构序列(主谓宾等等)

			//构造函数
			SegmentInfo();

			//IndexSequence indexList_noun;//名词索引表
			//IndexSequence indexList_verb;//动词索引表
			//IndexSequence indexList_adjective;//形容词索引表
			//IndexSequence indexList_pronoun;//代词索引表
			//IndexSequence indexList_adverb;//副词索引表
			//IndexSequence indexList_preposition;//介词索引表
			//IndexSequence indexList_conjunction;//连词索引表
			//IndexSequence indexList_auxiliary;//助词索引表

			//重载<<
			friend ostream& operator<<(ostream& os, const SegmentInfo& tsi);
		};

		typedef vector<SegmentInfo> TextSegmentInfoVector;//文本段

		
		typedef vector<bool> FlagsVector;
	
		/****************************************
		*词类的标识符
		*	使用了constexpr关键字修饰以表示其为编译时可求值的量,可用于case语句(const是不行的,不知道为什么)
		****************************************/
		extern constexpr char identifier_worldClass[End_WorldClass]
		{
		'n',
		'v',
		'a',
		'r',
		'd',
		'p',
		'c',
		'u',
		'x'
		};

		//忽略标记
		constexpr int IGNORED = -1;
	}

	/********************************************************************************
	*依赖项集合
	*	该命名空间内定义了一些基本的函数
	********************************************************************************/
	namespace DependenciesSet
	{
		using namespace DefinitionsSet;//导入定义集

		size_t getCount_codeUnit_in_utf8_text(const string& text);

		/********************************************************************************
		*Function:transformWord_to_NumberSequence()
		*Description:文本转数字序列
		*Calls:暂无
		*Input:
		*	<string text:包含目标文本的字符串>
		*	<int start:在字符串中的开始位置>
		*	<int len:在字符串中的总长度>
		*	<EncodingType code:编码类型>
		*Output:无
		*Return:转换得到的数字序列
		*Remarks:
		*	[1]仅作为内部调用
		*	[2]传递字符串及目标文本在字符串中的位置
		********************************************************************************/
		void transformWord_to_NumberSequence
		(const string& text,const int& len, NumberSequence& ns);

		/********************************************************************************
		*Function:transformText_to_numberSequence()
		*Description:文本转数字序列
		*Calls:暂无
		*Input:
		*	<string text:包含目标文本的字符串>
		*	<int start:在字符串中的开始位置>
		*Output:无
		*Return:转换得到的数字序列
		*Remarks:
		*	[1]仅作为内部调用
		*	[2]该函数假定传入的容器有足够的空间
		********************************************************************************/
		void transformText_to_NumberSequence
		(const string& text, NumberSequence& ns);

		/********************************************************************************
		*Function:checkText_ASCII_CODE()
		*Description:判断给定的字符串是否符合ASCII编码的规律
		*Calls:暂无
		*Input:
		*	<string text:待检查的字符串>
		*
		*Output:无
		*Return:true如果全部是ASCII编码,反之false
		*Remarks:
		*	[1]仅作为内部调用
		********************************************************************************/
		inline bool checkText_ASCII_CODE(const string& text);

		/********************************************************************************
		*Function:checkChar_ASCII_CODE()
		*Description:判断给定的字符是否是ASCII编码
		*Calls:暂无
		*Input:
		*	<char c:待检查的字符>
		*
		*Output:无
		*Return:true如果给定的字符符合ASCII编码,反之false
		*Remarks:
		*	[1]仅作为内部调用
		********************************************************************************/
		inline bool checkChar_ASCII_CODE(const char& c);

		/********************************************************************************
		*Function:checkChar_GBK_CODE()
		*Description:判断给定的字符是否是ASCII编码
		*Calls:暂无
		*Input:
		*	<char c0:待检查的字符1>
		*	<char c1:待检查的字符2>
		*Output:无
		*Return:true如果给定的字符符合ASCII编码,反之false
		*Remarks:
		*	[1]仅作为内部调用
		********************************************************************************/
		inline bool checkChar_GBK_CODE(const char& c0, const char& c1);


		/********************************************************************************
		*Function:checkChar_UTF8_CODE()
		*Description:判断给定的字符是否是ASCII编码
		*Calls:暂无
		*Input:
		*	<string text:包含目标文本的字符串>
		*	<int start:在字符串中的开始位置>
		*	<int len:在字符串中的总长度>
		*
		*Output:无
		*Return:true如果给定的字符符合UTF-8编码规则,反之false
		*Remarks:
		*	[1]仅作为内部调用
		********************************************************************************/
		//inline bool checkChar_UTF8_CODE(const string& text, const int& start, const int& len);


		

		//函数指针数组
		extern double (* const functions_sequenceSimilarity[End_Implements_NumberSequenceSimilarity])(const NumberSequence& seq0, const NumberSequence& seq1);

		/********************************************************************************
		*Function:cal_sequenceRepetitiveRate()
		*Description:计算两个给定序列的相似度(使用二元扫描)
		*Calls:暂无
		*Input:
		*	<NumberSequence seq1:数字序列1>,
		*	<NumberSequence seq2:数字序列2>
		*
		*Output:无
		*Return:返回计算得到的相似度[0.0-1.0]
		*Remarks:
		*	[1]仅作为内部调用;该算法稳定(结果与两个序列的顺序无关)
		********************************************************************************/
		double cal_sequenceSimilarity_BinaryScan(const NumberSequence& seq1, const NumberSequence& seq2);

		/********************************************************************************
		*Function:cal_sequenceRepetitiveRate()
		*Description:计算两个给定序列的相似度(Jaccard(杰卡德)相似度)
		*Calls:暂无
		*Input:
		*	<NumberSequence seq1:数字序列1>,
		*	<NumberSequence seq2:数字序列2>
		*
		*Output:无
		*Return:返回计算得到的相似度[0.0-1.0]
		*Remarks:
		*	[1]仅作为内部调用;该算法稳定(结果与两个序列的顺序无关)
		********************************************************************************/
		double cal_sequenceSimilarity_Jaccard(const NumberSequence& seq1, const NumberSequence& seq2);

		/********************************************************************************
		*Function:cal_wordSimilarity_Levenshtein()
		*Description:计算两个数字序列的相似度,使用最小编辑距离算法(Levenshtein)
		*Calls:暂无
		*Input:
		*	<NumberSequence seq1:字符串1>,
		*	<NumberSequence seq2:字符串2>,
		*Output:无
		*Return:返回计算得到的相似度[0.0-1.0]
		*Remarks:
		*	[1]仅作为内部调用;该算法稳定(结果与两个序列的顺序无关)
		*	[2]不能用于处理长度过大的数字序列
		********************************************************************************/
		double cal_sequenceSimilarity_Levenshtein(const NumberSequence& seq1, const NumberSequence& seq2);


		//inline double cal_sequenceSimilarity(const NumberSequence& seq0, const NumberSequence& seq1, const Implements_NumberSequenceSimilarity& implement);
	
		/********************************************************************************
		*Function:cal_wordSimilarity_Levenshtein()
		*Description:计算两个单词(词语)的相似度,使用最小编辑距离算法(Levenshtein)
		*Calls:暂无
		*Input:
		*	<string str1:字符串1>,
		*	<string str2:字符串2>,
		*	<EncodingType codeType:编码类型>
		*Output:无
		*Return:返回计算得到的相似度[0.0-1.0]
		*Remarks:
		*	[1]该函数被废弃;该算法稳定(结果与两个字符串的顺序无关)
		*	[2]GBK用于处理中文词组,ASCII用于处理英文单词(或句子)
		*	[3]不能用于处理长度过大的字符串
		********************************************************************************/
		/*double cal_wordSimilarity_Levenshtein
		(const string& str1, const string& str2, const EncodingType& codeType);*/

		/********************************************************************************
		*Function:cal_wordSimilarity_Levenshtein()
		*Description:计算两个数字序列的相似度,使用最小编辑距离算法(Levenshtein)
		*Calls:暂无
		*Input:
		*	<NumberSequence seq1:字符串1>,
		*	<NumberSequence seq2:字符串2>,
		*Output:无
		*Return:返回计算得到的相似度[0.0-1.0]
		*Remarks:
		*	[1]被废弃;该算法稳定(结果与两个序列的顺序无关)
		*	[2]不能用于处理长度过大的数字序列
		********************************************************************************/
		/*double cal_sequenceSimilarity_Levenshtein
		(const NumberSequence& seq0,const int &start0,const int &len0, const NumberSequence& seq1, const int& start1, const int& len1);*/

		/********************************************************************************
		*Function:cal_result_SimilaritySequence_doubleMax_IGN()
		*Description:计算相似度序列的结果
		*Calls:暂无
		*Input:
		*	<SimilaritySequence ss0:相似度序列1>
		*Output:无
		*Return:返回计算得到的相似度结果[0.0-1.0]
		*Remarks:
		********************************************************************************/
		double cal_result_SimilaritySequence_doubleMax_IGN(const SimilaritySequence &ss0, const SimilaritySequence& ss1);
		double cal_result_SimilaritySequence_summary_IGN(const SimilaritySequence& ss0, const SimilaritySequence& ss1);


		enum SimilaritySequenceCalType
		{
			DoubleMaxCount,//双倍最大法
			SummaryCount,//加和法
			End_SSC//哨兵
		};


		//交叉匹配模板(分母取最大段落数两倍)
		template<typename container, typename func>
		double cal_similarity_crossing_max(container c0, container c1, func p_func)
		{
			size_t size0 = c0.size();
			size_t size1 = c1.size();

			SimilaritySequence ss0(size0, -2), ss1(size1, -2);//开两个相似度序列

			size_t i, j;
			double sim;

			for (i = 0; i < size0; i++)
			{
				for (j = 0; j < size1; j++)
				{
					sim = p_func(c0[i], c1[j]);

					if (ss0[i] < sim)
						ss0[i] = sim;
					if (ss1[j] < sim)
						ss1[j] = sim;
				}
			}

			return cal_result_SimilaritySequence_doubleMax_IGN(ss0, ss1);//根据相似度存储序列计算结果
		}

		//交叉匹配模板(分母取段落数相加)
		template<typename container, typename func>
		double cal_similarity_crossing_add(const container &c0,const container &c1, func p_func)
		{
			size_t size0 = c0.size();
			size_t size1 = c1.size();

			SimilaritySequence ss0(size0, -2), ss1(size1, -2);//开两个相似度序列

			size_t i, j;
			double sim;

			for (i = 0; i < size0; i++)
			{
				for (j = 0; j < size1; j++)
				{
					sim = p_func(c0[i], c1[j]);

					if (ss0[i] < sim)
						ss0[i] = sim;
					if (ss1[j] < sim)
						ss1[j] = sim;
				}
			}
			return cal_result_SimilaritySequence_summary_IGN(ss0, ss1);//根据相似度存储序列计算结果
		}




		string readTextFile(const string &filePath);



		/********************************************************************************
		*Function:operator<<()
		*Description:对std::vector重载<<运算符(模板),方便使用
		********************************************************************************/
		//template<typename _type>
		//ostream& operator<<(ostream& os, const vector<_type>& v)
		//{
		//	int size = v.size();
		//	os << "[ ";
		//	if (size != 0)
		//		os << v[0];
		//	else
		//		os << "(Empty)";

		//	for (int i = 1; i < size; i++)
		//	{
		//		os << ", " << v[i];
		//	}
		//	os << " ]";
		//	return os;
		//}
		


	}

	/********************************************************************************
	*设置项变量合集
	********************************************************************************/
	namespace Settings
	{
		using namespace DefinitionsSet;//导入定义集

		extern const double weight_wordClass_default[End_WorldClass];

		/****************************************
		*词性用户权重
		****************************************/
		extern double weight_wordClass_customized[End_WorldClass];

		/****************************************
		*默认相似度占比权重
		****************************************/
		extern const double weight_similarityComposition_default[End_SimilarityComposition];

		/****************************************
		*用户相似度占比权重
		****************************************/
		extern double weight_similarityComposition_customized[End_SimilarityComposition];

		/****************************************
		*默认限制
		****************************************/
		extern const int limits_default[End_Limits];

		/****************************************
		*用户限制
		****************************************/
		extern int limits_customized[End_Limits];

		/****************************************
		*默认序列相似度实现算法
		****************************************/
		extern const DependenciesSet::I_NSS seqSim_imp_default;

		/****************************************
		*用户序列相似度实现算法
		****************************************/
		extern DependenciesSet::I_NSS seqSim_imp_customized;

		//默认分隔符数量(舍弃)
		//extern const int TEXT_ANALYSIS_EXPORT num_separator_default;

		/****************************************
		*默认段落分隔符
		****************************************/
		extern const vector<Separator> separators_default;

		extern vector<Separator> separators_customized;

		//模板中忽略的符号
		extern vector<string> symbols_ignoredInTemplate_default;

		//模板开启状态(默认关闭)
		extern bool template_enable;



		/********************************************************************************
		*TextAnalysorSetter
		*		用于设置文本分析器的静态类,无法实例化,只有静态方法
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextAnalysorSetter
		{
		private:
			//构造函数
			TextAnalysorSetter() = default;
			//析构函数
			~TextAnalysorSetter() = default;
			
		public:
			//全部重置
			static void resetAll();
			//设置词性类别的权重
			static bool set_weight_wordClass(const FloatVector& newWeights);
			//设置分段符
			static bool set_separators(const vector<Separator>& newSeps);
			//设置模板开启状态
			static bool set_template_enabled(const bool& newState);
			//设置相似度组分权重
			static bool set_weight_similarityComposition(const FloatVector& newWeights);
			//设置序列相似度计算实现
			static bool set_seqSim_IMP(const Implements_NumberSequenceSimilarity& newIMP);


		};
	}

	/********************************************************************************
	*核心类命名空间
	*	完全开放到动态库
	********************************************************************************/
	namespace Core
	{
		using namespace DefinitionsSet;//导入定义集
		using namespace Settings;//导入设置集
		using namespace DependenciesSet;//导入依赖集

		/********************************************************************************
	*文本分析异常类
	*Description:
	*	该异常类适用且仅适用于该类所在命名空间中的其他类
	*	提供一些基本异常类型和异常信息
	*Inherited from:
	*	std::exception
	********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextAnalysisException :public std::exception
		{
		public:


			/****************************************
			*异常类型
			****************************************/
			enum TextAnalysisExceptionType
			{
				SegSysException,//分词系统异常
				CodingException,//编码异常
				OverLimitException,//超限异常
				InvalidPtrException,//无效指针异常
				SystemPlatformException,//平台系统异常
				OtherException,//其他异常
				UnknownException,//未知异常(默认)
				End_TextAnalysisExceptionType//哨兵
			};

			static const std::string details[End_TextAnalysisExceptionType];


		private:
			TextAnalysisExceptionType exceptionType;
			string otherInfo{};

		public:
			//构造函数
			explicit TextAnalysisException(const TextAnalysisExceptionType& exceptionType = UnknownException, const string& otherInfo = string());

			virtual char const* what() const;

		};

		/****************************************
		*前导声明
		****************************************/
		class TEXT_ANALYSIS_EXPORT TextStream;

		/********************************************************************************
		*模板工具类
		*Description:
		*	无法实例化
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT Template
		{
			friend class TextStream;//友元类声明
		private:
			static string content_origin;//原始内容
			static string content_processed;//内容
			static NumberSequence numSeq;
			static bool enabled;//是否可用
		private:
			//私有的构造函数,不允许实例化
			Template() = default;

		public:
			static bool setTemplate(const string& content);
			//重置
			static void reset();
			//获取模板内容
			static const string & getTemplate();
			//获取数字序列
			static const NumberSequence& getNumberSequence();
			//是否启用
			static bool isEnabled();

		};


		//TEXT_ANALYSIS_EXPORT ostream& operator<<(ostream& os, const TextStream&);

		/********************************************************************************
		*文本流
		*Description:
		*	该类对象用于存储读取的单个文件内容,并对其进行分段,
		*	每一段存储于TextSegment对象中,使用std::vector作为容器
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextStream
		{
			/****************************************
			*友元类声明
			****************************************/
			friend class TextAnalysor;
		public:
			//默认构造函数
			TextStream();
			//指定文本的构造函数
			TextStream(const string& text, const EncodingType& code = UTF_8);

		private:
		public:
			bool initialized{false};//初始化的
			string text{ "" };//完整文档
			NumberSequence numSeq_text{};//原始文本映射的数字序列
			FlagsVector flags_ignored{};//忽略标记,标记了原始文本中需要被忽略的u8码元

			size_t num_words{ 0 };//表项数(词元数)
			size_t num_units{ 0 };//UTF-8码元数
			size_t num_segs{ 0 };//段落数
			size_t num_unitsOutsizeTemplate{ 0 };//模板之外的码元数

			vector<cppjieba::Word>vector_words_extra{};//单词向量(额外信息)
			vector<pair<string, double>> vector_keyWords{};//关键词
			vector<NumberSequence> vector_NS_keyWords{};//关键词生成的数字序列
			vector<WordUnit>vector_wordsInfo{};//词元详细信息表(最终表)

			TextSegmentInfoVector vector_segmentsInfo{};//文本段信息

		public://接口
			void setText(const string& text, const EncodingType& code = GBK);

			const string& getText()const;


			//类型转换函数,允许隐式转换到std::string类型
			operator string();

			//友元,重载<<
			TEXT_ANALYSIS_EXPORT friend ostream& operator<<(ostream& os, const TextStream&);

			void init();//初始化
			void reset();//重置
			//获取单元数
			size_t getUnitNum();
			//获取段落数
			size_t getSegmentNum();
			//获取除模板外U8单元数
			size_t getUnitNumOutsideTemplate();

		private://内部函数

			void generateNumberSequence();//生成数字序列(0)

			void markTemplate();//标记模板(0.5)

			/********************************************************************************
			*Function:segmentWords()
			*Description:
			*	分词,对文本进行分词处理,并存储得到的信息表(vector作容器)
			*	同时获取关键词信息
			*Calls:暂无
			*Input:无
			*Output:无
			*Return:void
			*Remarks:
			*	[1]	TextStream对象的内部函数
			*	[2]	初始化的第一个步骤
			********************************************************************************/
			void segmentWords();//分词(1)

			void segmentText();//分段(2)

			void scanWords();//扫描分词结果,生成信息表(3)

			void scanNumberSequence();//扫描数字序列(4)

		};

		/********************************************************************************
		*文本分析器
		*Description:
		*	指定两个文本流,给出相似度
		*	对文本进行分段和分词不在此类对象的功能范围
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextAnalysor
		{
		private:
			/****************************************
			*待比较的两个文本流
			****************************************/
			static const TextStream(*textStream0);//第一个文本流指针
			static const TextStream(*textStream1);//第二个文本流指针

		public:

			static void setTextStream0(const TextStream& ts0);
			static void setTextStream1(const TextStream& ts1);

			static double sim_segments;//段相似度
			static double sim_keyWords;//关键词相似度


			static double getSimilarity();//获取相似度

			static void clear();


		private:
			//计算指定两个段落的相似度
			static double cal_segmentSimilarity(const SegmentInfo& seg0, const SegmentInfo& seg1);

			//计算两个词元表的相似度(同词性)(采用inline建议优化,无局部变量,仅仅是对函数调用的封装)
			inline static double cal_wordSimilarity(const size_t& i0, const size_t& i1);
			//double cal_wordUnitListSimilarity(const SegmentInfo& seg0, const SegmentInfo& seg1,const WorldClass& worldClass);

			//不允许实例化
			TextAnalysor();


		};
	}


	using namespace Core;

}



