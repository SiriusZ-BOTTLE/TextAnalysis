#define _CRT_SECURE_NO_WARNINGS


#include "TextAnalysis.h"

namespace Paths_JieBa
{
	const string dirPath_dict = "./data/cppjieba/dict";

	const string DICT_PATH = dirPath_dict + "/jieba.dict.utf8";
	const string HMM_PATH = dirPath_dict + "/hmm_model.utf8";
	const string USER_DICT_PATH = dirPath_dict + "/user.dict.utf8";
	const string IDF_PATH = dirPath_dict + "/idf.utf8";
	const string STOP_WORD_PATH = dirPath_dict + "/stop_words.utf8";
}



namespace TextAnalysis
{
	//结巴分词工具对象
	Jieba jieba
	(
		DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH
	);


	namespace DefinitionsSet
	{
		ostream& operator<<(ostream& os, const WordUnit& wu)
		{
			os
				<< "<ignored> " << wu.ignored
				<< " <Word> " << wu.word
				<< " <Type> " << wu.type
				<< " <STA> " << wu.offset
				<< " <UNum> " << wu.num_codeUnit
				<< " <NSeq> " << wu.numberSequence
				;
			return os;
		}

		ostream& operator<<(ostream& os, const SegmentInfo& tsi)
		{
			using namespace DependenciesSet;
			os
				<< "<index_begin>" << tsi.index_begin << endl
				<< "<index_end>" << tsi.index_end << endl;
			for (int i = 0; i < End_WorldClass; i++)
			{
				//int temp = tsi.set_indexList[i].size();
				os << "<WordType> " << identifier_worldClass[i];
				/*for (int j = 0; j < temp; j++)
					os << ' ' << tsi.set_indexList[i][j];*/
				os << tsi.set_indexList[i] << endl;
			}
			return os << endl;
		}
		
		SegmentInfo::SegmentInfo()
			:ignored(false)
			, index_begin(0)
			, index_end(0)
		{

		}
	}
	
	namespace Settings
	{
		const double weight_wordClass_default[End_WorldClass] =
		{
			0.3,//0名词n
			0.3,//1动词v
			0.1,//2形容词a
			0.0,//3代词r
			0.1,//4副词d
			0.0,//5介词p
			0.1,//6连词c
			0.0,//7助词u
			0.1//8未分类x
		};

		
		double weight_wordClass_customized[End_WorldClass] =
		{
			//使用默认值进行初始化
			weight_wordClass_default[0],
			weight_wordClass_default[1],
			weight_wordClass_default[2],
			weight_wordClass_default[3],
			weight_wordClass_default[4],
			weight_wordClass_default[5],
			weight_wordClass_default[6],
			weight_wordClass_default[7],
			weight_wordClass_default[8],
		};


		const double weight_similarityComposition_default[End_SimilarityComposition] =
		{
			0.7,//成分
			//0.0,//序列(被舍弃)
			0.3,//结构

			1,//SC_main_Segments,
			0.0//SC_main_KeyWords
		};


		double weight_similarityComposition_customized[End_SimilarityComposition] =
		{
			weight_similarityComposition_default[0],
			weight_similarityComposition_default[1],
			weight_similarityComposition_default[2],
			weight_similarityComposition_default[3]
		};


		const int limits_default[End_Limits] =
		{
			500
		};

		int limits_customized[End_Limits] =
		{
			//使用默认值进行初始化
			limits_default[0]
		};


		const DependenciesSet::I_NSS seqSim_imp_default = Jaccard;//默认使用杰卡德相似度

		DependenciesSet::I_NSS seqSim_imp_customized = seqSim_imp_default;

		const vector<Separator> separators_default =
		{
			{0,"\n"},
			{3,"."},
			{3,"。"},
			{8,","},
			{8,"，"},
			{8," "},
			{8,"\1\x1F",true}//包含了控制字符(除去\0外)
		};
	
		vector<Separator> separators_customized = separators_default;
	
		vector<string> symbols_ignoredInTemplate_default =
		{
			"\n",
			" ",
			",",
			".",
			"。"
			//"0",
			//"1",
			//"2",
			//"3",
			//"4",
			//"5",
			//"6",
			//"7",
			//"8",
			//"9",
			//"(",
			//")",
			//"【",
			//"】"
		};

		vector<string> symbols_ignoredInTemplate_customized = symbols_ignoredInTemplate_default;

		bool template_enable = false;

		void TextAnalysorSetter::resetAll()
		{
			int i;

			//重置词类权重
			for (i = 0; i < End_WorldClass; i++)
				weight_wordClass_customized[i] = weight_wordClass_default[i];
			//重置相似度组成成分权重
			for (i = 0; i < End_SimilarityComposition; i++)
				weight_similarityComposition_customized[i] = weight_similarityComposition_default[i];
			//重置数值限制表
			for (i = 0; i < End_Limits; i++)
				limits_customized[i] = limits_default[i];
			//重置序列相似度实现方案
			seqSim_imp_customized = seqSim_imp_default;
			//重置分段符
			separators_customized = separators_default;
			//重置模板忽略符
			symbols_ignoredInTemplate_customized = symbols_ignoredInTemplate_default;

			Template::reset();
			//模板状态设为关闭
			template_enable = false;

			return;
		}

		bool TextAnalysorSetter::set_weight_wordClass(const FloatVector& newWeights)
		{
			int temp;
			if (newWeights.size() == End_WorldClass)
			{
				double sum = 0;
				for (int i = 0; i < End_WorldClass; i++)
					sum += newWeights[i];
				if (sum != 1.0)
					return false;
				for (int i = 0; i < End_WorldClass; i++)
					weight_wordClass_customized[i] = newWeights[i];
				return true;
			}
			else
				return false;
		}

		bool TextAnalysorSetter::set_separators(const vector<Separator>& newSeps)
		{
			if (newSeps.size() > 0)
				separators_customized = newSeps;
			else
				return false;
		}

		bool TextAnalysorSetter::set_template_enabled(const bool& newState)
		{
			if (newState == true)
			{
				if (Template::isEnabled())//模板可用
					template_enable = true;//开启模板
				else
					return false;
			}
		}
		
		bool TextAnalysorSetter::set_weight_similarityComposition(const FloatVector& newWeights)
		{
			if (newWeights.size() == End_SimilarityComposition)
			{
				double sum = 0;
				for (int i = 0; i < End_SimilarityComposition; i++)
					sum += newWeights[i];
				if (sum != 1.0)
					return false;
				for (int i = 0; i < End_SimilarityComposition; i++)
					weight_similarityComposition_customized[i] = newWeights[i];
				return true;
			}
			else
				return false;
		}
		
		bool TextAnalysorSetter::set_seqSim_IMP(const Implements_NumberSequenceSimilarity& newIMP)
		{
			if (newIMP < 0 || newIMP >= End_Implements_NumberSequenceSimilarity)
				return false;
			else
				seqSim_imp_customized = newIMP;
			return true;
		}
}

	namespace DependenciesSet
	{
		
		size_t getCount_codeUnit_in_utf8_text(const string& text)
		{
			char c;
			size_t index = 0, size = text.size();
			size_t count = 0;
			size_t len_codeUnit = 0;//编码单元字节数
			while (index < size)
			{
				c = text[index];
				if (checkChar_ASCII_CODE(c) == true)
				{
					//ASCII
					index++;
					count++;
				}
				else
				{
					if ((c & 0xE0) == 0xC0)//110x xxxx
					{
						len_codeUnit = 2;//二字节UTF-8码元
					}
					else if ((c & 0xF0) == 0xE0)//1110 xxxx
					{
						len_codeUnit = 3;//三字节UTF-8码元
					}
					else if ((c & 0xF8) == 0xF0)//1111 0xxx
					{
						len_codeUnit = 4;//四字节UTF-8码元
					}
					else if ((c & 0xFC) == 0xE8)//1111 10xx
					{
						len_codeUnit = 5;//五字节UTF-8码元
					}
					else if ((c & 0xFE) == 0xEC)//1111 110x
					{
						len_codeUnit = 6;//六字节UTF-8码元
					}
					else
						throw TextAnalysisException(TextAnalysisException::CodingException);//抛出编码异常
					count++;
					index += len_codeUnit;
				}
				
			}
			return count;
		}

		void transformWord_to_NumberSequence(const string& text ,const int& len, NumberSequence& ns)
		{
			size_t i, j, k;
			size_t size;

			int res = 0;
			if (checkChar_ASCII_CODE(text[0]) == true)
			{
				//ASCII码

				size = text.size();//获取字符串大小(char数量)
				ns.assign(size, 0);//vector开空间

				for (j = 0; j < size; j++)
					ns[j] = text[j];//char直接转int
			}
			else
			{
				//UTF-8编码

				size = len;//获取码元数量(UTF-8编码单元数量)

				if (len <= 0)//不指定码元数量时,会采用稍微浪费空间的方式以提升速度
				{
					ns.assign(text.size() / 2, 0);//开空间
				}
				else
				{
					ns.assign(len, 0);//开空间
				}
				size = text.size();

				char code;
				size_t len_codeUnit = 0;//编码单元字节数
				int index = 0;
				k = 0;


				for (j = 0; index < size; j++)
				{
					code = text[index];//获取码元的第一个字节

					if ((code & 0xE0) == 0xC0)//110x xxxx
					{
						len_codeUnit = 2;//二字节UTF-8码元
					}
					else if ((code & 0xF0) == 0xE0)//1110 xxxx
					{
						len_codeUnit = 3;//三字节UTF-8码元
					}
					else if ((code & 0xF8) == 0xF0)//1111 0xxx
					{
						len_codeUnit = 4;//四字节UTF-8码元
					}
					else if ((code & 0xFC) == 0xE8)//1111 10xx
					{
						len_codeUnit = 0;//五字节UTF-8码元
						j += 5;//跳过
					}
					else if ((code & 0xFE) == 0xEC)//1111 110x
					{
						len_codeUnit = 0;//六字节UTF-8码元
						j += 6;//跳过
					}
					else
						throw TextAnalysisException(TextAnalysisException::CodingException);//抛出编码异常

					res = 0xffffffff;//赋初值
					for (k = 0; k < len_codeUnit; k++)
					{
						res <<= sizeof(char) * 8;
						res |= 0xff;
						res &= text[index++];//


					}
					//index += len_codeUnit;
					ns[j] = res;//位运算得到的值存入数字序列
				}
				ns.resize(j);//
			}

			return;
		}

		void transformText_to_NumberSequence(const string& text, NumberSequence& ns)
		{

			size_t index = 0, j, k, count = 0;
			size_t size = text.size();
			int res;
			char code;

			while (index < size)
			{
				code = text[index];
				if (checkChar_ASCII_CODE(code) == true)
				{
					//ASCII码
					ns[count++] = text[index++];//直接转int
				}
				else
				{
					//UTF-8编码
					size_t len_codeUnit = 0;//编码单元字节数

					if ((code & 0xE0) == 0xC0)//110x xxxx
					{
						len_codeUnit = 2;//二字节UTF-8码元
					}
					else if ((code & 0xF0) == 0xE0)//1110 xxxx
					{
						len_codeUnit = 3;//三字节UTF-8码元
					}
					else if ((code & 0xF8) == 0xF0)//1111 0xxx
					{
						len_codeUnit = 4;//四字节UTF-8码元
					}
					else if ((code & 0xFC) == 0xE8)//1111 10xx
					{
						len_codeUnit = 5;//五字节UTF-8码元
					}
					else if ((code & 0xFE) == 0xEC)//1111 110x
					{
						len_codeUnit = 6;//六字节UTF-8码元
					}
					else
						throw TextAnalysisException(TextAnalysisException::CodingException);//抛出编码异常

					res = 0xffffffff;//赋初值
					for (k = 0; k < len_codeUnit; k++)
					{
						res <<= 8;//左移8位
						res |= 0xff;
						res &= text[index++];//
					}
					ns[count++] = res;//位运算得到的值存入数字序列

				}
			}




		}

		bool checkText_ASCII_CODE(const string& text)
		{
			int size = text.size(), i;
			for (i = 0; i < size && !checkChar_ASCII_CODE(text[i]); i++)
				;
			return i == size;
		}

		bool checkChar_ASCII_CODE(const char& c)
		{
			return (0 <= c && c < 128);//返回结果
		}

		bool checkChar_GBK_CODE(const char& c0, const char& c1)
		{
			//GBK范围:8140-FEFE
			int i0 = 0x000000FF, i1 = 0x000000FF;
			i0 &= c0;
			i1 &= c1;
			//这里有一个坑点,当char的第一位是1(即为负)的情况下,与其他值进行位运算时默认是:(其他值)|0xff....ffxx,其他位都默认是1
			//return (c0 >= 0x81 && c0 <= 0xFE) && (c1 >= 0x40 && c1 <= 0xFE);
			return (i0 >= 0x81 && i0 <= 0xFE) && (i1 >= 0x40 && i1 <= 0xFE);
		}


		double (* const functions_sequenceSimilarity[End_Implements_NumberSequenceSimilarity])(const NumberSequence& seq0, const NumberSequence& seq1) =
		{
			cal_sequenceSimilarity_BinaryScan,
			cal_sequenceSimilarity_Levenshtein,
			cal_sequenceSimilarity_Jaccard
		};

		double cal_sequenceSimilarity_BinaryScan(const NumberSequence& seq1, const NumberSequence& seq2)
		{
			bool
				flag_repetitive = false,//重复标记
				flag_firstFound = false;//

			bool* flag_used;

			int index_crt = 0, index_temp = 0, size1 = 0, size2 = 0, total = 0;
			int n1, n2, count = 0, num_max;

			size1 = seq1.size();
			size2 = seq2.size();

			flag_used = new bool[num_max = (size1 > size2 ? size1 : size2)];//申请标记数组
			//memset(flag_used, 0, num_max * sizeof(bool));//清空标记数组

			//按组合数取序列1中的二元组(C(2,seq1.size()))
			for (index_crt = 0; index_crt < size1; index_crt++)
			{
				n1 = seq1[index_crt];//取得二元组第一个元素

				memset(flag_used, 0, num_max * sizeof(bool));//清空使用标记

				for (index_temp = index_crt + 1; index_temp < size1; index_temp++)
				{
					n2 = seq1[index_temp];//取得二元组第二个元素
					total++;//总次数更新

					//发现重复数字
					if (n1 == n2)
					{
						flag_repetitive = true;//标记为有重复数字
						//count++;//重复算正确序列
						//continue;//下一轮
					}

					flag_firstFound = false;//变量初始化

					//和第二个序列进行比对
					for (int i = 0; i < size2; i++)
					{
						if (flag_used[i])//已经被使用
							continue;//跳过
						if (!flag_firstFound&&seq2[i] == n1)
						{
							flag_firstFound = true;//第一个标记为找到
						}
						else if (seq2[i] == n2)
						{
							if (flag_firstFound)
							{
								flag_used[i] = true;//标记为使用过
								count++;//同时找到两个
								break;//退出
							}
							//break;//退出
						}
						
					}
				}
			}

			if (flag_repetitive == false && size1 == size2)//没有重复情况
			{
				return (count * 1.0) / total;
			}

			//互换
			//size2 = seq1.size();
			//size1 = seq2.size();

			//统计以第二个序列为基准的重复率
			for (index_crt = 0; index_crt < size2; index_crt++)
			{
				n1 = seq2[index_crt];//取得二元组第一个元素

				memset(flag_used, 0, num_max * sizeof(bool));//清空使用标记

				for (index_temp = index_crt + 1; index_temp < size2; index_temp++)
				{
					n2 = seq2[index_temp];//取得二元组第二个元素
					total++;//总次数更新

					//发现重复数字
					if (n1 == n2)
					{
						flag_repetitive = true;//标记为有重复数字
						//count++;//重复算正确序列
						//continue;//下一轮
					}

					flag_firstFound = false;//变量初始化

					//和第二个序列进行比对
					for (int i = 0; i < size1; i++)
					{
						if (flag_used[i])//已经被使用
							continue;//跳过
						if (!flag_firstFound && seq1[i] == n1)
						{
							flag_firstFound = true;//第一个标记为找到
						}
						else if (seq1[i] == n2)
						{
							if (flag_firstFound)
							{
								flag_used[i] = true;//标记为使用过
								count++;//同时找到两个
								break;//退出
							}
							//break;//退出
						}

					}
				}
			}
			delete[]flag_used;//释放内存

			return (count * 1.0) / total;
		}

		double cal_sequenceSimilarity_Jaccard(const NumberSequence& seq0, const NumberSequence& seq1)
		{
			size_t size_intersection = 0;//交集大小
			size_t size_union = 0;//并集大小

			NumberSequence seq0_c=seq0,seq1_c=seq1;

			//对两个序列进行非降序排序(优化)
			std::sort(seq0_c.begin(), seq0_c.end());//排序
			std::sort(seq1_c.begin(), seq1_c.end());//排序
			size_t i0 = 0, i1 = 0;
			size_t size0 = seq0.size(), size1 = seq1.size();
			//0 1 2 3
			//0 2 2 3
			long long compareResult;
			while (i0<size0&&i1<size1)
			{
				//提取比较结果,小幅度优化
				compareResult = seq0_c[i0] - seq1_c[i1];
				if (compareResult==0)
				{
					//相等
					size_intersection++;
					i0++;
					i1++;
				}
				else if (compareResult < 0)
				{
					//小于
					i0++;
				}
				else
				{
					//大于
					i1++;
				}
				size_union++;
			}

			size_union += ((size0-i0)+(size1-i1));
			return size_intersection/((double)size_union);
		}

		double cal_sequenceSimilarity_Levenshtein(const NumberSequence& seq1, const NumberSequence& seq2)
		{
			int size1, size2, size_max;
			int i, j, min, cost, temp;
			int** matrix = nullptr;//矩阵
			double result = 1.0;

			size1 = seq1.size();
			size2 = seq2.size();
			//cout << "<LEV> size1: " << size1 << " size2: " << size2 << endl;
			if (size1 > limits_customized[Limit_Levenshtein_MAX] && size2 > limits_customized[Limit_Levenshtein_MAX])//超限
				throw TextAnalysisException(TextAnalysisException::OverLimitException,"<AT> TextAnalysis.cpp <LINE>585");//抛出超限异常
			size_max = size1 > size2 ? size1 : size2;//取较大值
			size1++, size2++;

			//申请矩阵内存
			matrix = new int* [size2];
			for (i = 0; i < size2; i++)
				matrix[i] = new int[size1];

			//初始化矩阵
			for (i = 0; i < size1; i++)
				matrix[0][i] = i;
			for (i = 1; i < size2; i++)
				matrix[i][0] = i;

			for (i = 1; i < size2; i++)//每一行
			{
				for (j = 1; j < size1; j++)//每一行中的每一列
				{
					min = matrix[i][j - 1] + 1;//左
					if (min > (temp = matrix[i - 1][j] + 1))//上
						min = temp;
					//左上
					if (seq1[j - 1] == seq2[i - 1])//两个字母相等
					{
						if (min > (temp = matrix[i - 1][j - 1]))
							min = temp;
					}
					else//两个字母不等
					{
						if (min > (temp = matrix[i - 1][j - 1] + 1))
							min = temp;
					}
					matrix[i][j] = min;
				}
			}

			result = 1 - (1.0 * min) / size_max;

			//释放内存
			for (i=0;i< size2;i++)
			{
				delete[]matrix[i];
			}
			delete[]matrix;

			return result;
		}
		
		double cal_result_SimilaritySequence_doubleMax_IGN(const SimilaritySequence& ss0, const SimilaritySequence& ss1)
		{
			double res = 0;
			size_t i, size0, size1, count0 = 0, count1 = 0;
			size0 = ss0.size();
			size1 = ss1.size();

			if (size0 == 0 || size1 == 0)
				return IGNORED;//存在空表,返回特殊标记

			for (i = 0; i < size0; i++)
			{
				if (ss0[i] < 0)//小于0的被忽略
					continue;
				count0++;
				res += ss0[i];
			}
			if (count0 == 0)//全部是被忽略的项
				return IGNORED;
			for (i = 0; i < size1; i++)
			{
				if (ss1[i] < 0)//小于0的被忽略
					continue;
				count1++;
				res += ss1[i];
			}
			if (count1 == 0)//全部是被忽略的项
				return IGNORED;
			if (count0 == 0 && count1 == 0)
				return 0;
			return res / (2.0 * (count0 > count1 ? count0 : count1));//返回结果
			//return res / (size0 + size1);//返回结果
		}

		double cal_result_SimilaritySequence_summary_IGN(const SimilaritySequence& ss0, const SimilaritySequence& ss1)
		{
			double res = 0;
			size_t i, size0, size1, count0 = 0, count1 = 0;
			size0 = ss0.size();
			size1 = ss1.size();

			if (size0 == 0 || size1 == 0)
				return IGNORED;//存在空表,返回特殊标记

			for (i = 0; i < size0; i++)
			{
				if (ss0[i] < 0)//小于0的被忽略
				{
					continue;
				}
				count0++;
				res += ss0[i];
			}
			if (count0 == 0)//全部是被忽略的项
				return IGNORED;

			for (i = 0; i < size1; i++)
			{
				if (ss1[i] < 0)//小于0的被忽略
					continue;
				count1++;
				res += ss1[i];
			}
			if (count1 == 0)//全部是被忽略的项
				return IGNORED;

			//if (count0 == 0 && count1 == 0)
			//	return 0;
			return res / (count0 + count1);//返回结果
		}

		string readTextFile(const string& filePath)
		{

			fstream fs(filePath);
			istreambuf_iterator<char> beg(fs), end;
			string data(beg, end);
			fs.close();
			return data;
		}

		/*double cal_wordSimilarity_Levenshtein(const string& str1, const string& str2, const EncodingType& codeType)
		{
			int size1, size2, size_max;
			int i, j, min, cost, temp;
			int** matrix = nullptr;//矩阵
			double result = 1.0;

			if (codeType == ASCII)//ASCII码
			{
				size1 = str1.size();
				size2 = str2.size();
				size_max = size1 > size2 ? size1 : size2;//取较大值
				size1++, size2++;

				//申请矩阵内存
				matrix = new int* [size2];
				for (i = 0; i < size2; i++)
					matrix[i] = new int[size1];

				//初始化矩阵
				for (i = 0; i < size1; i++)
					matrix[0][i] = i;
				for (i = 1; i < size2; i++)
					matrix[i][0] = i;

				for (i = 1; i < size2; i++)//每一行
				{
					for (j = 1; j < size1; j++)//每一行中的每一列
					{
						min = matrix[i][j - 1] + 1;//左
						if (min > (temp = matrix[i - 1][j] + 1))//上
							min = temp;
						//左上
						if (str1[j - 1] == str2[i - 1])//两个字母相等
						{
							if (min > (temp = matrix[i - 1][j - 1]))
								min = temp;
						}
						else//两个字母不等
						{
							if (min > (temp = matrix[i - 1][j - 1] + 1))
								min = temp;
						}
						matrix[i][j] = min;
					}
				}

				result = 1 - (1.0 * min) / size_max;

			}
			else if (codeType == GBK)//GBK码
			{
				size1 = str1.size();
				size2 = str2.size();
				size_max = size1 > size2 ? size1 : size2;//取较大值
				size1++, size2++;

				//申请矩阵内存
				matrix = new int* [size2];
				for (i = 0; i < size2; i++)
					matrix[i] = new int[size1];

				//初始化矩阵
				for (i = 0; i < size1; i++)
					matrix[0][i] = i;
				for (i = 1; i < size2; i++)
					matrix[i][0] = i;

				for (i = 1; i < size2; i++)//每一行
				{
					for (j = 1; j < size1; j++)//每一行中的每一列
					{
						min = matrix[i][j - 1] + 1;//左
						if (min > (temp = matrix[i - 1][j] + 1))//上
							min = temp;
						//左上
						if (str1[j - 1] == str2[i - 1])//两个字母相等
						{
							if (min > (temp = matrix[i - 1][j - 1]))
								min = temp;
						}
						else//两个字母不等
						{
							if (min > (temp = matrix[i - 1][j - 1] + 1))
								min = temp;
						}
						matrix[i][j] = min;
					}
				}

				result = 1 - (1.0 * min) / size_max;
			}

			//释放内存
			for (i = 0; i < size2; i++)
			{
				delete[] matrix[i];
			}
			delete[] matrix;

			return result;
		}*/
		/*double cal_sequenceSimilarity_Levenshtein(const NumberSequence& seq0, const int& start0, const int& len0, const NumberSequence& seq1, const int& start1, const int& len1)
				{
					int size0, size1, size_max, limit0 = start0 + len0, limit1 = start1 + len1;
					int i, j, min, cost, temp;
					int** matrix = nullptr;//矩阵
					double result = 1.0;

					size0 = len0;
					size1 = len1;
					if (size0 > Limit_Levenshtein_MAX && size1 > Limit_Levenshtein_MAX)//超限
						throw TextAnalysisException(TextAnalysisException::OverLimitException);//抛出超限异常
					size_max = size0 > size1 ? size0 : size1;//取较大值
					size0++, size1++;

					//申请矩阵内存
					matrix = new int* [size1];
					for (i = 0; i < size1; i++)
						matrix[i] = new int[size0];

					//初始化矩阵
					for (i = 0; i < size0; i++)
						matrix[0][i] = i;
					for (i = 1; i < size1; i++)
						matrix[i][0] = i;

					for (i = 1; i < size0; i++)//每一行
					{
						for (j = 1; j < size1; j++)//每一行中的每一列
						{
							min = matrix[i][j - 1] + 1;//左
							if (min > (temp = matrix[i - 1][j] + 1))//上
								min = temp;
							//左上
							if (seq0[j - 1+start0] == seq1[i - 1+start1])//两个字母相等
							{
								if (min > (temp = matrix[i - 1][j - 1]))
									min = temp;
							}
							else//两个字母不等
							{
								if (min > (temp = matrix[i - 1][j - 1] + 1))
									min = temp;
							}
							matrix[i][j] = min;
						}
					}

					result = 1 - (1.0 * min) / size_max;
					return result;
				}*/
	}

	namespace Core
	{
		const std::string TextAnalysisException::details[TextAnalysisException::End_TextAnalysisExceptionType] =
		{
			"SegSysException",
			"CodingException",
			"OverLimitException",
			"InvalidPtrException",
			"SystemPlatformException",
			"OtherException",
			"UnknownException"
		};

		TextAnalysisException::TextAnalysisException(const TextAnalysisExceptionType& _exceptionType, const string& _otherInfo)
			:exceptionType(_exceptionType)
			,otherInfo(_otherInfo)
		{
			if (exceptionType < 0 || exceptionType >= End_TextAnalysisExceptionType)
				exceptionType = UnknownException;
			//cout << what()<<" constructed";
		}

		char const* TextAnalysisException::what() const
		{
			return (details[exceptionType]+" : "+ otherInfo).c_str();//返回异常信息
		}


		string Template::content_origin;
		string Template::content_processed;
		NumberSequence Template::numSeq;
		bool Template::enabled = false;


		bool Template::setTemplate(const string& _content)
		{
			if (_content.size() == 0)
				return false;
			//全部重置
			reset();
			//复制
			content_processed = content_origin = _content;

			//去分隔符
			size_t num = symbols_ignoredInTemplate_customized.size();
			size_t pos, len_separator;

			for (int i = 0; i < num; i++)//逐个去除被忽略的符号
			{
				pos = 0;
				len_separator = symbols_ignoredInTemplate_customized[i].length();
				//查找
				while ((pos = content_processed.find(symbols_ignoredInTemplate_customized[i], pos)) != string::npos)
				{
					content_processed.erase(pos, len_separator);//擦除
				}
			}

			//获取u8码元数
			size_t count = getCount_codeUnit_in_utf8_text(content_processed);
			//开空间
			numSeq.assign(count, 0);
			//生成数字序列
			transformText_to_NumberSequence(content_processed, numSeq);
			enabled = true;
			return true;
		}

		void Template::reset()
		{
			//清空容器
			content_origin.clear();
			content_processed.clear();
			numSeq.clear();
			//可用性设为false
			enabled = false;
			return;
		}

		const string& Template::getTemplate()
		{
			return content_processed;
		}

		const NumberSequence& Template::getNumberSequence()
		{
			return numSeq;
		}

		bool Template::isEnabled()
		{
			return isEnabled;
		}



		

		TextStream::TextStream()
		{
		}

		TextStream::TextStream(const string& _text, const EncodingType& code)
			:text(_text)
			, num_words(0)
			, num_units(0)
			, vector_wordsInfo()//vector使用默认构造
		{
			//if (code == GBK)
			//	this->text = GbkToUtf8(text.c_str());//转UTF-8
		}

		void  TextStream::setText(const string& text, const EncodingType& code)
		{
			this->text = text;
			//if (code == GBK)
			//	this->text = GbkToUtf8(text.c_str());//转UTF-8
			//numberSequence.assign(text.size(), 0);
		}

		const string& TextStream::getText()const
		{
			// TODO: 在此处插入 return 语句
			return this->text;
		}

		TextStream::operator string()
		{
			return this->text;
		}

		ostream& operator<<(ostream& os, const TextStream& ts)
		{
			using namespace DependenciesSet;
			os
				<< "<TextStream>\n<Text>\n" << ts.text << endl;//输出文本
			os << "<NumberSequence>\n" << ts.numSeq_text << endl;
			os << "<FlagsIgnored>\n" << ts.flags_ignored << endl;
			int i = 0;
			int temp;
			os
				<< "<KeyWords> \n"
				<< ts.vector_keyWords << endl
				<< ts.vector_NS_keyWords;
			temp = ts.vector_wordsInfo.size();
			os << "\n\n<WordsInfo>\n";
			for (i = 0; i < temp; i++)
			{
				os
					<< ts.vector_wordsInfo[i]
					<< endl;
			}
			temp = ts.vector_segmentsInfo.size();
			for (i = 0; i < temp; i++)
			{
				os
					<< "\n\n<SEG#" << i << ">\n"
					//<<ts.vector_segmentsInfo[i]
					;
				for (int j = ts.vector_segmentsInfo[i].index_begin; j <= ts.vector_segmentsInfo[i].index_end; j++)
				{
					os << ts.vector_wordsInfo[j].word << " ";
				}
				//os << endl << ts.vector_wordsInfo;
			}

			return os << endl;//返回流引用
		}

		void TextStream::generateNumberSequence()
		{
			//获取UTF-8码元数
			num_units = getCount_codeUnit_in_utf8_text(text);
			//容器开空间
			numSeq_text.assign(num_units, 0);
			//获取映射得到的数字序列(获取转换得到的数字序列)
			transformText_to_NumberSequence(text, numSeq_text);

			return;
		}

		void TextStream::markTemplate()
		{
			//检测模板可用性
			if (Template::isEnabled)
			{
				//模板可用
				//将模板内容标记为忽略

				flags_ignored.assign(num_units, false);//开空间
				size_t index_text, index_template, temp;
				size_t size_template = Template::numSeq.size();
				size_t limit;

				//逐次扫描
				for (int i = 0; i < 1/*num_templateScanTimes*/; i++)
				{
					//算法需要改进,可以尝试使用动态规划

					for (index_text = 0, index_template = 0; index_text < num_units && index_template < size_template;)
					{
						if (flags_ignored[index_text])//已经被标记
						{
							index_text++;
							continue;//跳过
						}

						if (numSeq_text[index_text] == Template::numSeq[index_template])//相同
						{
							flags_ignored[index_text] = true;//标记为忽略
							index_template++;//模板指针后移
							index_text++;
						}
						else
							index_text++;
						//else//不相同
						//{
						//	//以下的算法比较简陋,应该采取对比法,找到最短的

						//	//搜索模板,查找是否存在和目标文本段相同的单元
						//	temp = index_template + 5;//最多往后遍历5个
						//	limit = temp >= size_template ? size_template : temp;
						//	temp = index_template;//记录之前的值
						//	for (; index_template < limit && numSeq_text[index_text] != Template::numSeq[index_template]; index_template++)
						//		;
						//	if (index_template == limit)//没有搜索到
						//	{
						//		index_template = temp;//恢复之前的值
						//		index_text++;
						//	}
						//}
					}
				}
			}
			//统计未被忽略的码元数量
			num_unitsOutsizeTemplate = count(flags_ignored.begin(), flags_ignored.end(), false);
		}

		void TextStream::segmentWords()
		{
			//NLPIR分词,舍弃
			////初始化
			//if (NLPIR_Init(DataPath.c_str(), encodingType, "") == 0)
			//	//初始化失败
			//	throw TextAnalysisException(TextAnalysisException::SegSysException);//抛出异常
			////进行分词,可视化结果存入成员变量
			//text_processed = NLPIR_ParagraphProcess(text.c_str(), true);
			////分词,将列表结构存入字段,并获取项数
			//table_wordInfo=NLPIR_ParagraphProcessA(text.c_str(),&num_words);
			
			this->text.append(separators_customized[0].str_separator);//末尾加一个分段符,作为哨兵

			//结巴分词,结果存入vector容器
			jieba.CutForSearch(this->text, vector_words_extra);
			//jieba.Tag(this->text, vector_words_tags);
			num_words = vector_words_extra.size();//获取词元数量并存取

			//TF.IDF算法提取关键词
			jieba.extractor.Extract(text, vector_keyWords, num_words / 20);

			return;
		}

		void TextStream::segmentText()
		{
			size_t originSize = vector_words_extra.size();
			size_t step;//获取分段符长度
			size_t  i_origin = 0, i_new = 0, last_new = 0;//上一个分段符索引,当前分段符索引
			SegmentInfo seg;
			size_t  count_words = 0;
			num_words = 0;
			WordUnit emptyWU;
			vector_wordsInfo.assign(originSize, emptyWU);//开固定大小的空间
			size_t num_separator_customized = separators_customized.size();
			//逐个寻找分段符,同时生成段落信息
			for (i_origin = 0, i_new = 0; i_origin < originSize; i_origin++, i_new++)
			{

				int i;
				//逐个比较分段符
				for (i = 0; i < num_separator_customized; i++)
				{
					if (separators_customized[i].asciiRange == false)
					{
						if (vector_words_extra[i_origin].word == separators_customized[i].str_separator)//发现分段符
							break;//提前跳出
					}
					else
					{
						//ASCII码范围
						char c = vector_words_extra[i_origin].word[0];//只检查第一个字符
						if (c >= separators_customized[i].str_separator[0] && c <= separators_customized[i].str_separator[1])//发现范围中的ASCII字符
							break;//提前退出
					}
					
				}
				if (i == num_separator_customized)//当前不是分段符
				{
					//合并分词结果信息并存入vector_wordsInfo
					if(Template::isEnabled)//检查模板状态
						vector_wordsInfo[num_words].ignored = flags_ignored[vector_words_extra[i_origin].unicode_offset];//一个词元是否被忽略取决于第一个码元是否被忽略
					vector_wordsInfo[num_words].offset = vector_words_extra[i_origin].offset;
					vector_wordsInfo[num_words].offset_codeUnit = vector_words_extra[i_origin].unicode_offset;
					vector_wordsInfo[num_words].word = vector_words_extra[i_origin].word;
					vector_wordsInfo[num_words].num_codeUnit = vector_words_extra[i_origin].unicode_length;
					vector_wordsInfo[num_words].type = jieba.LookupTag(vector_wordsInfo[num_words].word);//设置词性
					num_words++;
					count_words++;
					continue;//寻找下一个
				}

				i_new--;
				//是分段符
				//分段符不加入词元vector
				if (count_words <= separators_customized[i].num_unitMin)//检查是否小于阈值
					continue;//小于指定,跳过

				seg.index_begin = last_new;//段头

				seg.index_end = i_new;//段尾(不包含分段符)
				last_new = i_new + 1;

				vector_segmentsInfo.push_back(seg);//加入段表
				count_words = 0;
			}

			//除去多余的项(开的时候开的稍微大一点,扫描之后分段符会被舍弃,这里删除不需要的空项)
			vector_wordsInfo.erase(vector_wordsInfo.end() - (originSize - num_words), vector_wordsInfo.end());
			num_words = vector_wordsInfo.size();
			num_segs = vector_segmentsInfo.size();//得到段落数

			//遍历段落信息表,生成语法结构序列
			for (int i = 0; i < num_segs; i++)
			{
				SegmentInfo& crtSeg = vector_segmentsInfo[i];//获取段落的引用,方便调用

				//vector开空间
				crtSeg.sequence_structure.assign(crtSeg.index_end - crtSeg.index_begin + 1, 0);

				int j, k;
				//遍历段落中的每一个U8码元
				for (j = crtSeg.index_begin, k = 0; j <= crtSeg.index_end; j++, k++)
				{
					crtSeg.sequence_structure[k] = static_cast<int>(vector_wordsInfo[j].type[0]);
					//if (vector_wordsInfo[j].ignored)//如果单词被忽略
					//	crtSeg.ignored = true;//将段落设为被忽略
				}
			}


			return;
		}

		void TextStream::scanWords()
		{
			SegmentInfo textSegmentInfo_temp;
			int index_wordUnit_temp;
			int num_seg = vector_segmentsInfo.size();//获取段落数

			int i = 0, j = 0;
			int index_crtSeg = 0;//当前段落索引

			for (i = 0; i < num_words && index_crtSeg < num_seg; i++)//逐个扫描分词词条
			{
				//index_wordUnit_temp = table_wordInfo + i;//获取当前词条指针
				if (i > vector_segmentsInfo[index_crtSeg].index_end)//当前词条已经超过了当前选中的段落区间右界
					index_crtSeg++;//定位到下一个段落

				//将当前词条的索引添加到当前段信息的对应词性表中
				for (j = 0; j < End_WorldClass && vector_wordsInfo[i].type[0] != identifier_worldClass[j]; j++)
					;
				if (j != End_WorldClass)
					vector_segmentsInfo[index_crtSeg].set_indexList[j].push_back(i);
			}

			return;
		}

		void TextStream::scanNumberSequence()
		{
			size_t i = 0, j = 0;
			//遍历词表,逐个生成序列
			for (i = 0; i < num_words; i++)
			{
				WordUnit& crtWU = vector_wordsInfo[i];//获取引用,方便调用

				//生成数字序列
				//transformWord_to_NumberSequence(crtWU.word, crtWU.num_codeUnit, crtWU.numberSequence);//包含开空间的步骤
				crtWU.numberSequence.assign(crtWU.num_codeUnit, 0);//开空间
				for (j = 0; j < crtWU.num_codeUnit; j++)
				{
					crtWU.numberSequence[j] = numSeq_text[crtWU.offset_codeUnit + j];
				}
			}

			//生成关键词的数字序列
			size_t num_KW = vector_keyWords.size();
			NumberSequence ns;
			if (num_KW != 0)
				vector_NS_keyWords.assign(num_KW, ns);
			//else
			//	vector_NS_keyWords.clear();//清空
			for (i = 0; i < num_KW; i++)
			{
				transformWord_to_NumberSequence
				(
					vector_keyWords[i].first,
					-1,//不指定码元长度
					vector_NS_keyWords[i]
				);
			}

			return;//退出
		}

		void TextStream::init()
		{
			this->generateNumberSequence();
			this->markTemplate();
			this->segmentWords();
			this->segmentText();
			this->scanWords();
			this->scanNumberSequence();
			initialized = true;
			return;
		}

		void TextStream::reset()
		{
			text.clear();
			numSeq_text.clear();
			flags_ignored.clear();
			num_words = 0;
			num_units = 0;
			vector_words_extra.clear();
			vector_keyWords.clear();
			vector_NS_keyWords.clear();
			vector_wordsInfo.clear();
			vector_segmentsInfo.clear();

			initialized = false;
			return;
		}

		size_t TextStream::getUnitNum()
		{
			return num_units;
		}
		
		size_t TextStream::getSegmentNum()
		{
			return num_segs;
		}

		size_t TextStream::getUnitNumOutsideTemplate()
		{
			return num_unitsOutsizeTemplate;
		}

		

		//静态变量定义
		const TextStream(*TextAnalysor::textStream0) = nullptr;
		const TextStream(*TextAnalysor::textStream1) = nullptr;
		double TextAnalysor::sim_segments = 0;
		double TextAnalysor::sim_keyWords = 0;


		void TextAnalysor::setTextStream0(const TextStream& ts0)
		{
			textStream0 = &ts0;
			return;
		}

		void TextAnalysor::setTextStream1(const TextStream& ts1)
		{
			textStream1 = &ts1;
		}

		double TextAnalysor::getSimilarity()
		{
			if (textStream0 == nullptr || textStream1 == nullptr)
				throw TextAnalysisException(TextAnalysisException::InvalidPtrException);
			if(!textStream0->initialized||!textStream1->initialized)
				throw TextAnalysisException(TextAnalysisException::OtherException,"TextStream not initialized");

			//调用模板函数
			sim_segments =cal_similarity_crossing_max(textStream0->vector_segmentsInfo,textStream1->vector_segmentsInfo,cal_segmentSimilarity);

			sim_keyWords =
				cal_similarity_crossing_max
				(
					textStream0->vector_NS_keyWords,
					textStream1->vector_NS_keyWords,
					static_cast<double(*)(const NumberSequence&, const NumberSequence&)> (cal_sequenceSimilarity_Levenshtein)
				);

			return
				(weight_similarityComposition_customized[SC_main_Segments]) * sim_segments +
				(weight_similarityComposition_customized[SC_main_KeyWords]) * sim_keyWords;//返回结果
		}

		void TextAnalysor::clear()
		{
			textStream0 = textStream1 = nullptr;
			sim_segments = 0;
			sim_keyWords = 0;
		}

		double TextAnalysor::cal_segmentSimilarity(const SegmentInfo& seg0, const SegmentInfo& seg1)
		{
			if ((seg0.ignored || seg1.ignored))//两个待比较的段落中存在一个被忽略的
				return IGNORED;//返回特殊标记

			double sim_component = 0, sim_sequence = 0, sim_structure = 0;//成分相似度,序列相似度,结构相似度
			int i;
			double rest = 1.0, temp;//剩余的将被计入的组分百分比


			//遍历每一个词性表,计算成分相似度
			for (i = 0; i < End_WorldClass; i++)
			{
				if (weight_wordClass_customized[i] == 0.0)
					continue;//跳过权重为0的词性表

				//计算词性表成分相似度结果(模板函数)
				temp = cal_similarity_crossing_add(seg0.set_indexList[i], seg1.set_indexList[i], cal_wordSimilarity);
				if (temp == IGNORED)//传入的容器为空或者存在完全被忽略的容器
					rest -= weight_wordClass_customized[i];//减去权重
				else
					sim_component += weight_wordClass_customized[i] * temp;
			}
			if (rest == 0)
				return IGNORED;
			else
				sim_component /= rest;//放大
			

			//成分相似度乘以权重
			sim_component *= (weight_similarityComposition_customized[SC_Component]);
			//sim_sequence=DependenciesSet::cal_sequenceSimilarity_Levenshtein()

			//计算(语法)结构相似度
			sim_structure = weight_similarityComposition_customized[SC_Structure] *
				cal_sequenceSimilarity_Levenshtein//调用序列相似度计算函数(这个函数速度要快(最小编辑),总体对结果的影响不大)
				//cal_sequenceSimilarity//调用序列相似度计算函数
				(seg0.sequence_structure, seg1.sequence_structure);

			return sim_component + sim_structure;
		}

		double TextAnalysor::cal_wordSimilarity(const size_t& i0, const size_t& i1)
		{
			if (template_enable && (textStream0->vector_wordsInfo[i0].ignored || textStream1->vector_wordsInfo[i1].ignored))//存在被忽略的词
				return IGNORED;//返回特殊标记

			//根据设置调用相应实现
			return DependenciesSet::functions_sequenceSimilarity[Settings::seqSim_imp_customized]
			(
				textStream0->vector_wordsInfo[i0].numberSequence, textStream1->vector_wordsInfo[i1].numberSequence
			);
		}
		
}

}

//8964586

