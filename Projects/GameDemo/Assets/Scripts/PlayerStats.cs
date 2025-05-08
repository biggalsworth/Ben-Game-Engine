using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class PlayerStats : GameObject
    {
        
        public float health = 50;

        public PlayerStats() : base ()
        {

        }

        public void Start()
        {
        }

        public void Update(float delta)
        {
            if (health <= 0)
            {
                health = 50;
                GetComponent<Rigidbody2DComponent>().Position = new Vector3(0, 0, 0);
            }
        }


        public override void OnTriggerEnter(uint other)
        {
            GameObject hit = new GameObject(other);
            if (hit.GetComponent<TagComponent>().Name == "Pickup")
            {
                health += hit.GetScript<HealthPickup>().amount;
                hit.Destroy();
            }
        }

        public void Damage(float d)
        {
            health -= d;
        }
    }
}


