using Braveheart;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Braveheart
{
    public static class InternalCalls
    {
        #region GameObject
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint FindEntityByName(string entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint CreateNewEntity(string name, Vector3 Pos);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Native_DestroyEntity(uint id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint EntityAddComponent(uint entityID, Type type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool NativeGetComponent(uint entityID, Type type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool NativeHasComponent(uint entityID, Type type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object EntityGetScript(uint entityID, Type type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int NativeGetID();
        #endregion

        #region Components
        //Tag
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string TagComponent_GetName(uint entityID);

        // Transform
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetUp(uint entityID, out Vector3 upDir);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(uint entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(uint entityID, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(uint entityID, out Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(uint entityID, ref Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(uint entityID, out Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(uint entityID, ref Vector3 scale);

        // Sprite
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteComponent_GetColour(uint entityID, out Vector4 colour);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteComponent_SetColour(uint entityID, ref Vector4 colour);

        #region Physics
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Physics2D_RayCast(Vector3 origin, Vector3 endPoint, out RayCastHit outhit);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static IntPtr Physics2D_CircleCast(Vector3 origin, float rotation, float radius, out int arraySize);//, out List<uint> outhit);
        #endregion

        #region Audio
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static IntPtr Native_PlayOneShot(uint id);//, out List<uint> outhit);
        #endregion

        #region Rigidbody
        // 2D
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_GetLinearVelocity(uint entityID, out Vector2 velocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_GetPosition(uint entityID, out Vector3 position);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetPosition(uint entityID, ref Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(uint entityID, ref Vector3 impulse, ref Vector3 worldPosition, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCentre(uint entityID, ref Vector3 impulse, bool wake);
        #endregion

        // Network
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NetworkClient_SendMessage(uint entityID, string data);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NetworkClient_GetMessages(uint entityID, out string outMessage);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NetworkHost_GetMessages(uint entityID, out string outMessage);
        #endregion

        #region Debug
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseDown(MouseCodes keycode);
        #endregion






        #region Mono

        // P/Invoke to get the length of the array
        [DllImport("mono-2.0.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern int mono_array_length(IntPtr array);

        // P/Invoke to get an element from Mono array
        [DllImport("mono-2.0.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern int mono_array_get(IntPtr array, Type type, int index);
        [DllImport("mono-2.0.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr mono_get_uint32_class(); // Mono API to get MonoClass for UInt32

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool VirtualFree(IntPtr lpAddress, uint dwSize, uint dwFreeType);
        #endregion
    }
}
