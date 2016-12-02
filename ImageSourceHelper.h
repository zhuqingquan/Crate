#include <fstream>
#include <vector>

using namespace std;

namespace zTools
{
	class ImageSourceHelper
	{
	public:
		static ImageSourceHelper* Instance();

		~ImageSourceHelper()
		{
			for(int i=0; i<m_ImageDatas.size(); i++)
			{
				free((char*)m_ImageDatas[i]);
			}
			m_ImageDatas.clear();
		}

		bool init(const char* filePathName, int imgWidth, int imgHeight)
		{
			ifstream file(filePathName, std::ios::in | std::ios::binary);
			if(!file)
				return false;
			ifstream::pos_type beg = file.tellg();
			file.seekg(0, file.end);
			ifstream::pos_type end = file.tellg();
			file.seekg(file.beg);
			int fileLen = end - beg;
			
			int picLen = imgWidth * imgHeight * 4;
			int noRead = fileLen;
			while(noRead>picLen)
			{
				char* buf = (char*)malloc(picLen);
				file.read(buf, picLen);
				noRead -= picLen;
				m_ImageDatas.push_back(buf);
			}
			m_ImgHeight = imgHeight;
			m_ImgWidth = imgWidth;
			return true;
		}

		int getFrameCount() const
		{
			return m_ImageDatas.size();
		}

		int getImgWidth() const
		{
			return m_ImgWidth;
		}

		int getImgHeight() const
		{
			return m_ImgHeight;
		}

		const char* getNextFrameImage()
		{
			if(m_ImageDatas.size()<=0)
				return NULL;
			m_CurFrame = (m_CurFrame+1) % m_ImageDatas.size();
			return m_ImageDatas[m_CurFrame];
		}

		const char* getFrameImage(int& frameIndex) const
		{
			if(m_ImageDatas.size()<=0)
				return NULL;
			frameIndex = frameIndex % m_ImageDatas.size();
			return m_ImageDatas[frameIndex];
		}
	private:
		ImageSourceHelper()
			: m_ImgHeight(0)
			, m_ImgWidth(0)
			, m_CurFrame(-1)
		{
		}

		int m_ImgWidth;
		int m_ImgHeight;
		vector<const char*> m_ImageDatas;
		int m_CurFrame;
	};
}