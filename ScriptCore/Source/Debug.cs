using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public static class Debug
    {

        public static void Log(string text)
        {
            InternalCalls.NativeLog(text, 0);
        }
        public static void LogWarning(string text)
        {
            InternalCalls.NativeLog(text, 1);
        }
        public static void LogError(string text)
        {
            InternalCalls.NativeLog(text, 2);
        }

    }
}
