using Braveheart;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Braveheart
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown(keycode);
        }
        public static bool IsMouseClicked(MouseCodes mouseButton)
        {
            return InternalCalls.Input_IsMouseDown(mouseButton);
        }
    }
}
