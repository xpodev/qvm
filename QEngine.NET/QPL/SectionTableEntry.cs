using System;
using System.IO;

namespace QEngine.QPL
{
    public struct SectionTableEntry
    {
        public const int MaxSectionNameLength = 8;

        public readonly string Name;
        public readonly int Length;
        public readonly int Offset;

        public readonly int Index;

        public SectionTableEntry(string name, int length, int offset, int index)
        {
            if (name == null) throw new ArgumentNullException(nameof(name));
            if (name.Length > MaxSectionNameLength) throw new ArgumentOutOfRangeException(nameof(name), $"Section name length must be lower than {MaxSectionNameLength}");
                    
            Name = name;
            Offset = offset;
            Length = length;

            Index = index;
        }

        internal static SectionTableEntry ReadFrom(Stream stream, int index) => ReadFrom(new BinaryReader(stream), index);

        internal static SectionTableEntry ReadFrom(BinaryReader stream, int index)
        {
            return new SectionTableEntry(
                new string(stream.ReadChars(MaxSectionNameLength)).TrimEnd('\0'), 
                stream.ReadInt32(), 
                stream.ReadInt32(), 
                index);
        }
    }
}
