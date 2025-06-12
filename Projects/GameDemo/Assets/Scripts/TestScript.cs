using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.IO;

namespace Braveheart
{
    public class TestScript : GameObject
    {
        public string name = "Hello";
        public int num = 0;

        public TestScript() : base ()
        {

        }

        public void Start()
        {
            Debug.Log("Start");
            Debug.Log(name);

            
            //Debug.LogWarning(file.Path);

        }

        public void Update(float delta)
        {
            //Debug.Log(name);
            //Debug.Log(num.ToString());
        }
    }
}


