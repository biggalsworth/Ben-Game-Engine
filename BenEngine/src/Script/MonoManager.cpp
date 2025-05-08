#include "Engine_PCH.h"
#include "MonoManager.h"

#include <mono/jit/jit.h>
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/attrdefs.h"
#include <string>

#include <windows.h>

#include "ScriptGlue.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"


namespace Engine
{
    static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
    {
        { "System.Single", ScriptFieldType::Float },
        { "System.Double", ScriptFieldType::Double },
        { "System.Boolean", ScriptFieldType::Bool },
        { "System.Char", ScriptFieldType::Char },
        { "System.Int16", ScriptFieldType::Short },
        { "System.Int32", ScriptFieldType::Int },
        { "System.Int64", ScriptFieldType::Long },
        { "System.Byte", ScriptFieldType::Byte },
        { "System.UInt16", ScriptFieldType::UShort },
        { "System.UInt32", ScriptFieldType::UInt },
        { "System.UInt64", ScriptFieldType::ULong },

        { "Braveheart.Vector2", ScriptFieldType::Vector2 },
        { "Braveheart.Vector3", ScriptFieldType::Vector3 },

        { "Braveheart.GameObject", ScriptFieldType::GameObject },
        { "System.String", ScriptFieldType::string },
    };


    namespace Utils 
    {
        static char* ReadBytes(const std::string& filepath, uint32_t* outSize)
        {
            std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

            if (!stream)
            {
                // Failed to open the file
                return nullptr;
            }

            std::streampos end = stream.tellg();
            stream.seekg(0, std::ios::beg);
            uint32_t size = end - stream.tellg();

            if (size == 0)
            {
                // File is empty
                return nullptr;
            }

            char* buffer = new char[size];
            stream.read((char*)buffer, size);
            stream.close();

            *outSize = size;
            return buffer;
        }

        static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
        {
            uint32_t fileSize = 0;
            char* fileData = ReadBytes(assemblyPath.string(), &fileSize);

            // NOTE: We can't use this image for anything other than loading the assembly because...
            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

            if (status != MONO_IMAGE_OK)
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }

            //MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
            std::string path = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
            mono_image_close(image);

            //free the file data
            delete[] fileData;

            return assembly;
        }
    
        ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
        {
            std::string typeName = mono_type_get_name(monoType);

            auto it = s_ScriptFieldTypeMap.find(typeName);
            if (it == s_ScriptFieldTypeMap.end())
            {
                CORE_ASSERT("Unknown type: {}", typeName);
                return ScriptFieldType::None;
            }

            return it->second;
        }



    }


    MonoDomain* MonoManager::rootDomain = nullptr;
    MonoDomain* MonoManager::domain = nullptr;
    MonoAssembly* MonoManager::CoreAssembly = nullptr;
    MonoAssembly* MonoManager::AppAssembly = nullptr;
    MonoImage* MonoManager::CoreImage = nullptr;
    MonoImage* MonoManager::image = nullptr;

    std::unordered_map<std::string, Ref<ScriptClass>> MonoManager::EntityClasses;
    ScriptClass* MonoManager::EntityClass = nullptr;
    std::unordered_map<UUID, ScriptFieldMap> MonoManager::EntityScriptFields;

    Scene* MonoManager::SceneContext = nullptr;


    //MonoManager::MonoManager()
    //{
    //    Initialize();
    //}
    //
    //MonoManager::~MonoManager()
    //{
    //    Shutdown();
    //}

    void PrintAssemblyTypes(MonoAssembly* assembly)
    {
        MonoImage* image = mono_assembly_get_image(assembly);
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
            MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

            LOG_TRACE("{}.{}", nameSpace, name);
        }
    }

    bool MonoManager::Initialize()
    {
        EntityScriptFields.clear();

        //GenerateBindings(); // when we used cppsharp

        std::cout << "Initialising mono" << std::endl;

        //Include Mono Directories
        std::string monoLibsDir = (std::filesystem::current_path() / "Middleware/Mono/lib").string();
        //std::string DataDir = (std::filesystem::current_path() / "Middleware/Mono/Data/bin/Debug/net8.0").lexically_normal().string();
        std::cout << "LIB DIR: " + monoLibsDir << std::endl;
        std::string monoDataDir = (std::filesystem::current_path() / "Middleware/Mono/etc").string();

        mono_set_assemblies_path(monoLibsDir.c_str());

        //Create the domains for runtime
        // Initialize runtime
        mono_set_dirs(monoLibsDir.c_str(), monoDataDir.c_str());
        rootDomain = mono_jit_init("ScriptsRuntime");

        if (!rootDomain)
        {
            std::cerr << "Failed to initialize Mono runtime." << std::endl;
            return false;
        }


        //Link all relevant commands that communicate with c++
        ScriptGlue::RegisterFunctions();


        //Load the core scripts assembly
        //this will create the core domain for the rest of the script to run from using the core scripts library
        LoadAssembly((std::filesystem::current_path() / ("Resource\\Scripts\\ScriptCore.dll")));
        //EntityClass = mono_class_from_name(CoreImage, "Braveheart", "GameObject"); //extracting the gameobject class
        EntityClass = new ScriptClass("Braveheart", "GameObject", true);
        PrintAssemblyTypes(CoreAssembly);
        LoadAppAssembly(std::filesystem::current_path() / ("Resource\\Scripts\\" + Project::currCodeProj.filename().string() + ".dll"));
        LoadClasses();


        //Link all relevant commands that communicate with c++
        ScriptGlue::RegisterFunctions();

        return true;
    }

    bool MonoManager::LoadAssembly(const std::filesystem::path& assemblyPath)
    {
        // Create an App Domain
        domain = mono_domain_create_appdomain("EngineScriptRuntime", nullptr);
        mono_domain_set(domain, true);

        //creating the core assembly
        CoreAssembly = Utils::LoadMonoAssembly(assemblyPath);
        if (CoreAssembly == nullptr)
            return false;

        //set the core image from the core assembly
        CoreImage = mono_assembly_get_image(CoreAssembly);
        return true;
    }
    
    bool MonoManager::LoadAppAssembly(const std::filesystem::path& assemblyPath)
    {

        //load current code project into an assembly
        //this will create the references to compiled functions by using an image of the assembly
        AppAssembly = Utils::LoadMonoAssembly(assemblyPath);
        if (!AppAssembly)
        {
            std::cerr << "Failed to load the dll from: " << assemblyPath << "\n";
        }
        //loading a class and calling a function
        image = mono_assembly_get_image(AppAssembly);
        PrintAssemblyTypes(AppAssembly);
        return true;
    }


    bool MonoManager::CompileScripts()
    {
        std::filesystem::path scriptFolder = std::filesystem::current_path() / "assets" / "Scripts";
        std::filesystem::path outputDllPath = std::filesystem::current_path() / "Resource" / "Scripts" / "ScriptCore.dll";  // New DLL
        std::filesystem::path MonoCommand = std::filesystem::current_path() / "Middleware" / "Mono" / "bin" / "mcs"; // Use mcs or csc

        LOG_INFO("Compiling " + Project::currCodeProj.string() + " ...");
        std::string batCommand = (Project::currCodeProj / "../CodeProjects.bat").lexically_normal().string();
        system(batCommand.c_str());

        std::filesystem::path expectedPath = Project::currCodeProj / L"GameDemo.csproj";
        if (!std::filesystem::exists(expectedPath)) {
            std::cerr << "Error: Project file not found at " << expectedPath.string() << std::endl;
            return false;
        }
        else 
        {
            LOG_INFO("Building at : " + expectedPath.string());
        }

        std::string cleanbuildCommand = "dotnet clean " + (Project::currCodeProj / "GameDemo.csproj").string();
        //system(cleanbuildCommand.c_str());
        std::string buildCommand = "dotnet build " + (Project::currCodeProj / "GameDemo.csproj").string();
        int result = system(buildCommand.c_str());
        
        if (result != 0) {
            std::cerr << "Failed to build the C# project: " << result << std::endl;
            return false;
        }

        std::cout << "Scripts compiled successfully into: " << outputDllPath << std::endl;

        return true;

    }

    void MonoManager::LoadClasses()
    {
        EntityClasses.clear();

        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
        //EntityClass = new ScriptClass("Braveheart", "GameObject", true);
        MonoClass* entityClass = mono_class_from_name(CoreImage, "Braveheart", "GameObject");
        //set the image to relate to script core assembly so we can access gameobject scripts

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
            
            std::string fullClassName;
            if (strlen(nameSpace) != 0)
                fullClassName = fmt::format("{}.{}", nameSpace, className);
            else
                fullClassName = className;

            MonoClass* newClass = mono_class_from_name(image, nameSpace, className);

            if (newClass == EntityClass->m_MonoClass)
                continue;   

            bool isEntity = mono_class_is_subclass_of(newClass, entityClass, false);
            if (isEntity)
            {
                LOG_ERROR("IS A CHILD OF ENTITY");
            }
            else
            {
                LOG_ERROR("SCRIPT IS NOT A GAMEOBJECT");
            }

            Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);

            EntityClasses[className] = scriptClass;

            // This routine is an iterator routine for retrieving the fields in a class.
            // You must pass a gpointer that points to zero and is treated as an opaque handle
            // to iterate over all of the elements. When no more values are available, the return value is NULL.

            int fieldCount = mono_class_num_fields(newClass);
            LOG_WARN("{} has {} fields:", className, fieldCount);
            void* iterator = nullptr;
            while (MonoClassField* field = mono_class_get_fields(newClass, &iterator))
            {
                const char* fieldName = mono_field_get_name(field);
                uint32_t flags = mono_field_get_flags(field);
                if (flags & MONO_FIELD_ATTR_PUBLIC)
                {
                    MonoType* type = mono_field_get_type(field);
                    ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
                    LOG_TRACE("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));
                    scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
                }   
            }

        }




    }

    ScriptFieldMap& MonoManager::GetScriptFieldMap(Entity entity)
    {
        UUID entityID = entity.GetUUID();
        return EntityScriptFields[entityID];
    }

    bool MonoManager::EntityClassExists(const std::string& fullClassName)
    {
        return EntityClasses.find(fullClassName) != EntityClasses.end();
    }

    void MonoManager::CreateScriptEntity(Entity* entity, ScriptComponent* scriptComponet)
    {
        const auto& sc = scriptComponet;
        if (MonoManager::EntityClassExists(scriptComponet->className))
        {
            UUID entityID = entity->GetUUID();
            LOG_INFO("FINDING DATA FOR " + entity->GetComponent<TagComponent>().Tag);
            Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(EntityClasses[sc->className], *entity);

            scriptComponet->AssignScriptClass(instance);
            // Copy field values
            if (EntityScriptFields.find(entityID) != EntityScriptFields.end())
            {
                const ScriptFieldMap& fieldMap = EntityScriptFields.at(entityID);
                for (const auto& [name, fieldInstance] : fieldMap)
                    instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
            }

            instance->InvokeOnCreate();
        }
    }


    //Redone in LoadClasses
    
        ////Mono instances (or a Mono Domain) are required to manage and execute C# code
        //ScriptInstance* MonoManager::CreateScriptInstance (const std::string& className, const std::string& namespaceName, uint32_t entId)
        //{
        //    //all available classes in the core dll
        //    MonoAssembly* scriptCoreAssembly = mono_domain_assembly_open(domain, "Resource\\Scripts\\ScriptCore.dll");
        //    if (!scriptCoreAssembly) {
        //        LOG_INFO("Failed to load ScriptCore.dll!");
        //    }
        //
        //
        //    std::string fullClassName = namespaceName + "." + className;
        //     // Log the full class name we're trying to find
        //    std::cout << "Attempting to find class: " << fullClassName << std::endl;
        //
        //
        //    MonoClass* newClass = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
        //    //ScriptClass* newClass = new ScriptClass(namespaceName, className);
        //
        //    if (!newClass)
        //    {
        //        std::cerr << "Failed to find class: " << className << std::endl;
        //        return nullptr;
        //    }
        //    if (!domain)
        //    {
        //        std::cerr << "Mono domain is null! Cannot create script instance." << std::endl;
        //        return nullptr;
        //    }
        //
        //    // Create a new instance of the Mono class
        //    //MonoObject* instance = mono_object_new(domain, newClass);
        //    //if (!instance)
        //    //{
        //    //    std::cerr << "Error: Failed to create an instance of class '" << className << "'." << std::endl;
        //    //    return nullptr;
        //    //}
        //    //else
        //    //{
        //    //    std::cout << "CREATED CLASS" << std::endl;
        //    //}
        //
        //    // Initialize the Mono object
        //    //mono_runtime_object_init(instance);
        //    //std::cout << "Successfully created an instance of class: " << namespaceName << "." << className << std::endl;
        //    
        //
        //    ScriptClass* scriptClass = new ScriptClass(namespaceName, className);
        //
        //
        //    //Is this class an entity child
        //    MonoClass* parentClass = mono_class_get_parent(newClass);
        //    bool isEntity = mono_class_is_subclass_of(newClass, EntityClass, false);
        //    if (isEntity)
        //        LOG_ERROR("IS A CHILD OF ENTITY");
        //    else
        //    {
        //        LOG_ERROR("SCIPRT IS NOT A GAMEOBJECT");
        //    }
        //    //check if it is an entity child
        //    const char* parentClassName = mono_class_get_name(parentClass);
        //    const char* entityClassName = mono_class_get_name(EntityClass);
        //
        //    if (std::string(parentClassName) == std::string(entityClassName))
        //    {
        //        LOG_ERROR("IS A CHILD OF ENTITY");
        //        LOG_ERROR("ENTITY: " + std::string(parentClassName));
        //        LOG_ERROR("PARENT: " + std::string(entityClassName));
        //
        //        //set the image to relate to script core assembly so we can access gameobject scripts
        //        MonoImage* CoreImage = mono_assembly_get_image(scriptCoreAssembly);
        //        MonoClass* baseClass = mono_class_from_name(CoreImage, "Braveheart", "GameObject");
        //        MonoMethod* baseSetIDMethod = mono_class_get_method_from_name(baseClass, "SetID", 1);
        //
        //        //pass in the id
        //        if (baseSetIDMethod) {
        //            void* args[1];
        //            args[0] = &entId;
        //
        //            mono_runtime_invoke(baseSetIDMethod, instance, args, nullptr);
        //        }
        //        else
        //        {
        //            std::cerr << "Error." << std::endl;
        //        }
        //    }
        //
        //
        //    int fieldCount = mono_class_num_fields(newClass);
        //    CORE_ASSERT("{} has {} fields:", className, fieldCount);
        //    void* iterator = nullptr;
        //    while (MonoClassField* field = mono_class_get_fields(newClass, &iterator))
        //    {
        //        const char* fieldName = mono_field_get_name(field);
        //        uint32_t flags = mono_field_get_flags(field);
        //        if (flags & MONO_FIELD_ATTR_PUBLIC)
        //        {
        //            MonoType* type = mono_field_get_type(field);
        //            ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
        //
        //            scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
        //        }
        //    }
        //
        //    //Call the start method
        //    MonoMethod* method = mono_class_get_method_from_name(newClass, "Start", 0);
        //    
        //    if (!method)
        //    {
        //        std::cerr << "Method " << "Start" << " not found!" << std::endl;
        //        return false;
        //    }
        //
        //    MonoObject* exception = nullptr;
        //    mono_runtime_invoke(method, instance, nullptr, &exception);
        //
        //    if (exception)
        //    {
        //        MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
        //        const char* message = mono_string_to_utf8(exceptionMessage);
        //        std::cerr << "Exception occurred: " << message << "\n";
        //        mono_free(&message);
        //        return false;
        //    }
        //    
        //    
        //    
        //    return scriptClass;
        //}

    bool MonoManager::CallEntityScriptMethod(Ref<ScriptInstance> script, const std::string& methodName)
    {
        MonoClass* scriptClass = mono_object_get_class(script.get()->GetManagedObject());

        MonoMethod* method = mono_class_get_method_from_name(scriptClass, methodName.c_str(), 0);
        if (!method)
        {
            std::cerr << "Method " << methodName << " not found!" << std::endl;
            return false;
        }
        else 
        {
            MonoObject* exception = nullptr;
            mono_runtime_invoke(method, script->m_Instance, nullptr, &exception);

            if (exception)
            {
                MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
                const char* message = mono_string_to_utf8(exceptionMessage);
                std::cerr << "Exception occurred: " << message << "\n";
                mono_free(&message);
                return false;
            }
        }
        return true;
    }


    bool MonoManager::CallScriptMethod(MonoObject* _instance, const std::string& methodName)
    {
        if (!_instance)
        {
            std::cerr << "Invalid script instance!" << std::endl;
            return false;
        }

        MonoClass* scriptClass = mono_object_get_class(_instance);
        MonoMethod* method = mono_class_get_method_from_name(scriptClass, methodName.c_str(), 0);

        if (!method)
        {
            std::cerr << "Method " << methodName << " not found!" << std::endl;
            return false;   
        }

        MonoObject* exception = nullptr;
        mono_runtime_invoke(method, _instance, nullptr, &exception);

        if (exception)
        {
            MonoString* exceptionMessage = mono_object_to_string(exception, nullptr);
            const char* message = mono_string_to_utf8(exceptionMessage);
            std::cerr << "Exception occurred: " << message << "\n";
            mono_free(&message);
            return false;   
        }


        return true;
    }


    Scene* MonoManager::GetSceneContext()
    {
        return SceneContext;
    }


    void MonoManager::RuntimeStart(Scene* context)
    {
        SceneContext = context;
        MonoManager::CompileScripts();  
        MonoManager::ReloadMono();
        //MonoManager::LoadAppAssembly(std::filesystem::current_path() / ("Resource\\Scripts\\" + Project::currCodeProj.filename().string() + ".dll"));
    }



    void MonoManager::ReloadMono()
    {

        mono_domain_set(mono_get_root_domain(), false);

        mono_domain_unload(domain);

        LoadAssembly((std::filesystem::current_path() / ("Resource\\Scripts\\ScriptCore.dll")));
        PrintAssemblyTypes(CoreAssembly);
        LoadAppAssembly(std::filesystem::current_path() / ("Resource\\Scripts\\" + Project::currCodeProj.filename().string() + ".dll"));
        LoadClasses();

        ScriptGlue::RegisterFunctions();

        //Redefine entity class
        EntityClass = new ScriptClass("Braveheart", "GameObject", true);
        //EntityClass = mono_class_from_name(CoreImage, "Braveheart", "GameObject"); //extracting the gameobject class

    }

    void MonoManager::Shutdown()
    {
        if (domain)
        {
            mono_jit_cleanup(domain);
        }
        //delete rootDomain;
    }



    bool MonoManager::CleanScripts()
    {
        std::cout << "Cleaning previous build artifacts..." << std::endl;

        std::filesystem::path projectPath = (std::filesystem::current_path() / "Middleware/Mono/Data/AssemblyScripts.csproj");
        if (!std::filesystem::exists(projectPath))
        {
            std::cerr << "C# project file not found: " << projectPath << std::endl;
            return false;
        }

        std::string cleanProjectCommand = "dotnet clean " + projectPath.string();
        int result = system(cleanProjectCommand.c_str());
        if (result != 0)
        {
            std::cerr << "Failed to clean project. Error code: " << result << std::endl;
            return false;
        }

        std::cout << "Clean completed successfully!" << std::endl;
        return true;
    }


    std::vector<std::string> MonoManager::FindAllScripts()
    {
        std::vector<std::string> scriptFiles;

        std::filesystem::path assetsPath = std::filesystem::current_path() / "assets/Scripts/";
        LOG_INFO("FOLDER AT " + assetsPath.string());

        if (!std::filesystem::exists(assetsPath) || !std::filesystem::is_directory(assetsPath))
        {
            //if path does not exist or is not a directory
            return scriptFiles;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".cs")
            {
                scriptFiles.push_back(entry.path().string());
            }
        }

        return scriptFiles;
    }

    void MonoManager::GenerateBindings()
    {
        //CleanScripts();

        std::filesystem::path projectPath = (std::filesystem::current_path() / "Middleware/CppSharp/CppSharpBindings/CppSharpBindings.csproj");

        //std::string command = "dotnet run --project \ " + (std::filesystem::current_path() / "Middleware/CppSharp/CppSharpBindings/").string();
        std::string command = "dotnet run --project " + projectPath.string();

        int result = std::system(command.c_str());

        if (result == 0)
            std::cout << "CppSharp bindings generated successfully!" << std::endl;
        else
            std::cerr << "Failed to generate bindings!" << std::endl;
    }


    MonoObject* MonoManager::InvokeMethod(const std::string& assemblyName, const std::string& className, const std::string& methodName, const std::vector<MonoObject*>& params)
    {
        // find class within the assembly
        MonoClass* monoClass = mono_class_from_name(image, assemblyName.c_str(), className.c_str());
        if (!monoClass)
        {
            std::cerr << "Class not found: " << className << std::endl;
            return nullptr;
        }

        // find method to invoke
        MonoMethod* method = mono_class_get_method_from_name(monoClass, methodName.c_str(), params.size());
        if (!method)
        {
            std::cerr << "Method not found: " << methodName << std::endl;
            return nullptr;
        }

        MonoObject* exception = nullptr;
        MonoObject* result = nullptr;

        try
        {
            result = mono_runtime_invoke(method, nullptr, (void**)params.data(), &exception);

            // Handle any exceptions that occur during invocation
            if (exception)
            {
                // Retrieve the exception message from MonoObject
                MonoClass* exceptionClass = mono_object_get_class(exception);
                MonoMethod* toStringMethod = mono_class_get_method_from_name(exceptionClass, "ToString", 0);
                MonoObject* exceptionMessage = mono_runtime_invoke(toStringMethod, exception, nullptr, nullptr);

                // Convert the exception message to a string
                if (exceptionMessage)
                {
                    MonoString* exceptionStr = (MonoString*)exceptionMessage;
                    const char* msg = mono_string_to_utf8(exceptionStr);
                    std::cerr << "Exception occurred during method invocation: " << msg << std::endl;
                    mono_free((void*)msg); // Free the memory allocated by mono_string_to_utf8
                }
                else
                {
                    std::cerr << "Failed to retrieve exception message." << std::endl;
                }

                return nullptr;  // Return nullptr or handle the exception as needed
            }
        }
        catch (const std::exception& e)
        {
            // This catches any C++ exceptions that might be thrown (though Mono typically uses MonoObject for errors)
            std::cerr << "Caught C++ exception: " << e.what() << std::endl;
            return nullptr;
        }
        return result;
    }


    void MonoManager::ListAllMethods(MonoClass* klass)
    {
        void* iter = nullptr;
        MonoMethod* method;

        while ((method = mono_class_get_methods(klass, &iter)) != nullptr)
        {
            const char* methodName = mono_method_get_name(method);
            std::cout << "Method: " << methodName << std::endl;
        }
    }

    void MonoManager::ListAllClassesAndMethods(MonoImage* image)
    {
        const MonoTableInfo* tableInfo = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int rows = mono_table_info_get_rows(tableInfo);

        for (int i = 0; i < rows; ++i)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(tableInfo, i, cols, MONO_TYPEDEF_SIZE);

            const char* className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
            const char* namespaceName = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);

            std::cout << "Class: " << namespaceName << "." << className << std::endl;

            // Get the MonoClass for this class
            MonoClass* klass = mono_class_from_name(image, namespaceName, className);
            if (klass)
            {
                ListAllMethods(klass);
            }
        }
    }

    MonoObject* MonoManager::InstantiateClass(MonoClass* monoClass)
    {
        MonoObject* instance = mono_object_new(MonoManager::domain, monoClass);
        mono_runtime_object_init(instance);
        return instance;
    }

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(isCore ? MonoManager::CoreImage : MonoManager::image, classNamespace.c_str(), className.c_str());
    }

    MonoObject* ScriptClass::Instantiate()
    {
        return MonoManager::InstantiateClass(m_MonoClass);
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
    {
        if (!method)
        {
            LOG_ERROR("InvokeMethod: method is NULL");
            return nullptr;
        }

        if (!instance)
        {
            LOG_ERROR("InvokeMethod: instance is NULL");
            return nullptr;
        }

        MonoObject* exception = nullptr;
        MonoObject* result = mono_runtime_invoke(method, instance, params, &exception);

        if (exception)
        {
            MonoString* excString = mono_object_to_string(exception, nullptr);
            const char* excCString = mono_string_to_utf8(excString);
            LOG_ERROR(std::string("InvokeMethod Exception: ") + excCString);
            mono_free((void*)excCString);
            return nullptr;
        }

        return result;
    }


    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass)
    {
        m_Instance = scriptClass->Instantiate();

        m_Constructor = MonoManager::EntityClass->GetMethod(".ctor", 1);
        m_OnCreateMethod = scriptClass->GetMethod("Start", 0);
        m_OnUpdateMethod = scriptClass->GetMethod("Update", 1);

        // Call Entity constructor
        {
            uint32_t entityID = (uint32_t)entity;//.GetUUID();
            void* param = &entityID;
            m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
        }
    }

    void ScriptInstance::InvokeOnCreate()
    {
        if (m_OnCreateMethod)
            m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
    }

    void ScriptInstance::InvokeOnUpdate(float ts)
    {
        if (m_OnUpdateMethod)
        {
            void* param = &ts;
            try
            {
                m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
            }
            catch (...)
            {
                LOG_ERROR("UPDATE ERROR ON ENTITY SCRIPT {0}", m_ScriptClass->m_ClassName);
            }
        }
    }

    bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
    {
        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;
        mono_field_get_value(m_Instance, field.ClassField, buffer);
        return true;
    }

    bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
    {
        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;
        mono_field_set_value(m_Instance, field.ClassField, (void*)value);
        return true;
    }
}