using System.Collections.Generic;
using System.Linq;

namespace QEngine.QPL
{
    public class SectionTable : Dictionary<string, SectionTableEntry>
    {
        public SectionTable() : base()
        {

        }

        public SectionTable(int sectionCount) : base(sectionCount)
        {

        }

        public void Add(SectionTableEntry entry) => Add(entry.Name, entry);

        public void Add(string name, int offset, int length) => Add(new SectionTableEntry(name, offset, length, Count));

        public SectionTableEntry this[int index] => Values.First(entry => entry.Index == index);
    }
}
