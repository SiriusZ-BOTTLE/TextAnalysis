#pragma once//������һ��
#pragma warning(disable:4996)
//��׼C++��
#include <vector>//����
#include <string>//�ַ���
#include <iostream>//���������
#include <sstream>
#include <chrono>//���ܵ�C++11��,ǿ���ʱ�ӿ�,��ȷ�����뼶������û��!
#include <ratio>




//�궨��
#define DLL_EXPORTS//����DLL
#define INSIDE_DLL//DLL�ڲ�

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


//��ͷִ�
#include "include/cppjieba/Jieba.hpp"






//��ͷִʿ������ļ�·��
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
Author:ƿ��
Date:2019.07.13-2019.09.16
Description:
	�ı�������
********************************************************************************/

/********************************************************************************
Optimization(�Ż�):
	[1]	��ʼ���ı�����ʱ���������ı�����Ӧ����������,����Ƚ�ʱ���ظ�����
	[2]	����vecotr����ֵ¼�붼�ȿ��ռ��ֵ,�����ٶ�
********************************************************************************/

/********************************************************************************
*TextAnalysis:�ı��������������ռ�
	������ѭģ�黯�����ȡ�ýϺõĿ���չ�ԺͿ�ά����(ʵ�ֵû�����)
********************************************************************************/
namespace TextAnalysis
{
	using namespace std;
	using namespace cppjieba;
	using namespace Paths_JieBa;
	
	extern cppjieba::Jieba jieba;//��ͷִʹ��߶���

	/********************************************************************************
	*���弯��
	*	�������ռ��ڶ���һЩ�������ݽṹ�ͱ���
	********************************************************************************/
	namespace DefinitionsSet
	{
		/****************************************
		*��������
		*	�о�һЩ��������.
		*
		****************************************/
		enum EncodingType
		{
			GBK = 0,//GBK����
			UTF_8 = 1,//UTF-8����(Ĭ��)
			ASCII//ASCII����
		};

		/****************************************
		*����
		*	�о�һЩ��������.
		*	���ǵ������кܶ��ִ���,����ѡ��һЩ��Ҫ����,
		*	�������ͽ�������.
		*	��ö�����ṩ����Լ��Ĺ���,
		*	�ֶα��е������С����End_WorldClass��Ա,
		*	���������һ���µ����Ӱ�����������
		*	��ø���identifier_worldClass��weight_default
		****************************************/
		enum WorldClass
		{
			Noun,//0����n
			Verb,//1����v
			Adj,//2���ݴ�a
			Pronoun,//3����r
			Adverb,//4����d
			Prep,//5���p
			Conj,//6����c
			Particle,//7����u
			Unclassified,//8δ����x
			End_WorldClass//�ڱ�
		};

		/****************************************
		*һЩ��������
		****************************************/
		enum Limits
		{
			Limit_Levenshtein_MAX,//�༭�����㷨�����㳤��(�����ı��������������׳��쳣)
			//Limit_Segment_MIN,//�ֶ�ʱ�����������С��Ԫ��(С�����ֵ�ᱻ����)
			End_Limits//�ڱ�
		};

		/****************************************
		*���ƶȳɷ�
		****************************************/
		enum SimilarityComposition
		{
			SC_Component,//�ɷ�
			//SC_Sequence,//����(������)
			SC_Structure,//�ṹ

			SC_main_Segments,//����
			SC_main_KeyWords,//�ؼ���
			End_SimilarityComposition//�ڱ�
		};

		/****************************************
		*�������ƶ��㷨ʵ��
		****************************************/
		enum Implements_NumberSequenceSimilarity
		{
			BinaryScan,//��Ԫɨ���㷨(����)
			Levenshtein,//�༭�����㷨(���)
			Jaccard,//�ܿ����㷨(�ڴ�������)
			End_Implements_NumberSequenceSimilarity//�ڱ�
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

		typedef std::vector<long long> NumberSequence;//��������
		typedef NumberSequence IndexSequence;//��������
		typedef std::vector<double> FloatSequence;//��������
		typedef FloatSequence SimilaritySequence;
		typedef FloatSequence FloatVector;

		struct WordUnit
		{
			bool ignored{false};
			string word{};
			size_t offset;//��ʼ����
			size_t offset_codeUnit{0};//u8����
			size_t num_codeUnit{0};//��Ԫ(UTF-8���뵥Ԫ)����
			string type{};//���Ա�ʶ��

			NumberSequence numberSequence{};//��������

			friend ostream& operator<<(ostream& os, const WordUnit& wu);
		};

		typedef vector<WordUnit> WordUnitsVector;

		//�ָ���
		struct Separator
		{
			size_t num_unitMin{0};//��С��Ԫ��
			string str_separator{};//�ֶη�
			bool asciiRange{false};//�ֶη��Ƿ���һ��ASCII�뷶Χ�ı��
		};


		/****************************************
		*�ı�����Ϣ�ṹ
		****************************************/
		struct SegmentInfo
		{
			bool ignored;//��Ǹö����Ƿ�ᱻ����
			size_t index_begin;//�ο�ͷ����a
			size_t index_end;//�ν�β����

			//�����ǻ��ڷִʽ�����������
			IndexSequence set_indexList[End_WorldClass];

			NumberSequence sequence_structure;//�﷨�ɷֽṹ����(��ν���ȵ�)

			//���캯��
			SegmentInfo();

			//IndexSequence indexList_noun;//����������
			//IndexSequence indexList_verb;//����������
			//IndexSequence indexList_adjective;//���ݴ�������
			//IndexSequence indexList_pronoun;//����������
			//IndexSequence indexList_adverb;//����������
			//IndexSequence indexList_preposition;//���������
			//IndexSequence indexList_conjunction;//����������
			//IndexSequence indexList_auxiliary;//����������

			//����<<
			friend ostream& operator<<(ostream& os, const SegmentInfo& tsi);
		};

		typedef vector<SegmentInfo> TextSegmentInfoVector;//�ı���

		
		typedef vector<bool> FlagsVector;
	
		/****************************************
		*����ı�ʶ��
		*	ʹ����constexpr�ؼ��������Ա�ʾ��Ϊ����ʱ����ֵ����,������case���(const�ǲ��е�,��֪��Ϊʲô)
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

		//���Ա��
		constexpr int IGNORED = -1;
	}

	/********************************************************************************
	*�������
	*	�������ռ��ڶ�����һЩ�����ĺ���
	********************************************************************************/
	namespace DependenciesSet
	{
		using namespace DefinitionsSet;//���붨�弯

		size_t getCount_codeUnit_in_utf8_text(const string& text);

		/********************************************************************************
		*Function:transformWord_to_NumberSequence()
		*Description:�ı�ת��������
		*Calls:����
		*Input:
		*	<string text:����Ŀ���ı����ַ���>
		*	<int start:���ַ����еĿ�ʼλ��>
		*	<int len:���ַ����е��ܳ���>
		*	<EncodingType code:��������>
		*Output:��
		*Return:ת���õ�����������
		*Remarks:
		*	[1]����Ϊ�ڲ�����
		*	[2]�����ַ�����Ŀ���ı����ַ����е�λ��
		********************************************************************************/
		void transformWord_to_NumberSequence
		(const string& text,const int& len, NumberSequence& ns);

		/********************************************************************************
		*Function:transformText_to_numberSequence()
		*Description:�ı�ת��������
		*Calls:����
		*Input:
		*	<string text:����Ŀ���ı����ַ���>
		*	<int start:���ַ����еĿ�ʼλ��>
		*Output:��
		*Return:ת���õ�����������
		*Remarks:
		*	[1]����Ϊ�ڲ�����
		*	[2]�ú����ٶ�������������㹻�Ŀռ�
		********************************************************************************/
		void transformText_to_NumberSequence
		(const string& text, NumberSequence& ns);

		/********************************************************************************
		*Function:checkText_ASCII_CODE()
		*Description:�жϸ������ַ����Ƿ����ASCII����Ĺ���
		*Calls:����
		*Input:
		*	<string text:�������ַ���>
		*
		*Output:��
		*Return:true���ȫ����ASCII����,��֮false
		*Remarks:
		*	[1]����Ϊ�ڲ�����
		********************************************************************************/
		inline bool checkText_ASCII_CODE(const string& text);

		/********************************************************************************
		*Function:checkChar_ASCII_CODE()
		*Description:�жϸ������ַ��Ƿ���ASCII����
		*Calls:����
		*Input:
		*	<char c:�������ַ�>
		*
		*Output:��
		*Return:true����������ַ�����ASCII����,��֮false
		*Remarks:
		*	[1]����Ϊ�ڲ�����
		********************************************************************************/
		inline bool checkChar_ASCII_CODE(const char& c);

		/********************************************************************************
		*Function:checkChar_GBK_CODE()
		*Description:�жϸ������ַ��Ƿ���ASCII����
		*Calls:����
		*Input:
		*	<char c0:�������ַ�1>
		*	<char c1:�������ַ�2>
		*Output:��
		*Return:true����������ַ�����ASCII����,��֮false
		*Remarks:
		*	[1]����Ϊ�ڲ�����
		********************************************************************************/
		inline bool checkChar_GBK_CODE(const char& c0, const char& c1);


		/********************************************************************************
		*Function:checkChar_UTF8_CODE()
		*Description:�жϸ������ַ��Ƿ���ASCII����
		*Calls:����
		*Input:
		*	<string text:����Ŀ���ı����ַ���>
		*	<int start:���ַ����еĿ�ʼλ��>
		*	<int len:���ַ����е��ܳ���>
		*
		*Output:��
		*Return:true����������ַ�����UTF-8�������,��֮false
		*Remarks:
		*	[1]����Ϊ�ڲ�����
		********************************************************************************/
		//inline bool checkChar_UTF8_CODE(const string& text, const int& start, const int& len);


		

		//����ָ������
		extern double (* const functions_sequenceSimilarity[End_Implements_NumberSequenceSimilarity])(const NumberSequence& seq0, const NumberSequence& seq1);

		/********************************************************************************
		*Function:cal_sequenceRepetitiveRate()
		*Description:���������������е����ƶ�(ʹ�ö�Ԫɨ��)
		*Calls:����
		*Input:
		*	<NumberSequence seq1:��������1>,
		*	<NumberSequence seq2:��������2>
		*
		*Output:��
		*Return:���ؼ���õ������ƶ�[0.0-1.0]
		*Remarks:
		*	[1]����Ϊ�ڲ�����;���㷨�ȶ�(������������е�˳���޹�)
		********************************************************************************/
		double cal_sequenceSimilarity_BinaryScan(const NumberSequence& seq1, const NumberSequence& seq2);

		/********************************************************************************
		*Function:cal_sequenceRepetitiveRate()
		*Description:���������������е����ƶ�(Jaccard(�ܿ���)���ƶ�)
		*Calls:����
		*Input:
		*	<NumberSequence seq1:��������1>,
		*	<NumberSequence seq2:��������2>
		*
		*Output:��
		*Return:���ؼ���õ������ƶ�[0.0-1.0]
		*Remarks:
		*	[1]����Ϊ�ڲ�����;���㷨�ȶ�(������������е�˳���޹�)
		********************************************************************************/
		double cal_sequenceSimilarity_Jaccard(const NumberSequence& seq1, const NumberSequence& seq2);

		/********************************************************************************
		*Function:cal_wordSimilarity_Levenshtein()
		*Description:���������������е����ƶ�,ʹ����С�༭�����㷨(Levenshtein)
		*Calls:����
		*Input:
		*	<NumberSequence seq1:�ַ���1>,
		*	<NumberSequence seq2:�ַ���2>,
		*Output:��
		*Return:���ؼ���õ������ƶ�[0.0-1.0]
		*Remarks:
		*	[1]����Ϊ�ڲ�����;���㷨�ȶ�(������������е�˳���޹�)
		*	[2]�������ڴ����ȹ������������
		********************************************************************************/
		double cal_sequenceSimilarity_Levenshtein(const NumberSequence& seq1, const NumberSequence& seq2);


		//inline double cal_sequenceSimilarity(const NumberSequence& seq0, const NumberSequence& seq1, const Implements_NumberSequenceSimilarity& implement);
	
		/********************************************************************************
		*Function:cal_wordSimilarity_Levenshtein()
		*Description:������������(����)�����ƶ�,ʹ����С�༭�����㷨(Levenshtein)
		*Calls:����
		*Input:
		*	<string str1:�ַ���1>,
		*	<string str2:�ַ���2>,
		*	<EncodingType codeType:��������>
		*Output:��
		*Return:���ؼ���õ������ƶ�[0.0-1.0]
		*Remarks:
		*	[1]�ú���������;���㷨�ȶ�(����������ַ�����˳���޹�)
		*	[2]GBK���ڴ������Ĵ���,ASCII���ڴ���Ӣ�ĵ���(�����)
		*	[3]�������ڴ����ȹ�����ַ���
		********************************************************************************/
		/*double cal_wordSimilarity_Levenshtein
		(const string& str1, const string& str2, const EncodingType& codeType);*/

		/********************************************************************************
		*Function:cal_wordSimilarity_Levenshtein()
		*Description:���������������е����ƶ�,ʹ����С�༭�����㷨(Levenshtein)
		*Calls:����
		*Input:
		*	<NumberSequence seq1:�ַ���1>,
		*	<NumberSequence seq2:�ַ���2>,
		*Output:��
		*Return:���ؼ���õ������ƶ�[0.0-1.0]
		*Remarks:
		*	[1]������;���㷨�ȶ�(������������е�˳���޹�)
		*	[2]�������ڴ����ȹ������������
		********************************************************************************/
		/*double cal_sequenceSimilarity_Levenshtein
		(const NumberSequence& seq0,const int &start0,const int &len0, const NumberSequence& seq1, const int& start1, const int& len1);*/

		/********************************************************************************
		*Function:cal_result_SimilaritySequence_doubleMax_IGN()
		*Description:�������ƶ����еĽ��
		*Calls:����
		*Input:
		*	<SimilaritySequence ss0:���ƶ�����1>
		*Output:��
		*Return:���ؼ���õ������ƶȽ��[0.0-1.0]
		*Remarks:
		********************************************************************************/
		double cal_result_SimilaritySequence_doubleMax_IGN(const SimilaritySequence &ss0, const SimilaritySequence& ss1);
		double cal_result_SimilaritySequence_summary_IGN(const SimilaritySequence& ss0, const SimilaritySequence& ss1);


		enum SimilaritySequenceCalType
		{
			DoubleMaxCount,//˫�����
			SummaryCount,//�Ӻͷ�
			End_SSC//�ڱ�
		};


		//����ƥ��ģ��(��ĸȡ������������)
		template<typename container, typename func>
		double cal_similarity_crossing_max(container c0, container c1, func p_func)
		{
			size_t size0 = c0.size();
			size_t size1 = c1.size();

			SimilaritySequence ss0(size0, -2), ss1(size1, -2);//���������ƶ�����

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

			return cal_result_SimilaritySequence_doubleMax_IGN(ss0, ss1);//�������ƶȴ洢���м�����
		}

		//����ƥ��ģ��(��ĸȡ���������)
		template<typename container, typename func>
		double cal_similarity_crossing_add(const container &c0,const container &c1, func p_func)
		{
			size_t size0 = c0.size();
			size_t size1 = c1.size();

			SimilaritySequence ss0(size0, -2), ss1(size1, -2);//���������ƶ�����

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
			return cal_result_SimilaritySequence_summary_IGN(ss0, ss1);//�������ƶȴ洢���м�����
		}




		string readTextFile(const string &filePath);



		/********************************************************************************
		*Function:operator<<()
		*Description:��std::vector����<<�����(ģ��),����ʹ��
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
	*����������ϼ�
	********************************************************************************/
	namespace Settings
	{
		using namespace DefinitionsSet;//���붨�弯

		extern const double weight_wordClass_default[End_WorldClass];

		/****************************************
		*�����û�Ȩ��
		****************************************/
		extern double weight_wordClass_customized[End_WorldClass];

		/****************************************
		*Ĭ�����ƶ�ռ��Ȩ��
		****************************************/
		extern const double weight_similarityComposition_default[End_SimilarityComposition];

		/****************************************
		*�û����ƶ�ռ��Ȩ��
		****************************************/
		extern double weight_similarityComposition_customized[End_SimilarityComposition];

		/****************************************
		*Ĭ������
		****************************************/
		extern const int limits_default[End_Limits];

		/****************************************
		*�û�����
		****************************************/
		extern int limits_customized[End_Limits];

		/****************************************
		*Ĭ���������ƶ�ʵ���㷨
		****************************************/
		extern const DependenciesSet::I_NSS seqSim_imp_default;

		/****************************************
		*�û��������ƶ�ʵ���㷨
		****************************************/
		extern DependenciesSet::I_NSS seqSim_imp_customized;

		//Ĭ�Ϸָ�������(����)
		//extern const int TEXT_ANALYSIS_EXPORT num_separator_default;

		/****************************************
		*Ĭ�϶���ָ���
		****************************************/
		extern const vector<Separator> separators_default;

		extern vector<Separator> separators_customized;

		//ģ���к��Եķ���
		extern vector<string> symbols_ignoredInTemplate_default;

		//ģ�忪��״̬(Ĭ�Ϲر�)
		extern bool template_enable;



		/********************************************************************************
		*TextAnalysorSetter
		*		���������ı��������ľ�̬��,�޷�ʵ����,ֻ�о�̬����
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextAnalysorSetter
		{
		private:
			//���캯��
			TextAnalysorSetter() = default;
			//��������
			~TextAnalysorSetter() = default;
			
		public:
			//ȫ������
			static void resetAll();
			//���ô�������Ȩ��
			static bool set_weight_wordClass(const FloatVector& newWeights);
			//���÷ֶη�
			static bool set_separators(const vector<Separator>& newSeps);
			//����ģ�忪��״̬
			static bool set_template_enabled(const bool& newState);
			//�������ƶ����Ȩ��
			static bool set_weight_similarityComposition(const FloatVector& newWeights);
			//�����������ƶȼ���ʵ��
			static bool set_seqSim_IMP(const Implements_NumberSequenceSimilarity& newIMP);


		};
	}

	/********************************************************************************
	*�����������ռ�
	*	��ȫ���ŵ���̬��
	********************************************************************************/
	namespace Core
	{
		using namespace DefinitionsSet;//���붨�弯
		using namespace Settings;//�������ü�
		using namespace DependenciesSet;//����������

		/********************************************************************************
	*�ı������쳣��
	*Description:
	*	���쳣�������ҽ������ڸ������������ռ��е�������
	*	�ṩһЩ�����쳣���ͺ��쳣��Ϣ
	*Inherited from:
	*	std::exception
	********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextAnalysisException :public std::exception
		{
		public:


			/****************************************
			*�쳣����
			****************************************/
			enum TextAnalysisExceptionType
			{
				SegSysException,//�ִ�ϵͳ�쳣
				CodingException,//�����쳣
				OverLimitException,//�����쳣
				InvalidPtrException,//��Чָ���쳣
				SystemPlatformException,//ƽ̨ϵͳ�쳣
				OtherException,//�����쳣
				UnknownException,//δ֪�쳣(Ĭ��)
				End_TextAnalysisExceptionType//�ڱ�
			};

			static const std::string details[End_TextAnalysisExceptionType];


		private:
			TextAnalysisExceptionType exceptionType;
			string otherInfo{};

		public:
			//���캯��
			explicit TextAnalysisException(const TextAnalysisExceptionType& exceptionType = UnknownException, const string& otherInfo = string());

			virtual char const* what() const;

		};

		/****************************************
		*ǰ������
		****************************************/
		class TEXT_ANALYSIS_EXPORT TextStream;

		/********************************************************************************
		*ģ�幤����
		*Description:
		*	�޷�ʵ����
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT Template
		{
			friend class TextStream;//��Ԫ������
		private:
			static string content_origin;//ԭʼ����
			static string content_processed;//����
			static NumberSequence numSeq;
			static bool enabled;//�Ƿ����
		private:
			//˽�еĹ��캯��,������ʵ����
			Template() = default;

		public:
			static bool setTemplate(const string& content);
			//����
			static void reset();
			//��ȡģ������
			static const string & getTemplate();
			//��ȡ��������
			static const NumberSequence& getNumberSequence();
			//�Ƿ�����
			static bool isEnabled();

		};


		//TEXT_ANALYSIS_EXPORT ostream& operator<<(ostream& os, const TextStream&);

		/********************************************************************************
		*�ı���
		*Description:
		*	����������ڴ洢��ȡ�ĵ����ļ�����,��������зֶ�,
		*	ÿһ�δ洢��TextSegment������,ʹ��std::vector��Ϊ����
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextStream
		{
			/****************************************
			*��Ԫ������
			****************************************/
			friend class TextAnalysor;
		public:
			//Ĭ�Ϲ��캯��
			TextStream();
			//ָ���ı��Ĺ��캯��
			TextStream(const string& text, const EncodingType& code = UTF_8);

		private:
		public:
			bool initialized{false};//��ʼ����
			string text{ "" };//�����ĵ�
			NumberSequence numSeq_text{};//ԭʼ�ı�ӳ�����������
			FlagsVector flags_ignored{};//���Ա��,�����ԭʼ�ı�����Ҫ�����Ե�u8��Ԫ

			size_t num_words{ 0 };//������(��Ԫ��)
			size_t num_units{ 0 };//UTF-8��Ԫ��
			size_t num_segs{ 0 };//������
			size_t num_unitsOutsizeTemplate{ 0 };//ģ��֮�����Ԫ��

			vector<cppjieba::Word>vector_words_extra{};//��������(������Ϣ)
			vector<pair<string, double>> vector_keyWords{};//�ؼ���
			vector<NumberSequence> vector_NS_keyWords{};//�ؼ������ɵ���������
			vector<WordUnit>vector_wordsInfo{};//��Ԫ��ϸ��Ϣ��(���ձ�)

			TextSegmentInfoVector vector_segmentsInfo{};//�ı�����Ϣ

		public://�ӿ�
			void setText(const string& text, const EncodingType& code = GBK);

			const string& getText()const;


			//����ת������,������ʽת����std::string����
			operator string();

			//��Ԫ,����<<
			TEXT_ANALYSIS_EXPORT friend ostream& operator<<(ostream& os, const TextStream&);

			void init();//��ʼ��
			void reset();//����
			//��ȡ��Ԫ��
			size_t getUnitNum();
			//��ȡ������
			size_t getSegmentNum();
			//��ȡ��ģ����U8��Ԫ��
			size_t getUnitNumOutsideTemplate();

		private://�ڲ�����

			void generateNumberSequence();//������������(0)

			void markTemplate();//���ģ��(0.5)

			/********************************************************************************
			*Function:segmentWords()
			*Description:
			*	�ִ�,���ı����зִʴ���,���洢�õ�����Ϣ��(vector������)
			*	ͬʱ��ȡ�ؼ�����Ϣ
			*Calls:����
			*Input:��
			*Output:��
			*Return:void
			*Remarks:
			*	[1]	TextStream������ڲ�����
			*	[2]	��ʼ���ĵ�һ������
			********************************************************************************/
			void segmentWords();//�ִ�(1)

			void segmentText();//�ֶ�(2)

			void scanWords();//ɨ��ִʽ��,������Ϣ��(3)

			void scanNumberSequence();//ɨ����������(4)

		};

		/********************************************************************************
		*�ı�������
		*Description:
		*	ָ�������ı���,�������ƶ�
		*	���ı����зֶκͷִʲ��ڴ������Ĺ��ܷ�Χ
		********************************************************************************/
		class TEXT_ANALYSIS_EXPORT TextAnalysor
		{
		private:
			/****************************************
			*���Ƚϵ������ı���
			****************************************/
			static const TextStream(*textStream0);//��һ���ı���ָ��
			static const TextStream(*textStream1);//�ڶ����ı���ָ��

		public:

			static void setTextStream0(const TextStream& ts0);
			static void setTextStream1(const TextStream& ts1);

			static double sim_segments;//�����ƶ�
			static double sim_keyWords;//�ؼ������ƶ�


			static double getSimilarity();//��ȡ���ƶ�

			static void clear();


		private:
			//����ָ��������������ƶ�
			static double cal_segmentSimilarity(const SegmentInfo& seg0, const SegmentInfo& seg1);

			//����������Ԫ������ƶ�(ͬ����)(����inline�����Ż�,�޾ֲ�����,�����ǶԺ������õķ�װ)
			inline static double cal_wordSimilarity(const size_t& i0, const size_t& i1);
			//double cal_wordUnitListSimilarity(const SegmentInfo& seg0, const SegmentInfo& seg1,const WorldClass& worldClass);

			//������ʵ����
			TextAnalysor();


		};
	}


	using namespace Core;

}



