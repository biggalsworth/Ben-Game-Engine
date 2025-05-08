using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class EnemyStats : GameObject
    {

        public float health = 5;

        public EnemyStats() : base ()
        {

        }

        public void Start()
        {
            //Debug.Log("Start");
        }

        public void Update(float delta)
        {
            //Debug.Log("Update");
            if(health <= 0)
            {
                Destroy();
            }
        }
    }
}


