using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Braveheart
{
    public static class World
    {

        public static GameObject FindEntityByName(string name)
        {
            uint entityID = InternalCalls.FindEntityByName(name);


            return new GameObject(entityID);
        }
    }

    public class GameObject : Component
    {
        //  public ulong id = 0;    
        //  private List<Component> components = new List<Component>();

        protected GameObject() { id = 0; }

        public GameObject(uint _id)
        {
            id = _id;
        }

        // this constructor supports script initialization
        //internal GameObject(IntPtr scriptPtr)
        //{
        //    //initialise something with the script
        //}


        public readonly uint id;

        #region Component Management
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            //Debug.LogWarning(typeof(T).ToString());
            return InternalCalls.NativeHasComponent(id, typeof(T));

        }
        
        public T AddComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            //Debug.LogWarning(typeof(T).ToString());
            InternalCalls.EntityAddComponent(id, typeof(T));

            return GetComponent<T>();

        }

        public T GetComponent<T>() where T : Component, new()
        {
            //Debug.LogWarning(typeof(T).ToString());
            //InternalCalls.NativeGetComponent(id, typeof(T));

            if (!HasComponent<T>())
                return null;

            T component = new T() { GameObject = this };
            return component;

        }

        //public T GetScript<T>()
        //{
        //    IntPtr scriptPtr = InternalCalls.EntityGetScript(id, typeof(T));
        //
        //    if (scriptPtr == IntPtr.Zero)
        //    {
        //        Debug.LogError($"Failed to retrieve script of type {typeof(T)}");
        //        return default;
        //    }
        //
        //    return (T)Activator.CreateInstance(typeof(T), BindingFlags.Instance | BindingFlags.NonPublic, null, new object[] { scriptPtr }, null);
        //}
        public T GetScript<T>() where T : class
        {
            object scriptInstance = InternalCalls.EntityGetScript(id, typeof(T));

            if (scriptInstance == null)
            {
                throw new InvalidOperationException($"Failed to retrieve script of type {typeof(T)}");
            }

            return scriptInstance as T; // Direct cast
        }





        #endregion


        public static int GetID()
        {
            return InternalCalls.NativeGetID();
        }

        //public GameObject FindEntityByName(string name)
        //{
        //    uint entityID = InternalCalls.FindEntityByName(name);
        //
        //
        //    return new GameObject(entityID);
        //}


        public GameObject Instatiate(string name = "New Entity", Vector3 Position = default)
        {
            return new GameObject(InternalCalls.CreateNewEntity(name, Position));
        }

        #region overrides
        public virtual void OnCollisionEnter(uint other)
        {
            Debug.LogError("GameObject Collision");
        }
        
        public virtual void OnTriggerEnter(uint other)
        {
            Debug.LogError("GameObject Trigger");
        }

        #endregion




        public void Destroy()
        {
            Debug.LogWarning("DESTROY " + id.ToString());
            InternalCalls.Native_DestroyEntity(id);
        }
    }

}
