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
	//��ͷִʹ��߶���
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
			0.3,//0����n
			0.3,//1����v
			0.1,//2���ݴ�a
			0.0,//3����r
			0.1,//4����d
			0.0,//5���p
			0.1,//6����c
			0.0,//7����u
			0.1//8δ����x
		};

		
		double weight_wordClass_customized[End_WorldClass] =
		{
			//ʹ��Ĭ��ֵ���г�ʼ��
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
			0.7,//�ɷ�
			//0.0,//����(������)
			0.3,//�ṹ

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
			//ʹ��Ĭ��ֵ���г�ʼ��
			limits_default[0]
		};


		const DependenciesSet::I_NSS seqSim_imp_default = Jaccard;//Ĭ��ʹ�ýܿ������ƶ�

		DependenciesSet::I_NSS seqSim_imp_customized = seqSim_imp_default;

		const vector<Separator> separators_default =
		{
			{0,"\n"},
			{3,"."},
			{3,"��"},
			{8,","},
			{8,"��"},
			{8," "},
			{8,"\1\x1F",true}//�����˿����ַ�(��ȥ\0��)
		};
	
		vector<Separator> separators_customized = separators_default;
	
		vector<string> symbols_ignoredInTemplate_default =
		{
			"\n",
			" ",
			",",
			".",
			"��"
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
			//"��",
			//"��"
		};

		vector<string> symbols_ignoredInTemplate_customized = symbols_ignoredInTemplate_default;

		bool template_enable = false;

		void TextAnalysorSetter::resetAll()
		{
			int i;

			//���ô���Ȩ��
			for (i = 0; i < End_WorldClass; i++)
				weight_wordClass_customized[i] = weight_wordClass_default[i];
			//�������ƶ���ɳɷ�Ȩ��
			for (i = 0; i < End_SimilarityComposition; i++)
				weight_similarityComposition_customized[i] = weight_similarityComposition_default[i];
			//������ֵ���Ʊ�
			for (i = 0; i < End_Limits; i++)
				limits_customized[i] = limits_default[i];
			//�����������ƶ�ʵ�ַ���
			seqSim_imp_customized = seqSim_imp_default;
			//���÷ֶη�
			separators_customized = separators_default;
			//����ģ����Է�
			symbols_ignoredInTemplate_customized = symbols_ignoredInTemplate_default;

			Template::reset();
			//ģ��״̬��Ϊ�ر�
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
				if (Template::isEnabled())//ģ�����
					template_enable = true;//����ģ��
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
			size_t len_codeUnit = 0;//���뵥Ԫ�ֽ���
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
						len_codeUnit = 2;//���ֽ�UTF-8��Ԫ
					}
					else if ((c & 0xF0) == 0xE0)//1110 xxxx
					{
						len_codeUnit = 3;//���ֽ�UTF-8��Ԫ
					}
					else if ((c & 0xF8) == 0xF0)//1111 0xxx
					{
						len_codeUnit = 4;//���ֽ�UTF-8��Ԫ
					}
					else if ((c & 0xFC) == 0xE8)//1111 10xx
					{
						len_codeUnit = 5;//���ֽ�UTF-8��Ԫ
					}
					else if ((c & 0xFE) == 0xEC)//1111 110x
					{
						len_codeUnit = 6;//���ֽ�UTF-8��Ԫ
					}
					else
						throw TextAnalysisException(TextAnalysisException::CodingException);//�׳������쳣
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
				//ASCII��

				size = text.size();//��ȡ�ַ�����С(char����)
				ns.assign(size, 0);//vector���ռ�

				for (j = 0; j < size; j++)
					ns[j] = text[j];//charֱ��תint
			}
			else
			{
				//UTF-8����

				size = len;//��ȡ��Ԫ����(UTF-8���뵥Ԫ����)

				if (len <= 0)//��ָ����Ԫ����ʱ,�������΢�˷ѿռ�ķ�ʽ�������ٶ�
				{
					ns.assign(text.size() / 2, 0);//���ռ�
				}
				else
				{
					ns.assign(len, 0);//���ռ�
				}
				size = text.size();

				char code;
				size_t len_codeUnit = 0;//���뵥Ԫ�ֽ���
				int index = 0;
				k = 0;


				for (j = 0; index < size; j++)
				{
					code = text[index];//��ȡ��Ԫ�ĵ�һ���ֽ�

					if ((code & 0xE0) == 0xC0)//110x xxxx
					{
						len_codeUnit = 2;//���ֽ�UTF-8��Ԫ
					}
					else if ((code & 0xF0) == 0xE0)//1110 xxxx
					{
						len_codeUnit = 3;//���ֽ�UTF-8��Ԫ
					}
					else if ((code & 0xF8) == 0xF0)//1111 0xxx
					{
						len_codeUnit = 4;//���ֽ�UTF-8��Ԫ
					}
					else if ((code & 0xFC) == 0xE8)//1111 10xx
					{
						len_codeUnit = 0;//���ֽ�UTF-8��Ԫ
						j += 5;//����
					}
					else if ((code & 0xFE) == 0xEC)//1111 110x
					{
						len_codeUnit = 0;//���ֽ�UTF-8��Ԫ
						j += 6;//����
					}
					else
						throw TextAnalysisException(TextAnalysisException::CodingException);//�׳������쳣

					res = 0xffffffff;//����ֵ
					for (k = 0; k < len_codeUnit; k++)
					{
						res <<= sizeof(char) * 8;
						res |= 0xff;
						res &= text[index++];//


					}
					//index += len_codeUnit;
					ns[j] = res;//λ����õ���ֵ������������
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
					//ASCII��
					ns[count++] = text[index++];//ֱ��תint
				}
				else
				{
					//UTF-8����
					size_t len_codeUnit = 0;//���뵥Ԫ�ֽ���

					if ((code & 0xE0) == 0xC0)//110x xxxx
					{
						len_codeUnit = 2;//���ֽ�UTF-8��Ԫ
					}
					else if ((code & 0xF0) == 0xE0)//1110 xxxx
					{
						len_codeUnit = 3;//���ֽ�UTF-8��Ԫ
					}
					else if ((code & 0xF8) == 0xF0)//1111 0xxx
					{
						len_codeUnit = 4;//���ֽ�UTF-8��Ԫ
					}
					else if ((code & 0xFC) == 0xE8)//1111 10xx
					{
						len_codeUnit = 5;//���ֽ�UTF-8��Ԫ
					}
					else if ((code & 0xFE) == 0xEC)//1111 110x
					{
						len_codeUnit = 6;//���ֽ�UTF-8��Ԫ
					}
					else
						throw TextAnalysisException(TextAnalysisException::CodingException);//�׳������쳣

					res = 0xffffffff;//����ֵ
					for (k = 0; k < len_codeUnit; k++)
					{
						res <<= 8;//����8λ
						res |= 0xff;
						res &= text[index++];//
					}
					ns[count++] = res;//λ����õ���ֵ������������

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
			return (0 <= c && c < 128);//���ؽ��
		}

		bool checkChar_GBK_CODE(const char& c0, const char& c1)
		{
			//GBK��Χ:8140-FEFE
			int i0 = 0x000000FF, i1 = 0x000000FF;
			i0 &= c0;
			i1 &= c1;
			//������һ���ӵ�,��char�ĵ�һλ��1(��Ϊ��)�������,������ֵ����λ����ʱĬ����:(����ֵ)|0xff....ffxx,����λ��Ĭ����1
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
				flag_repetitive = false,//�ظ����
				flag_firstFound = false;//

			bool* flag_used;

			int index_crt = 0, index_temp = 0, size1 = 0, size2 = 0, total = 0;
			int n1, n2, count = 0, num_max;

			size1 = seq1.size();
			size2 = seq2.size();

			flag_used = new bool[num_max = (size1 > size2 ? size1 : size2)];//����������
			//memset(flag_used, 0, num_max * sizeof(bool));//��ձ������

			//�������ȡ����1�еĶ�Ԫ��(C(2,seq1.size()))
			for (index_crt = 0; index_crt < size1; index_crt++)
			{
				n1 = seq1[index_crt];//ȡ�ö�Ԫ���һ��Ԫ��

				memset(flag_used, 0, num_max * sizeof(bool));//���ʹ�ñ��

				for (index_temp = index_crt + 1; index_temp < size1; index_temp++)
				{
					n2 = seq1[index_temp];//ȡ�ö�Ԫ��ڶ���Ԫ��
					total++;//�ܴ�������

					//�����ظ�����
					if (n1 == n2)
					{
						flag_repetitive = true;//���Ϊ���ظ�����
						//count++;//�ظ�����ȷ����
						//continue;//��һ��
					}

					flag_firstFound = false;//������ʼ��

					//�͵ڶ������н��бȶ�
					for (int i = 0; i < size2; i++)
					{
						if (flag_used[i])//�Ѿ���ʹ��
							continue;//����
						if (!flag_firstFound&&seq2[i] == n1)
						{
							flag_firstFound = true;//��һ�����Ϊ�ҵ�
						}
						else if (seq2[i] == n2)
						{
							if (flag_firstFound)
							{
								flag_used[i] = true;//���Ϊʹ�ù�
								count++;//ͬʱ�ҵ�����
								break;//�˳�
							}
							//break;//�˳�
						}
						
					}
				}
			}

			if (flag_repetitive == false && size1 == size2)//û���ظ����
			{
				return (count * 1.0) / total;
			}

			//����
			//size2 = seq1.size();
			//size1 = seq2.size();

			//ͳ���Եڶ�������Ϊ��׼���ظ���
			for (index_crt = 0; index_crt < size2; index_crt++)
			{
				n1 = seq2[index_crt];//ȡ�ö�Ԫ���һ��Ԫ��

				memset(flag_used, 0, num_max * sizeof(bool));//���ʹ�ñ��

				for (index_temp = index_crt + 1; index_temp < size2; index_temp++)
				{
					n2 = seq2[index_temp];//ȡ�ö�Ԫ��ڶ���Ԫ��
					total++;//�ܴ�������

					//�����ظ�����
					if (n1 == n2)
					{
						flag_repetitive = true;//���Ϊ���ظ�����
						//count++;//�ظ�����ȷ����
						//continue;//��һ��
					}

					flag_firstFound = false;//������ʼ��

					//�͵ڶ������н��бȶ�
					for (int i = 0; i < size1; i++)
					{
						if (flag_used[i])//�Ѿ���ʹ��
							continue;//����
						if (!flag_firstFound && seq1[i] == n1)
						{
							flag_firstFound = true;//��һ�����Ϊ�ҵ�
						}
						else if (seq1[i] == n2)
						{
							if (flag_firstFound)
							{
								flag_used[i] = true;//���Ϊʹ�ù�
								count++;//ͬʱ�ҵ�����
								break;//�˳�
							}
							//break;//�˳�
						}

					}
				}
			}
			delete[]flag_used;//�ͷ��ڴ�

			return (count * 1.0) / total;
		}

		double cal_sequenceSimilarity_Jaccard(const NumberSequence& seq0, const NumberSequence& seq1)
		{
			size_t size_intersection = 0;//������С
			size_t size_union = 0;//������С

			NumberSequence seq0_c=seq0,seq1_c=seq1;

			//���������н��зǽ�������(�Ż�)
			std::sort(seq0_c.begin(), seq0_c.end());//����
			std::sort(seq1_c.begin(), seq1_c.end());//����
			size_t i0 = 0, i1 = 0;
			size_t size0 = seq0.size(), size1 = seq1.size();
			//0 1 2 3
			//0 2 2 3
			long long compareResult;
			while (i0<size0&&i1<size1)
			{
				//��ȡ�ȽϽ��,С�����Ż�
				compareResult = seq0_c[i0] - seq1_c[i1];
				if (compareResult==0)
				{
					//���
					size_intersection++;
					i0++;
					i1++;
				}
				else if (compareResult < 0)
				{
					//С��
					i0++;
				}
				else
				{
					//����
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
			int** matrix = nullptr;//����
			double result = 1.0;

			size1 = seq1.size();
			size2 = seq2.size();
			//cout << "<LEV> size1: " << size1 << " size2: " << size2 << endl;
			if (size1 > limits_customized[Limit_Levenshtein_MAX] && size2 > limits_customized[Limit_Levenshtein_MAX])//����
				throw TextAnalysisException(TextAnalysisException::OverLimitException,"<AT> TextAnalysis.cpp <LINE>585");//�׳������쳣
			size_max = size1 > size2 ? size1 : size2;//ȡ�ϴ�ֵ
			size1++, size2++;

			//��������ڴ�
			matrix = new int* [size2];
			for (i = 0; i < size2; i++)
				matrix[i] = new int[size1];

			//��ʼ������
			for (i = 0; i < size1; i++)
				matrix[0][i] = i;
			for (i = 1; i < size2; i++)
				matrix[i][0] = i;

			for (i = 1; i < size2; i++)//ÿһ��
			{
				for (j = 1; j < size1; j++)//ÿһ���е�ÿһ��
				{
					min = matrix[i][j - 1] + 1;//��
					if (min > (temp = matrix[i - 1][j] + 1))//��
						min = temp;
					//����
					if (seq1[j - 1] == seq2[i - 1])//������ĸ���
					{
						if (min > (temp = matrix[i - 1][j - 1]))
							min = temp;
					}
					else//������ĸ����
					{
						if (min > (temp = matrix[i - 1][j - 1] + 1))
							min = temp;
					}
					matrix[i][j] = min;
				}
			}

			result = 1 - (1.0 * min) / size_max;

			//�ͷ��ڴ�
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
				return IGNORED;//���ڿձ�,����������

			for (i = 0; i < size0; i++)
			{
				if (ss0[i] < 0)//С��0�ı�����
					continue;
				count0++;
				res += ss0[i];
			}
			if (count0 == 0)//ȫ���Ǳ����Ե���
				return IGNORED;
			for (i = 0; i < size1; i++)
			{
				if (ss1[i] < 0)//С��0�ı�����
					continue;
				count1++;
				res += ss1[i];
			}
			if (count1 == 0)//ȫ���Ǳ����Ե���
				return IGNORED;
			if (count0 == 0 && count1 == 0)
				return 0;
			return res / (2.0 * (count0 > count1 ? count0 : count1));//���ؽ��
			//return res / (size0 + size1);//���ؽ��
		}

		double cal_result_SimilaritySequence_summary_IGN(const SimilaritySequence& ss0, const SimilaritySequence& ss1)
		{
			double res = 0;
			size_t i, size0, size1, count0 = 0, count1 = 0;
			size0 = ss0.size();
			size1 = ss1.size();

			if (size0 == 0 || size1 == 0)
				return IGNORED;//���ڿձ�,����������

			for (i = 0; i < size0; i++)
			{
				if (ss0[i] < 0)//С��0�ı�����
				{
					continue;
				}
				count0++;
				res += ss0[i];
			}
			if (count0 == 0)//ȫ���Ǳ����Ե���
				return IGNORED;

			for (i = 0; i < size1; i++)
			{
				if (ss1[i] < 0)//С��0�ı�����
					continue;
				count1++;
				res += ss1[i];
			}
			if (count1 == 0)//ȫ���Ǳ����Ե���
				return IGNORED;

			//if (count0 == 0 && count1 == 0)
			//	return 0;
			return res / (count0 + count1);//���ؽ��
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
			int** matrix = nullptr;//����
			double result = 1.0;

			if (codeType == ASCII)//ASCII��
			{
				size1 = str1.size();
				size2 = str2.size();
				size_max = size1 > size2 ? size1 : size2;//ȡ�ϴ�ֵ
				size1++, size2++;

				//��������ڴ�
				matrix = new int* [size2];
				for (i = 0; i < size2; i++)
					matrix[i] = new int[size1];

				//��ʼ������
				for (i = 0; i < size1; i++)
					matrix[0][i] = i;
				for (i = 1; i < size2; i++)
					matrix[i][0] = i;

				for (i = 1; i < size2; i++)//ÿһ��
				{
					for (j = 1; j < size1; j++)//ÿһ���е�ÿһ��
					{
						min = matrix[i][j - 1] + 1;//��
						if (min > (temp = matrix[i - 1][j] + 1))//��
							min = temp;
						//����
						if (str1[j - 1] == str2[i - 1])//������ĸ���
						{
							if (min > (temp = matrix[i - 1][j - 1]))
								min = temp;
						}
						else//������ĸ����
						{
							if (min > (temp = matrix[i - 1][j - 1] + 1))
								min = temp;
						}
						matrix[i][j] = min;
					}
				}

				result = 1 - (1.0 * min) / size_max;

			}
			else if (codeType == GBK)//GBK��
			{
				size1 = str1.size();
				size2 = str2.size();
				size_max = size1 > size2 ? size1 : size2;//ȡ�ϴ�ֵ
				size1++, size2++;

				//��������ڴ�
				matrix = new int* [size2];
				for (i = 0; i < size2; i++)
					matrix[i] = new int[size1];

				//��ʼ������
				for (i = 0; i < size1; i++)
					matrix[0][i] = i;
				for (i = 1; i < size2; i++)
					matrix[i][0] = i;

				for (i = 1; i < size2; i++)//ÿһ��
				{
					for (j = 1; j < size1; j++)//ÿһ���е�ÿһ��
					{
						min = matrix[i][j - 1] + 1;//��
						if (min > (temp = matrix[i - 1][j] + 1))//��
							min = temp;
						//����
						if (str1[j - 1] == str2[i - 1])//������ĸ���
						{
							if (min > (temp = matrix[i - 1][j - 1]))
								min = temp;
						}
						else//������ĸ����
						{
							if (min > (temp = matrix[i - 1][j - 1] + 1))
								min = temp;
						}
						matrix[i][j] = min;
					}
				}

				result = 1 - (1.0 * min) / size_max;
			}

			//�ͷ��ڴ�
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
					int** matrix = nullptr;//����
					double result = 1.0;

					size0 = len0;
					size1 = len1;
					if (size0 > Limit_Levenshtein_MAX && size1 > Limit_Levenshtein_MAX)//����
						throw TextAnalysisException(TextAnalysisException::OverLimitException);//�׳������쳣
					size_max = size0 > size1 ? size0 : size1;//ȡ�ϴ�ֵ
					size0++, size1++;

					//��������ڴ�
					matrix = new int* [size1];
					for (i = 0; i < size1; i++)
						matrix[i] = new int[size0];

					//��ʼ������
					for (i = 0; i < size0; i++)
						matrix[0][i] = i;
					for (i = 1; i < size1; i++)
						matrix[i][0] = i;

					for (i = 1; i < size0; i++)//ÿһ��
					{
						for (j = 1; j < size1; j++)//ÿһ���е�ÿһ��
						{
							min = matrix[i][j - 1] + 1;//��
							if (min > (temp = matrix[i - 1][j] + 1))//��
								min = temp;
							//����
							if (seq0[j - 1+start0] == seq1[i - 1+start1])//������ĸ���
							{
								if (min > (temp = matrix[i - 1][j - 1]))
									min = temp;
							}
							else//������ĸ����
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
			return (details[exceptionType]+" : "+ otherInfo).c_str();//�����쳣��Ϣ
		}


		string Template::content_origin;
		string Template::content_processed;
		NumberSequence Template::numSeq;
		bool Template::enabled = false;


		bool Template::setTemplate(const string& _content)
		{
			if (_content.size() == 0)
				return false;
			//ȫ������
			reset();
			//����
			content_processed = content_origin = _content;

			//ȥ�ָ���
			size_t num = symbols_ignoredInTemplate_customized.size();
			size_t pos, len_separator;

			for (int i = 0; i < num; i++)//���ȥ�������Եķ���
			{
				pos = 0;
				len_separator = symbols_ignoredInTemplate_customized[i].length();
				//����
				while ((pos = content_processed.find(symbols_ignoredInTemplate_customized[i], pos)) != string::npos)
				{
					content_processed.erase(pos, len_separator);//����
				}
			}

			//��ȡu8��Ԫ��
			size_t count = getCount_codeUnit_in_utf8_text(content_processed);
			//���ռ�
			numSeq.assign(count, 0);
			//������������
			transformText_to_NumberSequence(content_processed, numSeq);
			enabled = true;
			return true;
		}

		void Template::reset()
		{
			//�������
			content_origin.clear();
			content_processed.clear();
			numSeq.clear();
			//��������Ϊfalse
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
			, vector_wordsInfo()//vectorʹ��Ĭ�Ϲ���
		{
			//if (code == GBK)
			//	this->text = GbkToUtf8(text.c_str());//תUTF-8
		}

		void  TextStream::setText(const string& text, const EncodingType& code)
		{
			this->text = text;
			//if (code == GBK)
			//	this->text = GbkToUtf8(text.c_str());//תUTF-8
			//numberSequence.assign(text.size(), 0);
		}

		const string& TextStream::getText()const
		{
			// TODO: �ڴ˴����� return ���
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
				<< "<TextStream>\n<Text>\n" << ts.text << endl;//����ı�
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

			return os << endl;//����������
		}

		void TextStream::generateNumberSequence()
		{
			//��ȡUTF-8��Ԫ��
			num_units = getCount_codeUnit_in_utf8_text(text);
			//�������ռ�
			numSeq_text.assign(num_units, 0);
			//��ȡӳ��õ�����������(��ȡת���õ�����������)
			transformText_to_NumberSequence(text, numSeq_text);

			return;
		}

		void TextStream::markTemplate()
		{
			//���ģ�������
			if (Template::isEnabled)
			{
				//ģ�����
				//��ģ�����ݱ��Ϊ����

				flags_ignored.assign(num_units, false);//���ռ�
				size_t index_text, index_template, temp;
				size_t size_template = Template::numSeq.size();
				size_t limit;

				//���ɨ��
				for (int i = 0; i < 1/*num_templateScanTimes*/; i++)
				{
					//�㷨��Ҫ�Ľ�,���Գ���ʹ�ö�̬�滮

					for (index_text = 0, index_template = 0; index_text < num_units && index_template < size_template;)
					{
						if (flags_ignored[index_text])//�Ѿ������
						{
							index_text++;
							continue;//����
						}

						if (numSeq_text[index_text] == Template::numSeq[index_template])//��ͬ
						{
							flags_ignored[index_text] = true;//���Ϊ����
							index_template++;//ģ��ָ�����
							index_text++;
						}
						else
							index_text++;
						//else//����ͬ
						//{
						//	//���µ��㷨�Ƚϼ�ª,Ӧ�ò�ȡ�Աȷ�,�ҵ���̵�

						//	//����ģ��,�����Ƿ���ں�Ŀ���ı�����ͬ�ĵ�Ԫ
						//	temp = index_template + 5;//����������5��
						//	limit = temp >= size_template ? size_template : temp;
						//	temp = index_template;//��¼֮ǰ��ֵ
						//	for (; index_template < limit && numSeq_text[index_text] != Template::numSeq[index_template]; index_template++)
						//		;
						//	if (index_template == limit)//û��������
						//	{
						//		index_template = temp;//�ָ�֮ǰ��ֵ
						//		index_text++;
						//	}
						//}
					}
				}
			}
			//ͳ��δ�����Ե���Ԫ����
			num_unitsOutsizeTemplate = count(flags_ignored.begin(), flags_ignored.end(), false);
		}

		void TextStream::segmentWords()
		{
			//NLPIR�ִ�,����
			////��ʼ��
			//if (NLPIR_Init(DataPath.c_str(), encodingType, "") == 0)
			//	//��ʼ��ʧ��
			//	throw TextAnalysisException(TextAnalysisException::SegSysException);//�׳��쳣
			////���зִ�,���ӻ���������Ա����
			//text_processed = NLPIR_ParagraphProcess(text.c_str(), true);
			////�ִ�,���б�ṹ�����ֶ�,����ȡ����
			//table_wordInfo=NLPIR_ParagraphProcessA(text.c_str(),&num_words);
			
			this->text.append(separators_customized[0].str_separator);//ĩβ��һ���ֶη�,��Ϊ�ڱ�

			//��ͷִ�,�������vector����
			jieba.CutForSearch(this->text, vector_words_extra);
			//jieba.Tag(this->text, vector_words_tags);
			num_words = vector_words_extra.size();//��ȡ��Ԫ��������ȡ

			//TF.IDF�㷨��ȡ�ؼ���
			jieba.extractor.Extract(text, vector_keyWords, num_words / 20);

			return;
		}

		void TextStream::segmentText()
		{
			size_t originSize = vector_words_extra.size();
			size_t step;//��ȡ�ֶη�����
			size_t  i_origin = 0, i_new = 0, last_new = 0;//��һ���ֶη�����,��ǰ�ֶη�����
			SegmentInfo seg;
			size_t  count_words = 0;
			num_words = 0;
			WordUnit emptyWU;
			vector_wordsInfo.assign(originSize, emptyWU);//���̶���С�Ŀռ�
			size_t num_separator_customized = separators_customized.size();
			//���Ѱ�ҷֶη�,ͬʱ���ɶ�����Ϣ
			for (i_origin = 0, i_new = 0; i_origin < originSize; i_origin++, i_new++)
			{

				int i;
				//����ȽϷֶη�
				for (i = 0; i < num_separator_customized; i++)
				{
					if (separators_customized[i].asciiRange == false)
					{
						if (vector_words_extra[i_origin].word == separators_customized[i].str_separator)//���ֶַη�
							break;//��ǰ����
					}
					else
					{
						//ASCII�뷶Χ
						char c = vector_words_extra[i_origin].word[0];//ֻ����һ���ַ�
						if (c >= separators_customized[i].str_separator[0] && c <= separators_customized[i].str_separator[1])//���ַ�Χ�е�ASCII�ַ�
							break;//��ǰ�˳�
					}
					
				}
				if (i == num_separator_customized)//��ǰ���Ƿֶη�
				{
					//�ϲ��ִʽ����Ϣ������vector_wordsInfo
					if(Template::isEnabled)//���ģ��״̬
						vector_wordsInfo[num_words].ignored = flags_ignored[vector_words_extra[i_origin].unicode_offset];//һ����Ԫ�Ƿ񱻺���ȡ���ڵ�һ����Ԫ�Ƿ񱻺���
					vector_wordsInfo[num_words].offset = vector_words_extra[i_origin].offset;
					vector_wordsInfo[num_words].offset_codeUnit = vector_words_extra[i_origin].unicode_offset;
					vector_wordsInfo[num_words].word = vector_words_extra[i_origin].word;
					vector_wordsInfo[num_words].num_codeUnit = vector_words_extra[i_origin].unicode_length;
					vector_wordsInfo[num_words].type = jieba.LookupTag(vector_wordsInfo[num_words].word);//���ô���
					num_words++;
					count_words++;
					continue;//Ѱ����һ��
				}

				i_new--;
				//�Ƿֶη�
				//�ֶη��������Ԫvector
				if (count_words <= separators_customized[i].num_unitMin)//����Ƿ�С����ֵ
					continue;//С��ָ��,����

				seg.index_begin = last_new;//��ͷ

				seg.index_end = i_new;//��β(�������ֶη�)
				last_new = i_new + 1;

				vector_segmentsInfo.push_back(seg);//����α�
				count_words = 0;
			}

			//��ȥ�������(����ʱ�򿪵���΢��һ��,ɨ��֮��ֶη��ᱻ����,����ɾ������Ҫ�Ŀ���)
			vector_wordsInfo.erase(vector_wordsInfo.end() - (originSize - num_words), vector_wordsInfo.end());
			num_words = vector_wordsInfo.size();
			num_segs = vector_segmentsInfo.size();//�õ�������

			//����������Ϣ��,�����﷨�ṹ����
			for (int i = 0; i < num_segs; i++)
			{
				SegmentInfo& crtSeg = vector_segmentsInfo[i];//��ȡ���������,�������

				//vector���ռ�
				crtSeg.sequence_structure.assign(crtSeg.index_end - crtSeg.index_begin + 1, 0);

				int j, k;
				//���������е�ÿһ��U8��Ԫ
				for (j = crtSeg.index_begin, k = 0; j <= crtSeg.index_end; j++, k++)
				{
					crtSeg.sequence_structure[k] = static_cast<int>(vector_wordsInfo[j].type[0]);
					//if (vector_wordsInfo[j].ignored)//������ʱ�����
					//	crtSeg.ignored = true;//��������Ϊ������
				}
			}


			return;
		}

		void TextStream::scanWords()
		{
			SegmentInfo textSegmentInfo_temp;
			int index_wordUnit_temp;
			int num_seg = vector_segmentsInfo.size();//��ȡ������

			int i = 0, j = 0;
			int index_crtSeg = 0;//��ǰ��������

			for (i = 0; i < num_words && index_crtSeg < num_seg; i++)//���ɨ��ִʴ���
			{
				//index_wordUnit_temp = table_wordInfo + i;//��ȡ��ǰ����ָ��
				if (i > vector_segmentsInfo[index_crtSeg].index_end)//��ǰ�����Ѿ������˵�ǰѡ�еĶ��������ҽ�
					index_crtSeg++;//��λ����һ������

				//����ǰ������������ӵ���ǰ����Ϣ�Ķ�Ӧ���Ա���
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
			//�����ʱ�,�����������
			for (i = 0; i < num_words; i++)
			{
				WordUnit& crtWU = vector_wordsInfo[i];//��ȡ����,�������

				//������������
				//transformWord_to_NumberSequence(crtWU.word, crtWU.num_codeUnit, crtWU.numberSequence);//�������ռ�Ĳ���
				crtWU.numberSequence.assign(crtWU.num_codeUnit, 0);//���ռ�
				for (j = 0; j < crtWU.num_codeUnit; j++)
				{
					crtWU.numberSequence[j] = numSeq_text[crtWU.offset_codeUnit + j];
				}
			}

			//���ɹؼ��ʵ���������
			size_t num_KW = vector_keyWords.size();
			NumberSequence ns;
			if (num_KW != 0)
				vector_NS_keyWords.assign(num_KW, ns);
			//else
			//	vector_NS_keyWords.clear();//���
			for (i = 0; i < num_KW; i++)
			{
				transformWord_to_NumberSequence
				(
					vector_keyWords[i].first,
					-1,//��ָ����Ԫ����
					vector_NS_keyWords[i]
				);
			}

			return;//�˳�
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

		

		//��̬��������
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

			//����ģ�庯��
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
				(weight_similarityComposition_customized[SC_main_KeyWords]) * sim_keyWords;//���ؽ��
		}

		void TextAnalysor::clear()
		{
			textStream0 = textStream1 = nullptr;
			sim_segments = 0;
			sim_keyWords = 0;
		}

		double TextAnalysor::cal_segmentSimilarity(const SegmentInfo& seg0, const SegmentInfo& seg1)
		{
			if ((seg0.ignored || seg1.ignored))//�������ȽϵĶ����д���һ�������Ե�
				return IGNORED;//����������

			double sim_component = 0, sim_sequence = 0, sim_structure = 0;//�ɷ����ƶ�,�������ƶ�,�ṹ���ƶ�
			int i;
			double rest = 1.0, temp;//ʣ��Ľ����������ְٷֱ�


			//����ÿһ�����Ա�,����ɷ����ƶ�
			for (i = 0; i < End_WorldClass; i++)
			{
				if (weight_wordClass_customized[i] == 0.0)
					continue;//����Ȩ��Ϊ0�Ĵ��Ա�

				//������Ա�ɷ����ƶȽ��(ģ�庯��)
				temp = cal_similarity_crossing_add(seg0.set_indexList[i], seg1.set_indexList[i], cal_wordSimilarity);
				if (temp == IGNORED)//���������Ϊ�ջ��ߴ�����ȫ�����Ե�����
					rest -= weight_wordClass_customized[i];//��ȥȨ��
				else
					sim_component += weight_wordClass_customized[i] * temp;
			}
			if (rest == 0)
				return IGNORED;
			else
				sim_component /= rest;//�Ŵ�
			

			//�ɷ����ƶȳ���Ȩ��
			sim_component *= (weight_similarityComposition_customized[SC_Component]);
			//sim_sequence=DependenciesSet::cal_sequenceSimilarity_Levenshtein()

			//����(�﷨)�ṹ���ƶ�
			sim_structure = weight_similarityComposition_customized[SC_Structure] *
				cal_sequenceSimilarity_Levenshtein//�����������ƶȼ��㺯��(��������ٶ�Ҫ��(��С�༭),����Խ����Ӱ�첻��)
				//cal_sequenceSimilarity//�����������ƶȼ��㺯��
				(seg0.sequence_structure, seg1.sequence_structure);

			return sim_component + sim_structure;
		}

		double TextAnalysor::cal_wordSimilarity(const size_t& i0, const size_t& i1)
		{
			if (template_enable && (textStream0->vector_wordsInfo[i0].ignored || textStream1->vector_wordsInfo[i1].ignored))//���ڱ����ԵĴ�
				return IGNORED;//����������

			//�������õ�����Ӧʵ��
			return DependenciesSet::functions_sequenceSimilarity[Settings::seqSim_imp_customized]
			(
				textStream0->vector_wordsInfo[i0].numberSequence, textStream1->vector_wordsInfo[i1].numberSequence
			);
		}
		
}

}

//8964586

