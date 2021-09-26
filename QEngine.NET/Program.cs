using System;
using QEngine.QPL;

namespace QEngine
{
    class Program
    {
        static void Main(string[] args)
        {
            QPLFile file = QPLFile.Create(QPLFileType.EntryPoint | QPLFileType.ExportTable, "test.qpl");

            file.Sections.Add("Code", 50, 50);
            file.Sections.Add("Data", 32, 64);

            file.Write();
        }
    }
}
