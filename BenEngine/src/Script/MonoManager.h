#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/class.h>

#include <filesystem>
#include <string>
#include <map>
#include <unordered_map>
#include "Core/UUID.h"

namespace Engine
{
    class Scene;
	class Entity;

	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3,
		GameObject, string
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	// ScriptField + data storage
	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[16];

		friend class MonoManager;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;



	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }

	public:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		std::map<std::string, ScriptField> m_Fields;

		MonoClass* m_MonoClass = nullptr;

		friend class MonoManager; //allows mono manager to change fields
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);

		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			SetFieldValueInternal(name, &value);
		}

		MonoObject* GetManagedObject() { return m_Instance; }
		void SetMangedObject(MonoObject* object) { m_Instance = object; }

	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		inline static char s_FieldValueBuffer[16];

		friend class MonoManager;
		friend struct ScriptFieldInstance;
	};




	struct ScriptComponent;

    static class MonoManager
    {
    public:
        //MonoManager();
        //~MonoManager();


        static bool Initialize();

        //handle script building
        static bool LoadAppAssembly(const std::filesystem::path& assemblyPath);
        static bool LoadAssembly(const std::filesystem::path& assemblyPath);
        static void ReloadMono();


        static std::vector<std::string> FindAllScripts();
        //static bool CompileScript(const std::filesystem::path& scriptPath);
        static bool CompileScripts();
        static bool CleanScripts();
        static void GenerateBindings();

        static void LoadClasses();

		static bool EntityClassExists(const std::string& fullClassName);
		static void CreateScriptEntity(Entity* entity, ScriptComponent* sc);

        //static ScriptInstance* CreateScriptInstance(const std::string& className, const std::string& namespaceName = "", uint32_t entId = 0);
		static MonoObject* InstantiateClass(MonoClass* monoClass);

        static bool CallEntityScriptMethod(Ref<ScriptInstance> script, const std::string& methodName);
        static bool CallScriptMethod(MonoObject* instance, const std::string& methodName);
		static ScriptFieldMap& GetScriptFieldMap(Entity entity);
        static void Shutdown();
        static MonoObject* InvokeMethod(const std::string& assemblyName, const std::string& className, const std::string& methodName, const std::vector<MonoObject*>& params);

        static void ListAllMethods(MonoClass* klass);

        static void ListAllClassesAndMethods(MonoImage* image);


        static Scene* GetSceneContext();

        //Runtime Management
        static void RuntimeStart(Scene* context);

		//static void UpdateScriptFieldsOnCopy(const std::unordered_map<UUID, UUID>& entityIDMap);

        static MonoDomain* rootDomain;
        static MonoDomain* domain;
        static MonoAssembly* CoreAssembly;
        static MonoAssembly* AppAssembly;
        static MonoImage* CoreImage;
        static MonoImage* image;

		static std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
        static ScriptClass* EntityClass;
        static MonoObject* currentInstance;

		static std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

        static Scene* SceneContext;

        static inline std::string CurrProj;
    };


	namespace Utils {

		inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
		{
			switch (fieldType)
			{
			case ScriptFieldType::None:    return "None";
			case ScriptFieldType::Float:   return "Float";
			case ScriptFieldType::Double:  return "Double";
			case ScriptFieldType::Bool:    return "Bool";
			case ScriptFieldType::Char:    return "Char";
			case ScriptFieldType::Byte:    return "Byte";
			case ScriptFieldType::Short:   return "Short";
			case ScriptFieldType::Int:     return "Int";
			case ScriptFieldType::Long:    return "Long";
			case ScriptFieldType::UByte:   return "UByte";
			case ScriptFieldType::UShort:  return "UShort";
			case ScriptFieldType::UInt:    return "UInt";
			case ScriptFieldType::ULong:   return "ULong";
			case ScriptFieldType::Vector2: return "Vector2";
			case ScriptFieldType::Vector3: return "Vector3";
			case ScriptFieldType::GameObject:  return "GameObject";
			case ScriptFieldType::string:  return "string";
			}
			CORE_ASSERT(false, "Unknown ScriptFieldType");
			return "None";
		}

		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")    return ScriptFieldType::None;
			if (fieldType == "Float")   return ScriptFieldType::Float;
			if (fieldType == "Double")  return ScriptFieldType::Double;
			if (fieldType == "Bool")    return ScriptFieldType::Bool;
			if (fieldType == "Char")    return ScriptFieldType::Char;
			if (fieldType == "Byte")    return ScriptFieldType::Byte;
			if (fieldType == "Short")   return ScriptFieldType::Short;
			if (fieldType == "Int")     return ScriptFieldType::Int;
			if (fieldType == "Long")    return ScriptFieldType::Long;
			if (fieldType == "UByte")   return ScriptFieldType::UByte;
			if (fieldType == "UShort")  return ScriptFieldType::UShort;
			if (fieldType == "UInt")    return ScriptFieldType::UInt;
			if (fieldType == "ULong")   return ScriptFieldType::ULong;
			if (fieldType == "Vector2") return ScriptFieldType::Vector2;
			if (fieldType == "Vector3") return ScriptFieldType::Vector3;
			if (fieldType == "GameObject")  return ScriptFieldType::GameObject;
			if (fieldType == "string")  return ScriptFieldType::string;

			CORE_ASSERT(false, "Unknown ScriptFieldType");
			return ScriptFieldType::None;
		}

	}
}