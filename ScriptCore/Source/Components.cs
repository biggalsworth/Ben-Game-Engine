using Braveheart;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


public abstract class Component
{
    public GameObject GameObject { get; internal set; }
}
public class TagComponent : Component
{
    public string Name
    {
        get
        {
            if (GameObject == null)
            {
                return "";
            }
            //string name = Marshal.PtrToStringUni(InternalCalls.TagComponent_GetName(GameObject.id));
            string name = InternalCalls.TagComponent_GetName(GameObject.id);
            if (name != null)

            {
                return name;
            }
            return "";
        }
    }
}

public class TransformComponent : Component
{
    //// Position, Rotation, and Scale properties
    //public Vector3 Position { get; set; } = new Vector3(0.0f, 0.0f, 0.0f);
    //public Vector3 Rotation { get; set; } = new Vector3(0.0f, 0.0f, 0.0f); // Euler angles
    //public Vector3 Scale { get; set; } = new Vector3(1.0f, 1.0f, 1.0f);
    //
    //// Constructor
    //public TransformComponent() : base() { }
    //
    //public TransformComponent(Vector3 position, Vector3 rotation, Vector3 scale)
    //{
    //    Position = position;
    //    Rotation = rotation;
    //    Scale = scale;
    //}


    public Vector3 Translation
    {
        get
        {
            if (GameObject == null)
            {
                Debug.LogError("TransformComponent: GameObject is null! Cannot get Translation.");
                return Vector3.Zero;
            }

            InternalCalls.TransformComponent_GetTranslation(GameObject.id, out Vector3 translation);
            return translation;
        }
        set
        {
            InternalCalls.TransformComponent_SetTranslation(GameObject.id, ref value);
        }
    }
    public Vector3 Rotation
    {
        get
        {
            if (GameObject == null)
            {
                Debug.LogError("TransformComponent: GameObject is null! Cannot get Rotation.");
                return Vector3.Zero;
            }

            InternalCalls.TransformComponent_GetRotation(GameObject.id, out Vector3 rotation);
            return rotation;
        }
        set
        {
            InternalCalls.TransformComponent_SetRotation(GameObject.id, ref value);
        }
    }
    public Vector3 Scale
    {
        get
        {
            if (GameObject == null)
            {
                Debug.LogError("TransformComponent: GameObject is null! Cannot get Rotation.");
                return Vector3.Zero;
            }

            InternalCalls.TransformComponent_GetScale(GameObject.id, out Vector3 scale);
            return scale;
        }
        set
        {
            InternalCalls.TransformComponent_SetScale(GameObject.id, ref value);
        }
    }

    public Vector3 GetUp()
    {
        InternalCalls.TransformComponent_GetUp(GameObject.id, out Vector3 up);
        return up;
    }

    //// Methods to manipulate the transform component
    //public void Translate(Vector3 translation)
    //{
    //    Position += translation;
    //}
    //
    //public void Rotate(Vector3 rotation)
    //{
    //    Rotation += rotation;
    //}
    //
    //public void ScaleBy(Vector3 scaling)
    //{
    //    Scale += scaling;
    //
    //}
}

public class Rigidbody2DComponent : Component
{

    public Vector2 LinearVelocity
    {
        get
        {
            InternalCalls.Rigidbody2DComponent_GetLinearVelocity(GameObject.id, out Vector2 velocity);
            return velocity;
        }
    } 
    
    public Vector3 Position
    {
        get
        {
            InternalCalls.Rigidbody2DComponent_GetPosition(GameObject.id, out Vector3 pos);
            return pos;
        } 
        set
        {
            InternalCalls.Rigidbody2DComponent_SetPosition(GameObject.id, ref value);
        }
    }
    public void ApplyLinearImpulse(Vector3 impulse, Vector3 worldPosition, bool wake)
    {
        InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(GameObject.id, ref impulse, ref worldPosition, wake);
    }

    public void ApplyLinearImpulseToCentre(Vector3 impulse, bool wake = true)
    {
        InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCentre(GameObject.id, ref impulse, wake);
    }
}

public class BoxCollider2DComponent : Component
{

}

public class NetworkClientComponent : Component
{

    public void SendMessage(string data)
    {
        InternalCalls.NetworkClient_SendMessage(GameObject.id, data);
    }

    public string GetMessage()
    {
        InternalCalls.NetworkClient_GetMessages(GameObject.id, out string Incoming);
        return Incoming;
    }
}
public class NetworkHostComponent : Component
{

    //public void SendMessage(string data)
    //{
    //    InternalCalls.NetworkClient_SendMessage(GameObject.id, data);
    //}

    public string GetMessage()
    {
        InternalCalls.NetworkHost_GetMessages(GameObject.id, out string Incoming);
        return Incoming;
    }
}


public class SpriteRendererComponent : Component
{
    public Vector4 Colour
    {
        get
        {
            if (GameObject == null)
            {
                Debug.LogError("TransformComponent: GameObject is null! Cannot get Translation.");
                return Vector4.Zero;
            }

            InternalCalls.SpriteComponent_GetColour(GameObject.id, out Vector4 colour);
            return colour;
        }
        set
        {
            InternalCalls.SpriteComponent_SetColour(GameObject.id, ref value);
        }
    }
}

public class AudioSourceComponent : Component
{

    public void PlayOneShot()
    {
        InternalCalls.Native_PlayOneShot(GameObject.id);
    }
}



