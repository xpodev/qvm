using System.IO;

namespace QEngine.QPL
{
    public class QPLFile
    {
        private QPLFileHeader m_header;

        public string Path { get; }

        public SectionTable Sections => m_header.Sections;

        internal QPLFile(string path)
        {
            Path = path;
        }

        public static QPLFile Create(QPLFileType type, string path = null)
        {
            QPLFile file = new QPLFile(path);
            file.m_header = new QPLFileHeader(type);

            return file;
        }

        public static QPLFile ReadFrom(string path)
        {
            QPLFile file = new QPLFile(path);

            using (FileStream fileStream = File.OpenRead(path))
            {
                file.m_header = QPLFileHeader.ReadFrom(fileStream);
            }

            return file;
        }

        public void Write() => Write(Path);

        public void Write(string path)
        {
            using (FileStream fileStream = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write))
            {
                fileStream.SetLength(0);
                m_header.WriteTo(fileStream);
            }
        }
    }
}
