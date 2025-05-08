using System;
using System.Diagnostics;
using System.IO;
using CppSharp;
using CppSharp.AST;
using CppSharp.Generators;
using CppSharp.Passes;
using CppSharpBindingsGenerator;

namespace CppSharpBindingsGenerator
{
    public class BindingsGenerator : ILibrary
    {
        // Configures the CppSharp driver and sets up the module
        public void Setup(Driver driver)
        {
            driver.Options.GeneratorKind = GeneratorKind.CSharp; // Generate C# bindings
            driver.Options.Verbose = true; // Enable detailed logging

            // Create a module for the C++ project
            var module = driver.Options.AddModule("GeneratedScripts");
            if (module == null) 
            {
                throw new Exception("Failed to create module. Check your configuration.");
            }

            module.SharedLibraryName = "ScriptCore";
            module.OutputNamespace = "Braveheart"; // Ensures all bindings are generated under this namespace



            string currentPath = Directory.GetCurrentDirectory();
            string includePath = Path.Combine(currentPath, @"src"); // Path to include folder
            //includePath += Path.Combine(currentPath, @"Middleware"); // Path to include folder
            //string libPath = Path.Combine(currentPath, @"Middleware\Mono\lib"); // Path to libraries folder (customizable)
            string libPath = Path.Combine(currentPath, @"mono\lib"); // Path to libraries folder (customizable)

            // Ensure paths are absolute
            includePath = Path.GetFullPath(includePath);
            libPath = Path.GetFullPath(libPath);

            Console.WriteLine($"Root directory -> {Path.GetFullPath(Directory.GetDirectoryRoot(Directory.GetCurrentDirectory()))}");
            Console.WriteLine($"Curr directory -> {Path.GetFullPath(Directory.GetCurrentDirectory())}");
            Console.WriteLine($"ProjectLib directory -> {Path.GetFullPath(Path.Combine(currentPath))}");
            Console.WriteLine($"Include directory -> {includePath}");
            Console.WriteLine($"Library directory -> {libPath}");

            module.IncludeDirs.Add(Path.GetFullPath(includePath));

            module.Headers.Add(Path.GetFullPath(Path.Combine(includePath, @"Minimal.h")));

            // Ensure the library is explicitly referenced
            module.LibraryDirs.Add(Path.GetFullPath(Path.Combine(currentPath, @"Resource\Scripts")));
            module.Libraries.Add("ScriptCore"); // Without .lib or .dll

            // Set the output directory for the generated bindings
            string outputDir = Path.Combine(Directory.GetCurrentDirectory(), @"..\Projects\GameDemo\Assets\Scripts  ");
            //string outputDir = Path.Combine(Directory.GetCurrentDirectory(), @"Middleware/CppSharp/CppSharpBindings/CustomGenerated");

            driver.Options.OutputDir = outputDir;
            Console.WriteLine($"Bindings will be generated in: {outputDir}");

        }



        public void Preprocess(Driver driver, ASTContext ctx)
        {
            Console.WriteLine("Preprocessing AST...");


            //remapping namespaces

            //// Apply the namespace remapping logic to all declarations
            //foreach (var unit in ctx.TranslationUnits)
            //{
            //    if (!unit.IsGenerated)
            //        continue;
            //
            //    foreach (var decl in unit.Declarations)
            //    {
            //        // Check if the declaration is in the "Engine" namespace
            //        if (decl.Namespace != null && decl.Namespace.Name == "RuntimeScript")
            //        {
            //            // Rename the namespace to "RuntimeBindings"
            //            decl.Namespace.Name = "Engine";
            //        }
            //    }
            //}
        }

        public void Postprocess(Driver driver, ASTContext ctx)
        {
            //throw new NotImplementedException();
        }

        public void SetupPasses(Driver driver) 
        {
            driver.Context.TranslationUnitPasses.RenameDeclsUpperCase(RenameTargets.Any);
            driver.Context.TranslationUnitPasses.AddPass(new FunctionToInstanceMethodPass());
        }

        // Main entry point for the bindings generator
        //public void GenerateCode(Driver driver)
        //{
        //    // Specify the directory where you want to save the generated files
        //    string outputDir = Path.Combine(Directory.GetCurrentDirectory(), "Middleware/Mono/Data/GeneratedBindings");
        //    Console.WriteLine($"Writing to: {outputDir}");
        //
        //    // Ensure the directory exists
        //    if (!Directory.Exists(outputDir))
        //    {
        //        Directory.CreateDirectory(outputDir);
        //    }
        //
        //    Console.WriteLine("Generating C# bindings...");
        //
        //    // Set the output directory for the generator
        //    driver.Options.OutputDir = outputDir;
        //
        //    // Generate the bindings
        //    driver.Generator.Generate();
        //
        //    Console.WriteLine($"Bindings generated in: {outputDir}");
        //}
    }
}


class Program
{
    public static void Main()
    {
        ConsoleDriver.Run(new BindingsGenerator());
    }
}