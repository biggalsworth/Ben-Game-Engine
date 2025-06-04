using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;  

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
        }

        public void Update(float delta)
        {
            Debug.Log(name);
            Debug.Log(num.ToString());
        }
    }
}


