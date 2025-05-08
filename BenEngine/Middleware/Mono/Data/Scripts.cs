using System;
namespace Engine
{
    public class ScriptComponent
    {
        public void Update()
        {
            Console.WriteLine("This is the Update function.");
        }

        public void Start()
        {
            Console.WriteLine("This is the Start function.");
        }
    }
}