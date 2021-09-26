using System;
using System.IO;

namespace QEngine.QPL
{
    internal struct QPLFileHeader
    {
        internal const string HeaderSignature = "QPL\0";

        public readonly string Signature; // 4 bytes
        public readonly QPLFileType Type; // 4 bytes
        public readonly int SectionCount; // 4 bytes
        public readonly int SectionTableOffset; // 4 bytes
        public readonly SectionTable Sections;

        internal QPLFileHeader(string signature, QPLFileType fileType, int sectionCount, int sectionTableOffset)
        {
            if (signature != HeaderSignature) throw new ArgumentException("QPL File Header signature must be \"QPL\\0\"", nameof(signature));
            Signature = signature;
            Type = fileType;
            Sections = new SectionTable(SectionCount = sectionCount);
            SectionTableOffset = sectionTableOffset;
        }

        internal QPLFileHeader(QPLFileType type)
        {
            Signature = HeaderSignature;
            Type = type;
            SectionCount = 0;
            SectionTableOffset = 0;
            Sections = new SectionTable();
        }

        public static QPLFileHeader ReadFrom(Stream stream) => ReadFrom(new BinaryReader(stream));

        public static QPLFileHeader ReadFrom(BinaryReader stream)
        {
            QPLFileHeader header = new QPLFileHeader(
                new string(stream.ReadChars(HeaderSignature.Length)), 
                (QPLFileType)stream.ReadInt32(), 
                stream.ReadInt32(),
                stream.ReadInt32());

            stream.BaseStream.Seek(header.SectionTableOffset, SeekOrigin.Begin);

            for (int i = 0; i < header.SectionCount; i++)
            {
                header.Sections.Add(SectionTableEntry.ReadFrom(stream, i));
            }

            return header;
        }


        public bool IsValid() => Signature == HeaderSignature;

        public void WriteTo(Stream stream) => WriteTo(new BinaryWriter(stream));

        public void WriteTo(BinaryWriter stream)
        {
            stream.Write(HeaderSignature.ToCharArray());
            stream.Write((int)Type);
            stream.Write(Sections.Count);

            int offset = SectionTableOffset;

            if (offset == 0) offset = (int)stream.BaseStream.Position + sizeof(int);
            stream.Write(offset);

            long position = stream.Seek(offset, SeekOrigin.Begin);
            if (position < offset) stream.Write(new char[offset - position]);

            if (stream.BaseStream.Position != offset) throw new Exception("IDK what's going on here");

            foreach (SectionTableEntry entry in Sections.Values)
            {
                char[] sectionNameBuffer = new char[SectionTableEntry.MaxSectionNameLength];
                entry.Name.CopyTo(0, sectionNameBuffer, 0, Math.Min(entry.Name.Length, SectionTableEntry.MaxSectionNameLength));
                stream.Write(sectionNameBuffer);
                stream.Write(entry.Offset);
                stream.Write(entry.Length);
            }
        }
    }
}
