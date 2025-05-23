using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class NetworkClient : GameObject
    {
        public NetworkClient() : base ()
        {

        }

        public void Start()
        {
            Debug.Log("Start");
        }

        public void Update(float delta)
        {
            if (GetComponent<NetworkClientComponent>() != null)
            {
                string latestMessage = GetComponent<NetworkClientComponent>().GetMessage();
                Debug.Log("LatestMessage " + latestMessage);

            }
        }
    }
}


