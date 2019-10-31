#include <conio.h>
#include <stdio.h>
#include <memory>
#include <chrono>

// Wraith application and api (Must be included before additional includes)
#include "WraithApp.h"
#include "WraithX.h"
#include "Console.h"
#include "Hashing.h"
#include "Sound.h"
#include "Instance.h"
#include "FileSystems.h"
#include "IFSLib.h"
#include "Systems.h"

// We need the online game module
#include "GameOnline.h"

// Allows straight unpacking of IFS
void UnpackIFSFile(const std::string& IFS, bool DDS = false)
{
    // Make it
    auto ExportFolder = FileSystems::CombinePath(FileSystems::CombinePath(FileSystems::GetApplicationPath(), "exported_files\\codol"), FileSystems::GetFileNameWithoutExtension(IFS));
    // Create it
    FileSystems::CreateDirectory(ExportFolder);

    // Mount the IFS file
    auto IFSHandler = IFSLib();
    // Load it
    auto ListFile = IFSHandler.ParsePackage(IFS, false);

    // Log info
    Console::WriteLineHeader("IFS", "Loaded \"%s\"", FileSystems::GetFileName(IFS).c_str());
    Console::WriteLineHeader("IFS", "Loaded %d files", ListFile.size());

    // Iterate and export them
    for (auto& File : ListFile)
    {
        // Fetch result file
        auto FileName = File;

        // Extract it
        uint32_t ResultSize = 0;
        // Read the entry
        auto ResultFile = IFSHandler.ReadFileEntry(FileName, ResultSize);

        // If not blank, export
        if (ResultFile != nullptr)
        {
            // Get Full Directory Path
            auto OutputDirectory = FileSystems::CombinePath(ExportFolder, FileSystems::GetDirectoryName(FileName));
            // Create it
            FileSystems::CreateDirectory(OutputDirectory);

            // If MP3, write it, else, convert it
            if (Strings::EndsWith(FileName, ".mp3"))
            {
                // Write to a file
                auto Writer = BinaryWriter();
                Writer.Create(FileSystems::CombinePath(ExportFolder, FileName));
                Writer.Write(ResultFile.get(), ResultSize);
            }
            else
            {
                // Convert IWI
                auto IWIConv = CoDIWITranslator::TranslateIWI(ResultFile, ResultSize);
                // Check
                if (IWIConv != nullptr)
                {
                    auto OutputPath = FileSystems::CombinePath(OutputDirectory, FileSystems::GetFileNameWithoutExtension(FileName));

                    // Save to a file
                    if (DDS)
                    {
                        // Write to a file, with a dds
                        auto Writer = BinaryWriter();
                        Writer.Create(OutputPath + ".dds");
                        Writer.Write(IWIConv->DataBuffer, IWIConv->DataSize);
                    }
                    else
                    {
                        // Transcode to PNG
                        Image::ConvertImageMemory(IWIConv->DataBuffer, IWIConv->DataSize, ImageFormat::DDS_WithHeader, OutputPath + ".png", ImageFormat::Standard_PNG);
                    }
                }
            }

            // Log it
            Console::WriteLineHeader("IFS", "Exported \"%s\"", FileSystems::GetFileNameWithoutExtension(FileName).c_str());
        }
    }

    // Log complete
    Console::WriteLineHeader("IFS", "Exported all existing IFS assets");
}

// Initializes Color Path (Duplicated from Console::InitializeWraithColorPallete())
void InitializeImageExporterPallete()
{
    // Prepare color map
    Console::RemapConsoleColor(ConsoleColor::Gray, RGB(0x0, 0x49, 0xC1));
    Console::RemapConsoleColor(ConsoleColor::Blue, RGB(0x1E, 0x90, 0xFF));
    Console::RemapConsoleColor(ConsoleColor::Red, RGB(0xFF, 0x0, 0x0));
    Console::RemapConsoleColor(ConsoleColor::Yellow, RGB(0xF1, 0x95, 0x0A));
    Console::RemapConsoleColor(ConsoleColor::Green, RGB(0xFF, 0xFF, 0xFF));
    // Make the text default to white!
    Console::SetForegroundColor(ConsoleColor::White);
    Console::SetBackgroundColor(ConsoleColor::Black);
    // Clear the console!
    Console::Clear();
}

// Main entry point of app
int main(int argc, char** argv)
{
    // Start the instance (We must provide the main window title, never include versions from now on)
    auto CanContinue = Instance::BeginSingleInstance("Mecha");
    // Only resume if we can
    if (CanContinue)
    {
        // Initialize the API (This must be done BEFORE running a WraithApp)
        if (!WraithX::InitializeAPI(true))
        {
            // Failed to initialize
            MessageBoxA(NULL, "A fatal error occured while initializing Wraith", "Wraith", MB_OK | MB_ICONEXCLAMATION);
            // Failed
            return -1;
        }

        // Enter debug mode
        Systems::EnterDebugMode();

        // Setup the custom color scheme
        InitializeImageExporterPallete();
        Console::SetTitle("Mecha - Fork of Wraith Cyborg");
        // Output heading
        Console::WriteLineHeader("Initialize", "-----------------------------------------------------------------");
        Console::WriteLineHeader("Initialize", "Mecha - Fork of Wraith Cyborg (CODOL Asset Extractor)");
        Console::WriteLineHeader("Initialize", "Author: DTZxPorter - Maintained by Scobalula");
        Console::WriteLineHeader("Initialize", "Desc: Allows extraction of models, animations, images and sounds");
        Console::WriteLineHeader("Initialize", "-----------------------------------------------------------------");

        // If we have an argument, and the argument is an IFS file, jump to the generic unpacker
        if (argc > 1 && Strings::EndsWith(argv[1], ".ifs"))
        {
            // DDS flags
            if (argc > 2 && Strings::StartsWith(argv[2], "dds"))
            {
                // Ask to unpack this
                UnpackIFSFile(std::string(argv[1]), true);
            }
            else
            {
                // Ask to unpack this
                UnpackIFSFile(std::string(argv[1]));
            }

            // End the routine
            return 0;
        }
        
        // Prepare the game manager, we must attach early in the spawn!
        if (GameOnline::LoadGame())
        {
            // From here, pass specific arguments to the exporter, we can do everything now... (All we needed was the handle...)
            while (true)
            {
                // Ask what we want to rip
                Console::WriteHeader("User Input", "Command: ");
                // Get the user input, split into command
                auto SplitCommand = Strings::SplitString(Strings::ToLower(Console::ReadLine()), ' ', true);

                // Settings
                bool Anims = false;
                bool Models = false;
                bool Images = false;
                bool Sounds = false;
                bool ListOnly = false;
                // Search queries
                std::vector<std::string> Search;

                // Reset export config
                GameOnline::ExportConfiguration = GameExportConfig();

                // Ignore blank
                if (SplitCommand.size() <= 0)
                    continue;
                
                // Check
                if (SplitCommand[0] == "exit")
                    break;
                // Iterate
                for (auto& Split : SplitCommand)
                {
                    // Loop through and find items to export
                    if (Split == "-ripanims")
                    {
                        Anims = true;
                    }
                    else if (Split == "-ripmodels")
                    {
                        Models = true;
                    }
                    else if (Split == "-ripimages")
                    {
                        Images = true;
                    }
                    else if (Split == "-ripsounds")
                    {
                        Sounds = true;
                    }
                    else if (Split == "-listassets")
                    {
                        ListOnly = true;
                    }
                    else if (Split == "-xanimwaw")
                    {
                        GameOnline::ExportConfiguration.XAnimsWAW = true;
                        GameOnline::ExportConfiguration.XAnimsBO = false;
                    }
                    else if (Split == "-xanimbo")
                    {
                        GameOnline::ExportConfiguration.XAnimsWAW = false;
                        GameOnline::ExportConfiguration.XAnimsBO = true;
                    }
                    else if (Split == "-seanim")
                    {
                        GameOnline::ExportConfiguration.SEAnims = true;
                    }
                    else if (Split == "-obj")
                    {
                        GameOnline::ExportConfiguration.OBJ = true;
                    }
                    else if (Split == "-xna")
                    {
                        GameOnline::ExportConfiguration.XNA = true;
                    }
                    else if (Split == "-smd")
                    {
                        GameOnline::ExportConfiguration.SMD = true;
                    }
                    else if (Split == "-ma")
                    {
                        GameOnline::ExportConfiguration.Maya = true;
                    }
                    else if (Split == "-xme")
                    {
                        GameOnline::ExportConfiguration.XME = true;
                    }
                    else if (Split == "-semodel")
                    {
                        GameOnline::ExportConfiguration.SEModel = true;
                    }
                    else
                    {
                        Search.push_back(Split);
                    }
                }

                if (Anims || Models || Images || Sounds)
                {
                    GameOnline::ExportConfiguration.ValidateConfig();
                    GameOnline::ExtractAssets(Anims, Models, Images, Sounds, ListOnly, Search);
                }
                else
                {
                    // Unknown command
                    Console::WriteLineHeader("Command", "Unknown command, try \"-ripanims, -ripmodels, -ripsounds, and/or -ripimages\"");
                }
            }
        }
        else
        {
            // Log error
            Console::SetBackgroundColor(ConsoleColor::Red);
            Console::WriteLineHeader("Game", "Failed to attach to the game, you may have launched too late!");
            Console::SetBackgroundColor(ConsoleColor::Black);
            Console::ReadKey();
        }

        // Leave debug
        Systems::LeaveDebugMode();

        // Shutdown the API, we're done
        WraithX::ShutdownAPI(false);

        // Stop the instance
        Instance::EndSingleInstance();
    }

    // We're done here
    return 0;
}