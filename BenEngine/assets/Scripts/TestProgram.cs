using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using Braveheart;

public class MainScript : Main
{
    public MainScript()
    {

        Debug.Log("Script Constructor");
        Debug.LogWarning("Warning Message");
        Debug.LogError("Error Message");

        Vector3 pos = new Vector3(1, 1, 1);

        Debug.Log("Position: " + pos.ToString());

        //LogError();
    }

    public static void Update()
    {
        Console.WriteLine("This is an update");
        //LogError("From User Scripts");

        int[] ar = { 1, 2, 3, 4 };
        Console.WriteLine(ar[1]);

        List<int> list = new List<int>();
        list.Add(1);
        list.Add(2);
        Console.WriteLine(list[1]);
    }
}

