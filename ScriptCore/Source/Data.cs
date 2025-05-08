using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Braveheart
{
    public struct Vector3
    {
        public float x, y, z;

        public static Vector3 Zero => new Vector3(0.0f);

        //constructors
        public Vector3(float X, float Y, float Z)
        {
            this.x = X;
            this.y = Y;
            this.z = Z;
        }
        public Vector3(Vector3 newVec)
        {
            this.x = newVec.x;
            this.y = newVec.y;
            this.z = newVec.z;
        }
        public Vector3(float value)
        {
            this.x = value;
            this.y = value;
            this.z = value;
        }

        public float Magnitude()
        {
            float magnitude = (float)Math.Sqrt(x * x + y * y + z * z);
            return magnitude;
        }

        //Keeps the direction unchanged while setting a fixed magnitude.
        public Vector3 Normalise()
        {
            float magnitude = this.Magnitude();

            // Prevent division by zero
            if (magnitude > 0)
            {
                return new Vector3(x / magnitude, y / magnitude, z / magnitude);
            }

            return new Vector3(x, y, z); // Return original if magnitude is zero
        }


        //public static Vector3 Zero()
        //{
        //    return new Vector3(0);
        //}


        //conversions
        public string ToString()
        {
            return (x.ToString() + "," + y.ToString() + "," + z.ToString());
        }


        //operators
        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        //operators
        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector3 operator *(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x * b.x, a.y * b.y, a.z * a.z);
        }
        public static Vector3 operator *(Vector3 a, float b)
        {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }
    };

    public struct Vector4
    {
        public float x, y, z, w;

        public static Vector4 Zero => new Vector4(0.0f);

        // Constructors
        public Vector4(float X, float Y, float Z, float W)
        {
            this.x = X;
            this.y = Y;
            this.z = Z;
            this.w = W;
        }
        public Vector4(Vector4 newVec)
        {
            this.x = newVec.x;
            this.y = newVec.y;
            this.z = newVec.z;
            this.w = newVec.w;
        }
        public Vector4(float value)
        {
            this.x = value;
            this.y = value;
            this.z = value;
            this.w = value;
        }

        // Conversions
        public override string ToString()
        {
            return $"{x},{y},{z},{w}";
        }

        // Operators
        public static Vector4 operator +(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        public static Vector4 operator -(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        public static Vector4 operator *(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }

        public static Vector4 operator *(Vector4 a, float b)
        {
            return new Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
        }
    }


    public struct Vector2
    {
        public float x, y;

        public static Vector2   Zero => new Vector2(0.0f);

        //constructors
        public Vector2(float X, float Y)
        {
            this.x = X;
            this.y = Y;
        }
        public Vector2(Vector2 newVec)
        {
            this.x = newVec.x;
            this.y = newVec.y;
        }
        public Vector2(float value)
        {
            this.x = value;
            this.y = value;
        }

        //public static Vector3 Zero()
        //{
        //    return new Vector3(0);
        //}


        //conversions
        public string ToString()
        {
            return (x.ToString() + "," + y.ToString());
        }

        public float Magnitude()
        {
            return (float)Math.Sqrt(x * x + y * y);
        }

        //Keeps the direction unchanged while setting a fixed magnitude.
        public Vector2 Normalise()
        {
            float magnitude = this.Magnitude();

            // Prevent division by zero
            if (magnitude > 0)
            {
                return new Vector2(x / magnitude, y / magnitude);
            }

            return new Vector2(x, y); // Return original if magnitude is zero
        }


        //operators
        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x + b.x, a.y + b.y);
        }

        public static Vector2 operator -(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x - b.x, a.y - b.y);
        }

        public static Vector2 operator *(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x * b.x, a.y * b.y);
        }

        public static Vector2 operator *(Vector2 a, float b)
        {
            return new Vector2(a.x * b, a.y * b);
        }

    };


}
