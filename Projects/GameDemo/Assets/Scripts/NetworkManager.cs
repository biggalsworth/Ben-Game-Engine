using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class NetworkManager : GameObject
    {

        string latestMessage;


        public NetworkManager() : base ()
        {

        }

        public void Start()
        {
            Debug.Log("Start");
        }

        public void Update(float delta)
        {

            latestMessage = GetComponent<NetworkHostComponent>().GetMessage();
        }
    }
}


