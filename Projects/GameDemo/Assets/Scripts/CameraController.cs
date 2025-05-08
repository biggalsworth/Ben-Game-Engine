using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class CameraController : GameObject
    {

        private GameObject Player;
        Vector3 newPos;

        public CameraController() : base ()
        {

        }

        public void Start()
        {

            Player = World.FindEntityByName("Player");

        }

        public void Update(float delta)
        {

            newPos.x = Player.GetComponent<TransformComponent>().Translation.x;
            newPos.y = Player.GetComponent<TransformComponent>().Translation.y;
            newPos.z = GetComponent<TransformComponent>().Translation.z;

            GetComponent<TransformComponent>().Translation = newPos; 
        }
    }
}


