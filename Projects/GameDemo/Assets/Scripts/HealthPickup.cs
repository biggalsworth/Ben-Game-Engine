using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class HealthPickup : GameObject
    {
        public float amount = 20;

        public HealthPickup() : base ()
        {

        }

        public void Start()
        {
            Debug.Log("Start");
        }

        public void Update(float delta)
        {
            Debug.Log("Update");
        }
    }
}


