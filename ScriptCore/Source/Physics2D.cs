using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Braveheart
{
    enum CastType
    {
        RayCastAny,
        RayCastClosest,
        RayCastMultiple,
        RayCastSorted
    };
    
    enum ShapeType
    {
        Circle,
        Box
    };

    public struct RayCastHit
    {
        public uint shapeId;
        public Vector2 point;
        public Vector2 normal;
        public float fraction;
        public int nodeVisits;
        public int leafVisits;
        public int count;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct OverlapResult
    {
        public uint shapeId;
    };

    public static class Physics2D
    {
        public static RayCastHit RayCast(Vector3 origin, Vector3 endPoint)
        {
            InternalCalls.Physics2D_RayCast(origin, endPoint, out RayCastHit hit);
            return hit;
        }

        public static uint[] CircleCast(Vector3 origin, float rotation, float radius)
        {
            IntPtr data = InternalCalls.Physics2D_CircleCast(origin, rotation, radius, out int arraySize);
            
            //adjust array size to be the current size of the vector
            int length = arraySize;
            
            // Create a C# array of the same length
            uint[] managedArray = new uint[length];
            
            // Use unsafe code to directly access memory
            unsafe
            {
                // Get a pointer to the unmanaged memory (MonoArray* -> uint*)
                uint* unmanagedPointer = (uint*)data.ToPointer();
            
                // Copy the data from unmanaged memory to the managed array
                for (int i = 0; i < length; i++)
                {
                    managedArray[i] = unmanagedPointer[i]; // Copy elements
                }
            }
            InternalCalls.VirtualFree(data, 0, 0x8000); //free up this pointer data for better memory management
            return managedArray;
        }
    }
}
