//===---------------- DependenciesMappingPlugin.cpp ---------------------------------------===//
//
// This plugin creates a dependencies graph for a file, based on its #include directives.
// The output of the plugin is a “<file-name>.dot” file, which is in a format that 
// is required by Graphviz ( https://www.graphviz.org ).
//
// Mandatory argument: name (full path) of the output file (.dot)
// Optional argument: "angle" - the plugin will not stop at system headers and will cover the 
// full depth of the #include-s chain
//
// The plugin's structur was adapted from: https://github.com/chisophugis/clang_plugin_example
//
//===--------------------------------------------------------------------------------------===//

#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormattedStream.h"

#include <string>
#include <fstream>
#include <set>
#include <list>
#include <map>
#include <stack>

using namespace clang;
using namespace std;

namespace {

    class FindDependencies : public PPCallbacks {
    private:
        SourceManager& SM;
        int fileIndex = 0;
        string outFile = string();               /* empty string */
        stack<std::string> filesStack;
        map <string, int> fileToID;
        map<string, list<string>> fileToIncludes;
        bool angleIncludes = false;
        set<string> angledIncludesFileNames;
        string firstAngledFileInIncludesChain = string(); /* empty string */
        list<string> userDefinedHeaders;
        
        string extractFileNameFromPath(string path) {
            size_t posLastSlash = path.find_last_of("/");
            if (posLastSlash == string::npos) {
                posLastSlash = path.find_last_of("\\");
            }
            return path.substr(posLastSlash + 1);
        }
        
        void writeToDotFile() {
            ofstream fd;
            fd.open(outFile, std::fstream::out);
            fd << "digraph d {\n";
            for (pair<string, int> fileToID : fileToID) {
                /* A [label="Hello"] */
                bool isUserDefinedHeader = find(userDefinedHeaders.begin(), userDefinedHeaders.end(), fileToID.first) != userDefinedHeaders.end(); 
                /* color user defined files */
                string color = "";
                if (fileToID.second == 1) {
                    color = ", style=filled, fillcolor=\"palegreen3\"";
                }
                else if (isUserDefinedHeader) {
                    color = ", style=filled, fillcolor=\"slategray3\"";
                }
                fd << " " << fileToID.second << " [label=\"" <<  extractFileNameFromPath(fileToID.first.c_str()) << "\"" << color << "]" << endl; 
            }
            
            for (pair<string, list<string>> includingFile: fileToIncludes) {
                /* A -> { B C } */
                fd << " " << fileToID[includingFile.first] << " -> { ";
                for (list<string>::iterator it = includingFile.second.begin(); it != includingFile.second.end(); ++it) {
                    fd <<  fileToID[*it] << " ";
                }
                fd << "}" << endl;
            }
            fd << "}\n";
            fd.close();
        }
   
    public:
        explicit FindDependencies(SourceManager& sm, bool angleIncludes, string outFile)
        : SM(sm), outFile(outFile), angleIncludes(angleIncludes) {}
        
        void FileChanged(SourceLocation Loc, FileChangeReason Reason, SrcMgr::CharacteristicKind FileType, FileID PrevFID) override {
            if (Reason != EnterFile && Reason != ExitFile)
                return;
            if (const FileEntry *FE = SM.getFileEntryForID(SM.getFileID(Loc))) {
                if (Reason == EnterFile) {
                    string includedFile = string(FE->getName());
                    bool isIncludedFileMapped = !(fileToID.find(includedFile)== fileToID.end());
                    /* stack is not empty */
                    if (filesStack.size() > 0) {
                        string includingFile = filesStack.top();
                        bool isInAngledFileIncludesChain = !firstAngledFileInIncludesChain.empty();
                        
                        if (angleIncludes || !isInAngledFileIncludesChain) {
                            if (!isIncludedFileMapped) {
                                fileToID[includedFile] = ++fileIndex;
                            }
                            fileToIncludes[includingFile].push_back(includedFile);
                            
                            bool isIncludedFileAngled = angledIncludesFileNames.find(includedFile) != angledIncludesFileNames.end();
                            
                            if (!isInAngledFileIncludesChain) {
                                if (isIncludedFileAngled) {
                                    /* current file is first angled file in #incled-s chain */
                                    firstAngledFileInIncludesChain = includedFile;
                                }
                                else {
                                    /* current file is user defined header */
                                    userDefinedHeaders.push_back(includedFile);
                                }
                            }
                        }
                    }
                    else {  /* stack is empty */
                        if (!isIncludedFileMapped) {
                            fileToID[includedFile] = ++fileIndex;
                        }
                    }
                    filesStack.push(includedFile);
                } else if (Reason == ExitFile) {
                    if (filesStack.top() == firstAngledFileInIncludesChain) {
                        firstAngledFileInIncludesChain = string(); /* exiting angled file's #incled-s chain */
                    }
                    if (const FileEntry *FE = SM.getFileEntryForID(PrevFID)) {
                        filesStack.pop();
                    }
                }
            }
        }
        
        void FileSkipped(const FileEntry &SkippedFile,
                         const Token &FilenameTok,
                         SrcMgr::CharacteristicKind FileType) override {            
            string includedFile = string(SkippedFile.getName().data());
            bool isIncludedFileMapped = !(fileToID.find(includedFile)== fileToID.end());
            
            if (filesStack.size() > 0) {
                string includingFile = filesStack.top();
                bool isInAngledFilePath = !firstAngledFileInIncludesChain.empty();
                
                if (angleIncludes || !isInAngledFilePath) {
                    if (!isIncludedFileMapped) {
                        fileToID[includedFile] = ++fileIndex;
                    }
                    fileToIncludes[includingFile].push_back(includedFile);
                }
            }
            else {  /* stack is empty */
                if (!isIncludedFileMapped) {
                    fileToID[includedFile] = ++fileIndex;
                }
            }
        }
        
        void InclusionDirective(SourceLocation HashLoc,
                                        const Token &IncludeTok,
                                        StringRef FileName,
                                        bool IsAngled,
                                        CharSourceRange FilenameRange,
                                        const FileEntry *File,
                                        StringRef SearchPath,
                                        StringRef RelativePath,
                                        const Module *Imported,
                                        SrcMgr::CharacteristicKind FileType) override {
            if (IsAngled) {
                angledIncludesFileNames.insert(string(File->getName().data()));
            }
        }
        
        void EndOfMainFile() override {
            writeToDotFile();
        }
    };

    class FindDependenciesAction : public PluginASTAction {
    private:
        bool angleIncludes = false;
        string outFile = string();
        
    public:
        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance & CI, llvm::StringRef InFile) override {
            Preprocessor &PP = CI.getPreprocessor();
            PP.addPPCallbacks(
                llvm::make_unique<FindDependencies>(CI.getSourceManager(), angleIncludes, outFile));
            return llvm::make_unique<ASTConsumer>();
        }
        
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args) override {
            for (unsigned i = 0, e = args.size(); i != e; ++i) {
                angleIncludes = angleIncludes || args[i] == "angle";
                
                if (args[i] == "-out-file" && i + 1 < args.size()) {
                    outFile = args[i + 1];
                }
            }
            
            if (outFile.empty()) {
                DiagnosticsEngine &D = CI.getDiagnostics();
                D.Report(D.getCustomDiagID(DiagnosticsEngine::Error, "missing -out-file argument"));
                return false;
            }
            
            return true;
        }
    };
}

static FrontendPluginRegistry::Add<FindDependenciesAction>
X("DependenciesMappingPlugin", "Print out header dependencies");
