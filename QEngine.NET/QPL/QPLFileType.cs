using System;

namespace QEngine.QPL
{
    [Flags]
    public enum QPLFileType
    {
        EntryPoint  = 0b0001,
        ExportTable = 0b0010
    }
}
